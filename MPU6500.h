#ifndef MPU6500_H
#define MPU6500_H

#define		MPU6500_Address			0xD0
#define		MPU6500_PWR_MGMT_1		0X6B
#define		MPU6500_PWR_MGMT_2		0X6C
#define		MPU6500_SMPLRT_DIV		0x19
#define		MPU6500_CONFIG			0X1A
#define		MPU6500_GYRO_CONFIG 	0X1B
#define		MPU6500_ACCEL_CONFIG	0X1C

#define		MPU6500_ACCEL_XOUT_H	0X3B
#define		MPU6500_ACCEL_XOUT_L	0X3C
#define		MPU6500_ACCEL_YOUT_H	0X3D
#define		MPU6500_ACCEL_YOUT_L	0X3E
#define		MPU6500_ACCEL_ZOUT_H	0X3F
#define		MPU6500_ACCEL_ZOUT_L	0X40
#define		MPU6500_GYRO_XOUT_H		0X43
#define		MPU6500_GYRO_XOUT_L		0X44
#define		MPU6500_GYRO_YOUT_H		0X45
#define		MPU6500_GYRO_YOUT_L		0X46
#define		MPU6500_GYRO_ZOUT_H		0X47
#define		MPU6500_GYRO_ZOUT_L		0X48

void Soft_I2C_Init(void);

void SCL_Switch(uint8_t BitVal);
void SDA_Switch(uint8_t BitVal);
uint8_t SDA_Read(void);

void Soft_I2C_Start(void);
void Soft_I2C_Stop(void);
void Soft_I2C_SendByte(uint8_t Byte);
uint8_t Soft_I2C_ReceiveByte(void);
void Soft_I2C_SendAck(uint8_t AckBit);
uint8_t Soft_I2C_ReceiveAck(void);

void MPU6500_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t MPU6500_ReadReg(uint8_t RegAddress);

void MPU6500_Init(void);
void MPU6500_GetData(int16_t *Ax, int16_t *Ay, int16_t *Az, int16_t *Bx, int16_t *By, int16_t *Bz);

#endif
