#include "timer.h"
#include "base.h"
#include "stm32f4xx.h"
#include "chassis.h"
u8  TIM2CH3_CAPTURE_STA=0,ppm_rx_sta=0,ppm_rx_num=0;	//输入捕获状态		    				
u32	TIM2CH3_CAPTURE_VAL;	//输入捕获值
u16 ppm_rx[10];//ppm_rx[0]   1   接收到ppm数据

int count;
extern s16 current_M3508[8];
extern chassis_t chassis;
extern moto_measure_t moto_chassis[8];
extern pid_t pid_spd[3];
extern DBUS dbus;


extern int robot_state;
extern int up_dist;
//extern odom_t odom;

void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 
	TIM_TimeBaseInitStructure.TIM_Period = arr; 
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; 
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM3,ENABLE); 
}

void TIM9_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE); 
	TIM_TimeBaseInitStructure.TIM_Period = arr; 
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc; 
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE); 
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; 
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; 
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_Cmd(TIM9,ENABLE); 
}

TIM_ICInitTypeDef TIM2_ICInitStructure;

void TIM2_CH3_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //TIM5 ????
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //?? PORTA ??
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //?? 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //??????
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //??
	GPIO_Init(GPIOB,&GPIO_InitStructure); //??? PA0
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2);
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc; //?????
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //??????
	TIM_TimeBaseStructure.TIM_Period=arr; //??????
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_3; //????? IC1 ??? TI1 ?
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling; //?????
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //??? TI1 ?
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; //??????,???
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ??????? ???
	TIM_ICInit(TIM2, &TIM2_ICInitStructure); //??? TIM5 ??????

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//????? 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;//????? 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ????
	NVIC_Init(&NVIC_InitStructure); //?????????? VIC ????
	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC3,ENABLE);//?????????
	TIM_Cmd(TIM2,ENABLE ); 
	
}

