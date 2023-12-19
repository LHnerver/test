#include "hardware_iic.h"
#include "linflexd_uart.h"

#define USE_I2C_0		0
#define USE_I2C_3		1

#define LTC2463_ADDR_1	0x28
#define LTC2463_ADDR_2	0xA8

typedef struct
{
	I2C_FLAG 	res_flg;
	I2C_STAGE 	stage;
	I2C_MODE 	mode;
	I2C_FAULT	fault;
	uint8_t		dev_addr;
	uint8_t 	nbytes;
	uint8_t 	*data_p;
	I2C_ADDR 	addr;
}I2C_STATE;

/*******************************************************************************
* Local variables
*******************************************************************************/
#if USE_I2C_0
I2C_STATE i2c_0_state;
#endif

#if USE_I2C_3
I2C_STATE i2c_3_state;
#endif

/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Local Function prototypes
*******************************************************************************/
#if USE_I2C_0
void i2c_0_callback(void);
#endif

#if USE_I2C_3
void i2c_3_callback(void);
#endif

/*******************************************************************************
* Local functions
*******************************************************************************/

/*******************************************************************************
* Global functions
*******************************************************************************/

#if USE_I2C_0
/*******************************************************************************
Function Name : i2c_0_init
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    : speed = transmission speed,kbps
Modifies      : NONE
Returns       : NONE
Notes         : Initialize I2C Module.
				Set transmission frequency based on given speed
Issues        : NONE
*******************************************************************************/
void i2c_0_init(I2C_SPEED speed)
{
	uint8_t divider;

	if (I2C_SPEED_400K == speed)
	{
		divider = 0x16;
	}
 	I2C_0.IBFD.B.IBC = divider;
 	// Set module's I2C address to 0x00
  	I2C_0.IBAD.R = 0x00;
  	// Clear the IBAL, IBIF flags;
  	I2C_0.IBSR.R = 0x12;
  	// I-Bus Interrupt Enable
 	I2C_0.IBCR.B.IBIE = 1;
 	// Enable I2C operations
    I2C_0.IBCR.B.MDIS = 0;
}

/*******************************************************************************
Function Name : i2c_0_write_block
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    : dev_addr = address selecting particular I2C device
				reg_addr = address selecting register address or memory address
    			reg_addr_nBytes: address mode 1=8bit, 2=16bit, rest is invalid
    			*pData = pointer to first data item
    			nBytes = data length in bytes
Modifies      : NONE
Returns       : 0=OK, 1=FRAME NO ENDED, 2=NO ACK, 3=BUS BUSY, 4=ARBITRATION LOST
Notes         :
Issues        : NONE
*******************************************************************************/
I2C_FAULT i2c_0_write_block(uint8_t dev_addr, uint16_t reg_addr,
	uint8_t reg_addr_nBytes, uint8_t *pData, uint8_t nBytes)
{

	if (i2c_0_state.res_flg == I2C_FLAG_NONE)
    {
		// address to select a slave
		i2c_0_state.dev_addr = dev_addr;
		// slave register/memory address to be written
		i2c_0_state.addr.addr_w = reg_addr;
		// pointer to data to be written
		i2c_0_state.data_p = pData;
		// number of bytes to be written
		i2c_0_state.nbytes = nBytes;

		// check if bus is busy
		if(I2C_0.IBSR.B.IBB == 1)
	    {
	    	i2c_0_state.fault = I2C_BUS_BUSY;
	    	return(i2c_0_state.fault);
	    }

	    if(reg_addr_nBytes == 2)
	    	i2c_0_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_HIGH;
		if(reg_addr_nBytes == 1)
			i2c_0_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_LOW;

	    i2c_0_state.mode = I2C_MODE_WRITE;

	    i2c_0_state.res_flg = I2C_FLAG_TRANSMISSION_PROGRESS;
	    i2c_0_state.fault = I2C_FRAME_NO_ENDED;

	    // Set transmit and master mode
	    I2C_0.IBCR.B.MSSL = 1;
	    // And generate start condition
	    I2C_0.IBCR.B.TXRX = 1;

	    // Send device address
	    I2C_0.IBDR.R = i2c_0_state.dev_addr;

	    // if interrupt is not used
		if (I2C_0.IBCR.B.IBIE == 0)
		{
			while (i2c_0_state.res_flg == I2C_FLAG_TRANSMISSION_PROGRESS)
			{
				// Wait until IBIF;
				while (I2C_0.IBSR.B.IBIF == 0){};
				i2c_0_callback();
			}
		}
    }

    if (i2c_0_state.res_flg == I2C_FLAG_TRANSMISSION_COMPLETE)
    {
    	i2c_0_state.res_flg = I2C_FLAG_NONE;
    	i2c_0_state.fault = I2C_NO_FAULT;

    	if (i2c_0_state.stage == I2C_STAGE_NAK)
    		i2c_0_state.fault = I2C_NO_ACK;
    	if (i2c_0_state.stage == I2C_STAGE_ARBITRATION_LOST)
    		i2c_0_state.fault = I2C_ARBITRATION_LOST;
    }

    return(i2c_0_state.fault);

}

