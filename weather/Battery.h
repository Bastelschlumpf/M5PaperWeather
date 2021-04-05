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
  * @file Battery.h
  * 
  * Helperfunctions for reading the battery value.
  */
#pragma once

#include "Data.h"

/**
  * Read the battery voltage
  */
bool GetBatteryValues(MyData &myData)
{
   uint32_t vol = M5.getBatteryVoltage();

   if (vol < 3300) {
      vol = 3300;
   } else if (vol > 4350) {
      vol = 4350;
   }
  
   float battery = (float)(vol - 3300) / (float)(4350 - 3300);

   myData.batteryVolt = vol / 1000.0f;
   Serial.println("batteryVolt: " + String(myData.batteryVolt));
   
   if (battery <= 0.01) {
      battery = 0.01;
   }
   if (battery > 1) {
      battery = 1;
   }
   myData.batteryCapacity = (int) (battery * 100);
   Serial.println("batteryCapacity: " + String(myData.batteryCapacity));
   
   return true;
}