void TIM2_IRQHandler(void)
{
	
 	if((TIM2CH3_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{	  
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		 
		{	    
			if(TIM2CH3_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM2CH3_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIM2CH3_CAPTURE_STA|=0X80;//标记成功捕获了一次
					TIM2CH3_CAPTURE_VAL=0XFFFFFFFF;
				}else TIM2CH3_CAPTURE_STA++;
			}	 
		}
	if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)//捕获3发生捕获事件
		{	
			if(TIM2CH3_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM2CH3_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
				TIM2CH3_CAPTURE_VAL=TIM_GetCapture3(TIM2);
			 // printf("Rising is %d\n",TIM2CH3_CAPTURE_VAL);
		   		TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Falling); //CC1P=0 设置为上升沿捕获
			}else  								//还未开始,第一次捕获上升沿
			{
				TIM2CH3_CAPTURE_STA=0;			//清空
				TIM2CH3_CAPTURE_VAL=0;
	 			TIM_SetCounter(TIM2,0);
				TIM2CH3_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
		   		TIM_OC3PolarityConfig(TIM3,TIM_ICPolarity_Rising);		//CC1P=1 设置为下降沿捕获
				TIM_Cmd(TIM2,ENABLE);
			}		    
		}			     	    					   
 	}
	
	//处理帧数据
		if(TIM2CH3_CAPTURE_STA&0X80)//成功捕获到了一次上升沿
		{
//		  temp=TIM5CH1_CAPTURE_STA&0X3F;
//			temp*=65536;//溢出时间总和
//			temp+=TIM5CH1_CAPTURE_VAL;//得到总的高电平时间
			if(ppm_rx_sta==1) {ppm_rx[ppm_rx_num+1]=TIM2CH3_CAPTURE_VAL;ppm_rx_num++;}//printf("TIM5CH1_CAPTURE_VAL:%d\r\n",TIM5CH1_CAPTURE_VAL);
			if(4>TIM2CH3_CAPTURE_STA&0X3F>0||TIM2CH3_CAPTURE_VAL>3000) ppm_rx_sta++;//低电平时间大于3000us为起始帧
			if(ppm_rx_sta==2) {ppm_rx_sta=0;ppm_rx[0]=1;ppm_rx_num=0;}//printf("receive\r\n");//ppm_rx_sta   1 表示接收到同步帧/ 2接收到到下一起始帧 ppm数据接收完毕
			
			TIM2CH3_CAPTURE_STA=0;//开启下一次捕获
		}						
    TIM_ClearITPendingBit(TIM2, TIM_IT_CC3|TIM_IT_Update); //清除中断标志位
}
u16 time=0;
u16 time1=0;
extern int flag1;
volatile u16 temp = 0;
extern pid_t pid_Speed;
extern pid_t pid_Pos;
extern int Arm_Speed[8];
extern int Arm_Pos[8];
extern int start_test;
extern int rc_get;
int up[][2] = {{0,0},{9,-25},{9,-25},{9,-25},{9,-24},{8,-24},{1,-20},{-8,3},{-12,31},{-50,69},{-93,83},{-131,144},{-178,155},{-233,198},{-295,219},{-373,244},{-429,292},{-470,326},{-524,375},{-568,406},{-632,464},{-702,485},{-774,573},{-823,621},{-831,622}};
//int step_1[][2] = {{0,0},{1,-1},{69,-111},{251,-137},{320,0},{422,84},{400,200},{366,352},{226,411}};
//int step_2[][2] = {{220,406},{217,401},{151,291},{11,99},{-170,-50},{-328,-158},{-398,-186},{-399,-185}};
int step_1[][2] ={{0,0},{15,0},{17,-3},{17,-3},{19,-4},{19,-38},{20,-109},{74,-200},{133,-252},{222,-259},{318,-259},{417,-259},{517,-247},{584,-193},{627,-92},{635,37},{635,174},{629,284},{577,373},{532,444},{485,468},{437,468},{423,468}};
int step_2[][2] = {{423,494},{423,478},{424,453},{424,357},{423,235},{366,133},{292,68},{267,31},{261,17},{243,-8},{229,-46},{143,-100},{63,-148},{-4,-161},{-12,-161},{-12,-161}};
//int cross_step[][2] = {{0,0},{0,-3},{0,-51},{0,-113},{12,-167},{83,-226},{192,-309},{290,-398},{387,-455},{495,-462},{625,-462},{765,-438},{902,-377},{1015,-281},{1082,-164},{1112,-60},{1111,20},{1111,99},{1111,167},{1109,228},{1109,265},{1109,294},{1109,340},{1109,354},{1109,369},{1109,380}};
int cross_step[][2] = {{0,-3},{0,-3},{0,-4},{0,-5},{0,-7},{0,-7},{4,-7},{15,-8},{33,-10},{51,-27},{76,-69},{115,-132},{172,-207},{234,-284},{297,-353},{357,-411},{410,-454},{466,-490},{525,-525},{596,-552},{666,-577},{730,-592},{784,-603},{836,-606},{896,-607},{953,-607},{1012,-606},{1061,-606},{1102,-605},{1138,-590},{1171,-572},{1206,-541},{1242,-506},{1280,-467},{1312,-429},{1337,-393},{1344,-356},{1344,-318},{1344,-280},{1344,-241},{1343,-203},{1343,-169},{1339,-139},{1339,-118},{1339,-101},{1338,-87},{1331,-80},{1308,-80},{1272,-80},{1228,-80},{1180,-80},{1132,-79},{1100,-64},{1092,-51},{1083,-50},{1083,-50},{1083,-49},{1083,-50}};
int step1_count = 0;
int step2_count = 0;
int up_count = 0;
	
int cross_step_count = 0;
int cross_walking_count=0;
	
char step1_ready = 0;
char step2_ready = 0;
char step3_ready = 0;
	
char cross_step_ready = 0;
char cross_step1_ready=0;
char cross_step2_ready=0;
char cross_step3_ready=0;
char cross_step4_ready=0;
char cross_step5_ready=0;
char cross_step6_ready=0;
	
