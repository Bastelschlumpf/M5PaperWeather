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

#ifdef USE_EPDIY_V7
#include <epdiy.h>
#include <epd_highlevel.h>
extern EpdiyHighlevelState hl;
extern uint8_t* epd_buffer;

/* Initialize the EPDiy driver */
void InitEPD(bool clearDisplay = true)
{
   auto cfg = M5.config();
   cfg.clear_display = false;
   M5.begin(cfg);

   epd_init(&epd_board_v7, &ED047TC1, EPD_LUT_64K);
   epd_set_vcom(1600);
   hl = epd_hl_init(EPD_BUILTIN_WAVEFORM);
   epd_buffer = epd_hl_get_framebuffer(&hl);
   epd_set_rotation(EPD_ROT_LANDSCAPE);
   if (clearDisplay) {
      epd_poweron();
      epd_hl_set_all_white(&hl);
      epd_hl_update_screen(&hl, MODE_GC16, 25);
      epd_poweroff();
   }
}

void ShutdownEPD(int sec)
{
   Serial.println("Shutdown");
   epd_poweroff();
   esp_sleep_enable_timer_wakeup((uint64_t) sec * 1000000ULL);
   esp_deep_sleep_start();
}
#elif defined(USE_M5_UNIFIED)
/* Initialize the M5Paper */
void InitEPD(bool clearDisplay = true)
{
   auto cfg = M5.config();
   cfg.clear_display = clearDisplay;
   M5.begin(cfg);

   M5.Display.setRotation(0);
   M5.Touch.setRotation(0);
}

void ShutdownEPD(int sec)
{
   Serial.println("Shutdown");
   M5.Display.sleep();
   M5.Display.waitDisplay();
   M5.Power.timerSleep(sec);
}
#else
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
#endif
