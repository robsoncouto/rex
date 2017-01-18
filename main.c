
#define F_CPU 8000000UL

#include<avr/io.h>
#include <avr/wdt.h>
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
   uint8_t isJumping;
   uint8_t x;
   uint8_t y;
   uint8_t w;
   uint8_t h;
   const uint8_t* sprite;
   uint8_t steps;
}dino;

typedef struct  {
   uint8_t x;
   uint8_t y;
   uint8_t w;
   uint8_t h;
   const uint8_t* sprite;
   uint8_t alive;
}cacti;

struct ground {
   uint8_t x;
   int8_t y;
   uint8_t w;
   uint8_t h;
   const uint8_t* sprite;
};

//const char points[]={8,13,19,23,28,31,35,37,40,41,43,44,43,41,40,37,35,31,28,23,19,13,8};
//const char points[]={60,53,50,47,45,44,42,41,40,39,38,37,36,36,35,34,34,33,33,32,32,32,31,31,31,30,30,30,30,30,30};
const char points[]={38,31,28,25,23,22,20,19,18,17,16,15,14,14,13,12,12,11,11,10,10,10,9,9,9,8,8,8,8,8,8};


void draw_ground(void){
  static uint8_t i=0;
  drawbitmap(buffer, 0, 56, gnd+i, 128-i, 8, 1);
  drawbitmap(buffer, 128-i, 56, gnd, i, 8, 1);
  i++;
  if(i==128)i=0;
}
uint8_t draw_cactus(cacti cactus,uint8_t color){
  return drawbitmap2(buffer, cactus.x, cactus.y, cactus.sprite, cactus.w, cactus.h, color);
}

uint8_t draw_dino(dino rex, uint8_t color){
  return drawbitmap2(buffer, rex.x, rex.y, rex.sprite, 20, 24, color);
}

void updateJump(dino* rex){
  static uint8_t index=0;
  if(rex->isJumping){
    if(rex->isJumping>31){
      rex->y=points[index];
      rex->isJumping--;
      index++;
    }else{
      index--;
      rex->y=points[index]  ;
      rex->isJumping--;
      if(rex->isJumping==0)rex->y=40;
    }
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

const uint8_t* cactsmall[6];
const uint8_t* cactbig[6];




void create_cactus(cacti* cactus){
  cactus->x=127;// Fixed
  cactus->alive=0xFF;
  if(get_rand2()%2==0){
    cactus->y=48;
    cactus->w=8;
    cactus->h=16;
    cactus->sprite=cactsmall[get_rand2()];//cactsmall[getrand(5)];//&cacts3[0];//
  }else{
    cactus->y=40;
    cactus->w=12;
    cactus->h=24;
    cactus->sprite=cactbig[get_rand2()];//cactsmall[getrand(5)];//&cacts3[0];//
  }
}
void delete_cactus(cacti* cactus){
  cactus->alive=0;
}


void create_dino(dino* rex){
  rex->x = 20;
  rex->y = 40;
  rex->w = 20;
  rex->h = 24;
  rex->sprite=dino3;
  rex->isJumping=0;
  rex->steps=0;
}

void draw_score(uint16_t score){
  char s[5];
  itoa(score,s,10);
  drawstring(buffer, 112, 0,s);
  write_part(buffer,112,0,15,8);
}
void draw_highscore(uint16_t high){
  //FIXME
  char s[5];
  itoa(high,s,10);
  drawstring(buffer, 92, 0,s);
  write_part(buffer,92,0,15,8);
}


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
  uint8_t nof_cacti=0;
  uint8_t tail=0;

  uint8_t nextCactus=0;
  uint16_t highscore=0,score=0;

  init_hardware();
  highscore=get_score();

  //update_score(0);
  clear_screen();
  uint8_t status=0;
  clear_buffer(buffer);
  for(int j=0;j<MAX_CAC;j++){
      //create_cactus(&cac[j]);
      delete_cactus(&cac[j]);
  }
  draw_highscore(highscore);
  draw_score(score);

  while(1){
    status=0;
    clear_buffer(buffer);
    if(buttonIsPressed()){
       if(!(Rex.isJumping)){
         Rex.isJumping=62;
       }
    }
    updateWalk(&Rex);
    updateJump(&Rex);
    draw_dino(Rex,1);

    //nof_cacti=0;
    if(nof_cacti<=MAX_CAC){
      if((!cac[tail].alive)&(nextCactus==0)){
        if(get_rand2()==0){
          create_cactus(&cac[tail]);
          tail++;
          nof_cacti++;
          nextCactus=60;
        }
      }
      if (tail==MAX_CAC){
        tail=0;
      }
    }
    write_part(buffer,Rex.x,Rex.y,Rex.w,Rex.h);
    for(int j=0;j<MAX_CAC;j++){
      if(cac[j].alive){
        if(cac[j].x<1){
          delete_cactus(&cac[j]);
          score++;
          nof_cacti--;
          draw_cactus(cac[j],0);
          draw_score(score);
        }else{
          cac[j].x--;//Wrong order -> buffer, update, LCD
                     //right order -> update, buffer, LCD
          status|=draw_cactus(cac[j],1);
        }
        write_part(buffer,cac[j].x,cac[j].y,cac[j].w,cac[j].h);
      }
    }


    draw_ground();
    write_part(buffer,0,56,128,8);//gnd
    //write_buffer(buffer);

    //_delay_ms(0);
    if (status) {
      drawstring(buffer,18,4,"G A M E  O V E R");
      write_part(buffer,18,32,100,8);//FIXME
      if(score>highscore)update_score(score);
      while (1) {
      if(buttonIsPressed()){
        reset();
      }
      }
    }
    if(nextCactus)nextCactus--;

    if(Rex.isJumping){
      draw_dino(Rex,0);
      write_part(buffer,Rex.x,Rex.y,Rex.w,Rex.h);
    }
    for(int j=0;j<MAX_CAC;j++){
      if(cac[j].alive){
        draw_cactus(cac[j],0);
        write_part(buffer,cac[j].x,cac[j].y,cac[j].w,cac[j].h);
      }
    }
    //i++;
    //if(i==50)i=0;
  }

}
