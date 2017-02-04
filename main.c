
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

//This is the buffer used to transfer data to the LCD
uint8_t buffer[128*64/8];

typedef struct {
   uint8_t isJumping;
   uint8_t x;
   uint8_t y;
   uint8_t w;
   uint8_t h;
   const uint8_t* sprite;
   uint8_t steps;
   uint8_t hasChanged;
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

//Points used for the rex jump
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
void Rex2screen(dino rex, uint8_t color){
  draw_dino(rex,color);
  write_part(buffer,rex.x,rex.y,rex.w,rex.h);
}

void updateJump(dino* rex){
  static uint8_t index=0;
  if(rex->isJumping){
    if(rex->isJumping>31){
      if(points[index]!=rex->y){
        rex->hasChanged=1;
        Rex2screen(*rex, 0);
      }
      rex->y=points[index];
      rex->isJumping--;
      index++;
    }else{
      index--;
      if(points[index]!=rex->y){
        rex->hasChanged=1;
        Rex2screen(*rex, 0);
      }
      rex->y=points[index];
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
        rex->hasChanged=1;
        Rex2screen(*rex, 0);
      }else{
        rex->sprite=dino3;
        rex->hasChanged=1;
        Rex2screen(*rex, 0);
      }
      rex->steps=0;
    }
  }
}

const uint8_t* cactsmall[6];
const uint8_t* cactbig[6];


void create_cactus(cacti* cactus){
  cactus->x=127;// Fixed (It cant be more than 127)
  cactus->alive=0xFF;
  if(getrand(2)%2==0){//gets the type of the cactus(big or small)
    cactus->y=48;
    cactus->w=8;
    cactus->h=16;
    cactus->sprite=cactsmall[getrand(5)];
  }else{
    cactus->y=40;
    cactus->w=12;
    cactus->h=24;
    cactus->sprite=cactbig[getrand(5)];
  }
}

void kill_cactus(cacti* cactus){
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
  rex->hasChanged=1;
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


  //Initise the arrays that keep the sprite addresses
  //One of these values in gotten randomly at cacti creation
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
  uint8_t nof_cacti=0; //current number of cacti on screen
  uint8_t tail=0;//the position of the new cactus on the ring
  uint8_t frames2nxtCac=0; //frames to next cactus. This is a delay to the creation of
                           //the next cactus. So the Rex has some space to land
  uint16_t highscore=0,score=0;

  init_hardware(); //low level atmega stuff (PORTS, ACD, etc)
  highscore=get_score();//FIXME make the score zero when first time (Not used memory=0xFF)

  uint8_t bump=0; //collision between dino and catus kept here
  clear_screen();
  clear_buffer(buffer);

  for(int j=0;j<MAX_CAC;j++){
      kill_cactus(&cac[j]);
  }
  draw_highscore(highscore);//only time this is written to the screen
  draw_score(score);

  while(1){
    bump=0;

    clear_buffer(buffer);//The memory is cleared, but not the LCD
    if(buttonIsPressed()){
       if(!(Rex.isJumping)){
         Rex.isJumping=2*sizeof(points);//The array points has the positions for the jump (2x because is back an forth )
       }
    }
    updateWalk(&Rex); //Updates dino sprite (which leg touches the ground)
    updateJump(&Rex); //Update the dinos position
    draw_dino(Rex,1);

    if(nof_cacti<=MAX_CAC){ //Checks if there are MAX_CAC cacti on screen already
      if((!cac[tail].alive)&(frames2nxtCac==0)){
        if(getrand(16)==0){//"1 in 16 chance" No, I know
          //If the previous conditions are met, create a new cactus and delay creation of new cacti
          create_cactus(&cac[tail]);
          tail++;
          nof_cacti++;
          frames2nxtCac=60;//can be changed

      }
      if (tail==MAX_CAC){
        tail=0;
      }
    }

    //Only writes the dino to the LCD when it has changed position or sprite
    if(Rex.hasChanged){
      write_part(buffer,Rex.x,Rex.y,Rex.w,Rex.h);
      Rex.hasChanged=0;
    }

    //Draw cacti to the buffer, checks collision, and write them to the LCD
    for(int j=0;j<MAX_CAC;j++){
      if(cac[j].alive){
        if(cac[j].x<1){
          kill_cactus(&cac[j]);
          score++;
          nof_cacti--;
          draw_cactus(cac[j],0);
          draw_score(score);//The score is also drawn to the LCD only when changed
        }else{
          cac[j].x--;
          bump|=draw_cactus(cac[j],1);//draw the cati to the buffer and gets a possible collision
        }
        write_part(buffer,cac[j].x,cac[j].y,cac[j].w,cac[j].h);//draw the cati to the LCD
      }
    }


    draw_ground();//draww ground to buffer
    write_part(buffer,0,56,128,8);//draw gnd from buffer to LCD

    _delay_ms(3);//FIXME needs a interrupt/timer scheme to keep fixed fps
    if (bump) {
      drawstring(buffer,18,4,"G A M E  O V E R");
      write_part(buffer,18,32,100,8);//FIXME put in the center
      if(score>highscore)update_score(score); //writes new score to EEPROM`
      while (1) {
      if(buttonIsPressed()){
        reset();//see hardware.c for implementation
      }
      }
    }
    if(frames2nxtCac)frames2nxtCac--;//Each frame decreases delay for new catcus

    //Erase cacti from screen
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
