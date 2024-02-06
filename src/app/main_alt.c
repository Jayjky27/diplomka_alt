/*
 * main_alt.c
 *
 *  Created on: 6. 2. 2024
 *      Author: Jan Fiedor
 */

#include "main_alt.h"
#include "helper_functions.h"
#include "assert.h"
#include "utilities.h"

void alt_main(void)
{
	/* SYSTEM INITIALIZATION + VARIABLE DECLARATION */
	/*switchtoBLPI();
	setVLPRmode();*/
	assert(!initADC());
	initPins();
	initLPTMR();

	float Qtable[NUM_STATES][NUM_ACTIONS], reward = 0.0;
	uint16_t valueADC = 0, valueLPTMR = 0;
	int action = 0, currentState = 0, previousState = 0, measureFlag = 0, epochsCnt = 1, epsilon = 90;
	init_Qtable(Qtable);
	srand1(2);

	/* MAIN LOOP */
	while(1) {
		/* DETERMINATION OF ENERGY STATUS */
		valueADC = adcRead();

		if(valueADC < 16000){
			currentState = 0;
		}else if(valueADC >= 48000){
			currentState = 3;
		}else if((valueADC >= 16000) && (valueADC < 32000)){
			currentState = 1;
		}else{
			currentState = 2;
		}

		/* CALCULATION OF THE REWARD + UPDATING THE VALUES IN THE Q-TABLE */
		reward = STATE_WEIGHT*(currentState - previousState) - ACTION_WEIGHT*(action + 1);
		updateQ(Qtable, previousState, action, reward, currentState);

		/* MEASUREMENT + COMMUNICATION */
		switch(currentState) {
			case 1:
				/* MEAS FUNCTION */
				measureFlag = 1;
				break;
			case 2:
				if (measureFlag) {
					/* SEND FUNCTION */
					measureFlag = 0;
				} else {
					/* MEAS FUNCTION */
					measureFlag = 1;
				}
				break;
			case 3:
				/* MEAS FUNCTION */
				/* SEND FUNCTION */
				/* CONSULTATION - possible addition of ADC read to detect energy status */
				measureFlag = 0;
				break;
			default:
				assert(0);
				break;
		}

		/* SELECTION OF ACTION */
		action = selectAction(Qtable, currentState, epochsCnt, epsilon);

		switch(action){
			case 0:
				valueLPTMR = 6000;
				break;

			case 1:
				valueLPTMR = 4000;
				break;

			case 2:
				valueLPTMR = 2000;
				break;

			case 3:
				valueLPTMR = 1000;
				break;
			default:
				assert(0);
				break;
		}

		/* OPERATIONS NEEDED FOR TRAINING */
		if(epochsCnt <= EPOCHS){
			epsilon -= 8;
			epochsCnt++;
		}

		/* STARTING LPTMR + ENTERING SLEEP MODE */
		startLPTMR(valueLPTMR);
		__asm("WFI");
		previousState = currentState;
	}
}
