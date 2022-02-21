/***************************************************
Miikka Kosonen

Hakee FoxerIOT API rajapinnasta huonearvot ja piirtää ne TFT-näytölle yms.

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
const char* ssid     = "x";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "y";     // The password of the Wi-Fi network

// API
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> <-- pitää ettii tää jstn
HTTPClient client;

const int httpsPort = 443;
const char* host = "https://backend.foxeriot.com/api/v1/get-devices?deviceGroup=(put the name of the group of your devices here, copy it from url)";
const char* apikey = "paste the long api key here";
// OMAT

#define TFT_ROTATION 1
#define MITTAUS_MAARA 9

#define YELLOW_LIMIT 50 // *10
#define BLUE_LIMIT 20 // *10


int lampoArray[] = {00,11,22,33,44,55,66,77,88};

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
  ts.begin();
  ts.setRotation(TFT_ROTATION);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLUE);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(30,60);
  tft.setTextSize(4);
  tft.println("Ladataan...");
  
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
  getFoxerGroup();


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
  int i, x, y;
  for (i = 0; i < MITTAUS_MAARA; i++) {
    x = (i % 3) ;
    x = (10 * x) + (100 * x); // border and multiply
    y = i / 3;
    y = (10 * y) + (73 * y);

    tft.setTextSize(3);
    tft.setCursor(x+15,y+43);
    tft.print((float)lampoArray[i]/10, 1);

    printRoomName(i,x,y);
  } 
}

void getFoxerGroup() {
  WiFiClientSecure client;
  HTTPClient http;
  client.setInsecure(); //the magic line, use with caution
  client.connect(host, httpsPort);
  
  Serial.print("Connectiong to ");
  Serial.println(host);
  
  http.begin(client, host);
  http.setAuthorization("");
  http.addHeader("authorization", apikey);
  
  int response = http.GET();  //this will make _base64Authorization.length == 0 
  Serial.print("HTTP Response code: ");
  Serial.print(response);
  Serial.println(" (200 = OK)");
  
  if(response == 200) {
 
 //   Serial.println("### Payload below ###");
   // String payload = http.getString();
    // Serial.print(payload);
    Serial.println("Skipping payload load to memory for limited RAM, but it's there.");
    
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
      return;
    }
    
    int paging_limit = doc["paging"]["limit"]; // Amount of sensors

    JsonArray data = doc["data"];

    // Decode JSON/Extract values
    Serial.println(F("Response:"));
    Serial.println(doc["data"][0].as<char*>());
    Serial.println(data[1].as<char*>());
  }
  else
  {
    Serial.println("Error in response");
  }
    
  delay(5000);
  Serial.println("Closing HTTP connection...");
  http.end();
}

void loop() {
  // put your main code here, to run repeatedly:
  //  tft.fillScreen(ILI9341_BLACK); << ei tarvi koska piirtää kuitenkin rectit päälle
    drawRects();
    drawValuesAndNames();
    delay(2000);
}
