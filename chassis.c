#include "chassis.h"
#include "PID.h"

#include "string.h"
#include <math.h>


//ActThreeVell ThreeWheelVellControl2(float Vx, float Vy, float angularVell,const int each_max_spd, float speed[])
//{
//#define AFA 60
//#define L   4
//	float   buf[3];
//	int   i;
//	float max = 0, rate;
//ActThreeVell vell;

//u8 theta = 0;

//vell.v1 = (float)(-cos((AFA + theta) / 180.0f*PI) * Vx - sin((theta + AFA) / 180.0f*PI) * Vy + L * angularVell);

//vell.v2 = (float)(cos(theta / 180.0f*PI) * Vx + sin(theta /180.0f*PI) * Vy      + L * angularVell);

//vell.v3 = (float)(-cos((AFA - theta) / 180.0f * PI) * Vx + sin((AFA - theta) / 180.0f*PI) * Vy + L * angularVell);


//	VAL_LIMIT(Vx -MAX_CHASSIS_VX_SPEED, MAX_CHASSIS_VX_SPEED);
//  VAL_LIMIT(Vy -MAX_CHASSIS_VY_SPEED, MAX_CHASSIS_VY_SPEED);
//  VAL_LIMIT(angularVell -MAX_CHASSIS_VR_SPEED, MAX_CHASSIS_VR_SPEED);

//			buf[0] = (float)vell.v1;  //391.55
//			buf[1] = (float)vell.v2;
//			buf[2] = (float)vell.v3;	
//			//buf[3] = (float)( +vy - vx - vw*(L+W));


//	//find max item
//	for (i = 0; i < 3; i++)
//	{
//			if (MyAbs(buf[i]) > max)
//					max = MyAbs(buf[i]);
//	}
//	//equal proportion
//	if (max > each_max_spd)
//	{
//			rate = each_max_spd / max;
//			for (i = 0; i < 3; i++)
//					buf[i] *= rate;
//	}
//	memcpy(speed, buf, sizeof(float) * 3);
//return vell;



//}

//#define L 0.2185
//#define W 0.2165

//#define K 433.00f //mm
//#define R 76			//mm

//#define K 433.00f //mm
//#define R 76			//mm


//extern moto_measure_t moto_chassis[4]; 
//extern chassis_t chassis;
extern pid_t pid_spd[4];
extern float Gyno;


void chassis_pid_param_init(void)
{
	
	int k;
	for ( k = 0; k < 4; k++) 
	{
		//max current = 20000
		PID_struct_init(&pid_spd[k], POSITION_PID, 11000, 1500, 18.0f, 1.0f, 1.0f,0); 
	}
}
extern u16 get3;
void mecanum_calc(float vx, float vy, float vw, const int each_max_spd, float speed[])
{
	
#define AFA 60
#define L   1
//	float   buf[3];
//	int   i;
//	float max = 0, rate;
//ActThreeVell vell;

u8 theta = 0;
	float   buf[3];
	int   i;
	float max = 0, rate;

	VAL_LIMIT(vx, -MAX_CHASSIS_VX_SPEED, MAX_CHASSIS_VX_SPEED);
  VAL_LIMIT(vy, -MAX_CHASSIS_VY_SPEED, MAX_CHASSIS_VY_SPEED);
  VAL_LIMIT(vw, -MAX_CHASSIS_VR_SPEED, MAX_CHASSIS_VR_SPEED);

			buf[1] = (float)(-cos((AFA + theta) / 180.0f*PI) * vx*2 - sin((theta + AFA) / 180.0f*PI) * vy*2 + L * vw);  //391.55zuo
			buf[0] =(float)(cos(theta / 180.0f*PI) * vx*2 + sin(theta /180.0f*PI) * vy*2      + L * vw);//qian
			buf[2] = (float)(-cos((AFA - theta) / 180.0f * PI) * vx*2 + sin((AFA - theta) / 180.0f*PI) * vy*2 + L * vw);	//you
		//	buf[3] = (float)( +vy - vx - vw*(L+W));
//			if(get3>300)
//			{
//				buf[1]=buf[1]-4;
//				buf[0]=buf[0]+2;
//			}

	//find max item
	for (i = 0; i < 3; i++)
	{
			if (MyAbs(buf[i]) > max)
					max = MyAbs(buf[i]);
	}
	//equal proportion
	if (max > each_max_spd)
	{
			rate = each_max_spd / max;
			for (i = 0; i < 3; i++)
					buf[i] *= rate;
	}
	memcpy(speed, buf, sizeof(float) * 3);
}

float Remote_Fliter(float base,float change)
{
	int before = base;
	int after  = change;
	
	if(MyAbs(after-before)>30)
		return before;
	else
		return after;
}

//void odom_calc(odom_t* odom,moto_measure_t* moto_chassis[])
//{
//	
//	float delta_x;
//	float delta_y;
//	odom->v_x = (-moto_chassis[0]->speed_rpm + moto_chassis[1]->speed_rpm + moto_chassis[2]->speed_rpm - moto_chassis[3]->speed_rpm)*PI/30;
//	odom->v_y = (-moto_chassis[0]->speed_rpm - moto_chassis[1]->speed_rpm + moto_chassis[2]->speed_rpm + moto_chassis[3]->speed_rpm)*PI/30;
//	odom->v_w = (-moto_chassis[0]->speed_rpm - moto_chassis[1]->speed_rpm - moto_chassis[2]->speed_rpm - moto_chassis[3]->speed_rpm)*6/391.55;
//	
//	delta_x = (odom->v_x*cos(Gyno) - odom->v_y*sin(Gyno)) * dt;
//	delta_y = (odom->v_x*sin(Gyno) + odom->v_y*cos(Gyno)) * dt;
//	
//	odom->x_pos += delta_x;
//	odom->y_pos += delta_y;
//	
//	
//}
