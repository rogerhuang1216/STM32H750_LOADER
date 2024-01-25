/*
 *	LiMon Monitor (LiMon) - Network.
 *
 *	Copyright 1994 - 2000 Neil Russell.
 *	(See License)
 *
 *
 * History
 *	9/16/00	  bor  adapted to TQM823L/STK8xxL board, RARP/TFTP boot added
 */

#ifndef __NET_H__
#define __NET_H__

/*
 *	The number of receive packet buffers, and the required packet buffer
 *	alignment in memory.
 *
 */

#ifdef CFG_RX_ETH_BUFFER
#define PKTBUFSRX	CFG_RX_ETH_BUFFER
#else
#define PKTBUFSRX	4
#endif

#define PKTALIGN	32
#define	CFG_HZ		100

typedef ulong		IPaddr_t;

typedef VOID	rxhand_f(uchar *, unsigned, unsigned, unsigned);
typedef VOID	thand_f(VOID);



/**********************************************************************/
/*
 *	Protocol headers.
 */

/*
 *	Ethernet header
 */
typedef struct {
	uchar		et_dest[6];	/* Destination node		*/
	uchar		et_src[6];	/* Source node			*/
	ushort		et_protlen;	/* Protocol or length		*/
	uchar		et_dsap;	/* 802 DSAP			*/
	uchar		et_ssap;	/* 802 SSAP			*/
	uchar		et_ctl;		/* 802 control			*/
	uchar		et_snap1;	/* SNAP				*/
	uchar		et_snap2;
	uchar		et_snap3;
	ushort		et_prot;	/* 802 protocol			*/
} Ethernet_t;

#define ETHER_HDR_SIZE	14		/* Ethernet header size		*/
#define E802_HDR_SIZE	22		/* 802 ethernet header size	*/

/*
 *	Ethernet header
 */

#define PROT_IP		0x0800		/* IP protocol			*/
#define PROT_ARP	0x0806		/* IP ARP protocol		*/
#define PROT_RARP	0x8035		/* IP ARP protocol		*/
#define PROT_VLAN	0x8100		/* IEEE 802.1q protocol		*/
/* support 0x1620 paradom ether type */
#define PROT_WMX	0x1620		/* WiMAX Management protocol */

#define IPPROTO_ICMP	1	/* Internet Control Message Protocol	*/
#define IPPROTO_UDP		17	/* User Datagram Protocol		*/

/*
 *	Internet Protocol (IP) header.
 */
typedef struct {
	uchar		ip_hl_v;	/* header length and version	*/
	uchar		ip_tos;		/* type of service		*/
	ushort		ip_len;		/* total length			*/
	ushort		ip_id;		/* identification		*/
	ushort		ip_off;		/* fragment offset field	*/
	uchar		ip_ttl;		/* time to live			*/
	uchar		ip_p;		/* protocol			*/
	ushort		ip_sum;		/* checksum			*/
	IPaddr_t	ip_src;		/* Source IP address		*/
	IPaddr_t	ip_dst;		/* Destination IP address	*/
	ushort		udp_src;	/* UDP source port		*/
	ushort		udp_dst;	/* UDP destination port		*/
	ushort		udp_len;	/* Length of UDP packet		*/
	ushort		udp_xsum;	/* Checksum			*/
} IP_t;

#define IP_HDR_SIZE_NO_UDP	(sizeof (IP_t) - 8)
#define IP_HDR_SIZE		(sizeof (IP_t))
#define UDP_HLEN        8


/*
 *	Address Resolution Protocol (ARP) header.
 */
typedef struct
{
	ushort		ar_hrd;		/* Format of hardware address	*/
#define ARP_ETHER	    1		/* Ethernet  hardware address	*/
	ushort		ar_pro;		/* Format of protocol address	*/
	uchar		ar_hln;		/* Length of hardware address	*/
	uchar		ar_pln;		/* Length of protocol address	*/
	ushort		ar_op;		/* Operation			*/
#define ARPOP_REQUEST    1		/* Request  to resolve  address	*/
#define ARPOP_REPLY	     2		/* Response to previous request	*/
#define RARPOP_REQUEST   3		/* Request  to resolve  address	*/
#define RARPOP_REPLY	 4		/* Response to previous request */

	/*
	 * The remaining fields are variable in size, according to
	 * the sizes above, and are defined as appropriate for
	 * specific hardware/protocol combinations.
	 */
	//leo//uchar		ar_data[0];
	uchar		ar_data[16];
#if 0
	uchar		ar_sha[];	/* Sender hardware address	*/
	uchar		ar_spa[];	/* Sender protocol address	*/
	uchar		ar_tha[];	/* Target hardware address	*/
	uchar		ar_tpa[];	/* Target protocol address	*/
#endif /* 0 */
} ARP_t;

#define ARP_HDR_SIZE	(8+20)		/* Size assuming ethernet	*/

/*
 * ICMP stuff (just enough to handle (host) redirect messages)
 */