/*******************************************************************************
Function Name : i2c_0_read_block_defined_addr
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    : dev_addr = address selecting particular I2C device
				reg_addr = address selecting register address or memory address
    			reg_addr_nBytes: address mode 1=8bit, 2=16bit, rest is invalid
    			*pData = pointer to first data item
    			nBytes = data length in bytes
Modifies      :
Returns       : 0=OK, 1=FRAME NO ENDED, 2=NO ACK, 3=BUS BUSY, 4=ARBITRATION LOST
Notes         :
Issues        :
*******************************************************************************/
I2C_FAULT i2c_0_read_block_defined_addr(uint8_t dev_addr, uint16_t reg_addr,
	uint8_t reg_addr_nBytes, uint8_t *pData, uint8_t nBytes)
{
	if (i2c_0_state.res_flg == I2C_FLAG_NONE)
    {
		// address to select a slave
		i2c_0_state.dev_addr = dev_addr;
		// slave register/memory address to be read
		i2c_0_state.addr.addr_w = reg_addr;
		// pointer to data buffer to be filled
		i2c_0_state.data_p = pData;
		// number of bytes to be read
		i2c_0_state.nbytes = nBytes;

		// check if bus is busy
		if (I2C_0.IBSR.B.IBB == 1)
	    {
	    	i2c_0_state.fault = I2C_BUS_BUSY;
	    	return(i2c_0_state.fault);
	    }

	    if (reg_addr_nBytes == 2)
	    	i2c_0_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_HIGH;
		if (reg_addr_nBytes == 1)
			i2c_0_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_LOW;

	    i2c_0_state.mode = I2C_MODE_READ;

	    i2c_0_state.res_flg = I2C_FLAG_TRANSMISSION_PROGRESS;
	    i2c_0_state.fault = I2C_FRAME_NO_ENDED;

	    // Set transmit and master mode
	    I2C_0.IBCR.B.MSSL = 1;
	    // And generate start condition
	    I2C_0.IBCR.B.TXRX = 1;

	    // Send device address
	    I2C_0.IBDR.R = i2c_0_state.dev_addr;

	    // if interrupt is not used
		if (I2C_0.IBCR.B.IBIE == 0)
		{
			while (i2c_0_state.res_flg == I2C_FLAG_TRANSMISSION_PROGRESS)
			{
				// Wait until IBIF;
				while (I2C_0.IBSR.B.IBIF==0){};
				i2c_0_callback();
			}
		}
    }

    if (i2c_0_state.res_flg == I2C_FLAG_TRANSMISSION_COMPLETE)
    {
    	i2c_0_state.res_flg = I2C_FLAG_NONE;
    	i2c_0_state.fault = I2C_NO_FAULT;

    	if (i2c_0_state.stage == I2C_STAGE_NAK)
    		i2c_0_state.fault = I2C_NO_ACK;
    	if (i2c_0_state.stage == I2C_STAGE_ARBITRATION_LOST)
    		i2c_0_state.fault = I2C_ARBITRATION_LOST;
    }

    return(i2c_0_state.fault);
}

