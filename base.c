#include "main.h"
#include "string.h"
#include <math.h>
#include "FIFO.h"
extern u8 SV1_State;
extern u8 SV2_State;
extern u8 SV3_State;
extern u8 SV4_State;
extern u8 SV5_State;

extern moto_measure_t moto_chassis[4]; 
unsigned char dbus_buf[DBUS_BUF_SIZE];
extern int rc_get;
DBUS dbus;
//uint8_t auto_send[50];

void Pause_Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;   //Pause PA0   Board PA1
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_StructInit(&GPIO_InitStructure);	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
}
u8 LED_KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_StructInit(&GPIO_InitStructure);	
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;					// Red LED
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;					// Green LED
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOF, &GPIO_InitStructure);		
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;						// Linear LED 1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOG, &GPIO_InitStructure);	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	
	RED_OFF;
	GREEN_OFF;
	LED_OFF;
	return 0;
}


void DBUS_Enc(const DBUS* pdbus,unsigned char* pbuf)
{
    pbuf[0] = pdbus->rc.ch0&0xff;
    pbuf[1] = (pdbus->rc.ch1<<3) | (pdbus->rc.ch0>>8);
    pbuf[2] = (pdbus->rc.ch2<<6) | (pdbus->rc.ch1>>5);
    pbuf[3] = pdbus->rc.ch2;
    pbuf[4] = (pdbus->rc.ch3<<1) | (pdbus->rc.ch2>>10);
    pbuf[5] = (pdbus->rc.s2<<6) | (pdbus->rc.s1<<4) | (pdbus->rc.ch3>>7);
    pbuf[6] = pdbus->mouse.x;
    pbuf[7] = pdbus->mouse.x>>8;
    pbuf[8] = pdbus->mouse.y;
    pbuf[9] = pdbus->mouse.y>>8;
    pbuf[10] = pdbus->mouse.z;
    pbuf[11] = pdbus->mouse.z>>8;
    pbuf[12] = pdbus->mouse.l;
    pbuf[13] = pdbus->mouse.r;
    pbuf[14] = pdbus->key.v;
    pbuf[15] = pdbus->key.v>>8;
    pbuf[16] = pdbus->res;
    pbuf[17] = pdbus->res>>8;
}


void DBUS_Dec(DBUS* pdbus,const unsigned char* pbuf)
{
	if(!rc_get && ((pbuf[16] | (pbuf[17] << 8))-1024) == 0 )
	{
		rc_get = 1;
		//RED_ON;
		printf("rc_get!\r\n");
	}
	
    pdbus->rc.ch0 = (pbuf[0] | (pbuf[1] << 8)) & 0x07ff;          //!< Channel 0  
		pdbus->rc.ch0-=1024;
    pdbus->rc.ch1 = ((pbuf[1] >> 3) | (pbuf[2] << 5)) & 0x07ff;   //!< Channel 1         
    pdbus->rc.ch1-=1024;
		pdbus->rc.ch2 = ((pbuf[2] >> 6) | (pbuf[3] << 2) |            //!< Channel 2                          
        (pbuf[4] << 10)) & 0x07ff;
		pdbus->rc.ch2-=1024;
    pdbus->rc.ch3 = ((pbuf[4] >> 1) | (pbuf[5] << 7)) & 0x07ff;   //!< Channel 3
		pdbus->rc.ch3-=1024;	
		pdbus->rc.s1 = ((pbuf[5] >> 4) & 0x000C) >> 2;                    //!< Switch left         
    pdbus->rc.s2 = ((pbuf[5] >> 4) & 0x0003);                         //!< Switch right  
    pdbus->mouse.x = pbuf[6] | (pbuf[7] << 8);                    //!< Mouse X axis 
    pdbus->mouse.y = pbuf[8] | (pbuf[9] << 8);                    //!< Mouse Y axis 
    pdbus->mouse.z = pbuf[10] | (pbuf[11] << 8);                  //!< Mouse Z axis 
    pdbus->mouse.l = pbuf[12];                                        //!< Mouse Left Is Press ?
    pdbus->mouse.r = pbuf[13];                                        //!< Mouse Right Is Press ? 
    pdbus->key.v = pbuf[14] | (pbuf[15] << 8);                    //!< KeyBoard value   
    pdbus->res = pbuf[16] | (pbuf[17] << 8);                      //!< Reserve 
	  pdbus->res -= 1024;
}



