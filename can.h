#ifndef _CAN_H_
#define _CAN_H_

#include "stm32f4xx.h"
#include "delay.h"

#include "chassis.h"
#include "PID.h"
#include "main.h"


#define CAN1_TX_PORT				GPIOD
#define CAN1_TX_PIN					GPIO_Pin_1
#define CAN1_TX_PORT_RCC 		RCC_AHB1Periph_GPIOD

#define CAN1_RCC						RCC_APB1Periph_CAN1

#define CAN1_RX_PORT				GPIOD
#define CAN1_RX_PIN					GPIO_Pin_0
#define CAN1_RX_PORT_RCC		RCC_AHB1Periph_GPIOD


void Init_CAN1(void);

u8 CAN_Send(s16 speed);

u8   Set_Motor_Speed_Can(s16 s1,s16 s2,s16 s3,s16 s4,s16 s5,s16 s6, s16 s7,s16 s8);
void USB_LP_CAN1_RX0_IRQHandler(void);
void get_moto_offset(moto_measure_t* ptr, CanRxMsg* hcan);
void get_moto_measure(moto_measure_t* ptr, CanRxMsg* hcan);

#endif
