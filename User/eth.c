/*
 * ethernet driver for boot loader, modify from system ethernet driver
 */

#include "loader.h"
#include "net.h"
#include "eth.h"
//#include "ftmac100.h"
//#include "ethphy.h"

/* __RMII__ ==> RMII connect directly */
//#define __RMII__	

#if 0

static FTMAC100 ETH_MacInfo[NIC_NUM] =
{
	{	/*eth 0*/
		0,NIC_BASE_ADDR,INTC_NIC_BIT_INDEX,1,3,0,0,0,1,0,0,	/* receive all mode */
		{0x00, 0x1b, 0xb4, 0x1a, 0x2b, 0x31},			/* default MAC */
		{0x00, 0x0, 0x0, 0x0, 0x0, 0x0}					/* clone MAC  */
	}
};

#endif


#if 0
#define PKT_BUF_NUM	FTMAC100_TX_DESCRIPTOR_NUMBER+FTMAC100_RX_DESCRIPTOR_NUMBER
static INT8U pktbuf[PKT_BUF_NUM][2048];
#endif


#define	NET_BUF_NUM	10

struct net_buf
{
	INT32S len;
	INT8U buf[2044];
};

static struct net_buf netbuf[NET_BUF_NUM];
static INT32S net_put, net_get, net_full;

VOID print_buff(INT8S *buf, INT32S len, INT8S type);		


extern VOID Disable(void);
extern VOID Enable(void);



#if 0
__align(16) FTMAC100TxDescriptor TxDesc[FTMAC100_TX_DESCRIPTOR_NUMBER];
__align(16) FTMAC100RxDescriptor RxDesc[FTMAC100_RX_DESCRIPTOR_NUMBER];

static VOID set_mac_addr(FTMAC100 *mac, INT8U addr[6]);
static VOID set_mac_cr(FTMAC100 *mac, INT32U item, INT32U flag);
static INT32S phy_stats(FTMAC100 *mac);
static INT16U phy_read(FTMAC100 *mac,INT32U bRegister);
static INT16U phy_write(FTMAC100 *mac,INT32U bRegister,INT16U bValue);
static INT32S reset_phy(FTMAC100 *mac);
static INT32U ResourceInit(FTMAC100 *mac);
#endif

//VOID NetReceive(volatile INT8U * inpkt, INT32S len);



#if 0