int cross_ready_step1 = 0;
int cross_ready_step2 = 0;
int cross_ready_step3 = 0;
int cross_ready_step4 = 0;
int cross_ready_step5 = 0;
int cross_ready_step6 = 0;
int cross_ready_step7 = 0;
int cross_ready_step8 = 0;
	
	
int moto_count = 0;
	
#define CAPTURE_MOVE
//#define REMOTE_MODE
	
void TIM3_IRQHandler(void)
{
	
	static u16 led_cnt;

  
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) 
	{
//		
//		if(Pause_Key == 0)
//		{
//			robot_state = IDLE;
//		}
//		else if((Pause_Key == 1) && (robot_state == IDLE))
//		{
//			robot_state = RELEASE;
//		}
		
		temp++; 
		
		if(temp>50000) 
			temp = 0;
		
		if(temp % 10 == 0) //50ms  2Hz
		{
			if(led_cnt++ >27)
				led_cnt = 0;
			if(led_cnt<2) 
				GREEN_ON;
			else if(led_cnt<5) 
				GREEN_OFF;
			else if(led_cnt<7)
				GREEN_ON;
			else  
				GREEN_OFF;
			}
		
			if(temp % 20 == 0)
		{
//#ifdef REMOTE_MODE
//			if(dbus.res < -100 && rc_get)
//			{
//					if(up_count < (sizeof(up)/sizeof(up[0])))
//							    up_count++;
//			}
//			else if(dbus.res > 200 && rc_get)
//			{
//				if(up_count > 0)
//					up_count--;
//			}
//#else
			if(up_count < up_dist)
				 up_count++;
//#endif
		}
		
//		if(temp % 10 == 0)
//		{
//		 
//		}
#ifdef CAPTURE_MOVE
	
		if(temp % 10 == 0)
#else
			
		if(temp % 2 == 0)
#endif
		{
			
#ifdef CAPTURE_MOVE
			printf("{%d,%d},",moto_chassis[4].total_angle,moto_chassis[5].total_angle);
			//printf("%d\r\n",sizeof(up)/sizeof(up[0]));
		//	printf("%d\r\n",dbus.res);
#else
			//printf("%d\r\n",dbus.res);
			
			
				Arm_Pos[0] = up[up_count][1];
				Arm_Pos[1] = up[up_count][0];
				
				Arm_Pos[2] = up[up_count][1];
				Arm_Pos[3] = up[up_count][0];
				
				Arm_Pos[4] = up[up_count][0];
				Arm_Pos[5] = up[up_count][1];
				
				Arm_Pos[6] = up[up_count][1];
				Arm_Pos[7] = up[up_count][0];
			
#ifdef	REMOTE_MODE
		if(dbus.rc.ch3 < -100 && rc_get)  //Go backward
#else
		if(robot_state == BACKWARD)
#endif
			{
				if(!step1_ready)
				{
					step1_count++;
					Arm_Pos[4] = up[up_count][0] + step_1[step1_count][0];
					Arm_Pos[5] = up[up_count][1] + step_1[step1_count][1];
					
					Arm_Pos[2] = up[up_count][1] - step_1[step1_count][0];
					Arm_Pos[3] = up[up_count][0] - step_1[step1_count][1];
					if(step1_count == (sizeof(step_1)/sizeof(step_1[0])))
					{
						step1_count = 0;
						step1_ready = 1;
					}
				}
				if(step1_ready && !step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[4] = up[up_count][0] + step_2[step2_count][0];
					Arm_Pos[5] = up[up_count][1] + step_2[step2_count][1];
					
					Arm_Pos[2] = up[up_count][1] - step_2[step2_count][0];
					Arm_Pos[3] = up[up_count][0] - step_2[step2_count][1];
					
					Arm_Pos[0] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[1] = up[up_count][0] + step_1[step1_count][0];
					
					Arm_Pos[6] = up[up_count][1] - step_1[step1_count][0];
					Arm_Pos[7] = up[up_count][0] - step_1[step1_count][1];
					
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 1;
						step1_count = 0;
						step2_count = 0;
					}
				}
				else if(step1_ready && step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[4] = up[up_count][0] + step_1[step1_count][0];
					Arm_Pos[5] = up[up_count][1] + step_1[step1_count][1];
					
					Arm_Pos[2] = up[up_count][1] - step_1[step1_count][0];
					Arm_Pos[3] = up[up_count][0] - step_1[step1_count][1];
					
					Arm_Pos[0] = up[up_count][1] + step_2[step2_count][1];
					Arm_Pos[1] = up[up_count][0] + step_2[step2_count][0];
					
					Arm_Pos[6] = up[up_count][1] - step_2[step2_count][0];
					Arm_Pos[7] = up[up_count][0] - step_2[step2_count][1];
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 0;
						step1_count = 0;
						step2_count = 0;
					}
				}
			}
