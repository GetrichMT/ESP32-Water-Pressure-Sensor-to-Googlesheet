#include <WiFi.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>
#include <HTTPClient.h>

const char* ssid = "*******";         //--> Your wifi name or SSID.
const char* password = "*********";  //--> Your wifi password.

//----------------------------------------
String Web_App_URL = "https://script.google.com/macros/s/********/exec"; //Google Apps script web app
//----------------------------------------

//----------------------------------------Pressure variable declaration
const int pressureInput = 34;             //select the analog input pin for the pressure transducer
const int pressureZero = 102.4;           //analog reading of pressure transducer at 0psi
const int pressureMax = 4095;            //analog reading of pressure transducer at 100psi
const int pressuretransducermaxbar = 12;  //bar value of transducer being used
const int baudRate = 115200;              //constant integer to set the baud rate for serial monitor
const int sensorreadDelay = 250;          //constant integer to set the sensor read delay in milliseconds

float pressureValue = 0;  //variable to store the value coming from the pressure transducer
float pressureValue2 = 0;
float pressureFinal = 0;   //final reading
float pressureFinal2 = 0;  //final reading
String pressureString;

float smoothedValue = 1.0;
float alpha = 0.05;
//-----------------------------------------

//-----------------------------------------RTC DS3231
RTC_DS3231 RTC;
DateTime now;
int interval = 10;
int SDA_PIN = 22;
int SCL_PIN = 21;
//-----------------------------------------
int ledMerah = 2;
//-----------------------------------------
unsigned long resetTime;
//-----------------------------------------
unsigned long readingDelay;
unsigned long millisNow;
int gs = 0;
int httpCode;
int send_gas;
