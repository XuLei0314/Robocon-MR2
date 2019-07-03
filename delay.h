#ifndef __DELAY_H
#define __DELAY_H


#include "stm32f4xx.h"

static volatile uint32_t TimingDelay;

void SysTick_Configuration(void); 
void Delay_ms(volatile uint32_t nTime);
void TimingDelay_Decrement(void);


void delay_init(void);
void delay_ms(u16 nms);
void delay_us(u32 nus);


#endif
