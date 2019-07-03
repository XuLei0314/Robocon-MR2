/********************************************************************/
#include "can.h"



#define Motor_1_RevID 0x201
#define Motor_2_RevID 0x202
#define Motor_3_RevID 0x203
#define Motor_4_RevID 0x204


#define Motor_5_RevID 0x205
#define Motor_6_RevID 0x206
#define Motor_7_RevID 0x207
#define Motor_8_RevID 0x208


/*******************************************************************/
//全局变量声明/引用处
volatile unsigned char Can_Flag;               //CAN发送数据完成flag 标志
extern volatile int Motor_1_Speed;
extern volatile long Motor_1_Angle;
extern moto_measure_t moto_chassis[4]; 
/*******************************************************************
作者：
描述：CAN 初始化、CAN波特率=PCLK1/(BRP+1)*(TS1+TS2+3)
********************************************************************/
void Init_CAN1(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
    CAN_InitStructure.CAN_Prescaler = 5;
    CAN_Init(CAN1, &CAN_InitStructure);

    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    //return 0;
//    NVIC_InitTypeDef NVIC_InitStructure;
//    GPIO_InitTypeDef GPIO_InitStructure;
//    CAN_InitTypeDef        CAN_InitStructure;
//    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
//		GPIO_StructInit(&GPIO_InitStructure);
//    /* Configure CAN pin: RX    PA11 */
//		GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1);//PA11 ??? CAN1
//		GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); //PA12 ??? CAN1
//	
//	
//		GPIO_InitStructure.GPIO_Pin = CAN1_TX_PIN|CAN1_RX_PIN;
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
//	
//	
//		GPIO_Init(GPIOD, &GPIO_InitStructure);
//	


//  /* CAN register init */
//		CAN_DeInit(CAN1);
//		CAN_StructInit(&CAN_InitStructure);

//  /* CAN cell init */
//		CAN_InitStructure.CAN_TTCM=DISABLE;         //禁止时间触发通信模式
//		CAN_InitStructure.CAN_ABOM=DISABLE;
//		CAN_InitStructure.CAN_AWUM=DISABLE;         //睡眠模式通过清除sleep位来唤醒
//		CAN_InitStructure.CAN_NART=DISABLE;         //ENABLE;报文自动重传
//		CAN_InitStructure.CAN_RFLM=DISABLE;         //接收溢出时，FIFO未锁定
//		CAN_InitStructure.CAN_TXFP=ENABLE;         //发送的优先级由标示符的大小决定
//		CAN_InitStructure.CAN_Mode=CAN_Mode_Normal; //CAN_Mode_LoopBack;//
//		//CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;
//		CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;     //SJW=0        //波特率计算，PCLK1=36MHZ,CAN波特率=36MHZ/(2+1)*(15+6+3)=500Kbs
//		CAN_InitStructure.CAN_BS1=CAN_BS1_3tq;    //TS1=15        //波特率计算，PCLK1=36MHZ,CAN波特率=PCLK1/(BRP+1)*(TS1+TS2+3)
//		CAN_InitStructure.CAN_BS2=CAN_BS2_5tq;     //TS2=6   
//		CAN_InitStructure.CAN_Prescaler=5;         //APB1时钟频率/Psc/1+BS1+BS2  42/6/7 = 1Mhz
//		CAN_Init(CAN1,&CAN_InitStructure);

//  /* CAN filter init */
//		CAN_FilterInitStructure.CAN_FilterNumber=0;                               //第1组过滤器
//		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;             //屏蔽位模式
//		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;            //32位
//		CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;                          
//		CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
//		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
//		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
//		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;                        //第一组过滤器指向FIFO0
//		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
//		CAN_FilterInit(&CAN_FilterInitStructure);

//		/* Enable CAN RX0 interrupt IRQ channel*/
//		                     //中断优先级组选择0组使用 
//	  CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE); 
//		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;            //USB_HP_CAN1_TX_IRQn     接收中断使能
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;             //抢占优先级
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;                    //响应优先级
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);
		
