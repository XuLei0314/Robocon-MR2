/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    06-March-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//sv4是交接令牌杆子，4是推板子，1是推兽骨的，2是旋转汽缸，3是夹
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;
moto_measure_t moto_chassis[8]; 
tReceTXoneData ReceData;
s16 current_M3508[8];

int robot_state = IDLE;
int up_dist = 0;

extern pid_t pid_spd[4];
extern pid_t pid_Speed;
extern pid_t pid_Pos;
extern DBUS dbus;

int rc_get = 0;
int run_count = 0;

int Arm_Speed[8] = {0};
int Arm_Pos[8]   = {0};

int start_test = 0;






/* Private functions ---------------------------------------------------------*/
void Send_DataScope(float ch1,float ch2);
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int fputc(int ch, FILE *f)
{      
	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
    USART6->DR = (u8) ch;      
	return ch;
}

int main(void)
{
		int j;
		SysTick_Configuration();
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
    LED_KEY_Init();
		Pause_Key_Init();
		Init_CAN1();
		SV_Init();
		TIM3_Int_Init(50-1,8400-1);  //84M  84M/psc = 10Khz  10Khz * 50 = 5ms
//		TIM9_Int_Init(5000-1,168-1);
//		TIM2_CH3_Cap_Init(0XFFFFFFFF,84-1);
	  UART1_Init(100000);
	  //TIM5_PWM_Init(2000-1,840-1);
		//UART2_Init(115200);
		UART6_Init(115200);
	
		
		SV1_ON;
		SV2_ON;
		SV3_ON;
		SV4_ON;
//		for(j=0;j<4;j++)
//			PID_struct_init(&pid_spd[j], POSITION_PID, 11000, 1500, 18.0f, 1.0f, 1.0f);
	
		PID_struct_init(&pid_Pos, POSITION_PID, 2000, 1000,5.0f, .01f, .0f,10);  
		//PID_struct_init(&pid_Speed,POSITION_PID,20000, 5000, 4.5f, 0.0f, 0.0f,0); 
		PID_struct_init(&pid_Speed,POSITION_PID,20000, 5000, 20.0f, 0.0f, 0.0f,0); 
//	
//	while(1)
//	{
//		if(Pause_Key)
//			RED_ON;
//		else
//			RED_OFF;
//	}
		up_dist = 15;

while(1)
{
		while(Board_Key)
		{
			Delay_ms(10);
		}
		
		RED_ON;
		Delay_ms(4000);
		rc_get = 1;
		while(1)
		{
			
			if(run_count < 10000 && !Pause_Key)
			{
				dbus.rc.ch3 = 150;
				dbus.rc.ch0 = 0;
			}
//			else if((run_count > 150) && (run_count < 200) && !Pause_Key)
//			{
//				dbus.rc.ch3 = 0;
//				//
//				
//			//	dbus.rc.ch0 = 150; //Left  Right
//				dbus.rc.ch0 = -150; //Right Blue
//			}
			else
			{
				dbus.rc.ch3 = 0;
				dbus.rc.ch0 = 0;
			}
			if(run_count < 10000 && !Pause_Key)
						run_count++;
			if(Pause_Key)
			{
				run_count = 0;
				break;
				//goto RESTART;
			}
			Delay_ms(100);
		}
}
		
//		while (1)
//		{

			
//					if(KEY1)
//					{
//						if(start_test == 2)
//						{
//							printf("Down \r\n");
//							start_test = 3;
//						}
//						else if(start_test == 0)
//						{
//							start_test = 1;
//							printf("Up \r\n");
//						}
//						//Arm_Pos[1]+= 500;
//						//printf("Arm Speed is %d\n", Arm_Pos[1]);
//						while(KEY1);
//					}
//					
//					//Send_DataScope(Arm_Pos[1],moto_chassis[1].total_angle);
//					//printf("%d %d %d %d %d %d %d \r\n",dbus.rc.ch0,dbus.rc.ch1,dbus.rc.ch2,dbus.rc.ch3,dbus.rc.s1,dbus.rc.s2,dbus.res);
//					//printf("[%d  %d]---[%d  %d]\r\n",moto_chassis[5].total_angle,moto_chassis[4].total_ecd,moto_chassis[0].total_angle,moto_chassis[1].total_ecd);
//					Delay_ms(100);
////		for(t=0;t<sizeof(tSendTXoneData);t++)
////	{
////				USART_SendData(USART6,auto_send1[t]);
////				while( USART_GetFlagStatus(USART6,USART_FLAG_TC)!= SET);
////	}
//		}
}

void Send_DataScope(float ch1,float ch2)
{
	unsigned char count;
	unsigned char Send_Count;
	int x=0;
	
	DataScope_Get_Channel_Data( ch1 , 1 );
	DataScope_Get_Channel_Data( ch2 , 2 );
	
	Send_Count = DataScope_Data_Generate(2); 
	
	 for( count = 0 ; count < Send_Count; count++) 
          {
             while((USART6->SR&0X40)==0);  
            USART6->DR = DataScope_OutPut_Buffer[count]; 
          }
		Delay_ms(10);
	
}




