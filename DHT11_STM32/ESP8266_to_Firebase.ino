/*
=====================================================
 Title:  ESP8266 Reciece data from STM32F103 via UART
 Author: TTrung
 Date:   13 Mar 2023
=====================================================
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>
#include <String.h>
const byte RX = D6;
const byte TX = D7;
SoftwareSerial espSerial(RX, TX); // RX, TX

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
int is_connected = 0;
struct wifi_info {
  const char* ssid;
  const char* password;
};
wifi_info wifiList[]={
  {"wifi_id","wifi_password"}
};
// Insert Firebase project API Key
#define API_KEY "API_KEY"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "your_email"
#define USER_PASSWORD "your_password"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "Firebase_database_url"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String tempPath = "/temperature";
String humPath = "/humidity";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

FirebaseJson json;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Variable to save current epoch time
int timestamp;

float temperature;
float humidity;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 2000;


// Function that gets current epoch time
unsigned long getTime() {
  timeClient.update();
  unsigned long now = timeClient.getEpochTime();
  return now;
}

void setup(){
  pinMode(D0, OUTPUT);
  //digitalWrite(16, LOW);
  Serial.begin(115200);
  espSerial.begin(115200);
  int numNetworks = WiFi.scanNetworks();
  Serial.printf("Found %d networks\n", numNetworks);

  // Loop over the available networks and compare their SSIDs to your list
  for (int i = 0; i < numNetworks; i++) {
    if(is_connected != 0) break;
    String ssid = WiFi.SSID(i);
    Serial.printf("Checking network %s\n", ssid.c_str());

    // Loop over the WifiInfo array to check if the current network's SSID matches one in the list
    for (int j = 0; j < sizeof(wifiList)/sizeof(wifiList[0]); j++) {
      if (strcmp(ssid.c_str(), wifiList[j].ssid) == 0) {
        // Found a matching network - try to connect to it
        Serial.printf("Connecting to %s\n", ssid.c_str());
        WiFi.begin(wifiList[j].ssid, wifiList[j].password);

        // Wait for WiFi to connect
        Serial.print("Waiting for WiFi to connect");
        while (WiFi.status() != WL_CONNECTED) {
          delay(1000);
          Serial.print(".");
        }
        Serial.println("\rWiFi connected");
        is_connected = 1;
        delay(1000);
        break;
      }
    }
  }
  Serial.println("Ready to use");
  timeClient.begin();

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";
}

void loop(){
  
  // Send new readings to database
  String data_="";
  int data_length = 0;
  String temp="";
  String humi="";
  int pass = 0;
  while (espSerial.available()) {
      char s = (char)espSerial.read();
      if(s==' '){
        pass = 1;
      }
      if(pass == 0){
        temp += s;
      }
      else{
        humi += s;
      }
      data_+=s;
  }
  float temp_f = temp.toFloat();
  float humi_f = humi.toFloat();
  if(!temp.length() || !humi.length()) {
       Serial.println("Failed to read from DHT sensor!");
       //Serial.printf("Nhiet do %s - Do am %s\r\n", String(temp.length()).c_str(), String(humi.length()).c_str());
       return;
  }
  else{
    Serial.printf("Nhiet do %s - Do am %s\r\n", String(temp_f), String(humi_f));
    
    if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
       digitalWrite(D0, LOW);
       delay(100);
       digitalWrite(D0,HIGH);
       sendDataPrevMillis = millis();

       //Get current timestamp
       timestamp = getTime();
       Serial.print ("time: ");
       Serial.println (timestamp);

       parentPath= databasePath + "/" + String(timestamp);

       json.set(tempPath.c_str(), String(temp_f,1));
       json.set(humPath.c_str(), String(humi_f,1));
       json.set(timePath, String(timestamp));
       Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
    }
    
  }
}