#ifdef REMOTE_MODE
			else if(dbus.rc.ch3 > 100 && rc_get)  //Go forward
#else
			else if(robot_state == FORWARD)
#endif
			{
				if(!step1_ready)
				{
					step1_count++;
					Arm_Pos[4] = up[up_count][0] - step_1[step1_count][1];
					Arm_Pos[5] = up[up_count][1] - step_1[step1_count][0];
					
					Arm_Pos[2] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[3] = up[up_count][0] + step_1[step1_count][0];
					if(step1_count == (sizeof(step_1)/sizeof(step_1[0])))
					{
						step1_count = 0;
						step1_ready = 1;
					}
				}
				if(step1_ready && !step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[4] = up[up_count][0] - step_2[step2_count][1];
					Arm_Pos[5] = up[up_count][1] - step_2[step2_count][0];
					
					Arm_Pos[2] = up[up_count][1] + step_2[step2_count][1];
					Arm_Pos[3] = up[up_count][0] + step_2[step2_count][0];
					
					Arm_Pos[0] = up[up_count][1] - step_1[step1_count][0];
					Arm_Pos[1] = up[up_count][0] - step_1[step1_count][1];
					
					Arm_Pos[6] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[7] = up[up_count][0] + step_1[step1_count][0];
					
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 1;
						step1_count = 0;
						step2_count = 0;
					}
				}
				else if(step1_ready && step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[4] = up[up_count][0] - step_1[step1_count][1];
					Arm_Pos[5] = up[up_count][1] - step_1[step1_count][0];
					
					Arm_Pos[2] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[3] = up[up_count][0] + step_1[step1_count][0];
					
					Arm_Pos[0] = up[up_count][1] - step_2[step2_count][0];
					Arm_Pos[1] = up[up_count][0] - step_2[step2_count][1];
					
					Arm_Pos[6] = up[up_count][1] + step_2[step2_count][1];
					Arm_Pos[7] = up[up_count][0] + step_2[step2_count][0];
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 0;
						step1_count = 0;
						step2_count = 0;
					}
				}
			}
#ifdef REMOTE_MODE
			else if(dbus.rc.ch0 > 100 && rc_get) //Turn Right
#else
			else if(robot_state ==  LEFT)
#endif
			{
				if(!step1_ready)
				{
					step1_count++;
					
						Arm_Pos[2] = up[up_count][1] + step_1[step1_count][1];
						Arm_Pos[3] = up[up_count][0] + step_1[step1_count][0];
					
						Arm_Pos[4] = up[up_count][0] + step_1[step1_count][0];
						Arm_Pos[5] = up[up_count][1] + step_1[step1_count][1];
					
					if(step1_count == (sizeof(step_1)/sizeof(step_1[0])))
					{
						step1_count = 0;
						step1_ready = 1;
					}
				}
				if(step1_ready && !step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					Arm_Pos[2] = up[up_count][1] + step_2[step2_count][1];
					Arm_Pos[3] = up[up_count][0] + step_2[step2_count][0];
					
					
					Arm_Pos[4] = up[up_count][0] + step_2[step2_count][0];
					Arm_Pos[5] = up[up_count][1] + step_2[step2_count][1];
					
					
					Arm_Pos[6] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[7] = up[up_count][0] + step_1[step1_count][0];
					
				
					Arm_Pos[0] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[1] = up[up_count][0] + step_1[step1_count][0];
					
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 1;
						step1_count = 0;
						step2_count = 0;
					}
				}
				else if(step1_ready && step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[6] = up[up_count][1] + step_2[step2_count][1];
					Arm_Pos[7] = up[up_count][0] + step_2[step2_count][0];
					
					
					Arm_Pos[0] = up[up_count][1] + step_2[step2_count][1];
					Arm_Pos[1] = up[up_count][0] + step_2[step2_count][0];
					
					
					Arm_Pos[2] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[3] = up[up_count][0] + step_1[step1_count][0];
					
					Arm_Pos[4] = up[up_count][0] + step_1[step1_count][0];
					Arm_Pos[5] = up[up_count][1] + step_1[step1_count][1];
					
					
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 0;
						step1_count = 0;
						step2_count = 0;
					}
				}
			}
