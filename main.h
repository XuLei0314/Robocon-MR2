#ifndef __MAIN_H__
#define __MAIN_H__

#include "stm32f4xx.h"
#include "delay.h"
#include "chassis.h"
#include "can.h"
#include "PID.h"
#include "base.h"
#include "stdio.h"
#include "timer.h"
#include <math.h>
#include "string.h"
#include "DataScope_DP.h"
#define MyAbs(x) ((x > 0) ? (x) : (-x))

#define FORWARD 0x01
#define CROSS   0x02
#define IDLE		0x03
#define RELEASE 0x04
#define BACKWARD 0x05
#define RIGHT   0x06
#define LEFT    0x07


#endif