/*
* Ethernet driver initial function
*/
INT32S ETH_Init()
{
	FTMAC100  *mac = &ETH_MacInfo[0];
	INT32U mac_ctrl;
	INT32S retval=0,count;
	
	set_mac_cr(mac,FTMAC100_REG_MACCR_SW_RST,1);
    count = 0;
	while(count<3)
	{
		retval = HAL_GETREG32(mac->regbase + FTMAC100_REG_MAC_CONTROL);
		if ((retval & FTMAC100_REG_MACCR_SW_RST)==0)
			break;
		Delay(10);
		count++;
	}	/*end of while*/
	if (count>=3)
	{
		printf( "Reset MAC fail\n");
		retval = -1;
		goto eth_end;
	}
		
	if (ResourceInit(mac)!=0)
	{	
		retval = -2;
		goto eth_end;
	}
	/*set up mac address*/	
   	set_mac_addr(mac, mac->mac_addr);

	//3.5 Set Interrupt Timer Control Register 
#ifdef  FTMAC100_USE_RECOMMAND_SETTING    
    HAL_SETREG32(mac->regbase+FTMAC100_REG_INT_TIMER_CONTROL,0x00001010); 
#else  
    HAL_SETREG32(mac->regbase+FTMAC100_REG_INT_TIMER_CONTROL,0); 
#endif          
    //3.6 Set Auto Polling Timer Control Register
#ifdef  FTMAC100_USE_RECOMMAND_SETTING 
    HAL_SETREG32(mac->regbase + FTMAC100_REG_AUTO_INT_TIMER_CONTROL,0x00000001); 
#else        
    HAL_SETREG32(mac->regbase + FTMAC100_REG_AUTO_INT_TIMER_CONTROL,0); 
#endif          
	//3.7 Set MAC control register
    HAL_SETREG32(mac->regbase + FTMAC100_REG_MAC_CONTROL,0xE003); 

    //3.8 Set Burst Length Default
    //HAL_SETREG32(mac->regbase + FTMAC100_REG_DMA_CONTROL,0x04); 
    	
#ifdef  FTMAC100_USE_RECOMMAND_SETTING        
    HAL_SETREG32(mac->regbase + FTMAC100_REG_BURST_LENGTH_ARBITRATION, 0x00000397);
#endif

	/*Set MAC Control Register */
	mac_ctrl =  FTMAC100_REG_MACCR_XDMA_EN +
	            FTMAC100_REG_MACCR_RDMA_EN +
	            FTMAC100_REG_MACCR_XMT_EN +
	            FTMAC100_REG_MACCR_ENRX_IN_HALFTX +
	            FTMAC100_REG_MACCR_RCV_EN +
	         //   FTMAC100_REG_MACCR_HT_MULTI_EN+
	            FTMAC100_REG_MACCR_CRC_APD+
	            FTMAC100_REG_MACCR_FULLDUP+
	            FTMAC100_REG_MACCR_RX_MULTIPKT+
	            FTMAC100_REG_MACCR_RX_BROADPKT +
	            FTMAC100_REG_MACCR_RX_SPEED_100;
		          
	if (mac->mode)
	    mac_ctrl |= FTMAC100_REG_MACCR_RCV_ALL;
	else
	    mac_ctrl &= ~FTMAC100_REG_MACCR_RCV_ALL;
		    
	/*Set up receive broadcast & maulticast address, full duplex mode*/
	/*TX/RX enable && TX/RX DMA enable, not receive all*/
	HAL_SETREG32(mac->regbase + FTMAC100_REG_MAC_CONTROL,mac_ctrl); 
	/*enable interrupt*/
	HAL_SETREG32(mac->regbase + FTMAC100_REG_INTMASK,FTMA100_ISR_MASK); 
	/*enable interrupt, Edge trigger ??*/
	INTC_INTERRUPT_MASK_REG |= (1<<mac->irq);
	
#ifndef __RMII__
	/*reset PHY*/
    if ((retval = reset_phy(mac))!= 0)
    {
		printf( "Reset ETH PHY fail\n");
   	}
   	else
#endif
		printf( "Ethernet initial ok \n");
eth_end:
	return retval;
}	/*end of MAC init*/

#endif


#if 1

/*
* Ethernet driver output function
*/
INT32S eth_tx(INT8U *buf, INT32S size)
{

}

#endif


#if 1

/*do ethernet receive function, call from ETH_Isr*/
VOID ETH_Rcv(INT8U *buf, INT32S len)
{
	/*get a free packet buffer*/
	if (net_full)
		return;
	/*copy the data to net buffer*/
	memcpy(netbuf[net_put].buf,buf, len);
	netbuf[net_put].len = len;
	net_put++;
	if (net_put>=NET_BUF_NUM)
		net_put=0;
	if (net_put == net_get)
		net_full = 1;
	/*enable ethernt rx*/
	//sys_event |= ETH_EVENT;	// roger 20160218 mark
}	/*end of eth rcv*/

#endif


/*call from main loop to process all received packet*/
VOID ETH_Rx(VOID)
{
	//INT32U cpusr;

	while(1)
	{
		//Disable(&cpusr);		// Roger 20150707 mark
		Disable();
		if (net_full==0 && net_put == net_get)
		{
			//Enable(cpusr);   // Roger 20150707 mark
			Enable();
			break;
		}
		//Enable(cpusr);
		Enable();
		/*process the received packet*/
	//	NetReceive(netbuf[net_get].buf,netbuf[net_get].len);	
		
		//Disable(&cpusr);
		Disable();
		++net_get;
		if (net_get>=NET_BUF_NUM)
			net_get = 0;
		net_full = 0;
		//Enable(cpusr);	
		Enable();	
	}	/*end of while*/
}



#if 0

