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
  * @file EPD.h
  * 
  * Helper functions for initialisizing and shutdown of the M5Paper.
  */
#pragma once

/* Initialize the M5Paper */
void InitEPD(bool clearDisplay = true)
{
   M5.begin(false, false, true, true, false);
   M5.RTC.begin();
   
   M5.EPD.SetRotation(0);
   M5.TP.SetRotation(0);
   if (clearDisplay) {
      M5.EPD.Clear(true);
   }

//   disableCore0WDT();
}

/* 
 *  Shutdown the M5Paper 
 *  NOTE: the M5Paper could not shutdown while on usb connection.
 *        In this case use the esp_deep_sleep_start() function.
*/
void ShutdownEPD(int sec)
{
   Serial.println("Shutdown");
/*
   M5.disableEPDPower();
   M5.disableEXTPower();
   M5.disableMainPower();
   esp_sleep_enable_timer_wakeup(sec * 1000000);
   esp_deep_sleep_start();   
*/   
   M5.shutdown(sec);
}
