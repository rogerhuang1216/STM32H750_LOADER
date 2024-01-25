/*
 *	Copied from Linux Monitor (LiMon) - Networking.
 *
 *	Copyright 1994 - 2000 Neil Russell.
 *	(See License)
 *	Copyright 2000 Roland Borde
 *	Copyright 2000 Paolo Scaffardi
 *	Copyright 2000-2002 Wolfgang Denk, wd@denx.de
 */

/*
 * General Desription:
 *
 * The user interface supports commands for BOOTP, RARP, and TFTP.
 * Also, we support ARP internally. Depending on available data,
 * these interact as follows:
 *
 * BOOTP:
 *
 *	Prerequisites:	- own ethernet address
 *	We want:	- own IP address
 *				- TFTP server IP address
 *				- name of bootfile
 *	Next step:	ARP
 *
 * RARP:
 *
 *	Prerequisites:	- own ethernet address
 *	We want:	- own IP address
 *				- TFTP server IP address
 *	Next step:	ARP
 *
 * ARP:
 *
 *	Prerequisites:	- own ethernet address
 *				- own IP address
 *				- TFTP server IP address
 *	We want:	- TFTP server ethernet address
 *	Next step:	TFTP
 *
 * DHCP:
 *
 *     Prerequisites:	- own ethernet address
 *     We want:		- IP, Netmask, ServerIP, Gateway IP
 *					- bootfilename, lease time
 *     Next step:		- TFTP
 *
 * TFTP:
 *
 *	Prerequisites:	- own ethernet address
 *				- own IP address
 *				- TFTP server IP address
 *				- TFTP server ethernet address
 *				- name of bootfile (if unknown, we use a default name
 *			  	   derived from our own IP address)
 *	We want:	- load the boot file
 *	Next step:	none
 *
 * NFS:
 *
 *	Prerequisites:	- own ethernet address
 *				- own IP address
 *				- name of bootfile (if unknown, we use a default name
 *			         derived from our own IP address)
 *	We want:	- load the boot file
 *	Next step:	none
 *
 * SNTP:
 *
 *	Prerequisites:	- own ethernet address
 *				- own IP address
 *	We want:	- network time
 *	Next step:	none
 */


#include "loader.h"
#include "cmd.h"
#include "net.h"
#include "eth.h"



#define ARP_TIMEOUT			3		/* Seconds before trying ARP again */
#define ARP_TIMEOUT_COUNT	5		/* # of timeouts before giving up  */


/** BOOTP EXTENTIONS **/

IPaddr_t		NetOurSubnetMask=0x00ffffff;		/* Our subnet mask (0=unknown)	*/
IPaddr_t		NetOurGatewayIP=0xfe04a8c0;		/* Our gateways IP address	*/
IPaddr_t		NetOurDNSIP=0;						/* Our DNS IP address		*/
INT8S		NetOurNISDomain[32]={0,};			/* Our NIS domain		*/
INT8S		NetOurHostName[32]={0,};			/* Our hostname			*/
INT8S		NetOurRootPath[64]={0,};				/* Our bootpath			*/
ushort		NetBootFileSize=0;					/* Our bootfile size in blocks	*/

/** END OF BOOTP EXTENTIONS **/

static INT32S	TftpServerPort;		/* The UDP port at their end		*/
INT32S	TftpOurPort = 69;			/* The UDP port at our end		*/

ulong		NetBootFileXferSize;	/* The actual transferred size of the bootfile (in bytes) */
#if (PCI_DEVICE_MODE==0)
uchar		NetOurEther[6] = { 0x00, 0xaa, 0x11, 0xbb, 0x22, 0xcc };		/* Our ethernet address			*/
#else
/*For minipci the default Mac addr. use the same with WiMAX-MAC's default addr.*/
uchar		NetOurEther[6] = { 0x00, 0x0b, 0x81, 0x00, 0x00, 0x01 };
#endif
uchar		NetServerEther[6] =	/* Boot server enet address		*/
			{ 0, 0, 0, 0, 0, 0 };