/*
* Ethernet driver output function
*/
INT32S ETH_Isr()
{
	FTMAC100  *mac = &ETH_MacInfo[0];
	INT32U stat, len;
	INT8U *buf;
	FTMAC100RxDescriptor  *rx_desc; 
	
	//1.Mask the InterruptMask
	HAL_SETREG32(mac->regbase + FTMAC100_REG_INTMASK, 0); 
      
	//2.Read the Interrupt register
    stat=HAL_GETREG32(mac->regbase + FTMAC100_REG_INTSTATUS);
    if((stat & FTMA100_ISR_MASK) == 0)
    	goto end_of_eth_isr;
    
    //Check RX Interrupt event 
	if (stat & FTMAC100_ISR_RX_RPKT_FINISH )
	{
		while(1)
    	{
    		INT32U status;
    		
   			rx_desc = mac->rx_desc_pool_dma + mac->rx_remove_idx ;
       		
	    	if (rx_desc->RXDES0.RXDMA_OWN)
    			break ;
      	  	
      	  	status = *((INT32U *)&rx_desc->RXDES0);
  			if ((status & DISCARD_PACKET)==0)
      		{
    			if ((rx_desc->RXDES0.FRS==1) && (rx_desc->RXDES0.LRS==1))
    			{
        			//One packet = 1 frame , no fragement         	
          			len = rx_desc->RXDES0.ReceiveFrameLength;
          			buf = rx_desc->Buf;
          			ETH_Rcv(buf+2, len);	//franky add unknow why???
   	   			}	/*end if */
   	 		}	/*end if rx descriptor process*/
			mac->rx_remove_idx++ ;      	        
	 		if (mac->rx_remove_idx == FTMAC100_RX_DESCRIPTOR_NUMBER)
   				mac->rx_remove_idx =0;
			//Set the RXDMA_OWN bit 
   			rx_desc->RXDES0.RXDMA_OWN =1 ; 
		}	/*end of while*/
	}	/*end of RX process*/
	
//#ifndef __RMII__	
	/*PHY status change*/	    
	if (stat & FTMAC100_ISR_ALL_PHYSTS_CHG) 
	{
		phy_stats(mac);
    }	/*end of link status process*/
//#endif 
    	
end_of_eth_isr:
	/*enable interrupt*/
    HAL_SETREG32(mac->regbase + FTMAC100_REG_INTMASK,FTMA100_ISR_MASK); 
	return 0;
}	/*end eth isr*/

#endif



#if 0

static INT32U ResourceInit(FTMAC100 *mac)
{
	INT32S i,retval=0,count=0;
	
	/*allocate RX descriptor*/
	mac->rx_desc_pool_dma = RxDesc ;
	/*initial rx descriptor*/
    memset((INT8U *)mac->rx_desc_pool_dma, 0x0, FTMAC100_RX_DESCRIPTOR_NUMBER*(sizeof(FTMAC100RxDescriptor)));
	
	for (i = 0; i < FTMAC100_RX_DESCRIPTOR_NUMBER; i++)
	{
    	mac->rx_desc_pool_dma[i].Next = mac->rx_desc_pool_dma+(i+1) ;
		mac->rx_desc_pool_dma[i].Buf = pktbuf[count++];
		mac->rx_desc_pool_dma[i].RXDES1.RXBUFSize=FTMAC100_RX_PREPARE_BUF_SIZE;
    	mac->rx_desc_pool_dma[i].RXDES0.RXDMA_OWN=1;		//Set the DMA_OWN   
	}
 	mac->rx_desc_pool_dma[FTMAC100_RX_DESCRIPTOR_NUMBER -1].Next = mac->rx_desc_pool_dma ;
	(mac->rx_desc_pool_dma + (FTMAC100_RX_DESCRIPTOR_NUMBER - 1))->RXDES1.EDOTR=1;
	
	/*allocate TX descriptor*/
	mac->tx_desc_pool_dma = TxDesc;
	/*initial tx descriptor*/
    memset((INT8U *)mac->tx_desc_pool_dma, 0x0, FTMAC100_TX_DESCRIPTOR_NUMBER*(sizeof(FTMAC100TxDescriptor)));
	
	for (i = 0; i < FTMAC100_TX_DESCRIPTOR_NUMBER; i++)
	{
    	mac->tx_desc_pool_dma[i].Next = mac->tx_desc_pool_dma+(i+1) ;  
		mac->tx_desc_pool_dma[i].Buf = pktbuf[count++];
	}
    mac->tx_desc_pool_dma[FTMAC100_TX_DESCRIPTOR_NUMBER -1].Next = mac->tx_desc_pool_dma ;
    (mac->tx_desc_pool_dma + (FTMAC100_TX_DESCRIPTOR_NUMBER -1))->TXDES1.EDOTR=1;
	
	/*setup MAC control register*/
	HAL_SETREG32(mac->regbase + FTMAC100_REG_TX_BASE_ADDRESS,
    		(INT32U)mac->tx_desc_pool_dma);  //TX
    HAL_SETREG32(mac->regbase + FTMAC100_REG_RX_BASE_ADDRESS,
    		(INT32U)mac->rx_desc_pool_dma);  //RX
    		
	return retval;
}	/*end of NIC memory init*/

