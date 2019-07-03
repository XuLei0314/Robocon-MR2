#ifndef _CHASSIS_H
#define _CHASSIS_H

#include "stm32f4xx.h"
#include "delay.h"


#define PI 3.1415926f
#define MyAbs(x) ((x > 0) ? (x) : (-x))


#define MAX_WHEEL_SPEED 100
#define MAX_CHASSIS_VX_SPEED 100
#define MAX_CHASSIS_VY_SPEED 100
#define MAX_CHASSIS_VR_SPEED 100

#define dt								0.005

#define auto_to_chassis_x 1.f
#define auto_to_chassis_y 1.f
#define auto_to_chassis_w 1.72992f



#define VAL_LIMIT(val, min, max)\
if(val<=min)\
{\
	val = min;\
}\
else if(val>=max)\
{\
	val = max;\
}\

//typedef struct
//{
//    float v1;
//    float v2;
//    float v3;
//}ActThreeVell;

//typedef struct
//{
//	float v_x;
//	float v_y;
//	float v_w;
//	
//	float x_pos;
//	float y_pos;
//	float z_ang;
//	float state;
//	
//} odom_t;


typedef struct
{
    int16_t  speed_rpm;

    int16_t  given_current;
    uint8_t  hall;

    uint16_t offset_angle;
    int32_t  round_cnt;
    int32_t  total_ecd;
    int32_t  total_angle;
	  u32      msg_cnt;
	
	  uint16_t angle; //abs angle range:[0,8191]
    uint16_t last_angle; //abs angle range:[0,8191]
} moto_measure_t;

typedef enum {
	  CHASSIS_AUTO = 0, 
	
    CHASSIS_STOP, // stall
    CHASSIS_RELAX, // relax
    CHASSIS_OPEN_LOOP,
    CHASSIS_CLOSE_GYRO_LOOP,
    CHASSIS_FOLLOW_GIMBAL,

} eChassisMode;

typedef struct
{
    float           vx; // forward/back
    float           vy; // left/right
    float           vw; // 
    float           kb_vx;
    float           kb_vy;
    float           mouse_vw;
    eChassisMode    mode;
    eChassisMode    last_mode;
    float           target_angle; //
    float           angle_from_gyro;
		float           palstance_from_gyro;
    float           last_angle;
    int             is_snipe_mode; //gimbal chassis separate
    int16_t         wheel_speed[4];
		float           wheel_w_speed[4];
} chassis_t; //chassis status



void chassis_pid_param_init(void);
void mecanum_calc(float vx, float vy, float vw, const int each_max_spd, float speed[]);
//void odom_calc(odom_t* odom,moto_measure_t* moto_chassis[]);

float Remote_Fliter(float base,float change);


#endif
