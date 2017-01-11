
#define F_CPU 8000000UL

#include<avr/io.h>
#include<util/delay.h>
#include <string.h>
#include<stdlib.h>
#include"sprites.h"
#include "hardware.h"
#include "ST7565-T3/c/stlcd.h"
#include "ST7565-T3/c/glcd.h"

//=========================================
uint8_t buffer[128*64/8];

typedef struct {
   int max_speed;
   uint8_t isJumping;
   int min_jum_height;
   int speed;
   uint8_t jumpFrame;
   int8_t x;
   int8_t y;
   uint8_t w;
   uint8_t h;
   const uint8_t* sprite;
   uint8_t steps;
   uint8_t hasChanged;
}dino;

typedef struct  {
   int x;
   int y;
   uint8_t w;
   uint8_t h;
   const uint8_t* sprite;
   uint8_t alive;
}cacti;

struct ground {
   int x;
   int y;
   uint8_t w;
   uint8_t h;
   const uint8_t* sprite;
};

const char points[]={8,13,19,23,28,31,35,37,40,41,43,44,43,41,40,37,35,31,28,23,19,13,8};

void draw_ground(void){
  static uint8_t i=0;
  drawbitmap(buffer, 0, 56, gnd+i, 128-i, 8, 1);
  drawbitmap(buffer, 128-i, 56, gnd, i, 8, 1);
  i++;
  if(i==128)i=0;
}
void draw_cactus(cacti cactus){
  drawbitmap(buffer, cactus.x, cactus.y, cactus.sprite, 8, 16, 1);
}
void draw_cactusb(cacti cactus){
  drawbitmap(buffer, cactus.x, cactus.y, cactus.sprite, 12, 24, 1);
}

void draw_dino(dino rex, uint8_t color){
  drawbitmap(buffer, rex.x, rex.y, rex.sprite, 20, 24, color);
}

void updateJump(dino* rex){
  if(rex->isJumping>0){
    rex->y=45 -points[24-rex->isJumping];
    rex->isJumping--;
  }else{
    rex->y=40;
  }
}
void updateWalk(dino* rex){
  if(!(rex->isJumping)){
    rex->steps++;
    if(rex->steps==20){
      if (rex->sprite==dino3) {
        rex->sprite=dino4;
      }else{
        rex->sprite=dino3;
      }
      rex->steps=0;
    }
  }
}
void draw_cacti(cacti* cactus){
    if(cactus->alive){
      draw_cactus(*cactus);
    }
    if(cactus->x==0){
      cactus->alive=0;
    }
}

void create_cactus(cacti* cactus){
  cactus->x=128;
  cactus->y=48;
  cactus->w=11;
  cactus->h=24;
  cactus->sprite=&cacts3[0];//cactsmall[getrand(6)];
  cactus->alive=1;
}
void delete_cactus(cacti* cactus){
  cactus->alive=0;
}


void create_dino(dino* rex){
  rex->x = 20;
  rex->y = 35;
  rex->w = 20;
  rex->h = 24;
  rex->sprite=dino3;
  rex->isJumping=0;
  rex->steps=0;
}


const unsigned char *cactsmall[6];
const unsigned char *cactbig[6];

#define MAX_CAC 3

void create_cactus(cacti* cactus);
int main(void){

  cactsmall[0]=cacts1;
  cactsmall[1]=cacts2;
  cactsmall[2]=cacts3;
  cactsmall[3]=cacts4;
  cactsmall[4]=cacts5;
  cactsmall[5]=cacts6;

  cactbig[0]=cactusb1;
  cactbig[1]=cactusb2;
  cactbig[2]=cactusb3;
  cactbig[3]=cactusb4;
  cactbig[4]=cactusb5;
  cactbig[5]=cactusb6;

  dino Rex;
  create_dino(&Rex);
  cacti cac[MAX_CAC];
  int nof_cacti=0;
  uint8_t tail=0;


  create_cactus(&cac[tail]);
  tail++;



  init_hardware();
  clear_screen();
  uint8_t cc=0;
  clear_buffer(buffer);

  while(1){

    clear_buffer(buffer);
    //clearPages();
    //clear_screen();
    draw_ground();
    if(!read_button()){
       if(!(Rex.isJumping)){
         Rex.isJumping=24;
       }
    }
    updateWalk(&Rex);
    updateJump(&Rex);
    draw_dino(Rex,1);

    char s[10];
    itoa(Rex.y,s,10);
    drawstring(buffer, 120, 0,s);
    cc=0;
    cc=getrand(6);
    memset(s,0,10);
    itoa(cc,s,10);
    drawstring(buffer, 110, 0,s);


    if(nof_cacti<MAX_CAC){
      if(getrand(6)==0){
        create_cactus(&cac[tail]);
        tail++;
        nof_cacti++;
      }
      if (tail==MAX_CAC){
        tail=0;
      }
    }

    for(int j=0;j<MAX_CAC;j++){
      if(cac[j].x<20)
        delete_cactus(&cac[j]);
      if(cac[j].alive)
        draw_cacti(&cac[j]);
        cac[j].x--;
        write_part(buffer,abs(cac[j].x),cac[j].y,cac[j].w,cac[j].h);
    }
    write_part(buffer,Rex.x,Rex.y,Rex.w,Rex.h);
    write_part(buffer,0,56,128,8);
    write_part(buffer,110,0,20,8);
    //write_buffer(buffer);
    // if(cac[0].x==0){
    //   cac[0].sprite=cactsmall[getrand(6)];//cactsmall[getrand(6)];
    //   cac[0].x=128;
    // }

    _delay_ms(10);
    if(Rex.isJumping){
      draw_dino(Rex,0);
      write_part(buffer,Rex.x,Rex.y,Rex.w,Rex.h);
    }
    //i++;
    //if(i==50)i=0;
  }
}
