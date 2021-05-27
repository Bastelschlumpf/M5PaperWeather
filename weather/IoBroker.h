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
  * @file IoBroker.h
  * 
  * Helper function to read the information from an IoBroker Server api.
  */
#pragma once
#include "Data.h"
#include <WiFiClient.h>

#define IOBROKER_PLAIN "/getPlainValue/"

/* Read one value from the IoBroker api */
bool GetIoBrokerValue(int &value, String topic)
{
   if (WiFi.status() != WL_CONNECTED) {
      Serial.println("--> connection failed");
      return false;
   } else {
      HTTPClient http;
      String     url = (String) "http://" + IOBROKER_URL + ":" + String(IOBROKER_PORT) + IOBROKER_PLAIN + topic;

      http.begin(url);
      Serial.println("Get command: " + url);
      int httpCode = http.GET();
      if (httpCode > 0) {
         String payload = http.getString();

         value = (int) payload.toFloat();
         Serial.println("payload: " + payload);
      } else {
         Serial.println("No response");
      }
      Serial.println("http code: " + String(httpCode));
      http.end();
      return true;
   }   
   return false;
}

/* Read all values from the IoBroker api */
bool GetIoBrokerValues(MyData &myData)
{
   myData.temperatur = 0;
   myData.humidity   = 0;
   return GetIoBrokerValue(myData.temperatur, IOBROKER_TEMP) && GetIoBrokerValue(myData.humidity, IOBROKER_HUM);
}