#ifdef REMOTE_MODE
				else if(dbus.rc.ch0 < -100 && rc_get)  //Turn left
#else
				else if(robot_state == RIGHT)
#endif
			{
				if(!step1_ready)
				{
					step1_count++;
					
					
					Arm_Pos[2] = up[up_count][1] - step_1[step1_count][0];
					Arm_Pos[3] = up[up_count][0] - step_1[step1_count][1];
					
					
					Arm_Pos[4] = up[up_count][0] - step_1[step1_count][1];
					Arm_Pos[5] = up[up_count][1] - step_1[step1_count][0];
					
					if(step1_count == (sizeof(step_1)/sizeof(step_1[0])))
					{
						step1_count = 0;
						step1_ready = 1;
					}
				}
				if(step1_ready && !step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[2] = up[up_count][1] - step_2[step2_count][0];
					Arm_Pos[3] = up[up_count][0] - step_2[step2_count][1];
					
					
					Arm_Pos[4] = up[up_count][0] - step_2[step2_count][1];
					Arm_Pos[5] = up[up_count][1] - step_2[step2_count][0];
					
					
					Arm_Pos[0] = up[up_count][1] - step_1[step1_count][0];
					Arm_Pos[1] = up[up_count][0] - step_1[step1_count][1];
					
					
					Arm_Pos[6] = up[up_count][1] - step_1[step1_count][0];
					Arm_Pos[7] = up[up_count][0] - step_1[step1_count][1];
					
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 1;
						step1_count = 0;
						step2_count = 0;
					}
				}
				else if(step1_ready && step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					
					Arm_Pos[0] = up[up_count][1] - step_2[step2_count][0];
					Arm_Pos[1] = up[up_count][0] - step_2[step2_count][1];
					
					
					Arm_Pos[6] = up[up_count][1] - step_2[step2_count][0];
					Arm_Pos[7] = up[up_count][0] - step_2[step2_count][1];
					
					
					Arm_Pos[2] = up[up_count][1] - step_1[step1_count][0];
					Arm_Pos[3] = up[up_count][0] - step_1[step1_count][1];
					
					
					Arm_Pos[4] = up[up_count][0] - step_1[step1_count][1];
					Arm_Pos[5] = up[up_count][1] - step_1[step1_count][0];
					
					
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 0;
						step1_count = 0;
						step2_count = 0;
					}
				}
			}
			//else if(robot_state == CROSS)
		else if(dbus.rc.ch1 > 100 && rc_get)
			{
				if((temp % 4) == 0)
				{
					if(!cross_step1_ready && !cross_step2_ready && !cross_step3_ready)
				{
					cross_step_count++;
					
					Arm_Pos[4] = up[up_count][0] - cross_step[cross_step_count][1];
					Arm_Pos[5] = up[up_count][1] - cross_step[cross_step_count][0];
					
//					Arm_Pos[2] = up[up_count][1] + step_1[step1_count][1];
//					Arm_Pos[3] = up[up_count][0] + step_1[step1_count][0];
					if(cross_step_count == (sizeof(cross_step)/sizeof(cross_step[0])))
					{
						cross_step_count = 0;
						cross_step1_ready = 1;
						cross_ready_step1 = Arm_Pos[4];
						cross_ready_step2 = Arm_Pos[5];
					}
				}
				else if(cross_step1_ready && !cross_step2_ready && !cross_step3_ready)
				{
					cross_step_count++;
						Arm_Pos[6] = up[up_count][1] + cross_step[cross_step_count][1];
						Arm_Pos[7] = up[up_count][0] + cross_step[cross_step_count][0];
						if(cross_step_count == (sizeof(cross_step)/sizeof(cross_step[0])))
					{
						cross_step_count = 0;
						cross_step2_ready = 1;
						cross_ready_step3 = Arm_Pos[6];
						cross_ready_step4 = Arm_Pos[7];
					}
				}
				else if(cross_step1_ready && cross_step2_ready && !cross_step3_ready)
				{
					if(cross_walking_count > 200) // 300 --- 3S
					{
						cross_walking_count = 0;
						cross_step3_ready = 1;
					}
					cross_walking_count++;
								if(!step1_ready)
				{
					step1_count++;
					Arm_Pos[4] = cross_ready_step1 - step_1[step1_count][1];
					Arm_Pos[5] = cross_ready_step2 - step_1[step1_count][0];
					
					Arm_Pos[2] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[3] = up[up_count][0] + step_1[step1_count][0];
					if(step1_count == (sizeof(step_1)/sizeof(step_1[0])))
					{
						step1_count = 0;
						step1_ready = 1;
					}
				}
				if(step1_ready && !step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[4] = cross_ready_step1 - step_2[step2_count][1];
					Arm_Pos[5] = cross_ready_step2 - step_2[step2_count][0];
					
					Arm_Pos[2] = up[up_count][1] + step_2[step2_count][1];
					Arm_Pos[3] = up[up_count][0] + step_2[step2_count][0];
					
					Arm_Pos[0] = up[up_count][1] - step_1[step1_count][0];
					Arm_Pos[1] = up[up_count][0] - step_1[step1_count][1];
					
					Arm_Pos[6] =cross_ready_step3 + step_1[step1_count][1];
					Arm_Pos[7] = cross_ready_step4 + step_1[step1_count][0];
					
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 1;
						step1_count = 0;
						step2_count = 0;
					}
				}
				else if(step1_ready && step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[4] = cross_ready_step1 - step_1[step1_count][1];
					Arm_Pos[5] = cross_ready_step2 - step_1[step1_count][0];
					
					Arm_Pos[2] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[3] = up[up_count][0] + step_1[step1_count][0];
					
					Arm_Pos[0] = up[up_count][1] - step_2[step2_count][0];
					Arm_Pos[1] = up[up_count][0] - step_2[step2_count][1];
					
					Arm_Pos[6] = cross_ready_step3 + step_2[step2_count][1];
					Arm_Pos[7] = cross_ready_step4 + step_2[step2_count][0];
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 0;
						step1_count = 0;
						step2_count = 0;
					}
				}
				}
				else if(cross_step1_ready && cross_step2_ready && cross_step3_ready && !cross_step4_ready)
				{
						cross_step_count++;
					
					Arm_Pos[4] = up[up_count][0];
					Arm_Pos[5] = up[up_count][1];
					
					
					Arm_Pos[2] = up[up_count][1] + cross_step[cross_step_count][1];
					Arm_Pos[3] = up[up_count][0] + cross_step[cross_step_count][0];
//					Arm_Pos[2] = up[up_count][1] + step_1[step1_count][1];
//					Arm_Pos[3] = up[up_count][0] + step_1[step1_count][0];
					if(cross_step_count == (sizeof(cross_step)/sizeof(cross_step[0])))
					{
						cross_step_count = 0;
						cross_step4_ready = 1;
						cross_ready_step1 = Arm_Pos[2];
						cross_ready_step2 = Arm_Pos[3];
					}
				}
				else if(cross_step4_ready && !cross_step5_ready)
				{
					cross_step_count++;
					Arm_Pos[0] = up[up_count][1] - cross_step[cross_step_count][0];
					Arm_Pos[1] = up[up_count][0] - cross_step[cross_step_count][1];
					if(cross_step_count == (sizeof(cross_step)/sizeof(cross_step[0])))
					{
						cross_step_count = 0;
						cross_step5_ready = 1;
						cross_ready_step3 = Arm_Pos[0];
						cross_ready_step4 = Arm_Pos[1];
					}
				}
				else if(cross_step5_ready && !cross_step6_ready)
				{
					if(cross_walking_count > 200) // 300 --- 3S
					{
						cross_walking_count = 0;
						cross_step6_ready = 1;
					}
					cross_walking_count++;
								if(!step1_ready)
				{
					step1_count++;
					Arm_Pos[4] = up[up_count][0] - step_1[step1_count][1];
					Arm_Pos[5] = up[up_count][1] - step_1[step1_count][0];
					
					Arm_Pos[2] = cross_ready_step1 + step_1[step1_count][1];
					Arm_Pos[3] = cross_ready_step2 + step_1[step1_count][0];
					if(step1_count == (sizeof(step_1)/sizeof(step_1[0])))
					{
						step1_count = 0;
						step1_ready = 1;
					}
				}
				if(step1_ready && !step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[4] = up[up_count][0] - step_2[step2_count][1];
					Arm_Pos[5] = up[up_count][1] - step_2[step2_count][0];
					
					Arm_Pos[2] = cross_ready_step1 + step_2[step2_count][1];
					Arm_Pos[3] = cross_ready_step2 + step_2[step2_count][0];
					
					Arm_Pos[0] = cross_ready_step3 - step_1[step1_count][0];
					Arm_Pos[1] = cross_ready_step4 - step_1[step1_count][1];
					
					Arm_Pos[6] = up[up_count][1] + step_1[step1_count][1];
					Arm_Pos[7] = up[up_count][0] + step_1[step1_count][0];
					
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 1;
						step1_count = 0;
						step2_count = 0;
					}
				}
				else if(step1_ready && step2_ready)
				{
					if(step2_count < (sizeof(step_2)/sizeof(step_2[0])))
							step2_count++;
					if(step1_count < (sizeof(step_1)/sizeof(step_1[0])))
							step1_count++;
					
					Arm_Pos[4] = up[up_count][0] - step_1[step1_count][1];
					Arm_Pos[5] = up[up_count][1] - step_1[step1_count][0];
					
					Arm_Pos[2] = cross_ready_step1 + step_1[step1_count][1];
					Arm_Pos[3] = cross_ready_step2 + step_1[step1_count][0];
					
					Arm_Pos[0] = cross_ready_step3 - step_2[step2_count][0];
					Arm_Pos[1] = cross_ready_step4 - step_2[step2_count][1];
					
					Arm_Pos[6] = up[up_count][1]  + step_2[step2_count][1];
					Arm_Pos[7] = up[up_count][0]  + step_2[step2_count][0];
					if((step2_count == (sizeof(step_2)/sizeof(step_2[0]))) && (step1_count == (sizeof(step_1)/sizeof(step_1[0]))))
					{
						step2_ready = 0;
						step1_count = 0;
						step2_count = 0;
					}
				}
				}
				
				else if(cross_step1_ready && cross_step2_ready && cross_step3_ready && cross_step4_ready && cross_step5_ready &&cross_step6_ready)
				{
					step1_ready = 0;
					step2_ready = 0;
					step1_count = 0;
					step2_count = 0;
					
					cross_step_ready = 1;
					cross_step1_ready = 0;
					cross_step2_ready = 0;
					cross_step3_ready = 0;
					
					cross_step4_ready = 0;
					cross_step5_ready = 0;
					cross_step6_ready = 0;
	//				cross_step4_ready = 0;
				}
			}
			}
	#ifdef REMOTE_MODE
			else
	#else
			else if(robot_state == IDLE)
	#endif
			{
				step1_ready = 0;
				step2_ready = 0;
				step1_count = 0;
				step2_count = 0;
				
				Arm_Pos[0] = up[up_count][1];
				Arm_Pos[1] = up[up_count][0];
				
				Arm_Pos[2] = up[up_count][1];
				Arm_Pos[3] = up[up_count][0];
				
				Arm_Pos[4] = up[up_count][0];
				Arm_Pos[5] = up[up_count][1];
				
				Arm_Pos[6] = up[up_count][1];
				Arm_Pos[7] = up[up_count][0];
			}
