/**
   BasicHTTPSClient.ino

    Created on: 20.08.2018

*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "FwBox_TwWeather.h"

#define WIFI_SSID "MY_SSID"
#define WIFI_PASSWORD "MY_PSK"

FwBox_TwWeather TwWeather;

void setup() {

  Serial.begin(9600);

  Serial.println();
  Serial.println();
  Serial.println();

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //
  // Begin the Taiwan weather object.
  //
  TwWeather.begin("CWB-xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx", "新北市");
}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {
    FwBox_WeatherResult result = TwWeather.read();
    if (result.WxResult == true) {
      Serial.println(result.Wx1);
      Serial.println(result.Wx2);
      Serial.println(result.Wx3);
    }
    if (result.MinTResult == true) {
      Serial.printf("%d °C\n", result.MinT1);
      Serial.printf("%d °C\n", result.MinT2);
      Serial.printf("%d °C\n", result.MinT3);
    }
    if (result.MaxTResult == true) {
      Serial.printf("%d °C\n", result.MaxT1);
      Serial.printf("%d °C\n", result.MaxT2);
      Serial.printf("%d °C\n", result.MaxT3);
    }
  }

  Serial.println("Wait 30s before next round...");
  delay(30 * 1000);
}
