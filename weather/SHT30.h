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
  * @file SHT30.h
  * 
  * Helper function to read the SHT30 environment data.
  */
#pragma once
#include "Data.h"

/* Read the SHT30 environment chip data */
bool GetSHT30Values(MyData &myData)
{
   M5.SHT30.UpdateData();
   if(M5.SHT30.GetError() == 0) {
      myData.sht30Temperatur = (int) M5.SHT30.GetTemperature();
      myData.sht30Humidity   = (int) M5.SHT30.GetRelHumidity();
      return true;
   }
   return false;
}