#endif
	//		printf("{%d,%d},",moto_chassis[0].total_angle,moto_chassis[1].total_angle);
		//	printf("500ms \n");
		}
		
			for(moto_count = 0;moto_count < 8;moto_count++)
		{
			Arm_Speed[moto_count] = pid_calc(&pid_Pos,moto_chassis[moto_count].total_angle,Arm_Pos[moto_count]);
			current_M3508[moto_count] = pid_calc(&pid_Speed,moto_chassis[moto_count].speed_rpm,Arm_Speed[moto_count]);	
		}
//				Arm_Pos[0] = 0;
//				Arm_Pos[1] = 0;
////			
//			
//			Arm_Speed[4] = pid_calc(&pid_Pos,moto_chassis[4].total_angle,Arm_Pos[4]);
//			Arm_Speed[5] = pid_calc(&pid_Pos,moto_chassis[5].total_angle,Arm_Pos[5]);
//			
//			current_M3508[4] = pid_calc(&pid_Speed,moto_chassis[4].speed_rpm,Arm_Speed[4]);	
//			current_M3508[5] = pid_calc(&pid_Speed,moto_chassis[5].speed_rpm,Arm_Speed[5]);	
//			
#ifndef CAPTURE_MOVE
			Set_Motor_Speed_Can(current_M3508[0],current_M3508[1],current_M3508[2],current_M3508[3],current_M3508[4],current_M3508[5],current_M3508[6],current_M3508[7]);
		//Set_Motor_Speed_Can(0,0,0,0,current_M3508[4],current_M3508[5],0,0);
