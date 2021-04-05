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
  * @file Utils.h
  * 
  * A collection of helper functions.
  */
#pragma once
#include <Time.h>
#include <TimeLib.h> 

/* Convert the RTC date time to DD.MM.YYYY HH:MM:SS */
String getRTCDateTimeString() 
{
   char       buff[32];
   rtc_date_t date_struct;
   rtc_time_t time_struct;
   
   M5.RTC.getDate(&date_struct);
   M5.RTC.getTime(&time_struct);

   sprintf(buff,"%02d.%02d.%04d %02d:%02d:%02d",
      date_struct.day,  date_struct.mon, date_struct.year,
      time_struct.hour, time_struct.min, time_struct.sec);

   return (String) buff;
}

/* Read the RTC timestamp */
time_t GetRTCTime()
{
  tmElements_t tmSet;
  rtc_date_t   date_struct;
  rtc_time_t   time_struct;
   
  M5.RTC.getDate(&date_struct);
  M5.RTC.getTime(&time_struct);
  
  tmSet.Year   = date_struct.year - 1970;
  tmSet.Month  = date_struct.mon;
  tmSet.Day    = date_struct.day;
  tmSet.Hour   = time_struct.hour;
  tmSet.Minute = time_struct.min;
  tmSet.Second = time_struct.sec;
  
  return makeTime(tmSet);
}

/* Convert the date part of the RTC timestamp */
String getRTCDateString() 
{
   char       buff[32];
   rtc_date_t date_struct;
   
   M5.RTC.getDate(&date_struct);

   sprintf(buff,"%02d.%02d.%04d",
      date_struct.day,  date_struct.mon, date_struct.year);

   return (String) buff;
}

/* Convert the time part of the RTC timestamp */
String getRTCTimeString() 
{
   char       buff[32];
   rtc_time_t time_struct;
   
   M5.RTC.getTime(&time_struct);

   sprintf(buff,"%02d:%02d:%02d",      
      time_struct.hour, time_struct.min, time_struct.sec);

   return (String) buff;
}

/* Convert the time_t to the DD.MM.YYYY HH:MM:SS format */
String getDateTimeString(time_t rawtime)
{
   char buff[32];
   
   sprintf(buff,"%02d.%02d.%04d %02d:%02d:%02d",
      day(rawtime), month(rawtime), year(rawtime),
      hour(rawtime), minute(rawtime), second(rawtime));

   return (String) buff;
}

/* Convert the time_t to the date part DD.MM.YYYY */
String getDateString(time_t rawtime)
{
   char buff[32];
   
   sprintf(buff,"%02d.%02d.%04d",
      day(rawtime), month(rawtime), year(rawtime));

   return (String) buff;
}

/* Convert the time_t to the time part HH:MM:SS format */
String getTimeString(time_t rawtime)
{
   char buff[32];
   
   sprintf(buff,"%02d:%02d:%02d",
      hour(rawtime), minute(rawtime), second(rawtime));

   return (String) buff;
}

/* Convert the hour of the time_t */
String getHourString(time_t rawtime)
{
   char buff[32];
   
   sprintf(buff,"%02d",
      hour(rawtime));

   return (String) buff;
}

/* Convert the minute of the time_t */
String getHourMinString(time_t rawtime)
{
   char buff[32];
   
   sprintf(buff,"%02d:%02d",
      hour(rawtime), minute(rawtime));

   return (String) buff;
}

/* Convert the rssi value to a string value between 0 and 100 % */
String WifiGetRssiAsQuality(int rssi)
{
   int quality = 0;

   if (rssi <= -100) {
      quality = 0;
   } else if (rssi >= -50) {
      quality = 100;
   } else {
      quality = 2 * (rssi + 100);
   }
   return String(quality);
}

/* Convert the rssi value to a int value between 0 and 100 % */
int WifiGetRssiAsQualityInt(int rssi)
{
   int quality = 0;

   if (rssi <= -100) {
      quality = 0;
   } else if (rssi >= -50) {
      quality = 100;
   } else {
      quality = 2 * (rssi + 100);
   }
   return quality;
}

/* Convert a day, month, year to a julian int
 * The moon phase calculation is part of the github project
 * https://github.com/G6EJD/ESP32-Revised-Weather-Display-42-E-Paper
 * See http://www.dsbird.org.uk
 * Copyright (c) David Bird
 */
int JulianDate(int d, int m, int y) 
{
   int mm, yy, k1, k2, k3, j;
   
   yy = y - (int)((12 - m) / 10);
   mm = m + 9;
   if (mm >= 12) mm = mm - 12;
   k1 = (int)(365.25 * (yy + 4712));
   k2 = (int)(30.6001 * mm + 0.5);
   k3 = (int)((int)((yy / 100) + 49) * 0.75) - 38;
   // 'j' for dates in Julian calendar:
   j = k1 + k2 + d + 59 + 1;
   if (j > 2299160) j = j - k3; // 'j' is the Julian date at 12h UT (Universal Time) For Gregorian calendar:
   return j;
} 

/* Normalize the moon phase with the julian date format
 * The moon phase calculation is part of the github project
 * https://github.com/G6EJD/ESP32-Revised-Weather-Display-42-E-Paper
 * See http://www.dsbird.org.uk
 * Copyright (c) David Bird
 */
double NormalizedMoonPhase(int d, int m, int y) 
{
   int    j     = JulianDate(d, m, y);
   double Phase = (j + 4.867) / 29.53059;
   
   return (Phase - (int) Phase);
}
