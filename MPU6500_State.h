#ifndef MPU6500_STATE_H
#define MPU6500_STATE_H

void TIM2_Init(void);
void GetAngleSpeed(void);
void Angle_Reset(void);
void GetOlaAngle(void);

extern int16_t Ax, Ay, Az, Bx, By, Bz;
extern float X_AngleSpeed, Y_AngleSpeed, Z_AngleSpeed;
extern float X_Angle, Y_Angle, Z_Angle;
#endif
