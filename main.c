#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MPU6500.h"
#include "MPU6500_State.h"

int main(void)
{
	OLED_Init();
	Soft_I2C_Init();
	TIM2_Init();
	MPU6500_Init();
	Angle_Reset();
	while(1)
	{
//		GetAngleSpeed();
//		OLED_ShowSignedNum(1,1,Ax, 5);
//		OLED_ShowSignedNum(2,1,Ay, 5);
//		OLED_ShowSignedNum(3,1,Az, 5);
//		
//		OLED_ShowSignedNum(1,8,Bx, 4);
//		OLED_ShowSignedNum(2,8,By, 4);
//		OLED_ShowSignedNum(3,8,Bz, 4);
		
		OLED_ShowSignedNum(1,1,X_Angle,3);
		OLED_ShowSignedNum(2,1,Y_Angle,3);
		OLED_ShowSignedNum(3,1,Z_Angle,3);
		
	}
}
