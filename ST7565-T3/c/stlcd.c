/*
$Id:$

ST7565 LCD library!

Copyright (C) 2010 Limor Fried, Adafruit Industries

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 // some of this code was written by <cstone@pobox.com> originally; it is in the public domain.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "stlcd.h"
#include "glcd.h"
//#include "util.h"
//#include "logo_glcd.h"


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

void testdrawbitmap(uint8_t *buff, const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
  srandom(buff[666]);

  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random() % 128;
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random() % 5 + 1;
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      drawbitmap(buff, icons[f][XPOS], icons[f][YPOS], bitmap, w, h, 1);
    }
    write_buffer(buff);
    _delay_ms(200);

    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      drawbitmap(buff, icons[f][XPOS], icons[f][YPOS], bitmap, w, h, 0);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > 64) {
	icons[f][XPOS] = random() % 128;
	icons[f][YPOS] = 0;
	icons[f][DELTAY] = random() % 5 + 1;
      }
    }
  }
}

void testdrawchar(uint8_t *buff) {
  for (uint8_t i=0; i < 168; i++) {
    drawchar(buff, (i % 21) * 6, i/21, i);
  }
}

void testdrawcircle(uint8_t *buff) {
  for (uint8_t i=0; i<64; i+=2) {
    drawcircle(buff, 63, 31, i, 1);
  }
}

void testdrawline(uint8_t *buff) {
  for (uint8_t i=0; i<128; i+=4) {
    drawline(buff, 0, 0, i, 63, 1);
  }
  for (uint8_t i=0; i<64; i+=4) {
    drawline(buff, 0, 0, 127, i, 1);
  }

  write_buffer(buff);
  _delay_ms(1000);

  for (uint8_t i=0; i<128; i+=4) {
    drawline(buff, i, 63, 0, 0, 0);
  }
  for (uint8_t i=0; i<64; i+=4) {
    drawline(buff, 127, i, 0, 0, 0);
  }
}

void testdrawrect(uint8_t *buff) {
  for (uint8_t i=0; i<64; i+=2) {
    drawrect(buff, i, i, 128-i, 64-i, 1);

  }
}


void testfillrect(uint8_t *buff) {
  for (uint8_t i=0; i<64; i++) {
    fillrect(buff, i, i, 128-i, 64-i, i%2);

  }
}

void clear_screen(void) {
  uint8_t p, c;

  for(p = 0; p < 8; p++) {
    st7565_command(CMD_SET_PAGE | p);
    for(c = 0; c < 129; c++) {
      st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
      st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
      st7565_data(0x0);
    }
  }
}


void st7565_init(void) {
  // set pin directions
  SID_DDR |= _BV(SID);
  SCLK_DDR |= _BV(SCLK);
  A0_DDR |= _BV(A0);
  RST_DDR |= _BV(RST);
  CS_DDR |= _BV(CS);

  // toggle RST low to reset; CS low so it'll listen to us
  CS_PORT &= ~_BV(CS);
  RST_PORT &= ~_BV(RST);
  _delay_ms(500);
  RST_PORT |= _BV(RST);

  // LCD bias select
  st7565_command(CMD_SET_BIAS_7);
  // ADC select
  st7565_command(CMD_SET_ADC_NORMAL);
  // SHL select
  st7565_command(CMD_SET_COM_NORMAL);
  // Initial display line
  st7565_command(CMD_SET_DISP_START_LINE);

  // turn on voltage converter (VC=1, VR=0, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x4);
  // wait for 50% rising
  _delay_ms(50);

  // turn on voltage regulator (VC=1, VR=1, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x6);
  // wait >=50ms
  _delay_ms(50);

  // turn on voltage follower (VC=1, VR=1, VF=1)
  st7565_command(CMD_SET_POWER_CONTROL | 0x7);
  // wait
  _delay_ms(10);

  // set lcd operating voltage (regulator resistor, ref voltage resistor)
  st7565_command(CMD_SET_RESISTOR_RATIO | 0x6);

  // initial display line
  // set page address
  // set column address
  // write display data
}

inline void spiwrite(uint8_t c) {
  int8_t i;
  for (i=7; i>=0; i--) {
    SCLK_PORT &= ~_BV(SCLK);
    if (c & _BV(i))
      SID_PORT |= _BV(SID);
    else
      SID_PORT &= ~_BV(SID);
    SCLK_PORT |= _BV(SCLK);
  }
}

void st7565_command(uint8_t c) {
  A0_PORT &= ~_BV(A0);

  spiwrite(c);
}

void st7565_data(uint8_t c) {
  A0_PORT |= _BV(A0);

  spiwrite(c);
}
void st7565_set_brightness(uint8_t val) {
    st7565_command(CMD_SET_VOLUME_FIRST);
    st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}


void write_buffer(uint8_t *buffer) {
  uint8_t c, p;
  for(p = 0; p < 8; p++) {
      st7565_command(CMD_SET_PAGE | p);
      st7565_command(CMD_SET_COLUMN_LOWER | 0);
      st7565_command(CMD_SET_COLUMN_UPPER | 0);
      st7565_command(CMD_RMW);

      for(c = 0; c < 128; c++) {
        st7565_data(buffer[(128*p)+c]);
      }
  }
}
void write_part(uint8_t *buffer,uint8_t x, uint8_t y, uint8_t w,uint8_t h) {
  uint8_t c, p;

  for(p = 0; p < 8; p++) {
    if((p)>=y/8 && (p*8)<(y+h)){
      st7565_command(CMD_SET_PAGE | p);

      st7565_command(CMD_SET_COLUMN_LOWER | (x&0x0f));
      st7565_command(CMD_SET_COLUMN_UPPER | ((x>>4)&0x0f));
      st7565_command(CMD_RMW);

      for(c = x; c < x+w; c++) {
        st7565_data(buffer[(128*p)+c]);
      }
    }
  }
}