#endif


#if 0

static VOID set_mac_addr(FTMAC100 *mac, INT8U addr[6])
{
	INT32U  val =0;
#if __BYTE_ORDER == __BIG_ENDIAN	 //#ifdef __BIG_ENDIAN	#ifdef __BIG_ENDIAN
    val = (addr[1] << 8 ) | addr[0] ;
	HAL_SETREG32(mac->regbase+FTMAC100_REG_MAC_ADDRESS_H,val); 
	
	val = (addr[5] << 24) | (addr[4] << 16) | (addr[3] << 8) | addr[2] ;
	HAL_SETREG32(mac->regbase+FTMAC100_REG_MAC_ADDRESS_L,val); 
#else	
	val = (addr[0] << 8 ) | addr[1] ;
	HAL_SETREG32(mac->regbase+FTMAC100_REG_MAC_ADDRESS_H,val); 
	
	val = (addr[2] << 24) | (addr[3] << 16) | (addr[4] << 8) | addr[5] ;
	HAL_SETREG32(mac->regbase+FTMAC100_REG_MAC_ADDRESS_L,val); 
#endif
}

#endif


#if 0

static VOID set_mac_cr(FTMAC100 *mac, INT32U item, INT32U flag)
{
	INT32U  val;

    val = HAL_GETREG32(mac->regbase + FTMAC100_REG_MAC_CONTROL);
    if (flag)
    	val |= item;
    else
    	val &= ~item;
    HAL_SETREG32(mac->regbase + FTMAC100_REG_MAC_CONTROL,val); 
}

#endif


#if 0

static INT16U phy_write(FTMAC100 *mac,INT32U bRegister,INT16U bValue)
{
  	INT32U dwTemp, wResult;
  	INT32U base = mac->regbase;
	INT32U bPHYAddress = mac->phy_addr ;
  
  	bPHYAddress&=0x1F;
  	dwTemp=(bPHYAddress<<16);
  
  	bRegister&=0x1F;  
  	dwTemp|=bRegister<<(16+5);
  	dwTemp|=FTMAC100_REG_PHY_WRITE;

  	//1.Write PHY Write_Data registetr 
  	HAL_SETREG16(base + FTMAC100_REG_PHY_WRITE_DATA, bValue);
  	//2.Write PHY Control	
  	HAL_SETREG32(base + FTMAC100_REG_PHY_CONTROL, dwTemp);
   	do {
      	wResult=HAL_GETREG32(base + FTMAC100_REG_PHY_CONTROL);
      
    }while((wResult&FTMAC100_REG_PHY_WRITE)>0);
    
	return (1);	 
}

#endif


#if 0

