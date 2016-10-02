#include "stlcd.h"
#include "sprites.h"

typedef struct {
   int max_speed;
   uint8_t isJumping;
   int min_jum_height;
   int speed;
   uint8_t jumpFrame;
   int x;
   int y;
   char* sprite;
   uint8_t steps;
}dino;

typedef struct  {
   int x;
   int y;
   const unsigned char* sprite;
   uint8_t alive;
}cacti;

struct ground {
   int x;
   int y;
   char* sprite;
};

const char points[]={8,13,19,23,28,31,35,37,40,41,43,44,43,41,40,37,35,31,28,23,19,13,8};

void draw_ground(void){
  static uint8_t i=0;
  drawbitmap( 0, 55, gnd+i, 128, 8, 1);
  i++;
  if(i==256)i=0;
}
void draw_cactus(cacti cactus){
  drawbitmap( cactus.x, cactus.y, cactus.sprite, 8, 16, 1);
}
void draw_cactusb(cacti cactus){
  drawbitmap( cactus.x, cactus.y, cactus.sprite, 12, 24, 1);
}

void draw_dino(dino rex){
  drawbitmap( rex.x, rex.y, rex.sprite, 20, 24, 1);
}
void start(void){
  DDRD=0x01;
  PORTD=0x01;
}
uint8_t buttonIsPressed(void){
  return PIND&(1<<3);
}
void updateJump(dino* rex){
  if(rex->isJumping>0){
    rex->y=40 -points[24-rex->isJumping];
    rex->isJumping--;
  }else{
    rex->y=35;
  }
}
void updateWalk(dino* rex){
  if(!(rex->isJumping)){
    rex->steps++;
    if(rex->steps==3){
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
  cactus->y=40;
  cactus->sprite=&cacts3[0];//cactsmall[getrand(6)];
  cactus->alive=1;
}
void delete_cactus(cacti* cactus){
  cactus->alive=0;
}


void create_dino(dino* rex){
  rex->x = 20;
  rex->y = 35;
  rex->sprite=dino3;
  rex->isJumping=0;
  rex->steps=0;
}



void init_hardware(void){
  //DDRC&=~(1<<PIN3);
  //ADC
  ADMUX = (1<<REFS0)|(1<<MUX1)|(1<<MUX0);//PIN ADC7 used (ADC exclusive)
  ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS0);
  //button
  DDRD&=~(1<<7);
}
uint16_t get_adc(uint8_t channel){
  ADMUX&=0xF0;
  ADMUX|=channel;
  ADCSRA |= (1<<ADSC);
  while(ADCSRA & (1<<ADSC));
  return (ADC);
}
uint8_t read_button(void){
  return PIND&(1<<7);
}
uint8_t getrand(uint8_t max){
  srand(get_adc(2));
  uint8_t randNumber;
  randNumber=rand()%max;
  return randNumber;
}

//TODO
/*
//update game:
//-read button state
//-draw dino
//-move and draw cacti
//-move ground
//-check colisions
//-display score
//-write ADC code for rand

*/
