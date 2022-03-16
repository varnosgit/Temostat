#include <Arduino.h>

#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <display.h>

#include "wireless.h"
#include "timerSetups.h"
#include "algos.h"

#include "LCD_Driver.h"

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */
#define EEPROM_SIZE 1

extern hc_message myData;
extern uint8_t registerStatus;
extern bool newData_flag;
extern uint8_t myMAC_Address[], Brodcast_Address[], Controller_Address[], TERMO_Address[];

const int DIN_PIN = 35;
const int DIN_PIN2 = 0;

RTC_DATA_ATTR int bootCount = 0;

uint8_t myTemperature = 27;
bool loop_start_flag = false;
TaskHandle_t CoreZEROTasks;
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite logtxt1 = TFT_eSprite(&tft); // Sprite object stext1


int interruptCounter_last = 0;
extern volatile int interruptCounter;

int dataCounter = 0;

////////////////////////////////////////////////////////////////////////////////////
void coreZEROTasks_code( void * pvParameters ){
  for(;;){
    delay(4);
  } 
}
////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  pinMode(DIN_PIN, INPUT); pinMode(DIN_PIN2, INPUT);
  Serial.begin(115200);
  EEPROM.begin(512);
  registerStatus = EEPROM.read(0);  // the 0 location determind if the vent is registered to the controller before 0 = no, 1 = yes
  registerStatus = 0;
  EEPROM.write(0, registerStatus);
  EEPROM.commit();
  
  // ++bootCount;
  // Serial.println("Boot number: " + String(bootCount));
  // logtxt1.drawNumber(getCpuFrequencyMhz(), 71, 240, 2);
  // delay(300); display_log_print("Boot number: " + String(bootCount));

  // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  // Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) + " Seconds");


  display_init();
  display_log_init();   display_log_print("Initialising...");
  delay(100);    display_log_print("Serial Debug connect!");
  //logtxt1.drawNumber(getCpuFrequencyMhz(), 71, 240, 2);
  delay(300); display_log_print("CPU Freq.: " + String(getCpuFrequencyMhz()) + "MHz");
  xTaskCreatePinnedToCore(
                    coreZEROTasks_code,      /* Task function. */
                    "Task1",        /* name of task. */
                    10000,          /* Stack size of task */
                    NULL,           /* parameter of the task */
                    1,              /* priority of the task */
                    &CoreZEROTasks, /* Task handle to keep track of created task */
                    0);             /* pin task to core 0 */                  
  delay(500); 
  delay(200); display_log_print("2nd Core setup!");
  delay(100); timer_init(); display_log_print("Timers connected!");

  wireless_init();
}
////////////////////////////////////////////////////////////////////////////////////
char buf[18]; 
uint8_t din_counter=0, din_counter2=0, register_wait_counter = 0;
uint8_t register_mode_flag = 0;
void loop()
{
  if (digitalRead(DIN_PIN) == LOW)
  {
    delay(100);
    din_counter ++;
    if (din_counter == 25) 
    {
      register_mode_flag = 1;
      register_wait_counter = 0;
      display_log_print("registering ...");
    }
  }
  else
  {
    if ((din_counter > 1) & (din_counter < 25))
    {
      myTemperature++;
      display_log_print("Set Point: "+String(myTemperature));
            myData.mode = 1;
            myData.batStat = 98;
            myData.fanStatus = 2;
            myData.setPoint_temp = myTemperature;
            myData.ventStatus = 11;
            sendDataTo(Controller_Address, 0x03, Brodcast_Address);
    }
    din_counter = 0;
  }
  if (digitalRead(DIN_PIN2) == LOW)
  {
    delay(100);
    din_counter2 ++;
    if (din_counter2 == 25) 
    {

    }
  }
  else
  {
    if ((din_counter2 > 1) & (din_counter2 < 25))
    {
      myTemperature--;
      display_log_print("Set Point: "+String(myTemperature));
            myData.mode = 1;
            myData.batStat = 98;
            myData.fanStatus = 2;
            myData.setPoint_temp = myTemperature;
            myData.ventStatus = 11;
            sendDataTo(Controller_Address, 0x03, Brodcast_Address);
    }
    din_counter2 = 0;
  }
  // if (registerStatus == 0)  // this device is not registerd before
  // {
  //   sendDataTo(Brodcast_Address, 0x01, Brodcast_Address);
  //   delay(2000);
  // }

  if ((registerStatus == 0) & (register_mode_flag == 1))  // this device is not registerd before
  {
    myData.mode = 0;
    sendDataTo(Brodcast_Address, 0x01, Brodcast_Address);
    delay(1000);
    register_wait_counter ++;
    if (register_wait_counter > 10)
    {
      register_mode_flag = 0;
      display_log_print("no controller found.");
    }
  }

  if (newData_flag)
  {
    newData_flag = false;
    if (myData._sender == 0x01) // data recieved from controller
    {
      switch (myData._command)
      {
        case 0x01: // registeration command
          if ((registerStatus == 0) & (register_mode_flag == 1))  // this device is not registerd before
          {
            registerStatus = 1;
            EEPROM.write(0, registerStatus);
            for(int i=0; i<6; i++) 
            {
              Controller_Address[i] = myData.sender_MAC_addr[i];
              EEPROM.write(i+1, myData.sender_MAC_addr[i]);
            }
            pairNew_device(Controller_Address);
            EEPROM.commit();
            display_log_print("Controller saved :)");

            myData.mode = 1;
            sendDataTo(Controller_Address, 0x02, Brodcast_Address);
            register_mode_flag = 0;
                
            snprintf(buf, 18, "%02X:%02X:%02X:%02X:%02X:%02X", Controller_Address[0], Controller_Address[1], 
            Controller_Address[2], Controller_Address[3], Controller_Address[4], Controller_Address[5]); 
            display_log_print(String(buf));

          }
          break;
          
        case 0x02: // open close vent  or   change set point of termostat
          // if (myData.ventStatus== 0)
          //      display_log_print("closing vent"); // vent door open/close command
          // else display_log_print("opening vent");
          myTemperature = myData.setPoint_temp;
          display_log_print("Set Point: "+String(myTemperature));
            break;    

        case 0x03: // read status  
            myData.mode = 1;
            myData.batStat = 98;
            myData.fanStatus = 2;
            myData.setPoint_temp = myTemperature;
            myData.ventStatus = 11;
            sendDataTo(Controller_Address, 0x03, Brodcast_Address);
            break;  
        default:
          break;
      }
    }
  }

delay(2);

    // //LCD_LED(1);
    // setpin_high(HT1621LED);
    //  Serial.println("set led on");
    // delay(1000);
    // setpin_low(HT1621LED);
    //  Serial.println("off");
    // //LCD_LED(0);
    // delay(1000);

 // esp_deep_sleep_start();
}
