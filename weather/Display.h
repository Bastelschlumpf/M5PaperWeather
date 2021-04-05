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
  * @file Display.h
  * 
  * Main class for drawing the content to the e-paper display.
  */
#pragma once
#include "Data.h"
#include "Icons.h"


M5EPD_Canvas canvas(&M5.EPD); // Main canvas of the e-paper

/* Main class for drawing the content to the e-paper display. */
class WeatherDisplay
{
protected:
   MyData &myData; //!< Reference to the global data
   int     maxX;   //!< Max width of the e-paper
   int     maxY;   //!< Max height of the e-paper

protected:
   void DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom = 0, int32_t degTo = 360);
   void Arrow(int x, int y, int asize, float aangle, int pwidth, int plength);
   void DisplayDisplayWindSection(int x, int y, float angle, float windspeed, int radius);    
   
   void DrawIcon(int x, int y, const uint16_t *icon, int dx = 64, int dy = 64, bool highContrast = false);
   void DrawMoon(int x, int y, int dd, int mm, int yy);
   
   void DrawHead();
   void DrawRSSI(int x, int y);
   void DrawBattery(int x, int y);

   void DrawSunInfo(int x, int y, int dx, int dy);
   void DrawMoonInfo(int x, int y, int dx, int dy);
   void DrawWindInfo(int x, int y, int dx, int dy);
   void DrawM5PaperInfo(int x, int y, int dx, int dy);

   void DrawHourly(int x, int y, int dx, int dy, Weather &weather, int index);
   
   void DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[]);

public:
   WeatherDisplay(MyData &md, int x = 960, int y = 540)
      : myData(md)
      , maxX(x)
      , maxY(y)
   {
   }

   void Show();

   void ShowM5PaperInfo();
};

/* Draw a circle with optional start and end point */
void WeatherDisplay::DrawCircle(int32_t x, int32_t y, int32_t r, uint32_t color, int32_t degFrom /* = 0 */, int32_t degTo /* = 360 */)
{
   for (int i = degFrom; i < degTo; i++) {
      double radians = i * PI / 180;
      double px      = x + r * cos(radians);
      double py      = y + r * sin(radians);
      
      canvas.drawPixel(px, py, color);
   }
} 

/* Draw a the rssi value as circle parts */
void WeatherDisplay::DrawRSSI(int x, int y)
{
   int iQuality = WifiGetRssiAsQualityInt(myData.wifiRSSI);

   if (iQuality >= 80) DrawCircle(x + 12, y, 16, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 40) DrawCircle(x + 12, y, 12, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 20) DrawCircle(x + 12, y,  8, M5EPD_Canvas::G15, 225, 315); 
   if (iQuality >= 10) DrawCircle(x + 12, y,  4, M5EPD_Canvas::G15, 225, 315); 
   DrawCircle(x + 12, y, 2, M5EPD_Canvas::G15, 225, 315); 
}

/* Draw a the battery icon */
void WeatherDisplay::DrawBattery(int x, int y)
{
   canvas.drawRect(x, y, 40, 16, M5EPD_Canvas::G15);
   canvas.drawRect(x + 40, y + 3, 4, 10, M5EPD_Canvas::G15);
   for (int i = x; i < x + 40; i++) {
      canvas.drawLine(i, y, i, y + 15, M5EPD_Canvas::G15);
      if ((i - x) * 100.0 / 40.0 > myData.batteryCapacity) {
         break;
      }
   }
}

/* Draw a the head with version, city, rssi and battery */
void WeatherDisplay::DrawHead()
{
   canvas.drawString(VERSION, 20, 10);
   canvas.drawCentreString(CITY_NAME, maxX / 2, 10, 1);
   canvas.drawString(WifiGetRssiAsQuality(myData.wifiRSSI) + "%", maxX - 200, 10);
   DrawRSSI(maxX - 155, 25);
   canvas.drawString(String(myData.batteryCapacity) + "%", maxX - 110, 10);
   DrawBattery(maxX - 65, 10);
}

/* Draw one icon from the binary data */
void WeatherDisplay::DrawIcon(int x, int y, const uint16_t *icon, int dx /*= 64*/, int dy /*= 64*/, bool highContrast /*= false*/)
{
   for (int yi = 0; yi < dy; yi++) {
      for (int xi = 0; xi < dx; xi++) {
         uint16_t pixel = icon[yi * dx + xi];

         if (highContrast) {
            if (15 - (pixel / 4096) > 0) canvas.drawPixel(x + xi, y + yi, M5EPD_Canvas::G15);
         } else {
            canvas.drawPixel(x + xi, y + yi, 15 - (pixel / 4096));
         }
      }
   }
}

