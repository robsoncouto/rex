#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <stdlib.h>
#include <stdint.h>

#include "hardware.h"
#include "ST7565-T3/c/stlcd.h"


void init_hardware(void){
  //DDRC&=~(1<<PIN3);
  //ADC
  BLA_DDR |= _BV(BLA);
  BLA_PORT |= _BV(BLA);

  LED_DDR |= _BV(LED);

  LED_PORT |= _BV(LED);
  st7565_init();

  st7565_command(CMD_DISPLAY_ON);
  st7565_command(CMD_SET_ALLPTS_NORMAL);
  st7565_set_brightness(0x05);

  ADMUX = (1<<REFS0)|(1<<MUX1)|(1<<MUX0);//PIN ADC7 used (ADC exclusive)
  ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS0);
  //button
  DDRD&=~(1<<7);
  //PORTD=(1<<7);
  init_timer();
}
uint16_t get_adc(uint8_t channel){
  ADMUX&=0xF0;
  ADMUX|=channel;
  ADCSRA |= (1<<ADSC);
  while(ADCSRA & (1<<ADSC));
  return (ADC);
}
uint8_t getrand(uint8_t max){
  srand(get_adc(2));
  uint8_t randNumber;
  randNumber=rand()%max;
  return randNumber;
}
uint8_t buttonIsPressed(void){
  return !(PIND&(1<<7));
}


uint8_t mcusr_mirror __attribute__ ((section (".noinit")));

void get_mcusr(void) \
  __attribute__((naked)) \
  __attribute__((section(".init3")));
void get_mcusr(void)
{
  mcusr_mirror = MCUSR;
  MCUSR = 0;
  wdt_disable();
}
void update_score(uint16_t score){
  eeprom_update_word(( uint16_t *)SCORE_LOCATION, score);
}
uint16_t get_score(void){
  return eeprom_read_word((const uint16_t *)SCORE_LOCATION);
}

void init_timer(void){
  TCCR0A=1<<WGM01; //CTC mode
  TCCR0B=(0<<WGM02)|(1<<CS00); //using main clock
  OCR0A=0x06; //Max
  TCNT0=0; //start from zero
}
uint8_t get_rand2(void){
  return TCNT0;
}
