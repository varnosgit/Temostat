/* USER CODE BEGIN Includes */
#include "lm75.h"

//#include "Command_Control.h"

/* External variables --------------------------------------------------------*/
//extern UART_HandleTypeDef huart2;
//extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;

extern char strd[40];
extern int _mainFlag[15];
extern int old_temp;

//*******(Serial Port Communication)****************


// Read 16-bit LM75 register
uint16_t LM75_ReadReg(uint8_t reg) 
{	
	uint8_t aTxBuffer[1]; 
	uint16_t aRxBuffer[1]; 
		
	aTxBuffer[0]=0x00; //Pointer buffer	
//	HAL_I2C_Master_Transmit(&hi2c1,(LM75_ADDR<<1), (uint8_t*)aTxBuffer, 1,100);
//	HAL_I2C_Master_Receive(&hi2c1,(LM75_ADDR<<1), (uint8_t*)aRxBuffer,1,100);
transfer_i2c_Low((LM75_ADDR<<1), (uint8_t*)aTxBuffer, 1);
recieve_i2c_Low((LM75_ADDR<<1), (uint8_t*)aRxBuffer, 1);
	return aRxBuffer[0];
}


// Read temperature readings from LM75 in decimal format
int16_t LM75_Temperature(void) 
{
	uint16_t raw =  LM75_ReadReg(LM75_REG_TEMP);
	
	int  temp= (raw & 0xFF);
//	uint16_t raw2 =  LM75_ReadReg(LM75_REG_TEMP);

	//raw = LM75_ReadReg(LM75_REG_TEMP) << 8;

/*	if(raw & 0x4000)
		temp=temp+64;
	if(raw & 0x2000)
		temp=temp+32;
	if(raw & 0x1000)
		temp=temp+16;
	if(raw & 0x0800)
		temp=temp+8;
	if(raw & 0x0400)
		temp=temp+4;
	if(raw & 0x0200)
		temp=temp+2;
	if(raw & 0x0100)
		temp=temp+1;
	
	if(raw & 0x8000)	//Negative Temperature
		temp=temp*-1;
*/
	return temp;
}


void Check_Temperature(void)
{
		int temp = LM75_Temperature();

		if(((old_temp-temp)>=1)||(temp-old_temp>=1))
				{
								if( _mainFlag[LS]==DLM)	 //If Steady to Display Parameters
										{
											send_data(temp+100);
											LCD_Digit(temp,'C');
										}						
				}
				
				/// Filter Initial State in DS75 Digital sensor
						if( _mainFlag[LS]==DLM)	 //If Steady to Display Parameters					
								old_temp=temp;
}

void transfer_i2c_Low(uint8_t SlaveAddrSize, uint8_t* mydata, uint8_t mydataLen)
{
	uint8_t data_counter = 0;
	//I2C_TypeDef *I2Cx, uint32_t SlaveAddr, uint32_t SlaveAddrSize, uint32_t TransferSize, uint32_t EndMode, uint32_t Request)
	LL_I2C_HandleTransfer(I2C1, SlaveAddrSize, LL_I2C_ADDRESSING_MODE_7BIT, mydataLen, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
	while (!LL_I2C_IsActiveFlag_STOP(I2C1))
	{
		if (LL_I2C_IsActiveFlag_TXE(I2C1))
		{
			LL_I2C_TransmitData8(I2C1, mydata[data_counter++]);
		}
	}
	LL_I2C_ClearFlag_STOP(I2C1);
	
}
/*
*
*/
void recieve_i2c_Low(uint8_t SlaveAddrSize, uint8_t* mydata, uint8_t mydataLen)
{
	uint8_t dataCounter =0;
	//I2C_TypeDef *I2Cx, uint32_t SlaveAddr, uint32_t SlaveAddrSize, uint32_t TransferSize, uint32_t EndMode, uint32_t Request)
	LL_I2C_HandleTransfer(I2C1, SlaveAddrSize, LL_I2C_ADDRESSING_MODE_7BIT, mydataLen, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
	while (!LL_I2C_IsActiveFlag_STOP(I2C1))
	{
		if (LL_I2C_IsActiveFlag_RXNE(I2C1))
		{
			mydata[dataCounter++] = LL_I2C_ReceiveData8(I2C1);
		}
	}
	dataCounter = 0;
	LL_I2C_ClearFlag_STOP(I2C1);
}

