#ifndef _TIMER_H
#define _TIMER_H

#include "main.h"



void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_CH3_Cap_Init(u32 arr,u16 psc);
void TIM9_Int_Init(u16 arr,u16 psc);
//void TIM5_Int_Init(u16 arr,u16 psc);
void TIM5_PWM_Init(u32 arr,u32 psc);


#endif
