/*
 * helper_functions.c
 *
 *  Created on: 6. 2. 2024
 *      Author: Jan Fiedor
 */

/*
 * helper_function.c
 *
 *  Created on: 20. 11. 2023
 *      Author: Jan Fiedor
 */
#include "helper_functions.h"

void switchtoBLPI(void)
{
	/* FEI -> FBI */
	MCG->C1 |= MCG_C1_CLKS(1);

	while(!(MCG->S & MCG_S_CLKST(1))){/* wait until CLKST=01b */}

	/* FBI -> BLPI */
	MCG->C2 |= 0x02;

	SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV1(3u); // System/Core clock (4 MHz IRC) / 4 -> = 1MHz
	SIM->CLKDIV1 |= SIM_CLKDIV1_OUTDIV4(1u); // Flash/Bus clock (1MHz) / 2 -> = 500 KHz
}

void setVLPRmode(void)
{
	/* To prevent unexpected loss of clock reset events, all clock
		monitors must be disabled before entering any low power
		modes, including VLPR and VLPW
		MCG->C6 &= ~MCG_C6_CME0_MASK; // disable monitoring */

	SMC->PMPROT |= SMC_PMPROT_AVLP_MASK;	//Allow VLPR mode
	SMC->PMCTRL |= SMC_PMCTRL_RUNM(2); 		//Entry VLPR mode
}

void __attribute__ ((interrupt)) LPTMR0_IRQHandler(void){
	LPTMR0->CSR = LPTMR_CSR_TCF_MASK;	//Clear the LPTMR flag (w1c)
	GPIOB->PCOR |= (1 << 18);			// Turn ON LED
}

void initLPTMR(void)
{
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK; // Clock enable
	LPTMR0->CSR &= ~LPTMR_CSR_TMS_MASK; // Set Time counter mode
	LPTMR0->PSR |= LPTMR_PSR_PRESCALE(1) | LPTMR_PSR_PCS(1); /* Set prescaler and glitch filter ->
																Chip Clock = LPO — 1 kHz clock */
	NVIC_SetVector(LPTMR0_IRQn, (uint32_t)&LPTMR0_IRQHandler);
	NVIC_ClearPendingIRQ(LPTMR0_IRQn);
	NVIC_SetPriority(LPTMR0_IRQn, 2);
	NVIC_EnableIRQ(LPTMR0_IRQn);
	/*	Kód pro nastavení hodnoty čítače (hodnota bude volena MCU jako výsledek akce, kterou zvolí)*/
	//LPTMR0->CMR |= LPTMR_CMR_COMPARE(x);
	//LPTMR0->CMR = x;
	//LPTMR0->CSR |= LPTMR_CSR_TIE_MASK | LPTMR_CSR_TEN_MASK; // Interrupt and LPTMR enable
	//LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
	//while (!(LPTMR0->CSR & LPTMR_CSR_TCF_MASK));

	/* Disable counter and Clear Timer Compare Flag */
	//LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;
}

void startLPTMR(uint16_t x)
{
	LPTMR0->CMR = x;
	LPTMR0->CSR |= LPTMR_CSR_TIE_MASK | LPTMR_CSR_TEN_MASK; // Interrupt and LPTMR enable
}

int initADC(void)
{
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;	// ADC0 clock enable
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; // PORT E clock enable

	// Reset ADC0
	ADC0->SC1[0] = ADC_SC1_ADCH_MASK; // Set the ADCH field to a known value

	ADC0->CFG1 |= ADC_CFG1_ADICLK(0); 	// Set Bus clock as clock source
	ADC0->CFG1 |= ADC_CFG1_ADIV(0);		// Set clock rate: Bus clock/1
	ADC0->CFG1 |= ADC_CFG1_ADLPC_MASK; 	// Low Power mode enable
	ADC0->CFG1 |= ADC_CFG1_MODE(3);		// Set 16-bit conversion

	ADC0->SC2 &= ~(ADC_SC2_ADTRG_MASK); // Set SW trigger
	//ADC0->SC2 |= ADC_SC2_DMAEN_MASK; 	// DMA enable
	ADC0->SC2 |= ADC_SC2_REFSEL(0);		// Select voltage reference 0= default voltage reference pair

	//ADC0->SC3 |= ADC_SC3_AVGE_MASK;	// HW average function enable
	//ADC0->SC3 |= ADC_SC3_AVGS(1);		// 8 samples for HW average function
	ADC0->SC3 |= ADC_SC3_CAL_MASK;		// Start calibration

	while(ADC0->SC3 & ADC_SC3_CAL_MASK); // wait for calibration to end

	if(ADC0->SC3 & ADC_SC3_CALF_MASK){
			return 1;
		}

	/* PLUS side gain register */
	uint16_t calib = 0;
	calib += ADC0->CLPS + ADC0->CLP4 + ADC0->CLP3 +
				 ADC0->CLP2 + ADC0->CLP1 + ADC0->CLP0;
	calib /= 2;
	calib |= 0x8000; // Set MSB
	ADC0->PG |= calib;

	/* MINUS side gain register */
	calib = 0;
	calib += ADC0->CLMS + ADC0->CLM4 + ADC0->CLM3 +
			 ADC0->CLM2 + ADC0->CLM1 + ADC0->CLM0;
	calib /= 2;
	calib |= 0x8000; // Set MSB
	ADC0->MG |= calib;
	return 0;
}

