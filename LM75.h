#include <Arduino.h>

/* LM75 defines */
#define LM75_ADDR                     0x49 // LM75 address

/* LM75 registers */
/* LM75 registers */
#define LM75_REG_TEMP                 0x00 // Temperature
//#define LM75_REG_CONF                 0x01 // Configuration
//#define LM75_REG_THYS                 0x02 // Hysteresis
//#define LM75_REG_TOS                  0x03 // Overtemperature shutdown


uint16_t LM75_ReadReg(uint8_t reg);
int16_t LM75_Temperature(void);
void Check_Temperature(void);
//void send_data(int temp);

void transfer_i2c_Low(uint8_t SlaveAddrSize, uint8_t* mydata, uint8_t mydataLen);
void recieve_i2c_Low(uint8_t SlaveAddrSize, uint8_t* mydata, uint8_t mydataLen);
