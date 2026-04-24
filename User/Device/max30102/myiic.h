#ifndef __MAX30102_H
#define __MAX30102_H
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//IIC 驱动函数	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/6/10 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

   	   		   
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "main.h"

#define SCL_GPIO_Port      GPIOB;
#define SDA_GPIO_Port      GPIOB;
#define SCL_Pin            GPIO_PIN_5
#define SDA_Pin            GPIO_PIN_4


#define SCL_H              HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET)
#define SCL_L              HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET)
    
#define SDA_H              HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET)
#define SDA_L              HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET)
#define MAX_INT_H          HAL_GPIO_WritePin(MAX30102_INT_GPIO_Port, MAX30102_INT_Pin, GPIO_PIN_SET)
#define SCL_read           HAL_GPIO_ReadPin(SCL_GPIO_Port,SCL_Pin) 
#define SDA_read           HAL_GPIO_ReadPin(SDA_GPIO_Port,SDA_Pin) 
#define MAX_INT_read       HAL_GPIO_ReadPin(MAX30102_INT_GPIO_Port,MAX30102_INT_Pin) 
#define bool int
#define TRUE 1
#define FALSE 0 

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
void IIC_Read_One_Byte(uint8_t daddr,uint8_t addr,uint8_t* data);

void IIC_WriteBytes(uint8_t WriteAddr,uint8_t* data,uint8_t dataLength);
void IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr,uint8_t* data,uint8_t dataLength);
#endif
