/*******************************************************************************
Function Name : i2c_0_read_block_preset_addr
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    : dev_addr = address selecting particular I2C device
				*pData = pointer to first data item
    			nBytes = data length in bytes
Modifies      :
Returns       : 0=OK, 1=FRAME NO ENDED, 2=NO ACK, 3=BUS BUSY, 4=ARBITRATION LOST
Notes         :
Issues        :
*******************************************************************************/
I2C_FAULT i2c_0_read_block_preset_addr(uint8_t dev_addr, uint8_t *pData, uint8_t nBytes)
{

	if (i2c_0_state.res_flg == I2C_FLAG_NONE)
    {
		// address to select a slave
	    i2c_0_state.dev_addr = dev_addr | 1;
	    // pointer to data buffer to be filled
		i2c_0_state.data_p = pData;
		// number of bytes to be read
		i2c_0_state.nbytes = nBytes;

		// check if bus is busy
		if (I2C_0.IBSR.B.IBB == 1)
	    {
	    	i2c_0_state.fault = I2C_BUS_BUSY;
	    	return(i2c_0_state.fault);
	    }

		i2c_0_state.stage = I2C_STAGE_READ_DUMMY_DATA;

	    i2c_0_state.res_flg = I2C_FLAG_TRANSMISSION_PROGRESS;
	    i2c_0_state.fault = I2C_FRAME_NO_ENDED;

	    // Set transmit and master mode
	    I2C_0.IBCR.B.MSSL = 1;
	    // And generate start condition
	    I2C_0.IBCR.B.TXRX = 1;

	    // Send device address
	    I2C_0.IBDR.R = i2c_0_state.dev_addr;

	    // if interrupt is not used
		if (I2C_0.IBCR.B.IBIE == 0)
		{
			while (i2c_0_state.res_flg == I2C_FLAG_TRANSMISSION_PROGRESS)
			{
				// Wait until IBIF;
				while (I2C_0.IBSR.B.IBIF == 0){};
				i2c_0_callback();
			}
		}
    }

    if (i2c_0_state.res_flg == I2C_FLAG_TRANSMISSION_COMPLETE)
    {
    	i2c_0_state.res_flg = I2C_FLAG_NONE;
    	i2c_0_state.fault = I2C_NO_FAULT;

    	if (i2c_0_state.stage == I2C_STAGE_NAK)
    		i2c_0_state.fault = I2C_NO_ACK;
    	if (i2c_0_state.stage == I2C_STAGE_ARBITRATION_LOST)
    		i2c_0_state.fault = I2C_ARBITRATION_LOST;
    }

    return(i2c_0_state.fault);
}

/*******************************************************************************
Function Name : i2c_0_callback
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    :
Modifies      :
Returns       :
Notes         : Follows RM's Flow-Chart of Typical I2C Interrupt Routine
Issues        :
*******************************************************************************/
void i2c_0_callback(void)
{
	uint8_t __attribute__ ((unused)) dummy;
	uint8_t status;

	status = I2C_0.IBSR.R;

	// if master mode
	if (I2C_0.IBCR.B.MSSL)
	{
		// if TX
		if (I2C_0.IBCR.B.TXRX)
        {
			// if ACK received, RXAK == 0
        	if ((status & 1) == 0)
    		{
    			switch (i2c_0_state.stage)
    			{
    				case I2C_STAGE_WRITE_REG_ADDRESS_HIGH:
					{
						// Send upper byte of word address;
						I2C_0.IBDR.R = i2c_0_state.addr.addr_b.h;
						i2c_0_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_LOW;

						break;
					}
					case I2C_STAGE_WRITE_REG_ADDRESS_LOW:
					{
						// Send lower byte of word address;
						I2C_0.IBDR.R = i2c_0_state.addr.addr_b.l;

						if (i2c_0_state.mode == 0)
							i2c_0_state.stage = I2C_STAGE_WRITE_DATA;
						else
							i2c_0_state.stage = I2C_STAGE_REPEATED_START;

						break;
					}
    				case I2C_STAGE_REPEATED_START:
					{
						// generate repeated START
						I2C_0.IBCR.B.RSTA = 1;
						// Send byte Control
						I2C_0.IBDR.R = i2c_0_state.dev_addr | 1;
						i2c_0_state.stage = I2C_STAGE_READ_DUMMY_DATA;

						break;
					}
					case I2C_STAGE_READ_DUMMY_DATA:
					{
						// set for receive
						I2C_0.IBCR.B.TXRX = 0;
						// if just 1 byte is going to be read
						// NOACK next received byte
						if (i2c_0_state.nbytes == 1)
							I2C_0.IBCR.B.NOACK = 1;

						// initiates next byte data receiving
						dummy = I2C_0.IBDR.R;

						break;
					}
    				case I2C_STAGE_WRITE_DATA:
					{
						// if not last byte TXed
						if (i2c_0_state.nbytes)
						{
							// write data;
							I2C_0.IBDR.R = *i2c_0_state.data_p++;
							i2c_0_state.nbytes--;
						}
						else
						{
							// last byte TXed
							// Generate stop signal;
							I2C_0.IBCR.B.MSSL = 0;
							i2c_0_state.stage = I2C_STAGE_NONE;
							// end of I2C transfer
							i2c_0_state.res_flg = I2C_FLAG_TRANSMISSION_COMPLETE;
						}

						break;
					}
    				default:
    					break;
    			}
    		}
    		else
    		{
    			// byte not ACKed
    			// Generate stop signal;
    			I2C_0.IBCR.B.MSSL = 0;
    			i2c_0_state.stage = I2C_STAGE_NAK;
    			i2c_0_state.res_flg = I2C_FLAG_TRANSMISSION_COMPLETE;
    		}
        }
        else
        {
        	// if RX
           	if ((i2c_0_state.nbytes - 2) == 0)
        	{
           		// NOACK next received byte
        		I2C_0.IBCR.B.NOACK = 1;
        	}

        	if ((i2c_0_state.nbytes - 1) == 0)
			{
        		// Set IBCR, Generate stop signal;
				I2C_0.IBCR.B.MSSL = 0;
				// Reset the NOACK to default value
				I2C_0.IBCR.B.NOACK = 0;
				i2c_0_state.stage = I2C_STAGE_NONE;
				// end of I2C transfer
				i2c_0_state.res_flg = I2C_FLAG_TRANSMISSION_COMPLETE;
			}

        	// Read the received byte from slave
			*(i2c_0_state.data_p++) = I2C_0.IBDR.R;

			i2c_0_state.nbytes--;
        }
	}
	else
	{
		// if slave mode
		// if IBAL bit is set
		if (status & 0x10)
		{
			i2c_0_state.stage = I2C_STAGE_ARBITRATION_LOST;
			// end of I2C transfer
			i2c_0_state.res_flg = I2C_FLAG_TRANSMISSION_COMPLETE;
			// clear IBAL flag
			I2C_0.IBSR.R = 0x10;
		}
	}

	// clear IBIF flag
	I2C_0.IBSR.R = 0x2;
}
#endif