/* Draw the sun information with sunrise and sunset */
void WeatherDisplay::DrawSunInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(3);
   canvas.drawCentreString("Sun", x + dx / 2, y + 7, 1);
   canvas.drawLine(x, y + 35, x + dx, y + 35, M5EPD_Canvas::G15);

   canvas.setTextSize(3);
   DrawIcon(x + 25, y + 55, (uint16_t *) SUNRISE64x64);
   canvas.drawString(getHourMinString(myData.weather.sunrise), x + 105, y + 80, 1);
   
   DrawIcon(x + 25, y + 150, (uint16_t *) SUNSET64x64);
   canvas.drawString(getHourMinString(myData.weather.sunset), x + 105, y + 175, 1);
}

/* The moon phase drawing was from the github project
 * https://github.com/G6EJD/ESP32-Revised-Weather-Display-42-E-Paper
 * See http://www.dsbird.org.uk
 * Copyright (c) David Bird
 */
void WeatherDisplay::DrawMoon(int x, int y, int dd, int mm, int yy)
{
   const int diameter        = 45;
   const int number_of_lines = 90;
   double    Phase           = NormalizedMoonPhase(dd, mm, yy);
   
   canvas.drawCircle(x + diameter - 1, y + diameter, diameter / 2 + 1, M5EPD_Canvas::G15);
   
   for (double Ypos = 0; Ypos <= number_of_lines / 2; Ypos++) {
      double Xpos = sqrt(number_of_lines / 2 * number_of_lines / 2 - Ypos * Ypos);
      
      double Rpos = 2 * Xpos;
      double Xpos1, Xpos2;
      
      if (Phase < 0.5) {
         Xpos1 = -Xpos;
         Xpos2 = Rpos - 2 * Phase * Rpos - Xpos;
      } else {
         Xpos1 = Xpos;
         Xpos2 = Xpos - 2 * Phase * Rpos + Rpos;
      }
      double pW1x = (Xpos1 + number_of_lines) / number_of_lines * diameter + x;
      double pW1y = (number_of_lines - Ypos)  / number_of_lines * diameter + y;
      double pW2x = (Xpos2 + number_of_lines) / number_of_lines * diameter + x;
      double pW2y = (number_of_lines - Ypos)  / number_of_lines * diameter + y;
      double pW3x = (Xpos1 + number_of_lines) / number_of_lines * diameter + x;
      double pW3y = (Ypos + number_of_lines)  / number_of_lines * diameter + y;
      double pW4x = (Xpos2 + number_of_lines) / number_of_lines * diameter + x;
      double pW4y = (Ypos + number_of_lines)  / number_of_lines * diameter + y;
      
      canvas.drawLine(pW1x, pW1y, pW2x, pW2y, M5EPD_Canvas::G15);
      canvas.drawLine(pW3x, pW3y, pW4x, pW4y, M5EPD_Canvas::G15);
   }
   canvas.drawCircle(x + diameter - 1, y + diameter, diameter / 2, M5EPD_Canvas::G15);
}

/* Draw the moon information with moonrise, moonset and moon phase */
void WeatherDisplay::DrawMoonInfo(int x, int y, int dx, int dy)
{
   rtc_date_t date_struct;
   
   M5.RTC.getDate(&date_struct);
   
   canvas.setTextSize(3);
   canvas.drawCentreString("Moon", x + dx / 2, y + 7, 1);
   canvas.drawLine(x, y + 35, x + dx, y + 35, M5EPD_Canvas::G15);

   canvas.setTextSize(3);
   DrawIcon(x + 30, y + 40, (uint16_t *) MOONRISE64x64);
   canvas.drawString(getHourMinString(myData.moonRise), x + 110, y + 65, 1);
   
   DrawIcon(x + 30, y + 105, (uint16_t *) MOONSET64x64);
   canvas.drawString(getHourMinString(myData.moonSet), x + 110, y + 130, 1);

   DrawMoon(x + dx / 2 - 45, y + 160, date_struct.day, date_struct.mon, date_struct.year);
}

/* Draw the in the wind section
 * The wind section drawing was from the github project
 * https://github.com/G6EJD/ESP32-Revised-Weather-Display-42-E-Paper
 * See http://www.dsbird.org.uk
 * Copyright (c) David Bird
 */