void UART1_Init(int BaudRate)
{
	
    GPIO_InitTypeDef gpio;
	  USART_InitTypeDef usart;
	  NVIC_InitTypeDef nvic;
    DMA_InitTypeDef dma;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
    
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
    
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_100MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &gpio);
    
    USART_DeInit(USART1);
    USART_StructInit(&usart);
    usart.USART_BaudRate = BaudRate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_Even;
    usart.USART_Mode = USART_Mode_Rx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &usart);
    
  
    USART_Cmd(USART1, ENABLE);
		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    
		nvic.NVIC_IRQChannel = DMA2_Stream5_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
		
    DMA_DeInit(DMA2_Stream5);
    dma.DMA_Channel= DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (u32)&(USART1->DR);
    dma.DMA_Memory0BaseAddr = (u32)dbus_buf;
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = DBUS_BUF_SIZE;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_VeryHigh;
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    dma.DMA_MemoryBurst = DMA_Mode_Normal;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream5,&dma);

    DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);
    DMA_Cmd(DMA2_Stream5,ENABLE);
		
}

void DMA2_Stream5_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA2_Stream5, DMA_IT_TCIF5))
    {
       
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5);
        DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);

        /*******************decode DBUS data*******************/
        DBUS_Dec(&dbus,dbus_buf);
        
    }
}

u8 UART2_Init(int BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOD,&GPIO_InitStructure); 
	
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
	USART_Init(USART2, &USART_InitStructure); 
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//????? 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0; //????? 2
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure); 
	
	USART_Cmd(USART2, ENABLE); 
	
return 0;
}

u8 UART6_Init(int BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
	
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); 
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOG,&GPIO_InitStructure); 
	
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
	USART_Init(USART6, &USART_InitStructure); 
	
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//????? 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0; //????? 2
	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure); 
	
	USART_Cmd(USART6, ENABLE); 
	
return 0;
}
u8 SV_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;

  GPIO_Init(GPIOH, &GPIO_InitStructure);
	
	
	SV1_OFF;
	SV2_OFF;
	SV3_OFF;
	SV4_OFF;
//	SV5_OFF;
	
	return 0;
	
}

//static void USART1_FIFO_Init(void)
//{
//    FIFO_Init(&_USART1_RX_FIFO, (void *)_USART1_RX_BUF, RC_FRAME_LENGTH, BSP_USART1_RX_BUF_SIZE_IN_FRAMES); 
//}


//void *USART1_GetRxBuf(void)
//{
//    return (void *)&_USART1_RX_FIFO;
//}



//void RemoteDataPrcess(uint8_t *pData)
//{
//    if(pData == NULL)
//    {
//        return;
//    }
//    
//    RC_CtrlData.rc.ch0 = ((int16_t)pData[0] | ((int16_t)pData[1] << 8)) & 0x07FF; 
//		 RC_CtrlData.rc.ch0-=1024;
//    RC_CtrlData.rc.ch1 = (((int16_t)pData[1] >> 3) | ((int16_t)pData[2] << 5)) & 0x07FF;
//		RC_CtrlData.rc.ch1-=1024;
//    RC_CtrlData.rc.ch2 = (((int16_t)pData[2] >> 6) | ((int16_t)pData[3] << 2) |
//                         ((int16_t)pData[4] << 10)) & 0x07FF;
//		RC_CtrlData.rc.ch2-=1024;
//    RC_CtrlData.rc.ch3 = (((int16_t)pData[4] >> 1) | ((int16_t)pData[5]<<7)) & 0x07FF;
//    RC_CtrlData.rc.ch3-=1024;
//    RC_CtrlData.rc.s1 = ((pData[5] >> 4) & 0x000C) >> 2;
//    RC_CtrlData.rc.s2 = ((pData[5] >> 4) & 0x0003);

//    RC_CtrlData.mouse.x = ((int16_t)pData[6]) | ((int16_t)pData[7] << 8);
//    RC_CtrlData.mouse.y = ((int16_t)pData[8]) | ((int16_t)pData[9] << 8);
//    RC_CtrlData.mouse.z = ((int16_t)pData[10]) | ((int16_t)pData[11] << 8);    

//    RC_CtrlData.mouse.press_l = pData[12];
//    RC_CtrlData.mouse.press_r = pData[13];
// 
//    RC_CtrlData.key.v = ((int16_t)pData[14]);// | ((int16_t)pData[15] << 8);
//	

////		RemoteControlProcess(&(RC_CtrlData.rc));

//}