/*******************************************************************************
Function Name : i2c_3_init
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    : speed = transmission speed,kbps
Modifies      : NONE
Returns       : NONE
Notes         : Initialize I2C Module.
				Set transmission frequency based on given speed
Issues        : NONE
*******************************************************************************/
void i2c_3_init(I2C_SPEED speed)
{
	uint8_t divider;

	if (I2C_SPEED_400K == speed)
	{
		divider = 0x16;
	}
 	I2C_3.IBFD.B.IBC = divider;
 	// Set module's I2C address to 0x00
  	I2C_3.IBAD.R = 0x00;
  	// Clear the IBAL, IBIF flags;
  	I2C_3.IBSR.R = 0x12;
  	// I-Bus Interrupt Enable
 	I2C_3.IBCR.B.IBIE = 1;
 	// Enable I2C operations
    I2C_3.IBCR.B.MDIS = 0;
}

/*******************************************************************************
Function Name : i2c_3_write_block
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    : dev_addr = address selecting particular I2C device
				reg_addr = address selecting register address or memory address
    			reg_addr_nBytes: address mode 1=8bit, 2=16bit, rest is invalid
    			*pData = pointer to first data item
    			nBytes = data length in bytes
Modifies      :
Returns       : 0=OK, 1=FRAME NO ENDED, 2=NO ACK, 3=BUS BUSY, 4=ARBITRATION LOST
Notes         :
Issues        :
*******************************************************************************/
I2C_FAULT i2c_3_write_block(uint8_t dev_addr, uint16_t reg_addr,
	uint8_t reg_addr_nBytes, uint8_t *pData, uint8_t nBytes)
{

	if (i2c_3_state.res_flg == I2C_FLAG_NONE)
    {
		// address to select a slave
		i2c_3_state.dev_addr = dev_addr;
		// slave register/memory address to be written
		i2c_3_state.addr.addr_w = reg_addr;
		// pointer to data to be written
		i2c_3_state.data_p = pData;
		// number of bytes to be written
		i2c_3_state.nbytes = nBytes;

		// check if bus is busy
		if(I2C_3.IBSR.B.IBB == 1)
	    {
	    	i2c_3_state.fault = I2C_BUS_BUSY;
	    	return(i2c_3_state.fault);
	    }

	    if(reg_addr_nBytes == 2)
	    	i2c_3_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_HIGH;
		if(reg_addr_nBytes == 1)
			i2c_3_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_LOW;

	    i2c_3_state.mode = I2C_MODE_WRITE;

	    i2c_3_state.res_flg = I2C_FLAG_TRANSMISSION_PROGRESS;
	    i2c_3_state.fault = I2C_FRAME_NO_ENDED;

	    // Set transmit and master mode
	    I2C_3.IBCR.B.MSSL = 1;
	    // And generate start condition
	    I2C_3.IBCR.B.TXRX = 1;

	    // Send device address
	    I2C_3.IBDR.R = i2c_3_state.dev_addr;

	    // if interrupt is not used
		if (I2C_3.IBCR.B.IBIE == 0)
		{
			while (i2c_3_state.res_flg == I2C_FLAG_TRANSMISSION_PROGRESS)
			{
				// Wait until IBIF;
				while (I2C_3.IBSR.B.IBIF == 0){};
				i2c_3_callback();
			}
		}
    }

    if (i2c_3_state.res_flg == I2C_FLAG_TRANSMISSION_COMPLETE)
    {
    	i2c_3_state.res_flg = I2C_FLAG_NONE;
    	i2c_3_state.fault = I2C_NO_FAULT;

    	if (i2c_3_state.stage == I2C_STAGE_NAK)
    		i2c_3_state.fault = I2C_NO_ACK;
    	if (i2c_3_state.stage == I2C_STAGE_ARBITRATION_LOST)
    		i2c_3_state.fault = I2C_ARBITRATION_LOST;
    }

    return(i2c_3_state.fault);

}

