/*
 * main.h
 *
 *  Created on: 2018Äê5ÔÂ9ÈÕ
 *      Author: weihaochen
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_


#define						PULSE_10MS							10000
#define						PULSE_1MS                           1000
#define						S8_MODE								0x0
#define						S16_MODE							0x1

void GPIO_INIT(void);
interrupt void  t0Isr(void);      // CPU-Timer 0
void display_all(uint16_t speed, uint16_t dir, uint16_t mode );


#endif /* INC_MAIN_H_ */
