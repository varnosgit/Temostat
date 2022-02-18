#include <LCD_Driver.h>
#include <math.h>
#include <string.h>

HT1621_Map Seg_LCD; //Caution! No variable definitions after first instruction.
uint8_t cfStat;


void setpin_high(int mypin)
{
	digitalWrite(mypin, HIGH);
}
void setpin_low(int mypin)
{
	digitalWrite(mypin, LOW);	
}

void HT1621_SendBits(uint8_t sdata,uint8_t cnt) //Send cnt MSB bits of sdata.
{ 
  uint8_t i; 
  for(i=0;i<cnt;i++) 
    { 
      setpin_low(HT1621WR); // WR0;
      if(sdata&0x80)
				{
					 setpin_high(HT1621Data); //DATA1;
				}
			else
				{
					setpin_low(HT1621Data); //DATA0;
				}
      setpin_high(HT1621WR); //WR1;
      sdata<<=1; 
    } 
	delay(11);
}

void HT1621_SendCmd(uint8_t command) //Write to the command registers.
{ 
  setpin_low(HT1621CS); //CS0;                     //Select,
  HT1621_SendBits(0x80,4); //Next byte is a command, (implemented as 1000+8
                           // bits, rather than the 100+9 bits specified
                           // in the datasheet.)
  HT1621_SendBits(command,8);//Write the 8 bit command value,
  setpin_high(HT1621CS);  //CS1;                     //Deselect
  delay(11);
}

void HT1621_Write(uint8_t addr,uint8_t sdata) //Write sdata to address addr:
{ 
	Seg_LCD.register_data[addr]=sdata;

  addr<<=2; 
  setpin_low(HT1621CS); //CS0;   
  HT1621_SendBits(0xA0,3);     //Write following to data memory
  HT1621_SendBits(addr,6);     // at address addr
  HT1621_SendBits(sdata,8);    // this is the data to be written.
   setpin_high(HT1621CS);  //CS1; 
   delay(11);
} 

void HT1621_AllOff(uint8_t number) //Clear all segments.
{
  uint8_t i; 
  uint8_t addr=0; 
  for(i=0;i<number;i++) 
    { 
      HT1621_Write(addr,0x00); 
      addr+=2;
    } 
	delay(11);
}

void HT1621_AllOn(uint8_t number) //Set all segments.
{
  uint8_t i; 
  uint8_t addr=0; 
  for(i=0;i<number;i++) 
    {
      HT1621_Write(addr,0xFF);
      addr+=2; 
    } 	
	delay(11);
}

void HT1621_Init(void)//Initialize the HT1621 chip.
{
   setpin_high(HT1621CS);  //CS1; 
	delay(200);	
  setpin_high(HT1621Data);
	delay(200);
   setpin_high(HT1621WR);  //WR1; 
	delay(200);
	
  HT1621_SendCmd(Sys_en);
	delay(200);
  HT1621_SendCmd(RCosc); 
	delay(200);	
  HT1621_SendCmd(ComMode);
	delay(200);  
  HT1621_SendCmd(LCD_on);
	
	
	////******************(Define All DIGITAL)***************************
	
	////***********(HEX codes for Digit of Zero #0)**************
	Seg_LCD.digit[0][0]=0x0F;
	Seg_LCD.digit[0][1]=0x0A;
	////***********(HEX codes for Digit of One #1)**************
	Seg_LCD.digit[1][0]=0x00;
	Seg_LCD.digit[1][1]=0x0A;  
	////***********(HEX codes for Digit of Two #2)**************
	Seg_LCD.digit[2][0]=0x0B;
	Seg_LCD.digit[2][1]=0x0C;
	////***********(HEX codes for Digit of Three #3)**************
	Seg_LCD.digit[3][0]=0x09;
	Seg_LCD.digit[3][1]=0x0E;  
	////***********(HEX codes for Digit of Four #4)**************	
	Seg_LCD.digit[4][0]=0x04;
	Seg_LCD.digit[4][1]=0x0E;
	////***********(HEX codes for Digit of Five #5)**************
	Seg_LCD.digit[5][0]=0x0D;
	Seg_LCD.digit[5][1]=0x06;  
	////***********(HEX codes for Digit of Six #6)**************
	Seg_LCD.digit[6][0]=0x0F;
	Seg_LCD.digit[6][1]=0x06;
	////***********(HEX codes for Digit of Seven #7)**************
	Seg_LCD.digit[7][0]=0x08;
	Seg_LCD.digit[7][1]=0x0A;  
	////***********(HEX codes for Digit of Eight #8)**************
	Seg_LCD.digit[8][0]=0x0F;
	Seg_LCD.digit[8][1]=0x0E;
	////***********(HEX codes for Digit of Nine #9)**************
	Seg_LCD.digit[9][0]=0x0D;
	Seg_LCD.digit[9][1]=0x0E;	
}    