void WeatherDisplay::Arrow(int x, int y, int asize, float aangle, int pwidth, int plength) 
{
   float dx = (asize + 21) * cos((aangle - 90) * PI / 180) + x; // calculate X position
   float dy = (asize + 21) * sin((aangle - 90) * PI / 180) + y; // calculate Y position
   float x1 = 0;           float y1 = plength;
   float x2 = pwidth / 2;  float y2 = pwidth / 2;
   float x3 = -pwidth / 2; float y3 = pwidth / 2;
   float angle = aangle * PI / 180;
   float xx1 = x1 * cos(angle) - y1 * sin(angle) + dx;
   float yy1 = y1 * cos(angle) + x1 * sin(angle) + dy;
   float xx2 = x2 * cos(angle) - y2 * sin(angle) + dx;
   float yy2 = y2 * cos(angle) + x2 * sin(angle) + dy;
   float xx3 = x3 * cos(angle) - y3 * sin(angle) + dx;
   float yy3 = y3 * cos(angle) + x3 * sin(angle) + dy;
   canvas.fillTriangle(xx1, yy1, xx3, yy3, xx2, yy2, M5EPD_Canvas::G15);
}

/* Draw the wind circle with the windspeed data
 * The wind section drawing was from the github project
 * https://github.com/G6EJD/ESP32-Revised-Weather-Display-42-E-Paper
 * See http://www.dsbird.org.uk
 * Copyright (c) David Bird
 */
void WeatherDisplay::DisplayDisplayWindSection(int x, int y, float angle, float windspeed, int cradius) 
{
   int dxo, dyo, dxi, dyi;

   canvas.setTextSize(2);
   canvas.drawLine(0, 15, 0, y + cradius + 30, M5EPD_Canvas::G15);
   canvas.drawCircle(x, y, cradius, M5EPD_Canvas::G15);     // Draw compass circle
   canvas.drawCircle(x, y, cradius + 1, M5EPD_Canvas::G15); // Draw compass circle
   canvas.drawCircle(x, y, cradius * 0.7, M5EPD_Canvas::G15); // Draw compass inner circle
   for (float a = 0; a < 360; a = a + 22.5) {
      dxo = cradius * cos((a - 90) * PI / 180);
      dyo = cradius * sin((a - 90) * PI / 180);
      if (a == 45)  canvas.drawCentreString("NE", dxo + x + 15, dyo + y - 15, 1);
      if (a == 135) canvas.drawCentreString("SE", dxo + x + 15, dyo + y  + 5, 1);
      if (a == 225) canvas.drawCentreString("SW", dxo + x - 15, dyo + y  + 5, 1);
      if (a == 315) canvas.drawCentreString("NW", dxo + x - 15, dyo + y - 15, 1);
      dxi = dxo * 0.9;
      dyi = dyo * 0.9;
      canvas.drawLine(dxo + x, dyo + y, dxi + x, dyi + y, M5EPD_Canvas::G15);
      dxo = dxo * 0.7;
      dyo = dyo * 0.7;
      dxi = dxo * 0.9;
      dyi = dyo * 0.9;
      canvas.drawLine(dxo + x, dyo + y, dxi + x, dyi + y, M5EPD_Canvas::G15);
   }
   canvas.drawCentreString("N", x, y - cradius - 20, 1);
   canvas.drawCentreString("S", x, y + cradius + 5, 1);
   canvas.drawCentreString("W", x - cradius - 15, y - 3, 1);
   canvas.drawCentreString("E", x + cradius + 15,  y - 3, 1);
   canvas.drawCentreString(String(windspeed, 1), x, y - 20, 1);
   canvas.drawCentreString("m/s", x, y, 1);

   Arrow(x, y, cradius - 17, angle, 15, 27);
}

/* Draw the wind information part */
void WeatherDisplay::DrawWindInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(3);
   canvas.drawCentreString("Wind", x + dx / 2, y + 7, 1);
   canvas.drawLine(x, y + 35, x + dx, y + 35, M5EPD_Canvas::G15);

   DisplayDisplayWindSection(x + dx / 2, y + dy / 2 + 20, myData.weather.winddir, myData.weather.windspeed, 75);
}

