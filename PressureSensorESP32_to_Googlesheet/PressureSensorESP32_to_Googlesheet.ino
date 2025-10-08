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
    // digitalWrite(ledMerah, HIGH);
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
      // digitalWrite(relayPower, HIGH);d
    }
  }
  status = 0;
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
  }

  //-------------------------------------------------------------

  //------------Print Device ONLINE----------------//
  status4 = 1;
  time();
  // Status();
  //-----------------------------------------------//

  delay(4000);
  digitalWrite(2, LOW);
  // client.setInsecure();
}

void loop() {
  if (!RTC.begin()) {
    ESP.restart();
  }
  if (now.hour() == 23 && now.minute() == 1 && now.second() == 50) {
    ESP.restart();
  }

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);  //--> Connect to your WiFi router
  }

  //---------------------------Start time
  time();
  pressureSensor();
  //---------------------------

  // if (millis() - SDWrite >= 60000) {
  //   writeSD2(pressureFinal);
  //   SDWrite = millis();
  // }

  // Serial.println(millisNow);
  if (now.second() >= 2 && now.second() <= 5) {  // && send_gas == 0) {  //&& (now.minute() == 0 || now.minute() == interval || now.minute() == interval * 2 || now.minute() == interval * 3 || now.minute() == interval * 4 || now.minute() == interval * 5)) {  // || now.minute() == interval * 6 || now.minute() == interval * 7 || now.minute() == interval * 8 || now.minute() == interval * 9 || now.minute() == interval * 10)) {
    send_gas = 1;
    // httpCode = 0;
    // reset = 0;
  }

  if (send_gas == 1) {
    // if (httpCode != 200) {
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

    // digitalWrite(On_Board_LED_PIN, LOW);
    Serial.println("-------------");
    if (httpCode != -1) {
      send_gas = 0;
    }
  }
}


void pressureSensor() {
  // if (millis() - readingDelay >= 300) {
  pressureValue = analogRead(pressureInput);  //reads value from input pin and assigns to variable

  // pressureValue2 = ((pressureValue - pressureZero) * pressuretransducermaxbar) / (pressureMax - pressureZero);  //conversion equation to convert analog reading to psi                                                                                           //to clear the display after large values or negatives
  smoothedValue = alpha * pressureValue + (1 - alpha) * smoothedValue;

  pressureFinal = (0.00479 * smoothedValue) - 1.21 + 0.15;
  readingDelay = millis();
  if (pressureFinal <= (-1.21 + 0.15)) {
    ESP.restart();
  }

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
    pressureString = String(pressureFinal, 2);  // Convert float to String with 2 decimal places
    pressureString.replace('.', ',');           // Replace '.' with ','

    Serial.print(pressureString);
    Serial.print("   ");
    // Serial.print(rstESP2);
    // Serial.print("   ");
    // Serial.print(totalUsage);
    Serial.print("   ");
    Serial.println(pressureValue);
    nowSecond = now.second();
  }
}

//LOG

//Save using millis
void writeSD2(float message) {
  sprintf(filename3, "/%02d-%02d-%04d-millis.csv", now.day(), now.month(), now.year());
  // Serial.printf("Appending to file: %s\n", filename3);

  if (!SD.exists(filename3)) {
    File file3 = SD.open(filename3, FILE_WRITE);
    file3.println("Time Pressure Unit");
    file3.close();
  }
  File file3 = SD.open(filename3, FILE_APPEND);
  if (!file3) {
    Serial.println("Failed to open file for appending");
    ESP.restart();
    return;
  }

  file3.printf("%02d:%02d:%02d %f bar\n", now.hour(), now.minute(), now.second(), message);
  file3.close();
}

//Save using RTC
void writeSD(float message) {
  sprintf(filename, "/%02d-%02d-%04d.csv", now.day(), now.month(), now.year());
  File file = SD.open(filename, FILE_APPEND);

  if (!file) {
    Serial.println("Failed to open file for appending");
    ESP.restart();
    return;
  }
  file.printf("%02d:%02d:%02d %f bar\n", now.hour(), now.minute(), now.second(), message);
  file.close();
}

// void logDataUsage(unsigned long sent, unsigned long received) {
//   sentBytes += sent;
//   receivedBytes += received;
//   totalUsage = sentBytes + receivedBytes;

//   sprintf(filename4, "/%02d-%02d-%04d Data Usage.txt", now.day(), now.month(), now.year());
//   if (!SD.exists(filename4)) {
//     File file4 = SD.open(filename4, FILE_WRITE);
//     file4.println("Data_Usage\tUnit");
//     file4.close();
//   }
//   File file4 = SD.open(filename4, FILE_APPEND);
//   if (!file4) {
//     Serial.println("Failed to open file for appending");
//     return;
//   }

//   file4.printf("%f\tbytes\n", totalUsage);
//   file4.close();
// }
