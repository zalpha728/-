#include "stm32f10x.h"
#include "MPU6500.h"
#include "OLED.h"

//I2C姿态计算
//通过对角速度求和获取欧拉角

//加速度和角速度的最大量程，在初始化MPU6500时配置
//Mode1,低量程模式
	#define 	full_acce			19.6
	#define		full_angle_speed	250.0
//Mode2,高量程模式
//	#define 	full_acce			39.2
//	#define		full_angle_speed	500.0
//需要到MPU6500_Init中进行一些操作
//这两个量程应该就够用了，不够的话再说

//定时中断周期，每个周期对角速度求和获取角度
#define		Period				1000 - 1
#define		Prescaler			1800 - 1
#define		T					0.025

//定时中断初始化
void TIM2_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_InternalClockConfig(TIM2);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = Period;
    TIM_TimeBaseStructure.TIM_Prescaler = Prescaler;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_Cmd(TIM2, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//I2C读取到的数据，15位数据位，1位符号位
int16_t Ax, Ay, Az, Bx, By, Bz;
//转换到的角速度数据，单位：dps
float X_AngleSpeed, Y_AngleSpeed, Z_AngleSpeed;

//用来滤除静止时的杂波，当角速度小于设定值时，默认系统处于静止状态
#define Target_Angle 2

//获取角速度，更新到角速度变量总
//需要放到定时中断中执行，如果中断过多，可能导致系统崩溃，可以将这个函数拿到主函数中执行
void GetAngleSpeed(void)
{
	MPU6500_GetData(&Ax, &Ay, &Az, &Bx, &By, &Bz);
	X_AngleSpeed = Bx * full_angle_speed / 32768;
	Y_AngleSpeed = By * full_angle_speed / 32768;
	Z_AngleSpeed = Bz * full_angle_speed / 32768;
	if((X_AngleSpeed <= Target_Angle)&& (X_AngleSpeed >= -Target_Angle)) X_AngleSpeed = 0;
	if((Y_AngleSpeed <= Target_Angle)&& (Y_AngleSpeed >= -Target_Angle)) Y_AngleSpeed = 0;
	if((Z_AngleSpeed <= Target_Angle)&& (Z_AngleSpeed >= -Target_Angle)) Z_AngleSpeed = 0;
}
//x,y,z轴欧拉角，转动方向满足右手准则
float X_Angle, Y_Angle, Z_Angle;

//复位欧拉角
void Angle_Reset(void)
{
	X_Angle = 0;
	Y_Angle = 0;
	Z_Angle = 0;
}

//定时中断总持续计算，获取欧拉角
void GetOlaAngle(void)
{
	X_Angle += (X_AngleSpeed * T);
	Y_Angle += (Y_AngleSpeed * T);
	Z_Angle += (Z_AngleSpeed * T);
}


void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
		GetAngleSpeed();
		GetOlaAngle();

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