/*******************************************************************************
Function Name : i2c_3_read_block_defined_addr
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    : dev_addr = address selecting particular I2C device
				reg_addr = address selecting register address or memory address
    			reg_addr_nBytes: address mode 1=8bit, 2=16bit, rest is invalid
    			*pData = pointer to first data item
    			nBytes = data length in bytes
Modifies      :
Returns       : 0=OK, 1=FRAME NO ENDED, 2=NO ACK, 3=BUS BUSY, 4=ARBITRATION LOST
Notes         :
Issues        :
*******************************************************************************/
I2C_FAULT i2c_3_read_block_defined_addr(uint8_t dev_addr, uint16_t reg_addr,
	uint8_t reg_addr_nBytes, uint8_t *pData, uint8_t nBytes)
{
	if (i2c_3_state.res_flg == I2C_FLAG_NONE)
    {
		// address to select a slave
		i2c_3_state.dev_addr = dev_addr;
		// slave register/memory address to be read
		i2c_3_state.addr.addr_w = reg_addr;
		// pointer to data buffer to be filled
		i2c_3_state.data_p = pData;
		// number of bytes to be read
		i2c_3_state.nbytes = nBytes;

		// check if bus is busy
		if (I2C_3.IBSR.B.IBB == 1)
	    {
	    	i2c_3_state.fault = I2C_BUS_BUSY;
	    	return(i2c_3_state.fault);
	    }

	    if (reg_addr_nBytes == 2)
	    	i2c_3_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_HIGH;
		if (reg_addr_nBytes == 1)
			i2c_3_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_LOW;

	    i2c_3_state.mode = I2C_MODE_READ;

	    i2c_3_state.res_flg = I2C_FLAG_TRANSMISSION_PROGRESS;
	    i2c_3_state.fault = I2C_FRAME_NO_ENDED;

	    // Set transmit and master mode
	    I2C_3.IBCR.B.MSSL = 1;
	    // And generate start condition
	    I2C_3.IBCR.B.TXRX = 1;

	    // Send device address
	    I2C_3.IBDR.R = i2c_3_state.dev_addr;

	    // if interrupt is not used
		if (I2C_3.IBCR.B.IBIE == 0)
		{
			while (i2c_3_state.res_flg == I2C_FLAG_TRANSMISSION_PROGRESS)
			{
				// Wait until IBIF;
				while (I2C_3.IBSR.B.IBIF==0){};
				i2c_3_callback();
			}
		}
    }

    if (i2c_3_state.res_flg == I2C_FLAG_TRANSMISSION_COMPLETE)
    {
    	i2c_3_state.res_flg = I2C_FLAG_NONE;
    	i2c_3_state.fault = I2C_NO_FAULT;

    	if (i2c_3_state.stage == I2C_STAGE_NAK)
    		i2c_3_state.fault = I2C_NO_ACK;
    	if (i2c_3_state.stage == I2C_STAGE_ARBITRATION_LOST)
    		i2c_3_state.fault = I2C_ARBITRATION_LOST;
    }

    return(i2c_3_state.fault);
}

