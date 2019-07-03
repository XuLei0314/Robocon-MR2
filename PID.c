#include "PID.h"

#include "chassis.h"


#define Inter_Max   2000

static void pid_param_init(
    pid_t*   pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,
    float    kp,
    float    ki,
    float    kd)
{

    pid->IntegralLimit = intergral_limit;
    pid->MaxOutput     = maxout;
    pid->pid_mode      = mode;

    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
}
static void pid_reset(pid_t* pid, float kp, float ki, float kd)
{
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
}



void abs_limit(float *a, float ABS_MAX)
{
    if (*a > ABS_MAX)
        *a = ABS_MAX;
    if (*a < -ABS_MAX)
        *a = -ABS_MAX;
}

float pid_calc(pid_t* pid, float get, float set)
{
	pid->get[NOW] = get;
	pid->set[NOW] = set;
	pid->err[NOW] = set - get; 
	
	/* add max_err and deadband handle */
	if (pid->max_err != 0 && ABS(pid->err[NOW]) > pid->max_err)
			return 0;
	if (pid->deadband != 0 && ABS(pid->err[NOW]) < pid->deadband)
			return 0;

	if (pid->pid_mode == POSITION_PID) //position PID
	{
			pid->pout = pid->p * pid->err[NOW];
			pid->iout += pid->i * pid->err[NOW];
			pid->dout = pid->d/**(pid->FreOut+1)*/ * (pid->err[NOW] - pid->err[LAST]);
		
			abs_limit(&(pid->iout), pid->IntegralLimit);
			pid->pos_out = pid->pout + pid->iout + pid->dout;
			abs_limit(&(pid->pos_out), pid->MaxOutput);

			//pid->last_pos_out = pid->pos_out; //update last time
	}
	else if (pid->pid_mode == DELTA_PID) //delta PID
	{
			pid->pout = pid->p * (pid->err[NOW] - pid->err[LAST]);
			pid->iout = pid->i * pid->err[NOW];
			pid->dout = pid->d/**(pid->FreOut+1)*/ * (pid->err[NOW] - 2 * pid->err[LAST] + pid->err[LLAST]);

			pid->delta_u   = pid->pout + pid->iout + pid->dout;
			pid->delta_out += pid->delta_u;
			abs_limit(&(pid->delta_out), pid->MaxOutput);
		
			//pid->last_delta_out = pid->delta_out; //update last time
	}

	pid->err[LLAST] = pid->err[LAST];
	pid->err[LAST]  = pid->err[NOW];
	pid->get[LLAST] = pid->get[LAST];
	pid->get[LAST]  = pid->get[NOW];
	pid->set[LLAST] = pid->set[LAST];
	pid->set[LAST]  = pid->set[NOW];
	
	return pid->pid_mode == POSITION_PID ? pid->pos_out : pid->delta_out;
}


void PID_struct_init(
    pid_t*   pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,

    float kp,
    float ki,
    float kd,
		float dead_band)
{
    pid->f_param_init = pid_param_init;
    pid->f_pid_reset  = pid_reset;
		pid->deadband     = dead_band;

    pid->f_param_init(pid, mode, maxout, intergral_limit, kp, ki, kd);
}


pid_t pid_spd[3]        = { 0 };
pid_t pid_Speed				 = { 0 };
pid_t pid_Pos	         = { 0 };
