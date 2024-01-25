/***************************************************************************/
/* Module Name: eth.h                                                      */
/*                                                                         */
/* Description: Ethernet driver, the ethernet chipset is FMAC100     	   */
/*                                                                 		   */
/* History:                                                                */
/*                                                                         */
/*        Kenny create first version ver. 0.1                   06/18/2007 */
/*                                                                         */
/***************************************************************************/
#ifndef _ETH_INC_
#define _ETH_INC_

//#if (ETH0_EN == 1)
//#define MAC_NUM      		1
//#endif
//#if (ETH1_EN == 1)
//#undef MAC_NUM
//#define MAC_NUM      		2
//#endif

#ifndef MAC_NUM
#define MAC_NUM      		1
#endif


#define ETH_PROMISCUS		0x01
#define ETH_MULTICAST		0x02
#define ETH_LOOPBACK		0x04
#define ETH_INT_STATUS		0xfe00

#define PKT_BUFFER_SIZE		1536

#define ETH_ALEN		6			/* Ethernet address length*/
#define ETH_HLEN		14		    /* Total octets in header.	 */
#define ETH_ZLEN		60		    /* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */

#define DUPLEX			0
#define SPEED			1

#define ETH_GET_MAC_ADDR	0x2000
#define ETH_GET_CLONE_ADDR	0x2001
#define ETH_GET_CURR_ADDR	0x2002
#define ETH_GET_PHY_TYPE	0x2003
#define ETH_GET_PHY_SPEED	0x2004
#define ETH_GET_PHY_MODE	0x2005
#define ETH_GET_PHY_REG     0x2006
#define ETH_SET_PHY_REG     0x2007
#define ETH_GET_PHY_ADDR	0x2008
#define ETH_SET_MAC_ADDR	0x2009
#define ETH_SET_CLONE_ADDR	0x2010
#define ETH_SET_MULTI_ADDR	0x2011
#define ETH_SET_PHY_TYPE	0x2012
#define ETH_SET_PHY_SPEED	0x2013
#define ETH_SET_PHY_MODE	0x2014
#define ETH_SET_LOOP_MODE	0x2015
#define ETH_AUTO_NEGO    	0x2016
#define ETH_SET_PAUSE   0x2017

typedef struct _eth_
{
	INT16U	port;			/* eth0, eth1, eth2, eth3 */
	INT32U	io_base;		/* for base address of MAC*/
	INT8U	irq;			/* interrupt number of MAC*/
	INT8U	mode;			/* promiscus mode, broadcast, multicast, DRIBBLE*/
								/* 0 : normal mode  */
								/* 1 : promiscus mode */
								/* 2 : Multicate enable */
	INT32U	speed;			/* auto, 100M, 10M, 11M, (802.11b), 22M (802.11g), 54M (802.11a), 75M (WMAN)*/
	INT8U	data_len;		/* max packet length 1518/1522/1534/1537*/
	INT16U	phy_id;			/* PHY ID*/
	INT16U	phy_addr;		/* Phy address */
	INT16U	link;			/* add by kenny for PHY Link state */
	INT16U	count;			/* add by kenny for PHY Link down count state */
	INT8U	mac_addr[ETH_ALEN];		/* MAC address */
	INT8U	clone_addr[ETH_ALEN];	/* Clone MAC address */
	/*for PCI ETH device*/
	INT32U	mem_base;
	INT32U	handle;
#if (QT2410E == 1)
	TAILQ_HEAD(, NET_BUF) txq;
	INT16U	tx_num;
	INT8U	tx_flag;			/* in transmitting */
#endif    
} ETH_MAC_INFO;

/*Ethernet statistics */
struct ETH_STAT
{
    INT32U  tx_bcnt;    /* Transmission byte counter*/
    INT32U  rx_bcnt;    /* receive vyte counter */
    INT32U  tx_pcnt;    /* Transmission packet counter */
    INT32U  rx_pcnt;    /* receive packet counter */
    INT32U  tx_err;     /* transmit error */
    INT32U  rx_err;     /* receive error */
    INT16U	rx_dropped;	/* no buffer drop*/
    INT16U  tx_full;    /* TX buffer full */
    INT16U  collision;  /* TX collision */
    INT16U  under_run;  /* TX under run */
    INT16U  rx_full;    /* RX buffer full */
    INT16U  phy_err;    /* RX PHY error */
    INT16U  crc_err;    /* RX crc error */
    INT16U  runt;       /* RX runt packet*/
    INT16U  long_pkt;   /* RX long packet */
    INT16U  over_len;   /* RX over buffer length */
    INT16U  dribble;    /* RX dribble packet */
};

extern INT8U DBG_Eth;
extern int eth_tx(unsigned char *buf, int size);
extern void print_buff(char *buf, int len, char type);
extern VOID ETH_Rx(VOID);


#endif /*_ETH_INC_*/