/*******************************************************************************
Function Name : i2c_3_read_block_preset_addr
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    : dev_addr = address selecting particular I2C device
				*pData = pointer to first data item
    			nBytes = data length in bytes
Modifies      :
Returns       : 0=OK, 1=FRAME NO ENDED, 2=NO ACK, 3=BUS BUSY, 4=ARBITRATION LOST
Notes         :
Issues        :
*******************************************************************************/
I2C_FAULT i2c_3_read_block_preset_addr(uint8_t dev_addr, uint8_t *pData, uint8_t nBytes)
{

	if (i2c_3_state.res_flg == I2C_FLAG_NONE)
    {

		// address to select a slave
	    i2c_3_state.dev_addr = dev_addr | 1;
	    // pointer to data buffer to be filled
		i2c_3_state.data_p = pData;
		// number of bytes to be read
		i2c_3_state.nbytes = nBytes;

		// check if bus is busy
		if (I2C_3.IBSR.B.IBB == 1)
	    {
	    	i2c_3_state.fault = I2C_BUS_BUSY;
	    	return(i2c_3_state.fault);
	    }

		i2c_3_state.stage = I2C_STAGE_READ_DUMMY_DATA;

	    i2c_3_state.res_flg = I2C_FLAG_TRANSMISSION_PROGRESS;
	    i2c_3_state.fault = I2C_FRAME_NO_ENDED;

	    // Set transmit and master mode
	    I2C_3.IBCR.B.MSSL = 1;
	    // And generate start condition
	    I2C_3.IBCR.B.TXRX = 1;

	    // Send device address
	    I2C_3.IBDR.R = i2c_3_state.dev_addr;

	    // if interrupt is not used
		if (I2C_3.IBCR.B.IBIE == 0)
		{
			while (i2c_3_state.res_flg == I2C_FLAG_TRANSMISSION_PROGRESS)
			{
				// Wait until IBIF;
				while (I2C_3.IBSR.B.IBIF == 0){};
				i2c_3_callback();
			}
		}
    }

    if (i2c_3_state.res_flg == I2C_FLAG_TRANSMISSION_COMPLETE)
    {
    	i2c_3_state.res_flg = I2C_FLAG_NONE;
    	i2c_3_state.fault = I2C_NO_FAULT;

    	if (i2c_3_state.stage == I2C_STAGE_NAK)
    		i2c_3_state.fault = I2C_NO_ACK;
    	if (i2c_3_state.stage == I2C_STAGE_ARBITRATION_LOST)
    		i2c_3_state.fault = I2C_ARBITRATION_LOST;
    }

    return(i2c_3_state.fault);
}

