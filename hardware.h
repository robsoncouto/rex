#ifndef HARDWARE_H
#define HARDWARE_H

void init_hardware(void);
uint16_t get_adc(uint8_t channel);
uint8_t getrand(uint8_t max);
uint8_t buttonIsPressed(void);




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

#endif