//IPaddr_t		NetOurIP=0x6f04a8c0;		/* Our IP addr (0 = unknown)		*/
IPaddr_t	NetOurIP=0x03853b0a;	/* Our IP addr (0 = unknown)		*/        // roger 20150819 add, 10.59.133.3


IPaddr_t		NetServerIP;		/* Our IP addr (0 = unknown)		*/
volatile uchar *pNetRxPkt;		/* Current receive packet		*/
INT32S		NetRxPktLen;		/* Current rx packet length		*/
unsigned		NetIPID;		/* IP packet ID				*/
uchar		NetBcastAddr[6] =	/* Ethernet bcast address		*/
			{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
uchar		NetEtherNullAddr[6] =
			{ 0, 0, 0, 0, 0, 0 };
uchar		NetGatwayAddr[6] =		/* Ethernet bcast address		*/
			{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };		//temp gatway for debug use
net_loop_enum		NetState = NETLOOP_NONE;		/* Network loop state			*/

/* XXX in both little & big endian machines 0xFFFF == ntohs(-1) */

INT8S		BootFile[128];		/* Boot File name			*/

#if (CONFIG_COMMANDS & CFG_CMD_PING)
IPaddr_t		 NetPingIP;		/* the ip address to ping 		*/
static INT32S  PingCnt;		/* counter for ping function */
static INT32S  PingTimes;		/* times for ping function */
ulong		 PingStartTick;	/* ping tick to calculate ping used time */
static VOID PingStart(VOID);
arp_tab_struct ArpTabHdr = {0,{0, 0, 0, 0, 0, 0}, 0, NULL};	
#endif




volatile uchar	PktBuf[(PKTBUFSRX+1) * PKTSIZE_ALIGN + PKTALIGN];

volatile uchar *NetRxPackets[PKTBUFSRX]; /* Receive packets			*/

static rxhand_f *pPacketHandler;		/* Current RX packet handler		*/
static thand_f *pTimeHandler;		/* Current timeout handler		*/
static ulong	TimeStart;		/* Time base value			*/
static ulong	TimeDelta;		/* Current timeout value		*/
volatile uchar *pNetTxPacket = 0;	/* THE transmit packet			*/

extern INT32S		BootpTry;
extern INT32S		RarpTry;
extern struct	SYS_VENDOR_DEFINE SYS_Vendor;

//extern VOID Delay(INT32S tick);
//extern ulong GetTick(VOID);
extern VOID TFTPS_Inform(INT8U *ppkt, INT16U len);

static INT32S net_check_prereq (proto_t protocol)
{
	switch (protocol) {
		/* Fall through */
#if (CONFIG_COMMANDS & CFG_CMD_PING)
	case PING:
		if (NetPingIP == 0) {
			printf("*** ERROR: ping address not given\n");
			return (1);
		}
		goto common;
#endif
	case NETCONS:
	case TFTP:
		if (NetServerIP == 0) {
			printf("*** ERROR: `serverip' not set\n");
			return (1);
		}
#if (CONFIG_COMMANDS & CFG_CMD_PING)
    common:
#endif

		if (NetOurIP == 0) {
			printf("*** ERROR: `ipaddr' not set\n");
			return (1);
		}
		/* Fall through */

	case DHCP:
	case RARP:
	case BOOTP:
	case CDP:
		if (memcmp (NetOurEther, "\0\0\0\0\0\0", 6) == 0) 
		{
			printf("*** ERROR: `ethaddr' not set\n");
			return (1);
		}
		/* Fall through */
	default:
		return (0);
	}
	return (0);		/* OK */
}

/**********************************************************************/

IPaddr_t	NetArpWaitPacketIP;
IPaddr_t	NetArpWaitReplyIP;
uchar	   *NetArpWaitPacketMAC;	/* MAC address of waiting packet's destination	*/
uchar	   *NetArpWaitTxPacket;		/* THE transmit packet			*/
INT32S			NetArpWaitTxPacketSize;
uchar 		NetArpWaitPacketBuf[PKTSIZE_ALIGN + PKTALIGN];
ulong		NetArpWaitTimerStart;
INT32S			NetArpWaitTry;









/* Check WMX downlaod firmware if timeout */
extern 	WmxDev wdev;
extern INT32S put, get;
INT32S	wmx_timer;