/*******************************************************************************
Function Name : i2c_3_callback
Engineer      : suxiong
Date          : Jun-29-2021
Parameters    :
Modifies      :
Returns       :
Notes         : Follows RM's Flow-Chart of Typical I2C Interrupt Routine
Issues        :
*******************************************************************************/
void i2c_3_callback(void)
{
	uint8_t __attribute__ ((unused)) dummy;
	uint8_t status;

	status = I2C_3.IBSR.R;
    // uart8_printf("i2c_callback\n");
	// if master mode
	if (I2C_3.IBCR.B.MSSL)
	{
		// if TX
		if (I2C_3.IBCR.B.TXRX)
        {
			// if ACK received, RXAK == 0
        	if ((status & 1) == 0)
    		{
    			switch (i2c_3_state.stage)
    			{
    				case I2C_STAGE_WRITE_REG_ADDRESS_HIGH:
					{
						// Send upper byte of word address;
						I2C_3.IBDR.R = i2c_3_state.addr.addr_b.h;
						i2c_3_state.stage = I2C_STAGE_WRITE_REG_ADDRESS_LOW;

						break;
					}
					case I2C_STAGE_WRITE_REG_ADDRESS_LOW:
					{
						// Send lower byte of word address;
						I2C_3.IBDR.R = i2c_3_state.addr.addr_b.l;

						if (i2c_3_state.mode == 0)
							i2c_3_state.stage = I2C_STAGE_WRITE_DATA;
						else
							i2c_3_state.stage = I2C_STAGE_REPEATED_START;

						break;
					}
    				case I2C_STAGE_REPEATED_START:
					{
						// generate repeated START
						I2C_3.IBCR.B.RSTA = 1;
						// Send byte Control
						I2C_3.IBDR.R = i2c_3_state.dev_addr | 1;
						i2c_3_state.stage = I2C_STAGE_READ_DUMMY_DATA;

						break;
					}
					case I2C_STAGE_READ_DUMMY_DATA:
					{
						// set for receive
						I2C_3.IBCR.B.TXRX = 0;
						// if just 1 byte is going to be read
						// NOACK next received byte
						if (i2c_3_state.nbytes == 1)
							I2C_3.IBCR.B.NOACK = 1;

						// initiates next byte data receiving
						dummy = I2C_3.IBDR.R;

						break;
					}
    				case I2C_STAGE_WRITE_DATA:
					{
						// if not last byte TXed
						if (i2c_3_state.nbytes)
						{
							// write data;
							I2C_3.IBDR.R = *i2c_3_state.data_p++;
							i2c_3_state.nbytes--;
						}
						else
						{
							// last byte TXed
							// Generate stop signal;
							I2C_3.IBCR.B.MSSL = 0;
							i2c_3_state.stage = I2C_STAGE_NONE;
							// end of I2C transfer
							i2c_3_state.res_flg = I2C_FLAG_TRANSMISSION_COMPLETE;
						}

						break;
					}
    				default:
        				break;
    			}
    		}
    		else
    		{
    			// byte not ACKed
    			// Generate stop signal;
    			I2C_3.IBCR.B.MSSL = 0;
    			i2c_3_state.stage = I2C_STAGE_NAK;
    			i2c_3_state.res_flg = I2C_FLAG_TRANSMISSION_COMPLETE;
    		}
        }
        else
        {
        	// if RX
           	if ((i2c_3_state.nbytes - 2) == 0)
        	{
           		// NOACK next received byte
        		I2C_3.IBCR.B.NOACK = 1;
        	}

        	if ((i2c_3_state.nbytes - 1) == 0)
			{
        		// Set IBCR, Generate stop signal;
				I2C_3.IBCR.B.MSSL = 0;
				// Reset the NOACK to default value
				I2C_3.IBCR.B.NOACK = 0;
				i2c_3_state.stage = I2C_STAGE_NONE;
				// end of I2C transfer
				i2c_3_state.res_flg = I2C_FLAG_TRANSMISSION_COMPLETE;
			}

        	// Read the received byte from slave
			*(i2c_3_state.data_p++) = I2C_3.IBDR.R;

			i2c_3_state.nbytes--;
        }
	}
	else
	{
		// if slave mode
		// if IBAL bit is set
		if (status & 0x10)
		{
			i2c_3_state.stage = I2C_STAGE_ARBITRATION_LOST;
			// end of I2C transfer
			i2c_3_state.res_flg = I2C_FLAG_TRANSMISSION_COMPLETE;
			// clear IBAL flag
			I2C_3.IBSR.R = 0x10;
		}
	}

	// clear IBIF flag
	I2C_3.IBSR.R = 0x2;
}



uint16_t get_hardware_iic_value(void)
{

	uint16_t sampled_values = 0 ;
	I2C_FAULT i2c_fault = I2C_FRAME_NO_ENDED;
	uint8_t temp_data[2] = {0};

	while(I2C_FRAME_NO_ENDED == (i2c_fault = i2c_3_read_block_preset_addr(
			LTC2463_ADDR_2, temp_data, 2)));
	if (i2c_fault == I2C_NO_FAULT)
	{
		sampled_values = temp_data[0] << 8 | temp_data[1];

		return   sampled_values ;
	}
}

void   get_iic_pressure(uint16_t* pressure)
{
	uint16_t sampled_values = 0 ;
    uint16_t  voltage = 0 ;


	sampled_values = get_hardware_iic_value();

	voltage  =  ((sampled_values-32768)*125)/32768 ;  // À©´ó100±¶Êý

	*pressure = (voltage/2.5)*50 ;

//	uart8_printf("*pressure:%d\n",	*pressure);
//	uart8_printf("sampled_values :%d\n",sampled_values);
//	uart8_printf("voltage :%d\n",voltage);

}