#endif
			}
			TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
}
volatile u16 temp9 = 0;
void TIM1_BRK_TIM9_IRQHandler(void)
{
//		static u16 led_cnt;
	 if (TIM_GetITStatus(TIM9, TIM_IT_Update) != RESET) 
		 { 
//			 
//			 			temp9++; if(temp9>50000) temp9 = 0;
//		
//		
//			if(temp9 % 10 == 0) //50ms  2Hz
//			{
//				if(led_cnt++ >27)led_cnt = 0;
//				if(led_cnt<2)  RED_ON;
//				else if(led_cnt<5) RED_OFF;
//				else if(led_cnt<7) RED_ON;
//				else  RED_OFF;

//			}
			 TIM_ClearITPendingBit(TIM9, TIM_IT_Update);
		 }
}

//TIM14 PWM 部分初始化
//PWM 输出初始化
//arr：自动重装值 psc：时钟预分频数
void TIM5_PWM_Init(u32 arr,u32 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);//TIM14 时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); //使能 PORTF 时钟
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource10,GPIO_AF_TIM5); //GF9 复用为 TIM14
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource11,GPIO_AF_TIM5); //GF9 复用为 TIM14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11; //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //速度 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOH,&GPIO_InitStructure); //初始化 PF9
	TIM_TimeBaseStructure.TIM_Prescaler=psc; //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr; //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//STM32F4 开发指南(库函数版)

	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);//初始化定时器 14
	//初始化 TIM14 Channel1 PWM 模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM 调制模式 1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性低
	TIM_OC1Init(TIM5, &TIM_OCInitStructure); //初始化外设 TIM1 4OC1
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable); //使能预装载寄存器
	TIM_OC2Init(TIM5, &TIM_OCInitStructure); //初始化外设 TIM1 4OC1
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable); //使能预装载寄存器
	TIM_ARRPreloadConfig(TIM5,ENABLE);//ARPE 使能
	TIM_Cmd(TIM5, ENABLE); //使能 TIM14
}
