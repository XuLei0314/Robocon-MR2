#ifndef _PID_H
#define _PID_H

#include "stm32f4xx.h"
#include "delay.h"


#define ABS(x) ((x > 0) ? x : -x)



typedef struct __pid_t
{
    float p;
    float i;
    float d;

    float set[3]; //target NOW/LAST/LLAST
    float get[3]; //measure
    float err[3]; //error

    float pout; 
    float iout; 
    float dout; 
    
	  //position pid related
    float pos_out; //this time position output
    float last_pos_out; 
	
	  //delta pid related
    float delta_u; //this time delta value
    float delta_out; //this time delta output = last_delta_out + delta_u
    float last_delta_out;

	  ////deadband < err < max_err
    float    max_err;
    float    deadband; 
		
    uint32_t pid_mode;
    uint32_t MaxOutput;
    uint32_t IntegralLimit;

    void (*f_param_init)(struct __pid_t* pid, 
                         uint32_t        pid_mode,
                         uint32_t        maxOutput,
                         uint32_t        integralLimit,
                         float           p,
                         float           i,
                         float           d);
    void (*f_pid_reset)(struct __pid_t* pid, float p, float i, float d);
 
} pid_t;

enum
{
    LLAST = 0,
    LAST  = 1,
    NOW   = 2,
    POSITION_PID,
    DELTA_PID,
};



void abs_limit(float *a, float ABS_MAX);

void PID_struct_init(
	  pid_t*   pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,

    float kp,
    float ki,
    float kd,
		float dead_band);

float pid_calc(pid_t* pid, float fdb, float ref);

#endif
