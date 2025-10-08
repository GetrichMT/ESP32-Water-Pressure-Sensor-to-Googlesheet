#include "initialize.h"

void setup() {
  // put your setup code here, to run once:
  delay(5000);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  delay(500);

  if (!RTC.begin()) {
    Serial.println("Couldn't find RTC");
  } else {
    // Set RTC to the date & time this sketch was compiled
    // RTC.adjust(DateTime(2024, 9, 10, 15, 6, 0));
    // RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  delay(500);

  //----------------------------------------------------SD CARD

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    // ESP.restart();
  } else {
    Serial.println("\nSD card initialized successfully.");
  }  //-----------------------------------------------------

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);  //--> Connect to your WiFi router
  Serial.println("");

  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  int m = 0;
  int n = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (n < 10) {
      if (m < 3) {
        Serial.print(". ");
        delay(500);
        m++;
      } else {
        Serial.print("\n");
        m = 0;
        n++;
      }
    } else {
      n = 0;
    }
  }
  status = 0;
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
  }
  delay(4000);
  digitalWrite(2, LOW);
}

void loop() {
  if (!RTC.begin()) {
    ESP.restart();
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);  //--> Connect to your WiFi router
  }

  //---------------------------Start time
  time();
  pressureSensor();
  //---------------------------

  if (now.second() >= 2 && now.second() <= 5) {  // && send_gas == 0) {  //&& (now.minute() == 0 || now.minute() == interval || now.minute() == interval * 2 || now.minute() == interval * 3 || now.minute() == interval * 4 || now.minute() == interval * 5)) {  // || now.minute() == interval * 6 || now.minute() == interval * 7 || now.minute() == interval * 8 || now.minute() == interval * 9 || now.minute() == interval * 10)) {
    send_gas = 1;
  }

  if (send_gas == 1) {
    String Send_Data_URL = Web_App_URL + "?sts=write";
    Send_Data_URL += "&pressure=" + String(pressureString);
    Send_Data_URL += "&reset=read";

    Serial.println(Send_Data_URL);

    HTTPClient http;

    // HTTP GET Request.
    http.begin(Send_Data_URL.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    // Gets the HTTP status code.
    httpCode = http.GET();
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

    // Getting response from google sheets.
    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload : " + payload);
    }

    http.end();

    //::::::::::::::::::
    Serial.println("-------------");
    if (httpCode != -1) {
      send_gas = 0;
    }
  }
}


void pressureSensor() {
  pressureValue = analogRead(pressureInput);  //reads value from input pin and assigns to variable
  smoothedValue = alpha * pressureValue + (1 - alpha) * smoothedValue;
  pressureFinal = (0.00479 * smoothedValue) - 1.21 + 0.15;

  pressureString = String(pressureFinal, 2);  // Convert float to String with 2 decimal places
  pressureString.replace('.', ',');           // Replace '.' with ','

  // Serial.print("Smoothing: ");
  // Serial.print(smoothedValue);
  // Serial.print("rawADC: ");
  // Serial.print(pressureValue);
  // Serial.print("Pressure:  ");
  // Serial.println(pressureString);
  // }
}

int nowSecond;

void time() {
  now = RTC.now();
  if (now.second() != nowSecond) {
    Serial.print(now.year(), DEC);
    Serial.print('/');
    if (now.month() < 10) Serial.print('0');
    Serial.print(now.month());
    Serial.print('/');
    if (now.day() < 10) Serial.print('0');
    Serial.print(now.day());
    Serial.print(' ');
    if (now.hour() < 10) Serial.print('0');
    Serial.print(now.hour());
    Serial.print(':');
    if (now.minute() < 10) Serial.print('0');
    Serial.print(now.minute());
    Serial.print(':');
    if (now.second() < 10) Serial.print('0');
    Serial.print(now.second());
    Serial.print("\t");
    nowSecond = now.second();
  }
}