/* Draw the M5Paper environment and RTC information */
void WeatherDisplay::DrawM5PaperInfo(int x, int y, int dx, int dy)
{
   canvas.setTextSize(3);
   canvas.drawCentreString("M5Paper", x + dx / 2, y + 7, 1);
   canvas.drawLine(x, y + 35, x + dx, y + 35, M5EPD_Canvas::G15);

   canvas.setTextSize(3);
   canvas.drawCentreString(getRTCDateString(), x + dx / 2, y + 55, 1);
   canvas.drawCentreString(getRTCTimeString(), x + dx / 2, y + 95, 1);
   canvas.setTextSize(2);
   canvas.drawCentreString("updated", x + dx / 2, y + 120, 1);

   canvas.setTextSize(3);
   DrawIcon(x + 35, y + 140, (uint16_t *) TEMPERATURE64x64);
   canvas.drawString(String(myData.sht30Temperatur) + " C", x + 35, y + 210, 1);
   DrawIcon(x + 145, y + 140, (uint16_t *) HUMIDITY64x64);
   canvas.drawString(String(myData.sht30Humidity) + "%", x + 150, y + 210, 1);
   
}

/* Draw one hourly weather information */
void WeatherDisplay::DrawHourly(int x, int y, int dx, int dy, Weather &weather, int index)
{
   time_t time = weather.hourlyTime[index];
   int    temp = weather.hourlyMaxTemp[index];
   String main = weather.hourlyMain[index];
   String icon = weather.hourlyIcon[index];
   
   canvas.setTextSize(2);
   canvas.drawCentreString(getHourString(time) + ":00", x + dx / 2, y + 10, 1);
   canvas.drawCentreString(String(temp) + " C",         x + dx / 2, y + 30, 1);
   // canvas.drawCentreString(main,                        x + dx / 2, y + 70, 1);

   int iconX = x + dx / 2 - 32;
   int iconY = y + 50;
   
   // DrawIcon(x + dx / 2 - 32, y + 50, (uint16_t *) image_data_03d, 64, 64, true);
   
        if (icon == "01d") DrawIcon(iconX, iconY, (uint16_t *) image_data_01d, 64, 64, true);
   else if (icon == "01n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "02d") DrawIcon(iconX, iconY, (uint16_t *) image_data_02d, 64, 64, true);
   else if (icon == "02n") DrawIcon(iconX, iconY, (uint16_t *) image_data_02n, 64, 64, true);
   else if (icon == "03d") DrawIcon(iconX, iconY, (uint16_t *) image_data_03d, 64, 64, true);
   else if (icon == "03n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "04d") DrawIcon(iconX, iconY, (uint16_t *) image_data_04d, 64, 64, true);
   else if (icon == "04n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "09d") DrawIcon(iconX, iconY, (uint16_t *) image_data_09d, 64, 64, true);
   else if (icon == "09n") DrawIcon(iconX, iconY, (uint16_t *) image_data_09n, 64, 64, true);
   else if (icon == "10d") DrawIcon(iconX, iconY, (uint16_t *) image_data_10d, 64, 64, true);
   else if (icon == "10n") DrawIcon(iconX, iconY, (uint16_t *) image_data_03n, 64, 64, true);
   else if (icon == "11d") DrawIcon(iconX, iconY, (uint16_t *) image_data_11d, 64, 64, true);
   else if (icon == "11n") DrawIcon(iconX, iconY, (uint16_t *) image_data_11n, 64, 64, true);
   else if (icon == "13d") DrawIcon(iconX, iconY, (uint16_t *) image_data_13d, 64, 64, true);
   else if (icon == "13n") DrawIcon(iconX, iconY, (uint16_t *) image_data_13n, 64, 64, true);
   else if (icon == "50d") DrawIcon(iconX, iconY, (uint16_t *) image_data_50d, 64, 64, true);
   else if (icon == "50n") DrawIcon(iconX, iconY, (uint16_t *) image_data_50n, 64, 64, true);
   else DrawIcon(iconX, iconY, (uint16_t *) image_data_unknown, 64, 64, true);
}

/* Draw a graph with x- and y-axis and values */
void WeatherDisplay::DrawGraph(int x, int y, int dx, int dy, String title, int xMin, int xMax, int yMin, int yMax, float values[])
{
   String yMinString = String(yMin);
   String yMaxString = String(yMax);
   int    textWidth  = 5 + max(yMinString.length() * 3.5, yMaxString.length() * 3.5);
   int    graphX     = x + 5 + textWidth + 5;
   int    graphY     = y + 35;
   int    graphDX    = dx - textWidth - 20;
   int    graphDY    = dy - 35 - 20;
   float  xStep      = graphDX / (xMax - xMin);
   float  yStep      = graphDY / (yMax - yMin);
   int    iOldX      = 0;
   int    iOldY      = 0;

   canvas.setTextSize(2);
   canvas.drawCentreString(title, x + dx / 2, y + 10, 1);
   canvas.setTextSize(1);
   canvas.drawString(yMaxString, x + 5, graphY - 5);   
   canvas.drawString(yMinString, x + 5, graphY + graphDY - 3);   
   for (int i = 0; i <= (xMax - xMin); i++) {
      canvas.drawString(String(i), graphX + i * xStep, graphY + graphDY + 5);   
   }
   
   canvas.drawRect(graphX, graphY, graphDX, graphDY, M5EPD_Canvas::G15);   
   if (yMin < 0 && yMax > 0) { // null line?
      float yValueDX = (float) graphDY / (yMax - yMin);
      int   yPos     = graphY + graphDY - (0.0 - yMin) * yValueDX;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.drawString("0", graphX - 20, yPos);   
      for (int xDash = graphX; xDash < graphX + graphDX - 10; xDash += 10) {
         canvas.drawLine(xDash, yPos, xDash + 5, yPos, M5EPD_Canvas::G15);         
      }
   }
   for (int i = xMin; i <= xMax; i++) {
      float yValue   = values[i - xMin];
      float yValueDY = (float) graphDY / (yMax - yMin);
      int   xPos     = graphX + graphDX / (xMax - xMin) * i;
      int   yPos     = graphY + graphDY - (yValue - yMin) * yValueDY;

      if (yPos > graphY + graphDY) yPos = graphY + graphDY;
      if (yPos < graphY)           yPos = graphY;

      canvas.fillCircle(xPos, yPos, 2, M5EPD_Canvas::G15);
      if (i > xMin) {
         canvas.drawLine(iOldX, iOldY, xPos, yPos, M5EPD_Canvas::G15);         
      }
      iOldX = xPos;
      iOldY = yPos;
   }
}

/* Main function to show all the data to the e-paper */
void WeatherDisplay::Show()
{
   Serial.println("WeatherDisplay::Show");

   canvas.createCanvas(960, 540);

   canvas.setTextSize(2);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);

   DrawHead();

   // x = 960 y = 540
   // 540 - oben 35 - unten 10 = 495
   
   canvas.drawRect(14, 34, maxX - 28, maxY - 43, M5EPD_Canvas::G15);
   
   canvas.drawRect(15, 35, maxX - 30, 251, M5EPD_Canvas::G15);
   canvas.drawLine(232, 35, 232, 286, M5EPD_Canvas::G15);
   canvas.drawLine(465, 35, 465, 286, M5EPD_Canvas::G15);
   canvas.drawLine(697, 35, 697, 286, M5EPD_Canvas::G15);
   DrawSunInfo    ( 15, 35, 232, 251);
   DrawMoonInfo   (232, 35, 232, 251);
   DrawWindInfo   (465, 35, 232, 251);
   DrawM5PaperInfo(697, 35, 245, 251);

   canvas.drawRect(15, 286, maxX - 30, 122, M5EPD_Canvas::G15);
   for (int x = 15, i = 0; x <= 930; x += 116, i += 3) {
      canvas.drawLine(x, 286, x, 408, M5EPD_Canvas::G15);
      DrawHourly(x, 286, 116, 122, myData.weather, i);
   }

   canvas.drawRect(15, 408, maxX - 30, 122, M5EPD_Canvas::G15);
   DrawGraph( 15, 408, 232, 122, "Temperature (C)", 0, 7, -20,   30, myData.weather.forecastMaxTemp);
   DrawGraph( 15, 408, 232, 122, "Temperature (C)", 0, 7, -20,   30, myData.weather.forecastMinTemp);
   DrawGraph(247, 408, 232, 122, "Rain (mm)",       0, 7,   0,   myData.weather.maxRain, myData.weather.forecastRain);
   DrawGraph(479, 408, 232, 122, "Humidity (%)",    0, 7,   0,  100, myData.weather.forecastHumidity);
   DrawGraph(711, 408, 232, 122, "Pressure (hPa)",  0, 7, 800, 1400, myData.weather.forecastPressure);
   
   canvas.pushCanvas(0, 0, UPDATE_MODE_GC16);
   delay(1000);
}

/* Update only the M5Paper part of the global data */
void WeatherDisplay::ShowM5PaperInfo()
{
   Serial.println("WeatherDisplay::ShowM5PaperInfo");

   canvas.createCanvas(245, 251);

   canvas.setTextSize(2);
   canvas.setTextColor(WHITE, BLACK);
   canvas.setTextDatum(TL_DATUM);

   canvas.drawRect(0, 0, 245, 251, M5EPD_Canvas::G15);
   DrawM5PaperInfo(0, 0, 245, 251);
   
   canvas.pushCanvas(697, 35, UPDATE_MODE_GC16);
   delay(1000);
}
