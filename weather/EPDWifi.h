/*
   Copyright (C) 2021 SFini

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
  * @file EPDWifi.h
  * 
  * Helper functions for the wifi connection.
  */
#pragma once
#include <WiFi.h>

/* Start and connect to the wifi */
bool StartWiFi(int &rssi) 
{
   IPAddress dns(8, 8, 8, 8); // Google DNS
   
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   WiFi.setAutoConnect(true);
   WiFi.setAutoReconnect(true);

   Serial.print("Connecting to ");
   Serial.println(WIFI_SSID);
   delay(100);
   
   WiFi.begin(WIFI_SSID, WIFI_PW);

   for (int retry = 0; WiFi.status() != WL_CONNECTED && retry < 30; retry++) {
      delay(500);
      Serial.print(".");
   }

   rssi = 0;
   if (WiFi.status() == WL_CONNECTED) {
      rssi = WiFi.RSSI();
      Serial.println("WiFi connected at: " + WiFi.localIP().toString());
      return true;
   } else {
      Serial.println("WiFi connection *** FAILED ***");
      return false;
   }
}

/* Stop the wifi connection */
void StopWiFi() 
{
   Serial.println("Stop WiFi");
   WiFi.disconnect();
   WiFi.mode(WIFI_OFF);
}
