#include <WiFi.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <RTClib.h>
#include <HTTPClient.h>
// #include <WiFiClientSecure.h>

const char* ssid = "SARD 4";         //--> Your wifi name or SSID.
const char* password = "stpwtp123";  //--> Your wifi password.

// const char* ssid = "Tenda_8A5520";          //--> Your wifi name or SSID.
// const char* ssid = "iPhone";
// const char* password = "qwertyuiop";  //--> Your wifi password.

int status, status2, status3, status4, status5 = 0;  //1 = wifi, 2 = RTC, 3 = internet, 4 = device
int statusFinal = 0;
//----------------------------------------Host & httpsPort
// const char* host = "script.google.com";
// const int httpsPort = 443;
//----------------------------------------

//----------------------------------------
String Web_App_URL = "https://script.google.com/macros/s/AKfycbxte8QEBH03imhUIciJ8FVTVkgmXA9lYg7aUgV9Snattl3jT1hQb3A-M4_PcbUVsWU-/exec";

// WiFiClientSecure client;                                                                       //--> Create a WiFiClientSecure object.
// String GAS_ID = "AKfycbz-AAHUGQdJer-ACIQSDA_ABJ0QBoiYc8upYtHArwCD28uLUPrminoDll78de8tuLQ";  //--> spreadsheet script ID
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

//-----------------------------------------SD Card
int millisWriteSD = 0;
const int chipSelect = 5;  // CS pin
int previousDay;
int writeToSD = 0;
int writeToSD2 = 0;
long unsigned SDWrite;
char filename[30];
char filename2[30];
char filename3[30];
//-----------------------------------------
//-----------------------------------------Sampling
const int numSamples = 10;  // Number of samples to consider for the running average
float samples[numSamples];  // Array to store the samples
int sampleIndex = 0;        // Index to keep track of the current sample position
float runningAverage = 0;   // Variable to store the running average
//-----------------------------------------
int ledMerah = 2;
//-----------------------------------------
unsigned long resetTime;
//-----------------------------------------
unsigned long readingDelay;
unsigned long millisNow;
int gs = 0;
//-----------------------------------------alert state
// int x, y;
int httpCode;
int send_gas;