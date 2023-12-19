
#include "iic.h"
#include "MPC5748G.h"
 

void delay(void)
{
   unsigned char i;
   for (i=0; i<250; i++) 
   {
       ;
   }
}


void Init_IIC(void)    
{   
	I2C_0.IBFD.B.IBC = 0x16;
	I2C_0.IBAD.R = 0x00; 		// Set module's I2C address to 0x00

	I2C_0.IBSR.R = 0x12; 		// Clear the IBAL, IBIF flags;

 //	I2C_0.IBCR.B.IBIE =1;		// I-Bus Interrupt Enable
					// NOTE: assign I2C_0_Callback to interrupt vector
} 


void IIC_Enable(void) 
{
	I2C_0.IBCR.B.MDIS =0; 		// Enable I2C operations
	delay();
}

void IIC_Disenable(void) 
{
    I2C_0.IBCR.B.MDIS =1;
    delay();
}

void IIC_WR(unsigned char w) 
{
	I2C_0.IBSR.B.SRW = 0;
	I2C_0.IBCR.B.TXRX = w;
}

void IIC_Restart(void)
{
	I2C_0.IBCR.B.RSTA = 1;
    delay();
}

void IIC_Stop(void)
{ 
	I2C_0.IBCR.B.MSSL = 0;
   delay();
}

void IIC_Start(void)
{ 
   while(I2C_0.IBSR.B.IBB)
   {
      IIC_Stop();
   }
   I2C_0.IBCR.B.MSSL = 1;
   delay();
}

void IIC_ACK(unsigned char ack)
{
   I2C_0.IBCR.B.NOACK = ack;
   I2C_0.IBSR.B.IBIF = 1;
}

void IIC_Wait_ACK(void)
{                   
    while(I2C_0.IBSR.B.RXAK);       //等待应答
}

void IIC_Transimit_Data(unsigned char data)
{
	I2C_0.IBDR.R = data;
    while(!I2C_0.IBSR.B.IBIF);   //wait
    I2C_0.IBSR.B.IBIF = 1;
    IIC_ACK(0); 
} 

unsigned char IIC_Receive_Data()
{
    unsigned char temp = 0; 
    temp = I2C_0.IBDR.R;              //读一个字节数据
    while (!I2C_0.IBSR.B.IBIF);
    I2C_0.IBSR.B.IBIF = 1;
  	return temp;  
}

unsigned char IIC_Read_Byte(unsigned char addr,unsigned char reg)
{
	unsigned char __attribute__ ((unused)) dummy = 0;
	unsigned char data = 0;
    IIC_Enable();                    //使能IIC
    IIC_Start();                     //设为主模式,产生启动信号  
   
    IIC_WR(0x01);                    //发送
	  
  	IIC_Transimit_Data(addr);        //发送芯片ID,页地址，写命令 
  	
  	IIC_Transimit_Data(reg);         //发送页中地址
    
    IIC_Restart();                   //重新启动 
    
	  IIC_Transimit_Data(addr | 0x01);  //读命令
	  
	  IIC_WR(0x00);                    //读使能
	 
    dummy = IIC_Receive_Data();      //虚读
    
	  IIC_Wait_ACK();        
	
    IIC_ACK(0x01);                   //不产生应答
    
	  data = IIC_Receive_Data();       //读一个字节数据
    
    IIC_ACK(0x0);
    
    IIC_Stop();                      //产生停止信号     
    
    IIC_Disenable();       

    return data;  
}

void IIC_Receive(unsigned char addr,unsigned char reg, unsigned char *pData, unsigned char nBytes) 
{ 
	unsigned char __attribute__ ((unused)) dummy;
    unsigned char i; 
    
    IIC_Enable();
    
    IIC_Start();
    
    IIC_WR(0x01);
    
    IIC_Transimit_Data(addr);
    
    IIC_Transimit_Data(reg);
    
    IIC_Restart();                 
    
    IIC_Transimit_Data(addr|0x01);
                     
    IIC_WR(0x00);
    
    dummy = IIC_Receive_Data();       //虚读
	  IIC_Wait_ACK();        
	
    for (i=0; i<nBytes-2; i++)       //读前（length-2）个字节数据
    {
        *pData++ = IIC_Receive_Data();
    }

    *pData++ = IIC_Receive_Data();       //读第（length-1）个字节数据
  	IIC_Wait_ACK();
  	
  	IIC_ACK(0x01);                   //不产生应答
  	*pData = IIC_Receive_Data();         //读最后一个字节数据

	  
    IIC_Stop();                     //产生停止信号       
    
    IIC_Disenable();  
}

void IIC_Write_Byte(unsigned char addr,unsigned char reg,unsigned char data)
{
  	IIC_Enable();            //使能IIC
	  
	  IIC_ACK(0x00);            //产生应答信号    
    
    IIC_WR(0x01);             //发送
	  
    IIC_Start();              //设为主机模式,产生启动信号
	
  	IIC_Transimit_Data(addr);  //发送芯片ID,页地址，写命令 

  	IIC_Transimit_Data(reg);  //发送页中地址
  	
  	IIC_Transimit_Data(data);  //写数据

    IIC_Stop();               //停止信号        
    
    IIC_Disenable();                  
}

void IIC_Write_Data(unsigned char addr,unsigned char reg,unsigned char *pData,unsigned char nBytes)
{
    unsigned char i;      

    IIC_Enable();            //使能IIC
	  
	IIC_ACK(0x00);            //产生应答信号
    
    IIC_WR(0x01);             //发送
	  
    IIC_Start();              //设为主模式,产生启动信号
	
  	IIC_Transimit_Data(addr);  //发送芯片ID,页地址，写命令

  	IIC_Transimit_Data(reg);  //发送页中地址
  	
  	for (i=0; i<nBytes; i++)    //写length个数据
  	{
      	IIC_Transimit_Data(*pData);  //写数据
      	pData++; 
  	}
  	 
    IIC_Stop();                     //产生停止信号       
    
    IIC_Disenable();  
}


 
 