static INT16U phy_read(FTMAC100 *mac,INT32U bRegister)
{
  	INT32U wResult, dwTemp, bPHYAddress = mac->phy_addr ;
	INT32U base = mac->regbase;
	
  	bPHYAddress&=0x1F;
  	dwTemp=(bPHYAddress<<16);
  
  	bRegister&=0x1F;
  	dwTemp|=bRegister<<(16+5);
  	dwTemp|=FTMAC100_REG_PHY_READ;

	
  	HAL_SETREG32(base + FTMAC100_REG_PHY_CONTROL, dwTemp);
  	do 
  	{
      	wResult=HAL_GETREG32(base + FTMAC100_REG_PHY_CONTROL);
	}while((wResult & FTMAC100_REG_PHY_READ)>0);
	
	return (INT16U)(wResult);	
}

#endif


#if 0

static INT32S reset_phy(FTMAC100 *mac)
{
 	INT32U wValue;
 	INT32S i;
 	/*phy probe*/
 	for(i=0;i<8;i++)
 	{
 		mac->phy_addr = i;
    	mac->phy_id0 = phy_read(mac, PHYADDR_PHY_ID0);
 		if (mac->phy_id0 != 0xFFFF)
 			break;
	}
	if (mac->phy_id0 == 0xFFFF)
	{
		printf("Not found Ethernet PHY\n");
		return -1;		
	}	
   	mac->phy_id1 = phy_read(mac, PHYADDR_PHY_ID1);
   	
 	/*Read PHY control register*/
  	wValue=phy_read(mac, PHYADDR_CTRL);
 	/*Turn on Reset bits*/
  	wValue|= REG0_RESET;
 	/*Write back to PHY*/
  	phy_write(mac, PHYADDR_CTRL,wValue);
 	/*Waiting for reset complete*/
   	for(i=0;i<3;i++)
   	{
      	Delay(10);
      
		wValue=phy_read(mac, PHYADDR_CTRL);   
		if ((wValue& REG0_RESET)==0)
			break;
    }	/*end for loop*/
	return (i==3?-1:0);
}	/*end of PHY Reset*/

#endif


#if 0

/*PHY status*/
static INT32S phy_stats(FTMAC100 *mac)
{
	INT16U val;
	/*Gte phy status*/
	val=phy_read(mac,PHYADDR_STATUS);
	
	if (val&REG1_LINK_STATUS)
	{
		if (mac->link==0)
		{	
			//2.Read status and set the MAC
    		val=phy_read(mac, PHYADDR_CTRL);
    		//Set the MAC-->Half/Full
   			if ((val&REG0_DUPLEX_FULL)>0)
    		{
       			set_mac_cr(mac, FTMAC100_REG_MACCR_FULLDUP, 1);//Set to Full
       			mac->speed |= (1 << DUPLEX);       	
    		}
    		else
    		{
    			set_mac_cr(mac, FTMAC100_REG_MACCR_FULLDUP, 0);//Set to Half
      			mac->speed &= ~(1 << DUPLEX);
    		}
    
			//Set the MAC-->10/100
    		if ((val&REG0_SPD_100MBPS)>0)
    		{
    			set_mac_cr(mac, FTMAC100_REG_MACCR_MDC_SEL, 1);//Set to 100
		   		mac->speed |= (1 << SPEED);
   			}
    		else
    		{
       			set_mac_cr(mac, FTMAC100_REG_MACCR_MDC_SEL, 0);//Set to 10
       			mac->speed &= ~(1 << SPEED);
   			}
			mac->link = 1;
		}
	}
	else
	{
		if (mac->link==1)
		{
			mac->link = 0;
		}	
	}	
	return mac->link;
}	/*end of get phy status*/

#endif


#if 1

VOID print_buff(INT8S *buf, INT32S len, INT8S type)		//print Tx Rx buffer info
{
#if (PRINT_PACKET == 1)
	INT32S i=0;
	INT8S *pkt = buf;

	if(type == 0)
		printf("*** Tx ***");
	else
		printf("=== Rx ===");

	while(i<len)
	{
		if(i%8 == 0)
			printf("  ");
		if(i%16 == 0)
			printf("\n");
		printf("%02x ", *pkt);
		pkt++;
		i++;
	}
	printf("\n");
#endif
}

#endif


