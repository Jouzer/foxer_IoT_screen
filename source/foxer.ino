/***************************************************
Miikka Kosonen

Get temperature from Foxer IOT API and print them on screen
www.foxeriot.com

****************************************************/

// Touch screen asiat
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

#define TFT_CS D0  //for D1 mini or TFT I2C Connector Shield (V1.1.0 or later)
#define TFT_DC D8  //for D1 mini or TFT I2C Connector Shield (V1.1.0 or later)
#define TFT_RST -1 //for D1 mini or TFT I2C Connector Shield (V1.1.0 or later)
#define TS_CS D3   //for D1 mini or TFT I2C Connector Shield (V1.1.0 or later)
// #define TFT_CS 14  //for D32 Pro
// #define TFT_DC 27  //for D32 Pro
// #define TFT_RST 33 //for D32 Pro
// #define TS_CS  12 //for D32 Pro

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
XPT2046_Touchscreen ts(TS_CS);

// WLAN
#include <ESP8266WiFi.h> 
#include <WiFiClientSecure.h>
const char* ssid     = "secret";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "secret";     // The password of the Wi-Fi network

// API
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
HTTPClient client;

const int httpsPort = 443;
const char* host = "https://backend.foxeriot.com/api/v1/get-devices?deviceGroup=secret";
const char* apikey = "secretsecretsecretsecret";

// OWN DEFINES
#define TFT_ROTATION 1
#define MITTAUS_MAARA 9

#define YELLOW_LIMIT 220 // *10
#define BLUE_LIMIT 200 // *10

#define MAX_RETRIES_FOXER 6
#define UPDATE_INTERVAL_MINUTES 30
#define SLEEP_MINUTES 5

int lampoArray[] = {00,11,22,33,44,55,66,77,88};
int oldLampoArray[] = {00,11,22,33,44,55,66,77,88};

void printRoomName(int i, int x, int y) {
  tft.setTextSize(2);
  tft.setCursor(x+3,y+3);
  switch(i) {
    case 0: tft.print("Aikuiset"); break;
    case 1: tft.print("Tyttojen"); break;
    case 2: tft.print(" Otson");   break;
    case 3: tft.print("Olohuone"); break; // ehkä co2 tft.println("  huone");
    case 4: tft.print("Alakerta"); break;
    case 5: tft.print("Ylakerta"); break;
    case 6: tft.print("Alapohja"); break;
    case 7: tft.print(" Auto-"); break;
    case 8: tft.print("Lammonj."); break;
  }
  tft.setCursor(x+3,y+20);
  switch(i) {
    case 0: tft.print(" huone"); break;
    case 1: tft.print(" huone"); break;
    case 2: tft.print(" huone"); break;
    case 3: break; // ehkä co2 tft.println("  huone");
    case 4: tft.print(" suihku"); break;
    case 5: tft.print(" suihku"); break;
    case 6: break;
    case 7: tft.print(" talli"); break;
    case 8: tft.print(" huone"); break;
  }

}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  ts.begin();
  ts.setRotation(TFT_ROTATION);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(30,60);
  tft.setTextSize(4);
  tft.println("Loading...");
  
  tft.setTextSize(2);
  WiFi.begin(ssid, password);             // Connect to the network
  tft.print("Connecting to SSID ");
  tft.print(ssid); Serial.println(" ...");
  delay(1000);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    tft.print('.');
  }
  tft.println('\n');
  tft.println("Connection established!");  
  tft.print("IP address:\t");
  tft.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer  
  delay(3000);

  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0,0);
  tft.setTextSize(1);

  while (!Serial && (millis() <= 1000))
    ;

}

void drawRects() {
  int i, x, y, color;
  for (i = 0; i < MITTAUS_MAARA; i++) {
    x = (i % 3) ;
    x = (10 * x) + (100 * x); // border and multiply
    y = i / 3;
    y = (10 * y) + (73 * y);
    
    if (lampoArray[i] < BLUE_LIMIT) {
        color = ILI9341_BLUE;
      } else if (lampoArray[i] > YELLOW_LIMIT) {
        color = ILI9341_ORANGE;
      } else {
        color = ILI9341_GREEN;        
      }
    tft.fillRect(x,y,100,73, color);
  }
}

void drawValuesAndNames() {
  tft.setTextColor(ILI9341_WHITE);

  tft.setTextSize(4);
  int i, x, y, peakX, peakY, botLeftX, botLeftY, botRightX, botRightY;
  for (i = 0; i < MITTAUS_MAARA; i++) {
    x = (i % 3) ;
    x = (10 * x) + (100 * x); // border and multiply
    y = i / 3;
    y = (10 * y) + (73 * y);

    tft.setTextSize(3);
    tft.setCursor(x+5,y+43);
    tft.print((float)lampoArray[i]/10, 1);

    printRoomName(i,x,y);

    // Print arrow (triangle) (up, down, right) based on old reading
    if(lampoArray[i] == oldLampoArray[i]) {
      peakX = x + 95; peakY = y + 43 + 9;
      botLeftX = x + 77; botLeftY = y + 43;
      botRightX = x + 77; botRightY = y + 43 + 18;
    } else if(lampoArray[i] > oldLampoArray[i]) {
      peakX = x + 77 + 9; peakY = y + 43;
      botLeftX = x + 77; botLeftY = y + 43 + 18;
      botRightX = x + 95; botRightY = y + 43 + 18;
    } else {
      peakX = x + 77 + 9; peakY = y + 43 + 18;
      botLeftX = x + 77; botLeftY = y + 43;
      botRightX = x + 95; botRightY = y + 43;
    }
    tft.fillTriangle(peakX, peakY, botLeftX, botLeftY, botRightX, botRightY, ILI9341_WHITE);

  } 
}



