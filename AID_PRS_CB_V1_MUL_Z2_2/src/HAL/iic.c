
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
    while(I2C_0.IBSR.B.RXAK);       //�ȴ�Ӧ��
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
    temp = I2C_0.IBDR.R;              //��һ���ֽ�����
    while (!I2C_0.IBSR.B.IBIF);
    I2C_0.IBSR.B.IBIF = 1;
  	return temp;  
}

unsigned char IIC_Read_Byte(unsigned char addr,unsigned char reg)
{
	unsigned char __attribute__ ((unused)) dummy = 0;
	unsigned char data = 0;
    IIC_Enable();                    //ʹ��IIC
    IIC_Start();                     //��Ϊ��ģʽ,���������ź�  
   
    IIC_WR(0x01);                    //����
	  
  	IIC_Transimit_Data(addr);        //����оƬID,ҳ��ַ��д���� 
  	
  	IIC_Transimit_Data(reg);         //����ҳ�е�ַ
    
    IIC_Restart();                   //�������� 
    
	  IIC_Transimit_Data(addr | 0x01);  //������
	  
	  IIC_WR(0x00);                    //��ʹ��
	 
    dummy = IIC_Receive_Data();      //���
    
	  IIC_Wait_ACK();        
	
    IIC_ACK(0x01);                   //������Ӧ��
    
	  data = IIC_Receive_Data();       //��һ���ֽ�����
    
    IIC_ACK(0x0);
    
    IIC_Stop();                      //����ֹͣ�ź�     
    
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
    
    dummy = IIC_Receive_Data();       //���
	  IIC_Wait_ACK();        
	
    for (i=0; i<nBytes-2; i++)       //��ǰ��length-2�����ֽ�����
    {
        *pData++ = IIC_Receive_Data();
    }

    *pData++ = IIC_Receive_Data();       //���ڣ�length-1�����ֽ�����
  	IIC_Wait_ACK();
  	
  	IIC_ACK(0x01);                   //������Ӧ��
  	*pData = IIC_Receive_Data();         //�����һ���ֽ�����

	  
    IIC_Stop();                     //����ֹͣ�ź�       
    
    IIC_Disenable();  
}

void IIC_Write_Byte(unsigned char addr,unsigned char reg,unsigned char data)
{
  	IIC_Enable();            //ʹ��IIC
	  
	  IIC_ACK(0x00);            //����Ӧ���ź�    
    
    IIC_WR(0x01);             //����
	  
    IIC_Start();              //��Ϊ����ģʽ,���������ź�
	
  	IIC_Transimit_Data(addr);  //����оƬID,ҳ��ַ��д���� 

  	IIC_Transimit_Data(reg);  //����ҳ�е�ַ
  	
  	IIC_Transimit_Data(data);  //д����

    IIC_Stop();               //ֹͣ�ź�        
    
    IIC_Disenable();                  
}

void IIC_Write_Data(unsigned char addr,unsigned char reg,unsigned char *pData,unsigned char nBytes)
{
    unsigned char i;      

    IIC_Enable();            //ʹ��IIC
	  
	IIC_ACK(0x00);            //����Ӧ���ź�
    
    IIC_WR(0x01);             //����
	  
    IIC_Start();              //��Ϊ��ģʽ,���������ź�
	
  	IIC_Transimit_Data(addr);  //����оƬID,ҳ��ַ��д����

  	IIC_Transimit_Data(reg);  //����ҳ�е�ַ
  	
  	for (i=0; i<nBytes; i++)    //дlength������
  	{
      	IIC_Transimit_Data(*pData);  //д����
      	pData++; 
  	}
  	 
    IIC_Stop();                     //����ֹͣ�ź�       
    
    IIC_Disenable();  
}


 
 