/*
=====================================================
 Title:  ESP8266 Reciece data from STM32F103 via UART
 Author: TTrung
 Date:   5 Feb 2023
=====================================================
*/
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <String.h>
const byte RX = D6;
const byte TX = D7;
SoftwareSerial espSerial(RX, TX); // RX, TX

// Wi-Fi Settings
const char* ssid = "wifi name"; // your wireless network name (SSID)
const char* password = "wifi password"; // your Wi-Fi network password

WiFiClient client;

// ThingSpeak Settings
//const int channelID = 2024075;
const int channelID = 2024403;
//String writeAPIKey = "G73GXFJS8SVSGVU5"; // write API key for your ThingSpeak Channel
String writeAPIKey = "U4QQL62AWT7143RZ";
const char* server = "api.thingspeak.com";
const int postingInterval = 2 * 1000; // post data every 2 seconds

void setup() {  
        Serial.begin(9600);
        espSerial.begin(115200);
        Serial.print("Connecting");
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
                Serial.print(".");
                delay(100);
        }
        Serial.println("\r\nWiFi connected");
}

void loop() {
        // wait and then post again
        delay(postingInterval);
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
        //Serial.printf("Nhiet do %s - Do am %s\r\n", String(temp.length()).c_str(), String(humi.length()).c_str());
        float temp_f = temp.toFloat();
        float humi_f = humi.toFloat();
        if(!temp.length() || !humi.length()) {
             Serial.println("Failed to read from DHT sensor!");
             return;
        }
        else{
        if (client.connect(server, 80)) {
                // Construct API request body
                String body = "field1=" + String(temp_f,1) + "&field2=" + String(humi_f,1);

                client.print("POST /update HTTP/1.1\n");
                client.print("Host: api.thingspeak.com\n");
                client.print("Connection: close\n");
                client.print("X-THINGSPEAKAPIKEY: " + writeAPIKey + "\n");
                client.print("Content-Type: application/x-www-form-urlencoded\n");
                client.print("Content-Length: ");
                client.print(body.length());
                client.print("\n\n");
                client.print(body);
                client.print("\n\n");
                Serial.printf("Nhiet do %s - Do am %s\r\n", temp.c_str(), humi.c_str());
        }
        client.stop();
        }
        //Serial.printf("Nhiet do %s - Do am %s\r\n", temp.c_str(), humi.c_str());
}