//			NVIC_InitStructure.NVIC_IRQChannel = CAN1_TX_IRQn;            //USB_HP_CAN1_TX_IRQn     接收中断使能
//		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;             //抢占优先级
//		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;                    //响应优先级
//		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//		NVIC_Init(&NVIC_InitStructure);

		/* CAN FIFO0 message pending interrupt enable */ 
		   
//		CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE); 		//使能过滤器中断
  
}


/*******************************************************************
作者：
描述：CAN 发送 一标准帧数据
********************************************************************/
u8 CAN_Send(s16 speed)    //CAN 发送 一标准帧数据
{
	u16 i = 0;
	CanTxMsg TxMessage;
	//  CanRxMsg RxMessage;
	unsigned char TransmitMailbox;
	/* transmit 1 message */
	TxMessage.StdId=0x200;          //SID;//0x00;       ID标示符
	//  TxMessage.ExtId=0;
	TxMessage.IDE=CAN_ID_STD;         //CAN_ID_EXT;//        //选择标准帧
	TxMessage.RTR=CAN_RTR_DATA;       //选择数据帧
	TxMessage.DLC=0x08;
//  TxMessage.Data[0]=data[0];           //data1;
//  TxMessage.Data[1]=data[1];           //data2;
//  TxMessage.Data[2]=data[2];           //data3;
//  TxMessage.Data[3]=data[3];           //data4;
//  TxMessage.Data[4]=data[4];           //data5;
//  TxMessage.Data[5]=data[5];           //data6;
//  TxMessage.Data[6]=data[6];           //data7;
//  TxMessage.Data[7]=data[7];           //data8;
	TxMessage.Data[0]=speed>>8;           //data1;
	TxMessage.Data[1]=speed;           //data2;
  
	TxMessage.Data[2]=speed>>8;            //data3;
	TxMessage.Data[3]=speed;           //data4;
	TxMessage.Data[4]=speed>>8;            //data5;
	TxMessage.Data[5]=speed;          //data6;
	TxMessage.Data[6]=speed>>8;            //data7;
	TxMessage.Data[7]=speed;          //data8;
	TransmitMailbox = CAN_Transmit(CAN1,&TxMessage);
	i=0;
	while(CAN_TransmitStatus(CAN1,TransmitMailbox) != CANTXOK&&(i<0XFFF))i++;
	if(i>=0XFFF)return 1;
	return 0;	
  /* receive*/ 
//  RxMessage.StdId=0x00;
//  RxMessage.IDE=CAN_ID_STD;
//  RxMessage.DLC=0;
//  RxMessage.Data[0]=0x00;
//  RxMessage.Data[1]=0x00;
//  CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);
  
  
}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_RX0_IRQHandler(void)//CAN      中断接收程序  
{
  CanRxMsg RxMessage;

  RxMessage.StdId=0x00;
  RxMessage.ExtId=0x00;
  RxMessage.IDE=0;
  RxMessage.DLC=0;
  RxMessage.FMI=0;
  RxMessage.Data[0]=0x00;
  RxMessage.Data[1]=0x00;

  CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);
	//RED_ON;
	//printf("Receivecan!\n");
	switch(RxMessage.StdId)
	{
		case Motor_1_RevID:
		case Motor_2_RevID:
		case Motor_3_RevID:
		case Motor_4_RevID:
		case Motor_5_RevID:
		case Motor_6_RevID:
		case Motor_7_RevID:
		case Motor_8_RevID:
		{
				
				static u8 i;
					//printf("Receive!\n");
				 i = RxMessage.StdId - Motor_1_RevID;
				 moto_chassis[i].msg_cnt++ <= 50 ? get_moto_offset(&moto_chassis[i], &RxMessage) : get_moto_measure(&moto_chassis[i], &RxMessage);
              //printf("Speed is %d \n",moto_chassis[0].speed_rpm);
				 //Motor_1_Angle = (RxMessage.Data[0]<<8)|RxMessage.Data[1];
		}
				 
		break;
		
		default:
				 break;
	}
/*
  if((RxMessage.ExtId==0x1234) && (RxMessage.IDE==CAN_ID_EXT)
     && (RxMessage.DLC==2) && ((RxMessage.Data[1]|RxMessage.Data[0]<<8)==0xDECA))
  {
    ret = 1; 
  }
  else
  {
    ret = 0; 
  }
*/
}
/**********************************************************************************/

