#ifndef HARDWARE_H
#define HARDWARE_H

void init_hardware(void);
uint16_t get_adc(uint8_t channel);
uint8_t read_button(void);
uint8_t getrand(uint8_t max);
uint8_t buttonIsPressed(void);
#endif
