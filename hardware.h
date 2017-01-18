#ifndef HARDWARE_H
#define HARDWARE_H
#define SCORE_LOCATION 0x3FE
void init_hardware(void);
uint16_t get_adc(uint8_t channel);
uint8_t getrand(uint8_t max);
uint8_t buttonIsPressed(void);
void update_score(uint16_t score);
uint16_t get_score(void);

void init_timer(void);
uint8_t get_rand2(void);

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

//change random number generation
//reverse  font
//check glcd for improvements

*/

#endif