u8 Set_Motor_Speed_Can(s16 s1,s16 s2,s16 s3,s16 s4,s16 s5,s16 s6, s16 s7,s16 s8)
{
	u16 i = 0;
	CanTxMsg TxMessage;

	unsigned char TransmitMailbox;
	/* transmit 1 message */
	TxMessage.StdId=0x200;          //SID;//0x00;       ID标示符
	TxMessage.IDE=CAN_ID_STD;         //CAN_ID_EXT;//        //选择标准帧
	TxMessage.RTR=CAN_RTR_DATA;       //选择数据帧
	TxMessage.DLC=0x08;

	TxMessage.Data[0]=s1>>8;           //data1;
	TxMessage.Data[1]=s1;           //data2;
  
	TxMessage.Data[2]=s2>>8;            //data3;
	TxMessage.Data[3]=s2;           //data4;
	TxMessage.Data[4]=s3>>8;            //data5;
	TxMessage.Data[5]=s3;          //data6;
	TxMessage.Data[6]=s4>>8;            //data7;
	TxMessage.Data[7]=s4;          //data8;
	TransmitMailbox = CAN_Transmit(CAN1,&TxMessage);
	i=0;
	while(CAN_TransmitStatus(CAN1,TransmitMailbox) != CANTXOK&&(i<0XFFF))i++;
	if(i>=0XFFF)return 1;
	
	TxMessage.StdId=0x1FF;          //SID;//0x00;       ID标示符
	TxMessage.IDE=CAN_ID_STD;         //CAN_ID_EXT;//        //选择标准帧
	TxMessage.RTR=CAN_RTR_DATA;       //选择数据帧
	TxMessage.DLC=0x08;

	TxMessage.Data[0]=s5>>8;           //data1;
	TxMessage.Data[1]=s5;           //data2;
  
	TxMessage.Data[2]=s6>>8;            //data3;
	TxMessage.Data[3]=s6;           //data4;
	TxMessage.Data[4]=s7>>8;            //data5;
	TxMessage.Data[5]=s7;          //data6;
	TxMessage.Data[6]=s8>>8;            //data7;
	TxMessage.Data[7]=s8;          //data8;
	TransmitMailbox = CAN_Transmit(CAN1,&TxMessage);
	i=0;
	while(CAN_TransmitStatus(CAN1,TransmitMailbox) != CANTXOK&&(i<0XFFF))i++;
	if(i>=0XFFF)return 1;
	
	
	
	return 0;
}

void get_moto_measure(moto_measure_t* ptr, CanRxMsg* hcan)
{
    ptr->speed_rpm     = (int16_t)(hcan->Data[2] << 8 | hcan->Data[3]);
   
    ptr->given_current = (int16_t)(hcan->Data[4] << 8 | hcan->Data[5]) / -5;
    ptr->hall          = hcan->Data[6];
	
	  ptr->last_angle    = ptr->angle;
    ptr->angle         = (uint16_t)(hcan->Data[0] << 8 | hcan->Data[1]);
	
    if (ptr->angle - ptr->last_angle > 4096)
		{
			ptr->round_cnt--;
			//ptr->ecd_raw_rate = ptr->angle - ptr->last_angle - 8192;
		}
    else if (ptr->angle - ptr->last_angle < -4096)
		{
      ptr->round_cnt++;
			//ptr->ecd_raw_rate = ptr->angle - ptr->last_angle + 8192;
		}
		else
		{
			//ptr->ecd_raw_rate = ptr->angle - ptr->last_angle;
		}
		//total encoder value
    ptr->total_ecd = ptr->round_cnt * 8192 + ptr->angle - ptr->offset_angle;
	  //total angle/degree
		ptr->total_angle = ptr->total_ecd * 360 / 8192;
}

void get_moto_offset(moto_measure_t* ptr, CanRxMsg* hcan)
{
    ptr->angle        = (uint16_t)(hcan->Data[0] << 8 | hcan->Data[1]);
    ptr->offset_angle = ptr->angle;
}