uint16_t adcRead()
{
	//ADC0->SC1[0] |= ADC_SC1_AIEN_MASK;	// Interrupt enable
	//ADC0->SC1[0] &= ~ADC_SC1_DIFF_MASK;	// Set single-ended conversion mode
	ADC0->SC1[0] = ADC_SC1_ADCH(4);	// Set input channel 0
	//while(ADC0->SC2 & ADC_SC2_ADACT_MASK); // Run until conversion is complete
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));
	return ADC0->R[0];
}

void initPins(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;		// enable clock
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;		// enable clock

	PORTA->PCR[12] |= (PORT_PCR_MUX(1));	// MUX = alt 1 -> GPIO
	PORTB->PCR[19] |= (PORT_PCR_MUX(1));	// MUX = alt 1 -> GPIO
	PORTB->PCR[18] |= (PORT_PCR_MUX(1));	// MUX = alt 1 -> GPIO

	/* Testing ... will be deleted */
	PORTA->PCR[13] |= (PORT_PCR_MUX(1));
	PORTB->PCR[9] |= (PORT_PCR_MUX(1));
	PORTB->PCR[1] |= (PORT_PCR_MUX(1));

	GPIOA->PDDR |= (1 << 13);
	GPIOB->PDDR |= (1 << 9);
	GPIOB->PDDR |= (1 << 1);
	/* end of testing section */

	/* Testing ... will be deleted */
	/* end of testing section */

	GPIOA->PDDR |= (1 << 12); 				// Output pin
	GPIOB->PDDR |= (1 << 19);    			// Output pin
	GPIOB->PDDR |= (1 << 18);    			// Output pin

	GPIOA->PSOR |= (1 << 12);				// Set high PORTA12

	GPIOB->PSOR |= (1 << 19);			// Turn OFF LED
	GPIOB->PSOR |= (1 << 18);			// Turn OFF LED
}

/*********************** Q/learning functions **************************/
void init_Qtable(float Qtable[NUM_STATES][NUM_ACTIONS]){
	for(int i=0; i<NUM_STATES; i++){
		for(int j=0; j<NUM_ACTIONS; j++){
			Qtable[i][j] = 0.0;
		}
	}
}

int32_t selectAction(float Qtable[NUM_STATES][NUM_ACTIONS], int state, int currentEpoch, int currentEpsilon){
	if(currentEpoch > EPOCHS){
		if(randr(1,100) <= EPSILON){
			// Explorace
			return randr(0,(NUM_ACTIONS-1));
		}else{
			// Exploatace: výběr akce s nejvyšší Q-hodnotou
			int32_t bestAction = 0;
			for (int i = 1; i < NUM_ACTIONS; i++) {
				if (Qtable[state][i] > Qtable[state][bestAction]) {
					bestAction = i;
				}
			}
		return bestAction;
		}
	}else{
		if(randr(1,100) <= currentEpsilon){
			// Explorace
			return randr(0,(NUM_ACTIONS-1));
		}else{
			// Exploatace: výběr akce s nejvyšší Q-hodnotou
			int32_t bestAction = 0;
			for (int i = 1; i < NUM_ACTIONS; i++) {
				if (Qtable[state][i] > Qtable[state][bestAction]) {
					bestAction = i;
				}
			}
		return bestAction;
		}
	}

}

void updateQ(float Qtable[NUM_STATES][NUM_ACTIONS], int state, int action, float reward, int nextState) {
    int maxValue = 0;
    for(int i = 1; i < NUM_ACTIONS; i++){
    	if(Qtable[nextState][i] > Qtable[nextState][maxValue]){
    		maxValue = i;
    	}
    }
    Qtable[state][action] = Qtable[state][action] + ALPHA * (reward + GAMMA*Qtable[nextState][maxValue] - Qtable[state][action]);
}

int custom_rand(void) {
    static uint32_t seed = 42;
	seed = seed * 1103515245 + 12345;
    return (seed / 65536) % 32768;
}



