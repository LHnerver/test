#ifndef  _IIC_H_
#define  _IIC_H_


void Init_IIC(void); 

unsigned char IIC_Read_Byte(unsigned char addr, unsigned char reg);
void IIC_Receive(unsigned char addr,unsigned char reg, unsigned char *pData,
	unsigned char nBytes);
void IIC_Write_Byte(unsigned char addr, unsigned char reg, unsigned char data);
void IIC_Write_Data(unsigned char addr, unsigned char reg, unsigned char *pData,
	unsigned char nBytes);
 
#endif
