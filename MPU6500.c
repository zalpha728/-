#include "stm32f10x.h"
#include "Delay.h"
#include "MPU6500.h"

//软件I2C

//定义I2C引脚
#define 	SCL_Pin			GPIO_Pin_1
#define 	SCL_GPIO		GPIOA
#define 	SDA_Pin			GPIO_Pin_0
#define 	SDA_GPIO		GPIOA
#define		SCL_Clock		RCC_APB2Periph_GPIOA
#define		SDA_Clock		RCC_APB2Periph_GPIOA

//I2C通信时钟周期/延时时间
#define		Delay_time_us	1

//引脚初始化
void Soft_I2C_Init(void)
{
	RCC_APB2PeriphClockCmd(SCL_Clock, ENABLE);
	RCC_APB2PeriphClockCmd(SDA_Clock, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = SCL_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCL_GPIO, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SDA_Pin;
	GPIO_Init(SDA_GPIO, &GPIO_InitStructure);
	
	GPIO_WriteBit(SCL_GPIO, SCL_Pin, Bit_SET);
	GPIO_WriteBit(SDA_GPIO, SDA_Pin, Bit_SET);
}

//控制时钟线
void SCL_Switch(uint8_t BitVal)
{
	GPIO_WriteBit(SCL_GPIO, SCL_Pin, (BitAction)BitVal);
	Delay_us(Delay_time_us);
}

//控制数据线
void SDA_Switch(uint8_t BitVal)
{
	GPIO_WriteBit(SDA_GPIO, SDA_Pin, (BitAction)BitVal);
	Delay_us(Delay_time_us);
}

//读取数据线上的数据
uint8_t SDA_Read(void)
{
	uint8_t Bitvalue;
	Bitvalue = GPIO_ReadInputDataBit(SDA_GPIO, SDA_Pin);
	Delay_us(Delay_time_us);
	return Bitvalue;
}

//I2C起始条件
void Soft_I2C_Start(void)
{
	SDA_Switch(1);
	SCL_Switch(1);
	
	SDA_Switch(0);
	SCL_Switch(0);
}

//I2C终止条件
void Soft_I2C_Stop(void)
{
	SDA_Switch(0);
	
	SCL_Switch(1);
	SDA_Switch(1);
}

//I2C通信发送字节
void Soft_I2C_SendByte(uint8_t Byte)
{
	for(uint8_t i = 0; i < 8; i++)
	{
		SDA_Switch(Byte & (0x80 >> i));
		SCL_Switch(1);
		SCL_Switch(0);
	}
}

//I2C通信接收字节
uint8_t Soft_I2C_ReceiveByte(void)
{
	uint8_t Byte = 0x00;
	SDA_Switch(1);
	for(uint8_t i = 0; i < 8; i++)
	{
		SCL_Switch(1);
		if(SDA_Read() == 1)
			Byte = Byte | (0x80 >> i);
		SCL_Switch(0);
	}
	return Byte;
}

//I2C发送应答
void Soft_I2C_SendAck(uint8_t AckBit)
{
		SDA_Switch(AckBit);
		SCL_Switch(1);
		SCL_Switch(0);
}

//I2C接收应答
uint8_t Soft_I2C_ReceiveAck(void)
{
	uint8_t AckBit;
	SDA_Switch(1);
	SCL_Switch(1);
	AckBit = SDA_Read();
	SCL_Switch(0);
	return AckBit;
}

//指定地址写
void MPU6500_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	Soft_I2C_Start();
	Soft_I2C_SendByte(MPU6500_Address);
	Soft_I2C_ReceiveAck();
	Soft_I2C_SendByte(RegAddress);
	Soft_I2C_ReceiveAck();
	Soft_I2C_SendByte(Data);
	Soft_I2C_ReceiveAck();
	Soft_I2C_Stop();
}

//指定地址读
uint8_t MPU6500_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	Soft_I2C_Start();
	Soft_I2C_SendByte(MPU6500_Address);
	Soft_I2C_ReceiveAck();
	Soft_I2C_SendByte(RegAddress);
	Soft_I2C_ReceiveAck();
	
	Soft_I2C_Start();
	Soft_I2C_SendByte(MPU6500_Address | 0x01);
	Soft_I2C_ReceiveAck();
	Data = Soft_I2C_ReceiveByte();
	Soft_I2C_SendAck(1);
	Soft_I2C_Stop();
	
	return Data;
}

//MPU6500初始化
void MPU6500_Init(void)
{
	Soft_I2C_Init();
	MPU6500_WriteReg(MPU6500_PWR_MGMT_1, 0X01);
	MPU6500_WriteReg(MPU6500_PWR_MGMT_2, 0X00);
	MPU6500_WriteReg(MPU6500_SMPLRT_DIV, 0X09);
	MPU6500_WriteReg(MPU6500_CONFIG, 0X06);
	
	//Mode1,低量程模式
	MPU6500_WriteReg(MPU6500_GYRO_CONFIG, 0X00);
	MPU6500_WriteReg(MPU6500_ACCEL_CONFIG, 0X00);
	
	//Mode2,高量程模式
//	MPU6500_WriteReg(MPU6500_GYRO_CONFIG, 0X08);
//	MPU6500_WriteReg(MPU6500_ACCEL_CONFIG, 0X08);
}

//获取陀螺仪数据
void MPU6500_GetData(int16_t *Ax, int16_t *Ay, int16_t *Az, int16_t *Bx, int16_t *By, int16_t *Bz)
{
	uint8_t DataH, DataL;
	DataH = MPU6500_ReadReg(MPU6500_ACCEL_XOUT_H);
	DataL = MPU6500_ReadReg(MPU6500_ACCEL_XOUT_L);
	*Ax = (DataH << 8) | DataL;
	
	DataH = MPU6500_ReadReg(MPU6500_ACCEL_YOUT_H);
	DataL = MPU6500_ReadReg(MPU6500_ACCEL_YOUT_L);
	*Ay = (DataH << 8) | DataL;
	
	DataH = MPU6500_ReadReg(MPU6500_ACCEL_ZOUT_H);
	DataL = MPU6500_ReadReg(MPU6500_ACCEL_ZOUT_L);
	*Az = (DataH << 8) | DataL;
	
	DataH = MPU6500_ReadReg(MPU6500_GYRO_XOUT_H);
	DataL = MPU6500_ReadReg(MPU6500_GYRO_XOUT_L);
	*Bx = (DataH << 8) | DataL;
	
	DataH = MPU6500_ReadReg(MPU6500_GYRO_YOUT_H);
	DataL = MPU6500_ReadReg(MPU6500_GYRO_YOUT_L);
	*By = (DataH << 8) | DataL;
	
	DataH = MPU6500_ReadReg(MPU6500_GYRO_ZOUT_H);
	DataL = MPU6500_ReadReg(MPU6500_GYRO_ZOUT_L);
	*Bz = (DataH << 8) | DataL;
}
