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


#define NUM_STATES		4
#define NUM_ACTIONS		4
#define ALPHA			0.3 // Learning rate
#define GAMMA			0.9 // Discount rate
#define MAX_ITTERATION	1000
#define EPSILON			10 // Explorace 10%
#define ACTION_WEIGHT	0.3
#define STATE_WEIGHT 	0.6
#define EPOCHS			10 // Number of epochs
#define VTEMP25			701.2

void switchtoBLPI(void);
void setVLPRmode(void);
void setVLPS(void);
void __attribute__ ((interrupt)) LPTMR0_IRQHandler(void);
void initLPTMR(void);
void startLPTMR(uint16_t x);
int initADC(void);
uint16_t adcRead(void);
void initPins(void);
void init_Qtable(float Qtable[NUM_STATES][NUM_ACTIONS]);
int32_t selectAction(float Qtable[NUM_STATES][NUM_ACTIONS], int state, int currentEpoch, int currentEpsilon);
void updateQ(float Q[NUM_STATES][NUM_ACTIONS], int state, int action, float reward, int nextState);
int custom_rand(void);
float getTemp(void);


#endif /* SRC_APP_HELPER_FUNCTIONS_H_ */
