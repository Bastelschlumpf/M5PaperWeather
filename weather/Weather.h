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
  * @file Weather.h
  * 
  * Class for reading all the weather data from openweathermap.
  */
#pragma once
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include "Utils.h"

#define MAX_HOURLY   24
#define MAX_FORECAST  8
#define MIN_RAIN     10

/**
  * Class for reading all the weather data from openweathermap.
  */
class Weather
{
public:
   time_t currentTime;                     //!< Current timestamp
   int    currentTimeOffset;               //!< Current timezone

   time_t sunrise;                         //!< Sunrise timestamp
   time_t sunset;                          //!< Sunset timestamp
   float  winddir;                         //!< Wind direction
   float  windspeed;                       //!< Wind speed

   time_t hourlyTime[MAX_HOURLY];          //!< timestamp of the hourly forecast
   float  hourlyMaxTemp[MAX_HOURLY];       //!< max temperature forecast
   String hourlyMain[MAX_HOURLY];          //!< description of the hourly forecast
   String hourlyIcon[MAX_HOURLY];          //!< openweathermap icon of the forecast weather

   int    maxRain;                         //!< maximum rain in mm of the day forecast
   float  forecastMaxTemp[MAX_FORECAST];   //!< max temperature
   float  forecastMinTemp[MAX_FORECAST];   //!< min temperature
   float  forecastRain[MAX_FORECAST];      //!< max rain in mm
   float  forecastHumidity[MAX_FORECAST];  //!< humidity of the dayly forecast
   float  forecastPressure[MAX_FORECAST];  //!< air pressure

protected:
   /* Convert UTC time to local time */
   time_t LocalTime(time_t time)
   {
      return time + currentTimeOffset;
   }

   /* Calls the openweathermap request and deserialisation the json data. */
   bool GetOpenWeatherJsonDoc(DynamicJsonDocument &doc)
   {
      WiFiClient client;
      HTTPClient http;
      String     uri;
      
      uri += "/data/2.5/onecall";
      uri += "?lat=" + String((float) LATITUDE, 5);
      uri += "&lon=" + String((float) LONGITUDE, 5);
      uri += "&units=metric&lang=en&exclude=minutely";
      uri += "&appid=" + (String) OPENWEATHER_API;

      client.stop();
      http.begin(client, OPENWEATHER_SRV, OPENWEATHER_PORT, uri);
      
      int httpCode = http.GET();
      
      if (httpCode != HTTP_CODE_OK) {
         Serial.printf("GetWeather failed, error: %s", http.errorToString(httpCode).c_str());
         client.stop();
         http.end();
         return false;
      } else {
         DeserializationError error = deserializeJson(doc, http.getStream());
         
         if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return false;
         } else {
            return true;
         }
      }
   }

   /* Fill from the json data into the internal data. */
   bool Fill(const JsonObject &root) 
   {
      Clear();

      currentTimeOffset = root["timezone_offset"].as<int>();
      currentTime       = LocalTime(root["current"]["dt"].as<int>());

      sunrise           = LocalTime(root["current"]["sunrise"].as<int>());
      sunset            = LocalTime(root["current"]["sunset"].as<int>());
      winddir           = root["current"]["wind_deg"].as<float>();
      windspeed         = root["current"]["wind_speed"].as<float>();

      JsonArray hourly_list = root["hourly"];
      hourlyTime[0]    = LocalTime(root["current"]["dt"].as<int>());
      hourlyMaxTemp[0] = root["current"]["temp"].as<float>();
      hourlyMain[0]    = root["current"]["weather"][0]["main"].as<char *>();
      hourlyIcon[0]    = root["current"]["weather"][0]["icon"].as<char *>();
      for (int i = 1; i < MAX_HOURLY; i++) {
         if (i < hourly_list.size()) {
            hourlyTime[i]    = LocalTime(hourly_list[i - 1]["dt"].as<int>());
            hourlyMaxTemp[i] = hourly_list[i - 1]["temp"].as<float>();
            hourlyMain[i]    = hourly_list[i - 1]["weather"][0]["main"].as<char *>();
            hourlyIcon[i]    = hourly_list[i - 1]["weather"][0]["icon"].as<char *>();
         }
      }
      
      JsonArray dayly_list  = root["daily"];
      for (int i = 0; i < MAX_FORECAST; i++) {
         if (i < dayly_list.size()) {
            forecastMaxTemp[i]  = dayly_list[i]["temp"]["max"].as<float>();
            forecastMinTemp[i]  = dayly_list[i]["temp"]["min"].as<float>();
            forecastRain[i]     = dayly_list[i]["rain"].as<float>();
            forecastHumidity[i] = dayly_list[i]["humidity"].as<float>();
            forecastPressure[i] = dayly_list[i]["pressure"].as<float>();
            if (forecastRain[i] > maxRain) {
               maxRain = forecastRain[i];
            }
         }
      }
          
      return true;
   }

public:
   Weather()
      : currentTime(0)
      , currentTimeOffset(0)
      , sunrise(0)
      , sunset(0)
      , winddir(0)
      , windspeed(0)
      , maxRain(MIN_RAIN)
   {
      Clear();
   }

   /* Clear the internal data. */
   void Clear()
   {
      currentTime       = 0;
      currentTimeOffset = 0;
      sunrise           = 0;
      sunset            = 0;
      winddir           = 0;
      windspeed         = 0;
      maxRain           = MIN_RAIN;
      memset(hourlyMaxTemp,    0, sizeof(hourlyMaxTemp));
      memset(forecastMaxTemp,  0, sizeof(forecastMaxTemp));
      memset(forecastMinTemp,  0, sizeof(forecastMinTemp));
      memset(forecastRain,     0, sizeof(forecastRain));
      memset(forecastHumidity, 0, sizeof(forecastHumidity));
      memset(forecastPressure, 0, sizeof(forecastPressure));
   }

   /* Start the request and the filling. */
   bool Get()
   {
      DynamicJsonDocument doc(35 * 1024);
   
      if (GetOpenWeatherJsonDoc(doc)) {
         return Fill(doc.as<JsonObject>());
      }
      return false;
   }
};