#define ICMP_ECHO_REPLY		0	/* Echo reply 			*/
#define ICMP_REDIRECT		5	/* Redirect (change route)	*/
#define ICMP_ECHO_REQUEST	8	/* Echo request			*/

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET		0	/* Redirect Net			*/
#define ICMP_REDIR_HOST		1	/* Redirect Host		*/

typedef struct icmphdr {
	uchar		type;
	uchar		code;
	ushort		checksum;
	union {
		struct {
			ushort	id;
			ushort	sequence;
		} echo;
		ulong	gateway;
		struct {
			ushort	__unused;
			ushort	mtu;
		} frag;
	} un;
} ICMP_t;

/* WMX command response */
#define RSP_INIT        0x00
#define RSP_OK          0x01
#define RSP_ERR         0xFF

#define CMD_HEADER_LEN  24
#define MAX_CMDBUF      50
#define MAX_PAYLOAD     1468

/* WMX packet header */
typedef __packed struct cmdhdr {

	INT32U  	type;	/*command type*/
    INT32U  	idx;	/*index*/
    INT32S  	sockfd; /*pid for netlink*/
    INT16U  	ctrl;	/*bit mask for control putpose*/
    INT16U  	seq;	/*sequence number, PCI/Ethernet*/
    INT16U  	len;	/*length of following data*/
    INT16U  	size;	/*length of following buffer size, for get method*/
	INT32S 		retval;	/* return status*/
	uchar		data[1440];//data[MAX_PAYLOAD];
} WCMD_t;

/* WMX boot support command */
#define WMAN_GET_STAT        0x0001
#define WMAN_GET_MAC         0x0002
#define WMAN_GET_SYSDEV	     0x0003
#define WMAN_SET_CFG         0x0010
#define WMAN_SET_MAC         0x0011
#define WMAN_SET_FIRMLEN     0x0012
#define WMAN_SET_FIRM        0x0013
#define WMAN_SET_OSUP        0x0014
#define WMAN_SET_CERTI       0x0015
#define WMAN_SET_DEVUP       0x0016
#define WMAN_SET_DEVDOWN     0x0017
#define WMAN_SET_RESET	     0x0018

/* WMX Device state define */
typedef enum
{
        IDLE = 0x80,
        FIRMLENOK,
        FIRMOK
} DEV_ST;

/* WMX Device sstruct define */
typedef struct wmxdev_t
{
	INT32S 	FirmLen;		/* download file len */
	INT32S	CurFirmLen;		/* current download file len */
	INT32S 	TimeoutCnt;		/* download timeout cnt */
	INT8U	*pFirmAddr;		/* boot address */
	INT8U	*pCurFirmAddr;	/* current download file address */
	DEV_ST	dev_stat;		/* device state */
} WmxDev;

/*
 * Maximum packet size; used to allocate packet storage.
 * TFTP packets can be 524 bytes + IP header + ethernet header.
 * Lets be conservative, and go for 38 * 16.  (Must also be
 * a multiple of 32 bytes).
 */
/*
 * AS.HARNOIS : Better to set PKTSIZE to maximum size because
 * traffic type is not always controlled
 * maximum packet size =  1518
 * maximum packet size and multiple of 32 bytes =  1536
 */
#define PKTSIZE			1518
#define PKTSIZE_ALIGN		1536
/*#define PKTSIZE		608*/

/*
 * Maximum receive ring size; that is, the number of packets
 * we can buffer before overflow happens. Basically, this just
 * needs to be enough to prevent a packet being discarded while
 * we are processing the previous one.
 */
#define RINGSZ		4
#define RINGSZ_LOG2	2

/**********************************************************************/
/*
 *	Globals.
 *
 * Note:
 *
 * All variables of type IPaddr_t are stored in NETWORK byte order
 * (big endian).
 */

/* net.c */
/** BOOTP EXTENTIONS **/
extern IPaddr_t		NetOurGatewayIP;	/* Our gateway IP addresse	*/
extern IPaddr_t		NetOurSubnetMask;	/* Our subnet mask (0 = unknown)*/
extern IPaddr_t		NetOurDNSIP;	 /* Our Domain Name Server (0 = unknown)*/
extern INT8S		NetOurNISDomain[32];	/* Our NIS domain		*/
extern INT8S		NetOurHostName[32];	/* Our hostname			*/
extern INT8S		NetOurRootPath[64];	/* Our root path		*/
extern ushort		NetBootFileSize;	/* Our boot file size in blocks	*/
/** END OF BOOTP EXTENTIONS **/
extern ulong		NetBootFileXferSize;	/* size of bootfile in bytes	*/
extern uchar		NetOurEther[6];		/* Our ethernet address		*/
extern uchar		NetServerEther[6];	/* Boot server enet address	*/
extern IPaddr_t		NetOurIP;		/* Our    IP addr (0 = unknown)	*/
extern IPaddr_t		NetServerIP;		/* Server IP addr (0 = unknown)	*/
extern volatile uchar * pNetTxPacket;		/* THE transmit packet		*/
extern volatile uchar * NetRxPackets[PKTBUFSRX];/* Receive packets		*/
extern volatile uchar * pNetRxPkt;		/* Current receive packet	*/
extern INT32S		NetRxPktLen;		/* Current rx packet length	*/
extern unsigned		NetIPID;		/* IP ID (counting)		*/
extern uchar		NetBcastAddr[6];	/* Ethernet boardcast address	*/
extern uchar		NetEtherNullAddr[6];