void LCD_PW_Ctrl(int s)
{
	 setpin_high(HT1621CS);  //CS1; 
	delay(200);	
  setpin_high(HT1621Data);
	delay(200);
   setpin_high(HT1621WR);  //WR1; 
	delay(200);	
	
	if(s==ON)	
			{
				HT1621_SendCmd(Sys_en);
				delay(200);
				HT1621_SendCmd(RCosc); 
				delay(200);	
				HT1621_SendCmd(ComMode);
				HT1621_SendCmd(LCD_on);
			}
			
	if(s==OFF)	
			{
				HT1621_SendCmd(LCD_off);
			}
  
}


void LCD_Initialize(void)
{
	Serial.println("pin mode changes to output");
	delay(2000);

	HT1621_Init();			// Initialize HT1621 Chip
	Serial.println("HT1621 init");
	HT1621_AllOn(13);		// Turn On All Segments for Test
	Serial.println("all on");
	delay(1000);
	HT1621_AllOff(13);	// Turn Off All Segments after Test
	Serial.println(" all of");
}



void LCD_LED(int s)
{
	 if(s)  setpin_high(HT1621LED);  
	 else  setpin_low(HT1621LED); 

}

void LCD_Sign(char a,int s)
{
	switch (a)
	{
		case 'L':				//Learning Mode Sign		
			if(s)
				LCD_Data_l(1,1);				
			else
				LCD_Data_l(1,0);
		break;
			
		case 'S':				//SET TO: Sign
			if(s)
				LCD_Data_l(5,1);	
			else
				LCD_Data_l(5,0);	
		break;
		

		case 'C':				//Current Temprature Degree Sign
			
				if(s==1)					// Display Cantigrad 'C
				{
					LCD_Data_h(5,0xF0);
					LCD_Data_l(3,1);	
				}
				if(s==2)					// Display Faranhad 'F
				{
					
					LCD_Data_h(5,0xE4);
					LCD_Data_l(3,1);
				}
				
				if(s==0)					// Degree off
				{
					LCD_Data_h(5,0x00);
					LCD_Data_l(3,0)	;
				}
				
			break;	

			case 'T':				//Sepoint Temprature Degree Sign
			
				if(s==1)					// Display Cantigrad 'C
				{
					LCD_Data_l(8,1);
						LCD_Data_l(10,0);	
				}
				if(s==2)					// Display Faranhad 'F
				{
					LCD_Data_l(10,1);	
					LCD_Data_l(8,2);
				}
				
				if(s==0)					// Degree off
				{
					LCD_Data_l(10,0);					
					LCD_Data_l(8,0);
				}
				
			break;			


			case 'X':				//Battry Sign
			
				if(s==2)					// Display Puor Battry
				{
					LCD_Data_h(6,0x01);
				}
				if(s==3)					// Display 20% Battry
				{
					LCD_Data_h(6,0x09);
				}
				
				if(s==4)					// Display 50% Battry
				{
					LCD_Data_h(6,0x0D);
				}
				
				if(s==5)					// Display FUll Battry
				{
					LCD_Data_h(6,0x0F);
				}
				
				if(s==1)					// sign off
				{
					HT1621_Write(6,0x00);
				}
				
			break;									
			
				
				
				case 'W':				//WIFI Sign
			
				if(s==2)					// Display Puor WiFi
				{
					HT1621_Write(13,0x10);
				}
				if(s==3)					// Display 20% WiFi
				{
					HT1621_Write(13,0x30);
				}
				
				if(s==4)					// Display 50% WiFi
				{
					HT1621_Write(13,0x70);
				}
				
				if(s==5)					// Display FUll WiFi
				{
					HT1621_Write(13,0xF0);
				}
				
				if(s==1)					// sign off
				{
					HT1621_Write(13,0x00);
				}
				
			break;
				
	}	
			
}


