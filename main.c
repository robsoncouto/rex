#define F_CPU 16000000UL

#include<avr/io.h>
#include<util/delay.h>
#include<stdlib.h>
#include "sprites.h"
#include "data.h"
#include "stlcd.h"

const unsigned char *cactsmall[6];
unsigned char *cactbig[6];

void create_cactus(cacti* cactus);
int main(void){

  //array of bitmap pointers
  //Chosen randomly
  //Small cacti
  cactsmall[0]=cacts1;
  cactsmall[1]=cacts2;
  cactsmall[2]=cacts3;
  cactsmall[3]=cacts4;
  cactsmall[4]=cacts5;
  cactsmall[5]=cacts6;

  //bigger cacti
  cactbig[0]=cactusb1;
  cactbig[1]=cactusb2;
  cactbig[2]=cactusb3;
  cactbig[3]=cactusb4;
  cactbig[4]=cactusb5;
  cactbig[5]=cactusb6;

  dino Rex;
  create_dino(&Rex);
  cacti cac[10];
  int nof_cacti=0;
  uint8_t tail=0;


  create_cactus(&cac[tail]);
  tail++;
  setup();
  init_hardware();
  uint8_t i=0;
  clear_buffer();

  int k=1;
  while(1){
    clear_buffer();
    draw_ground();
    if(!read_button()){
       if(!(Rex.isJumping)){
         Rex.isJumping=24;
       }
    }
    updateWalk(&Rex);
    updateJump(&Rex);
    draw_dino(Rex);

    unsigned char s[10];
    itoa(Rex.y,s,10);
    drawstring(&i, 100, 0,s);
    i=0;
    i=getrand(6);
    memset(s,0,10);
    itoa(i,s,10);
    drawstring(&i, 50, 0,s);

    k++;
    //draw_cactus(cac[0]);
    if(nof_cacti<10){
      if(getrand(6)%2==0){
        create_cactus(&cac[tail]);
        tail++;
      }
    }
    if (tail==10){
      tail=0;
    }
    //checks on every cactus
    for(int j=0;j<10;j++){
      draw_cacti(&cac[j]);
      if (cac[j].x) {

      }
    }
    write_buffer();

    // if(cac[0].x==0){
    //   cac[0].sprite=cactsmall[getrand(6)];//cactsmall[getrand(6)];
    //   cac[0].x=128;
    // }

    //_delay_ms(500);
    //i++;
    //if(i==50)i=0;
  }
}