typedef enum
{
	NETLOOP_NONE,
	NETLOOP_CONTINUE,
	NETLOOP_RESTART,
	NETLOOP_SUCCESS,
	NETLOOP_FAIL
}net_loop_enum;
extern net_loop_enum		NetState;		/* Network loop state		*/

typedef enum { BOOTP, RARP, ARP, TFTP, DHCP, PING, DNS, NFS, CDP, NETCONS, SNTP } proto_t;

/* from net/net.c */
extern INT8S	BootFile[128];			/* Boot File name		*/

#if (CONFIG_COMMANDS & CFG_CMD_PING)
extern IPaddr_t	NetPingIP;			/* the ip address to ping 		*/

typedef struct						/* ARP link list structure */
{
	IPaddr_t 	arp_ip;
	uchar		eth_addr[6];
	uchar		alive_time;
	VOID*		next;
}arp_tab_struct;	
#define ARP_TAB_ALIVE_TIME	30
#endif



/* Initialize the network adapter */
extern INT32S	NetLoop(proto_t);

/* Shutdown adapters and cleanup */
extern VOID	NetStop(VOID);

/* Load failed.	 Start again. */
extern VOID	NetStartAgain(VOID);

/* Get size of the ethernet header when we send */
extern INT32S 	NetEthHdrSize(VOID);

/* Set ethernet header; returns the size of the header */
extern INT32S	NetSetEther(volatile uchar *, uchar *, uint);

/* Set IP header */
extern VOID	NetSetIP(volatile uchar *, IPaddr_t, INT32S, INT32S, INT32S);

/* Checksum */
extern INT32S	NetCksumOk(uchar *, INT32S);	/* Return true if cksum OK	*/
extern uint	NetCksum(uchar *, INT32S);		/* Calculate the checksum	*/

/* Transmit "pNetTxPacket" */
extern VOID	NetSendPacket(volatile uchar *, INT32S);

/* Transmit UDP packet, performing ARP request if needed */
extern INT32S	NetSendUDPPacket(uchar *ether, IPaddr_t dest, INT32S dport, INT32S sport, INT32S len);

/* Processes a received packet */
//extern VOID	NetReceive(volatile uchar *, INT32S);

/* Print an IP address on the console */
extern VOID	print_IPaddr (IPaddr_t);

/*ethernet rcv func*/
VOID ETH_Rx(VOID);

/*
 * The following functions are a bit ugly, but necessary to deal with
 * alignment restrictions on ARM.
 *
 * We're using inline functions, which had the smallest memory
 * footprint in our tests.
 */
/* return IP *in network byteorder* */
static __inline IPaddr_t NetReadIP(VOID *from)
{
	IPaddr_t ip;
	memcpy((VOID*)&ip, from, sizeof(ip));
	return ip;
}

/* return ulong *in network byteorder* */
static __inline ulong NetReadLong(ulong *from)
{
	ulong l;
	memcpy((VOID*)&l, (VOID*)from, sizeof(l));
	return l;
}

/* write IP *in network byteorder* */
static __inline VOID NetWriteIP(VOID *to, IPaddr_t ip)
{
	memcpy(to, (VOID*)&ip, sizeof(ip));
}

/* copy IP */
static __inline VOID NetCopyIP(VOID *to, VOID *from)
{
	memcpy(to, from, sizeof(IPaddr_t));
}

/* copy ulong */
static __inline VOID NetCopyLong(ulong *to, ulong *from)
{
	memcpy((VOID*)to, (VOID*)from, sizeof(ulong));
}

/* Convert an IP address to a string */
extern VOID	ip_to_string (IPaddr_t x, INT8S *s);

/* Convert a string to ip address */
extern IPaddr_t string_to_ip(INT8S *s);

/* read an IP address from a environment variable */
extern IPaddr_t getenv_IPaddr (INT8S *);

/* copy a filename (allow for "..." notation, limit length) */
extern VOID	copy_filename (INT8S *dst, INT8S *src, INT32S size);

extern VOID NetSetHandler(rxhand_f * f);

extern VOID NetSetTimeout(ulong iv, thand_f * f);

extern VOID ARP_Node_Add(IPaddr_t ip, uchar *eth_addr);

extern INT32S ARP_Node_Del(arp_tab_struct *node);

extern VOID ARP_Alive_Check(VOID);

extern INT32S ARP_Addr_Search(IPaddr_t ip, uchar *eth_addr);
/**********************************************************************/

#endif /* __NET_H__ */
