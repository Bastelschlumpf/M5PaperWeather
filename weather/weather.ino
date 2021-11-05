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
  * Main file with setup() and loop()
  */
  
#include <M5EPD.h>
#include "Config.h"
#include "ConfigOverride.h" // Remove this line
#include "Data.h"
#include "Display.h"
#include "Battery.h"
#include "EPD.h"
#include "EPDWifi.h"
#include "Moon.h"
#include "SHT30.h"
#include "Time.h"
#include "Utils.h"
#include "Weather.h"

// Refresh the M5Paper info more often.
// #define REFRESH_PARTLY 1

MyData         myData;            // The collection of the global data
WeatherDisplay myDisplay(myData); // The global display helper class

/* Start and M5Paper instance */
void setup()
{
#ifndef REFRESH_PARTLY
   InitEPD(true);
   if (StartWiFi(myData.wifiRSSI)) {
      GetBatteryValues(myData);
      GetSHT30Values(myData);
      GetMoonValues(myData);
      if (myData.weather.Get()) {
         SetRTCDateTime(myData);
      }
      myData.Dump();
      myDisplay.Show();
      StopWiFi();
   }
   ShutdownEPD(60 * 60); // every 1 hour
#else 
   myData.LoadNVS();
   if (myData.nvsCounter == 1) {
      InitEPD(true);
      if (StartWiFi(myData.wifiRSSI)) {
         GetBatteryValues(myData);
         GetSHT30Values(myData);
         GetMoonValues(myData);
         if (myData.weather.Get()) {
            SetRTCDateTime(myData);
         }
         myData.Dump();
         myDisplay.Show();
         StopWiFi();
      }
   } else {
      InitEPD(false);
      GetSHT30Values(myData);
      myDisplay.ShowM5PaperInfo();
      if (myData.nvsCounter >= 60) {
         myData.nvsCounter = 0;
      }
   }
   myData.nvsCounter++;
   myData.SaveNVS();
   ShutdownEPD(60); // 1 minute
#endif // REFRESH_PARTLY   
}

/* Main loop. Never reached because of shutdown */
void loop()
{
}
