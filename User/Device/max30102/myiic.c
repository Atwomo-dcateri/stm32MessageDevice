#include "myiic.h"
#include "stm32f1xx_hal.h"

//初始化IIC
//void IIC_Init(void)
//{					     
//	GPIO_InitTypeDef GPIO_InitStructure;
//	//RCC->APB2ENR|=1<<4;//先使能外设IO PORTC时钟 
//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
//	   
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
// 
//	IIC_SCL=1;
//	IIC_SDA=1;

//}
//产生IIC起始信号
void I2C_delay(void) 
{ 
   uint16_t i=60; //Set delay time value
   while(i)  
   {  
     i--;  
   }  
}

void IIC_Start(void)
{
	SDA_H;
	I2C_delay();
	SCL_H;
	I2C_delay();
	SDA_L;
	I2C_delay();
	SCL_L;
	I2C_delay();
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
    SCL_L;
    I2C_delay();	
	SDA_L; 
	I2C_delay();
	SCL_H; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay();							   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t re;
	SCL_L; 
	I2C_delay();
	SDA_H; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	if(SDA_read) 
	{ 
    re=1;   
	} 
	else re=0;
	SCL_L; 
	return re; 
	
//	uint8_t re;

//	SDA_L;	/* CPU释放SDA总线 */  //让它必须应答  自己修改的
//	I2C_delay();
//	SCL_H;	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
//	I2C_delay();
//	
//	re = SDA_read;/* CPU读取SDA口线状态 */

//	SCL_L;
//	I2C_delay();
//	return re;
} 
//产生ACK应答
void IIC_Ack(void)
{
    SCL_L; 
	I2C_delay(); 
	SDA_L; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SCL_L; 
	I2C_delay(); 
}
//不产生ACK应答		    
void IIC_NAck(void)
{
    SCL_L; 
	I2C_delay(); 
	SDA_H; 
	I2C_delay(); 
	SCL_H; 
	I2C_delay(); 
	SCL_L; 
	I2C_delay(); 
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t SendByte)
{                        
	uint8_t i=8;
	
  while(i--) 
  { 	
		SCL_L;
		I2C_delay(); 
    if((SendByte&0x80))
		{
     SDA_H; 
		}			
    else
		{			
     SDA_L; 
		}			
    SendByte<<=1; 
    I2C_delay(); 
	SCL_H; 
    I2C_delay(); 
		
  } 
	SCL_L; 	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i=8,receive=0; 	 
			SDA_H;	
    while(i--)
    {
        receive<<=1;      
        SCL_L;
        I2C_delay();
	      SCL_H;
        I2C_delay();
        if(SDA_read)
        {
          receive|=0x01;
        }
    }
     SCL_L;	
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


void IIC_WriteBytes(uint8_t WriteAddr,uint8_t* data,uint8_t dataLength)
{		
	uint8_t i;	
    IIC_Start();  

	IIC_Send_Byte(WriteAddr);	    //发送写命令
	IIC_Wait_Ack();
	
	for(i=0;i<dataLength;i++)
	{
		IIC_Send_Byte(data[i]);
		IIC_Wait_Ack();
	}				    	   
    IIC_Stop();//产生一个停止条件 
	I2C_delay(); 	 
}

void IIC_ReadBytes(uint8_t deviceAddr, uint8_t writeAddr,uint8_t* data,uint8_t dataLength)
{		
	uint8_t i;	
    IIC_Start();  

	IIC_Send_Byte(deviceAddr);	    //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(writeAddr);
	IIC_Wait_Ack();
	IIC_Send_Byte(deviceAddr|0X01);//进入接收模式			   
	IIC_Wait_Ack();
	
	for(i=0;i<dataLength-1;i++)
	{
		data[i] = IIC_Read_Byte(1);
	}		
	data[dataLength-1] = IIC_Read_Byte(0);	
    IIC_Stop();//产生一个停止条件 
	I2C_delay(); 	 
}

void IIC_Read_One_Byte(uint8_t daddr,uint8_t addr,uint8_t* data)
{				  	  	    																 
    IIC_Start();  
	
	IIC_Send_Byte(daddr);	   //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);//发送地址
	IIC_Wait_Ack();		 
	IIC_Start();  	 	   
	IIC_Send_Byte(daddr|0X01);//进入接收模式			   
	IIC_Wait_Ack();	 
    *data = IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
}

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data)
{				   	  	    																 
    IIC_Start();  
	
	IIC_Send_Byte(daddr);	    //发送写命令
	IIC_Wait_Ack();
	IIC_Send_Byte(addr);//发送地址
	IIC_Wait_Ack();	   	 										  		   
	IIC_Send_Byte(data);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	I2C_delay(); 	 
}



