void dealWithJsonDataEntry(const char* deviceId, int value) {
         if(strcmp(deviceId,  "53CRE700") == 0) { oldLampoArray[0] = lampoArray[0]; lampoArray[0] = value; } // "MK - Huone 1"        
    else if(strcmp(deviceId,  "53CRE700") == 0) { oldLampoArray[4] = lampoArray[4]; lampoArray[4] = value; } // "MK - Pesuhuone"   
    else if(strcmp(deviceId,  "53CRE700") == 0) { oldLampoArray[6] = lampoArray[6]; lampoArray[6] = value; } // "MK - Alapohja"   
    else if(strcmp(deviceId,  "53CRE700") == 0) { oldLampoArray[1] = lampoArray[1]; lampoArray[1] = value; } // "MK - Huone 2"     
    else if(strcmp(deviceId,  "53CRE700") == 0) { oldLampoArray[2] = lampoArray[2]; lampoArray[2] = value; } // "MK - Huone 3"     
    else if(strcmp(deviceId,  "53CRE700") == 0) { oldLampoArray[5] = lampoArray[5]; lampoArray[5] = value; } // "MK - Suihku"     
    else if(strcmp(deviceId,  "53CRE700") == 0) { oldLampoArray[3] = lampoArray[3]; lampoArray[3] = value; } // "MK - Olohuone"     
    else if(strcmp(deviceId,  "53CRE700") == 0) { oldLampoArray[8] = lampoArray[8]; lampoArray[8] = value; } // "MK - Lämmönjakohuone"     
    else if(strcmp(deviceId,  "53CRE700") == 0) { oldLampoArray[7] = lampoArray[7]; lampoArray[7] = value; } // "MK - Autotalli"
    else {Serial.print("Skipping deviceId "); Serial.print(deviceId); Serial.println(" because it's not on the list of approved sensors");}
}

void getFoxerGroup(int retryNo) {
  if(retryNo > MAX_RETRIES_FOXER) {
    tft.fillScreen(ILI9341_RED); // Red = problem in Json parse
    return;
  }
  
  WiFiClientSecure client;
  HTTPClient http;
  client.setInsecure(); //the magic line, use with caution
  client.connect(host, httpsPort);
  
  Serial.print("Connecting to ");
  Serial.println(host);
  
  http.begin(client, host);
  http.setAuthorization("");
  http.addHeader("authorization", apikey);
  
  int response = http.GET();  //this will make _base64Authorization.length == 0 
  Serial.print("HTTP Response code: ");
  Serial.print(response);
  Serial.println(" (200 = OK)");
  
  if(response == 200) {
    // Stream& input;
    
    StaticJsonDocument<176> filter;
    filter["paging"]["limit"] = true;
    
    JsonObject filter_data_0 = filter["data"].createNestedObject();
    filter_data_0["deviceId"] = true;
    filter_data_0["displayName"] = true;
    
    JsonObject filter_data_0_latestObservations_0 = filter_data_0["latestObservations"].createNestedObject();
    filter_data_0_latestObservations_0["value"] = true;
    filter_data_0_latestObservations_0["variable"] = true;
    filter_data_0_latestObservations_0["message-time"] = true;
    
    DynamicJsonDocument doc(8192);
    
    DeserializationError error = deserializeJson(doc, http.getString(), DeserializationOption::Filter(filter));
    
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      http.end();
      delay(10000);
      getFoxerGroup(retryNo);
      return;
    }
    
    JsonArray data = doc["data"];
    Serial.print("JSON array size: ");
    Serial.println(data.size());
    
    // Walk the JsonArray efficiently
    for (JsonObject obj : data) {
      JsonArray latestObs = obj["latestObservations"];
      for (JsonObject observation : latestObs) {
        if(observation["variable"] == "temperature" ) {
          const char* devId = obj["deviceId"];
          float value = observation["value"];
          dealWithJsonDataEntry(devId, round(value * 10));
        } else { 
          Serial.print("Skipping observation ");
          Serial.print(String(observation["variable"]));
          Serial.print(" for deviceId ");
          Serial.println(String(obj["deviceId"])); 
        }
      }
    }
    tft.fillScreen(ILI9341_BLACK); // Black = everything okay
    
  } else
  {
    Serial.println("Error in HTTP response");
    tft.fillScreen(ILI9341_YELLOW); // Yellow background = problem in connection
  }
    
  Serial.println("Closing HTTP connection...");
  http.end();
}

void loop() {
  // put your main code here, to run repeatedly:
  static int minutesLeft = 0;
  static int minutesSinceTouched = 0;
  static unsigned long resultLastCycle = 0;
  unsigned long resultNow = millis() % 60000;

  if(resultNow < resultLastCycle) { minutesLeft--; minutesSinceTouched++; }
  resultLastCycle = resultNow;

  if(minutesLeft < 1) {
    getFoxerGroup(0);
    drawRects();
    drawValuesAndNames();
    minutesLeft = UPDATE_INTERVAL_MINUTES;    
  }

  if(minutesSinceTouched == SLEEP_MINUTES) {
    minutesSinceTouched++;
    //  ESP.deepSleep(0); << doesnt sleep the screen either so not very useful for this project 
    // tft sleep test, didn't work for my tft tft.writeCommand(0x10); delay(5);
  }
  
  if (ts.touched()) {
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
      delay(1000);
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      minutesSinceTouched = 0;
    // tft sleep test, didn't work for my tft tft.writeCommand(0x11); delay(120);
  }
  
  delay(10000); // if I'm gonna actually do something with ts.touched() I think this needs to go but since it's now on reserve I'll put this back on, I imagine it's easier for the CPU and such  
}
