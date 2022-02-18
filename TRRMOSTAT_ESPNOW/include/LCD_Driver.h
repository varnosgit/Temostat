#include <Arduino.h>

#define OFF 0
#define ON 	1

#define HT1621LED 	13
#define HT1621Data 	32
#define HT1621WR   	27
#define HT1621CS   	26

// #define HT1621Signal_1(pin) 		{digitalWrite(pin, HIGH);}  
// #define HT1621Signal_0(pin) 		{digitalWrite(pin, LOW);} 


//Set or clear the signal pins:
// #define CS1    		HT1621Signal_1(HT1621CS)
// #define CS0    		HT1621Signal_0(HT1621CS)
// #define WR1    		HT1621Signal_1(HT1621WR)
// #define WR0    		HT1621Signal_0(HT1621WR)
// #define DATA1  	HT1621Signal_1(HT1621Data)
// #define DATA0  	HT1621Signal_0(HT1621Data)
// #define LEDON  	HT1621Signal_1(HT1621LED)
// #define LEDOFF  	HT1621Signal_0(HT1621LED)


//Commands specific to this LCD:
#define  ComMode     0x52  //4commons, 1/3bias  1000 010 10 0 1 0  
#define  RCosc      		0x30  //Clock set: On chip RC 1000 0011 0000 
#define  LCD_on     		0x06  //Turn on  LCD 1000 0000 0110 
#define  LCD_off    		0x04  //Turn off LCD 1000 0000 0100
#define  Sys_en    			0x02  //Turn oscil.on 1000 0000 0010 
#define  Sys_dis    		0x00  //Turn oscil.off 1000 0000 0000 AO!
//#define  CTRl_cmd   0x80  //Next byte is a command.
//#define  Data_cmd   0xa0  //Next byte is a data

typedef struct {
//Define the character (digit) to LCD memory address mapping
// For example, to display '0' we must write 0x7D :

  uint8_t digit[10][2];		 //Individual digits on the display.
  uint8_t degree1,degree2; //Location of the decimal point.
  uint8_t bat1,bat2,bat3;
	uint8_t ant1,ant2,ant3;
	uint8_t register_data[13];
}HT1621_Map;

void setpin_high(int mypin);
void setpin_low(int mypin);

void HT1621_SendBits(uint8_t sdata,uint8_t cnt);
void HT1621_SendCmd(uint8_t command);
void HT1621_Write(uint8_t addr,uint8_t sdata);
void HT1621_AllOff(uint8_t num);
void HT1621_AllOn(uint8_t num);
void HT1621_Init(void);

void LCD_PW_Ctrl(int s);
void LCD_Initialize(void);
void LCD_LED(int s);
void LCD_Sign(char a,int s);
void LCD_Digit(int d,char s);
void LCD_Data_l(int ad,int s);
void LCD_Data_h(int ad, uint8_t sd);

void LCD_OFF(void);