void LCD_Digit(int d,char s)
{
	uint8_t sd1,sd2,sd3;
	int d1,d2;
	if ((cfStat + 1) == 2) d = (d * 1.8) + 32;	
	if(s=='C')
	{
		d2=fmod(d,10);
		sd1=Seg_LCD.digit[d2][0];
		sd2=Seg_LCD.digit[d2][1];
		
		sd1<<=4;						// Shift 4 bit to HigherBit in 8 bit register (Address 0)
		sd3=sd1|sd2;				// Compine HigherBit & LowerBit together in 8 bit register (First Address 0 & second Address 1)
		
		LCD_Data_h(3,sd3);
		

		d1=d/10;
		sd1=Seg_LCD.digit[d1][0];
		sd2=Seg_LCD.digit[d1][1];
		
		sd1<<=4;						// Shift 4 bit to HigherBit in 8 bit register (Address 0)
		sd3=sd1|sd2;				// Compine HigherBit & LowerBit together in 8 bit register (First Address 0 & second Address 1)
		
		LCD_Data_h(1,sd3);

	}
	if(s=='S')
	{
		d1=d/10;
		
		sd1=Seg_LCD.digit[d1][1];
		sd2=Seg_LCD.digit[d1][0];
		
		sd1<<=4;						// Shift 4 bit to HigherBit in 8 bit register (Address 0)
		sd3=sd1|sd2;				// Compine HigherBit & LowerBit together in 8 bit register (First Address 0 & second Address 1)

		LCD_Data_h(11,sd3);		
		
		d2=fmod(d,10);
		sd1=Seg_LCD.digit[d2][1];
		sd2=Seg_LCD.digit[d2][0];
		
		sd1<<=4;						// Shift 4 bit to HigherBit in 8 bit register (Address 0)
		sd3=sd1|sd2;				// Compine HigherBit & LowerBit together in 8 bit register (First Address 0 & second Address 1)

		LCD_Data_h(9,sd3);
	}
}


void LCD_Data_l(int ad,int s)				// this function Hold C8,C4,C2 status and change C1 status as a Sign Bit
{
	uint8_t sd1,sd2;
		
	if(ad==8)		
		{
			sd1=Seg_LCD.register_data[ad+1];
			sd1>>=4;
			
			if(s==1)
				sd2=sd1|0xF1;
			else if (s==2)
				sd2=sd1|0xE1;
			else
			{
				sd2=sd1&0x0E;
			}
			
			HT1621_Write(ad,sd2);	
		}
		else if(ad==10)		
		{
			sd1=Seg_LCD.register_data[ad+1];
			
			if(s)
				sd2=sd1|0x10;
			else
			{
				sd2=sd1&0xEF;
			}
			
			HT1621_Write(ad+1,sd2);		
		}
	else
		{
			sd1=Seg_LCD.register_data[ad];

			if(s)
				sd2=sd1|0x01;
			else
			{
				sd2=sd1&0xFE;
			}
			
			HT1621_Write(ad,sd2);	
		}	
}


void LCD_Data_h(int ad,uint8_t sd)		// this function Hold C1 status and change C8,C4,C2 status as a Digital Bits
{
	uint8_t sd1,sd2,sd3;
	
		if(ad==6)
		{
			sd1=Seg_LCD.register_data[ad-1];
			sd1<<=4;
			sd3=sd|sd1;
		}
		
		else if(ad==9)
		{
			sd1=Seg_LCD.register_data[ad-1];
			sd1<<=4;
			sd2=sd1&0x10;
			sd3=sd|sd2;
		}
		else if(ad==11)
		{
			sd1=Seg_LCD.register_data[ad];
//			sd1<<=4;
			sd2=sd1&0x10;
			sd3=sd|sd2;
		}
		else
		{			
			sd1=Seg_LCD.register_data[ad];
			sd3=sd1&0x01;
			sd3=sd|sd3;
		}
		
	HT1621_Write(ad,sd3);	

}

void LCD_OFF(void)
{
	HT1621_SendCmd(0x04);
}
