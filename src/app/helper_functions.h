/*
 * helper_functions.h
 *
 *  Created on: 6. 2. 2024
 *      Author: Jan Fiedor
 */

#ifndef SRC_APP_HELPER_FUNCTIONS_H_
#define SRC_APP_HELPER_FUNCTIONS_H_


#include "MKL25Z4.h"
#include <stddef.h>
#include "utilities.h"
#include "adc.h"
#include "led.h"


#define NUM_STATES			6
#define NUM_ACTIONS			6
#define ALPHA				0.3 // Learning rate
#define GAMMA				0.9 // Discount rate
#define MAX_ITTERATION		1000
#define EPSILON				10 // Explorace 10%
#define ACTION_WEIGHT		0.2
#define STATE_WEIGHT 		0.8
#define EPOCHS				10 // Number of epochs
#define VTEMP25				701.2
#define VOLTAGE_MCU			2.5
#define VOLTAGE_SUPERCAP 	5.25
#define E_MIN				5.45 // Minimal energy on SuperCap
#define E_MAX				13.78
#define E_STORE				E_MAX-E_MIN
#define SLEEP_20MIN			(uint16_t) 9375	// Value for LPTMR
#define SLEEP_40MIN			(uint16_t) 18750
#define SLEEP_60MIN			(uint16_t) 28125
#define SLEEP_80MIN			(uint16_t) 37500
#define SLEEP_100MIN		(uint16_t) 46875
#define SLEEP_120MIN		(uint16_t) 56250
#define SLEEP_TESTING		(uint16_t) 70


extern int lptmrIntFlag;
extern int btnIntFlag;
extern uint8_t okFlag;

void switchtoBLPI(void);
void setVLPRmode(void);
int setVLPS(void);
void __attribute__ ((interrupt)) LPTMR0_IRQHandler(void);
void initLPTMR(void);
void startLPTMR(uint16_t x);
int initADC(void);
void deInitADC(void);
float getEnergy(void);
void initPins(void);
void init_Qtable(float Qtable[NUM_STATES][NUM_ACTIONS]);
int32_t selectAction(float Qtable[NUM_STATES][NUM_ACTIONS], int state, int currentEpoch, int currentEpsilon);
void updateQ(float Q[NUM_STATES][NUM_ACTIONS], int state, int action, float reward, int nextState);
int custom_rand(void);
float getTemp(void);
void dataPrep(uint8_t* buffer, float value);
void initButton(void);
void __attribute__ ((interrupt)) PORTD_IRQHandler(void);
void radio_ON(void);
void radi_OFF(void);


#endif /* SRC_APP_HELPER_FUNCTIONS_H_ */
