#ifndef __MAX30102_H
#define __MAX30102_H
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//Mini STM32������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/6/10 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
////////////////////////////////////////////////////////////////////////////////// 	  

   	   		   
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "main.h"

#define SCL_GPIO_Port      GPIOB
#define SDA_GPIO_Port      GPIOB
#define SCL_Pin            GPIO_PIN_5
#define SDA_Pin            GPIO_PIN_4


#define SCL_H              HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET)
#define SCL_L              HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET)
    
#define SDA_H              HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET)
#define SDA_L              HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET)
#if defined(MAX30102_INT_GPIO_Port) && defined(MAX30102_INT_Pin)
#define MAX_INT_H          HAL_GPIO_WritePin(MAX30102_INT_GPIO_Port, MAX30102_INT_Pin, GPIO_PIN_SET)
#define MAX_INT_read       HAL_GPIO_ReadPin(MAX30102_INT_GPIO_Port,MAX30102_INT_Pin)
#else
#define MAX_INT_H          ((void)0)
#define MAX_INT_read       GPIO_PIN_RESET
#endif
#define SCL_read           HAL_GPIO_ReadPin(SCL_GPIO_Port,SCL_Pin) 
#define SDA_read           HAL_GPIO_ReadPin(SDA_GPIO_Port,SDA_Pin) 
#define bool int
#define TRUE 1
#define FALSE 0 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
void IIC_Read_One_Byte(uint8_t daddr,uint8_t addr,uint8_t* data);

void IIC_WriteBytes(uint8_t WriteAddr,uint8_t* data,uint8_t dataLength);
void IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr,uint8_t* data,uint8_t dataLength);
#endif
















