
#ifndef HAL_HARDWARE_IIC_H_
#define HAL_HARDWARE_IIC_H_

#include "derivative.h"
//#include "config.h"

/*******************************************************************************
* Constants and Macros
*******************************************************************************/

/*******************************************************************************
* Types
*******************************************************************************/
typedef enum
{
	I2C_STAGE_NONE = 0,
	I2C_STAGE_WRITE_DATA,
	I2C_STAGE_WRITE_REG_ADDRESS_HIGH,
	I2C_STAGE_WRITE_REG_ADDRESS_LOW,
	I2C_STAGE_REPEATED_START,
	I2C_STAGE_READ_DUMMY_DATA,
	I2C_STAGE_READ_DATA,
	I2C_STAGE_NAK,
	I2C_STAGE_ARBITRATION_LOST
}I2C_STAGE;   // transmission stages

typedef enum
{
	I2C_MODE_WRITE = 0,
	I2C_MODE_READ
}I2C_MODE;

typedef enum
{
	I2C_FLAG_NONE = 0,
	I2C_FLAG_TRANSMISSION_PROGRESS,
	I2C_FLAG_TRANSMISSION_COMPLETE
}I2C_FLAG;

typedef enum
{
	I2C_NO_FAULT = 0,
	I2C_FRAME_NO_ENDED,
	I2C_NO_ACK,
	I2C_BUS_BUSY,
	I2C_ARBITRATION_LOST
}I2C_FAULT;

typedef enum
{
	I2C_SPEED_100K,
	I2C_SPEED_200K,
	I2C_SPEED_400K
}I2C_SPEED;

typedef union
{
    uint16_t addr_w;
    struct{
        uint8_t h;
        uint8_t l;
    }addr_b;
}I2C_ADDR;

/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Global function prototypes
*******************************************************************************/

void i2c_0_init(I2C_SPEED speed);
I2C_FAULT i2c_0_write_block(uint8_t dev_addr, uint16_t reg_addr,
	uint8_t reg_addr_nBytes, uint8_t *pData, uint8_t nBytes);
I2C_FAULT i2c_0_read_block_defined_addr(uint8_t dev_addr, uint16_t reg_addr,
	uint8_t reg_addr_nBytes, uint8_t *pData, uint8_t nBytes);
I2C_FAULT i2c_0_read_block_preset_addr(uint8_t dev_addr, uint8_t *pData,
	uint8_t nBytes);

void i2c_3_init(I2C_SPEED speed);
I2C_FAULT i2c_3_write_block(uint8_t dev_addr, uint16_t reg_addr,
	uint8_t reg_addr_nBytes, uint8_t *pData, uint8_t nBytes);
I2C_FAULT i2c_3_read_block_defined_addr(uint8_t dev_addr, uint16_t reg_addr,
	uint8_t reg_addr_nBytes, uint8_t *pData, uint8_t nBytes);
I2C_FAULT i2c_3_read_block_preset_addr(uint8_t dev_addr, uint8_t *pData,
	uint8_t nBytes);


uint16_t get_hardware_iic_value(void);
void    get_iic_pressure(uint16_t* pressure);
#endif /* HAL_HARDWARE_IIC_H_ */
