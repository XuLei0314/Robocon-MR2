#ifndef __BASE_H__
#define __BASE_H__

#include "stm32f4xx.h"
#include "sys.h"

#define RED_ON  	GPIOE->BSRRH = GPIO_Pin_11
#define RED_OFF 	GPIOE->BSRRL = GPIO_Pin_11

#define GREEN_ON 	GPIOF->BSRRH = GPIO_Pin_14
#define GREEN_OFF GPIOF->BSRRL = GPIO_Pin_14
#define LED_ON 	 GPIOG->BSRRH = GPIO_Pin_1
#define LED_OFF  GPIOG->BSRRL = GPIO_Pin_1

#define KEY1		    (GPIOB->IDR & GPIO_Pin_2)
//#define KEY2			(GPIOB->IDR & GPIO_Pin_4)

#define SV1_OFF		GPIOH->BSRRH = GPIO_Pin_2
#define SV1_ON		GPIOH->BSRRL = GPIO_Pin_2

#define SV2_OFF		GPIOH->BSRRH = GPIO_Pin_3
#define SV2_ON		GPIOH->BSRRL = GPIO_Pin_3

#define SV3_OFF		GPIOH->BSRRH = GPIO_Pin_4
#define SV3_ON		GPIOH->BSRRL = GPIO_Pin_4

#define SV4_OFF		GPIOH->BSRRH = GPIO_Pin_5
#define SV4_ON   GPIOH->BSRRL = GPIO_Pin_5


#define DBUS_BUF_SIZE    18
/* ----------------------- RC Channel Value Definition---------------------------- */
#define CH_VALUE_MIN                   ((uint16_t)364    ) 
#define CH_VALUE_OFFSET                ((uint16_t)1024   ) 
#define CH_VALUE_MAX                   ((uint16_t)1684   )  
/* ----------------------- RC Switch Value Definition----------------------------- */
#define SW_UP                          ((uint16_t)1      ) 
#define SW_MID                          ((uint16_t)3      ) 
#define SW_DOWN                        ((uint16_t)2      )  
/* ----------------------- PC Mouse Value Definition-------------------------------- */
#define MOUSE_MOVE_VALUE_MIN         ((uint16_t)-32768 ) 
#define MOUSE_MOVE_VALUE_OFFSET      ((uint16_t)0      ) 
#define MOUSE_MOVE_VALUE_MAX         ((uint16_t)32767  ) 
#define MOUSE_BTN_UP                  ((uint8_t)0       ) 
#define MOUSE_BTN_DN                  ((uint8_t)1       ) 
/* ----------------------- PC Key Value Definition-------------------------------- */
#define KEY_PRESSED_OFFSET_W         ((uint16_t)0x01<<0) 
#define KEY_PRESSED_OFFSET_S         ((uint16_t)0x01<<1) 
#define KEY_PRESSED_OFFSET_A         ((uint16_t)0x01<<2) 
#define KEY_PRESSED_OFFSET_D         ((uint16_t)0x01<<3) 
#define KEY_PRESSED_OFFSET_Q         ((uint16_t)0x01<<4) 
#define KEY_PRESSED_OFFSET_E         ((uint16_t)0x01<<5) 
#define KEY_PRESSED_OFFSET_SHIFT     ((uint16_t)0x01<<6) 
#define KEY_PRESSED_OFFSET_CTRL      ((uint16_t)0x01<<7)  


//#define SV5_ON		GPIOB->BSRRH = GPIO_Pin_11
//#define SV5_OFF  	GPIOB->BSRRL = GPIO_Pin_11


 //Pause PA0   Board PA1
#define Pause_Key		    (GPIOA->IDR & GPIO_Pin_0)
#define Board_Key		    (GPIOA->IDR & GPIO_Pin_1)
 
#define LED0      PIout(0)

#define compass_eof 		0x77

//typedef __packed struct
//{
//	int16_t ch0;
//	int16_t ch1;
//	int16_t ch2;
//	int16_t ch3;
//	int8_t s1;
//	int8_t s2;
//}Remote;

//typedef __packed struct
//{
//	int16_t x;
//	int16_t y;
//	int16_t z;
//	uint8_t last_press_l;
//	uint8_t last_press_r;
//	uint8_t press_l;
//	uint8_t press_r;
//}Mouse;	
//typedef	__packed struct
//{
//	uint16_t v;
//}Key;

//typedef __packed struct
//{
//	Remote rc;
//	Mouse mouse;
//	Key key;
//}RC_Ctl_t;


typedef struct
{
    struct
    {
        int16_t ch0;
        int16_t ch1;
        int16_t ch2;
        int16_t ch3;
        int8_t  s1;
        int8_t  s2;
    }rc;
    
    struct
    {
        int16_t x;
        int16_t y;
        int16_t z;
        int8_t l;
        int8_t r;
    }mouse;
    
    struct
    {
        int16_t v;
    }key;
    
    int16_t res;
    
}DBUS;

void DBUS_Enc(const DBUS* pdbus,unsigned char* pbuf);
void DBUS_Dec(DBUS* pdbus,const unsigned char* pbuf);


u8 LED_KEY_Init(void);
void UART1_Init(int BaudRate);
u8 UART2_Init(int BaudRate);
u8 UART6_Init(int BaudRate);
static void USART1_FIFO_Init(void);
u8 SV_Init(void);
void SV_Check(void);
void Board_Carryer_Init(void);
void Pause_Key_Init(void);
//void RemoteDataPrcess(uint8_t *pData);
//void *USART1_GetRxBuf(void);



#endif
