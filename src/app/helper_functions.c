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

int setVLPS(void)
{
	while(1){
		SMC->PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
		SMC->PMCTRL |= SMC_PMCTRL_STOPM(0b10);
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
		__asm("WFI");
		if(lptmrIntFlag) break;
		 __ISB();
	}

  if (SMC->PMCTRL & SMC_PMCTRL_STOPA_MASK)
	{
	  	okFlag = 0;
		return 0; // return 0 !!!!!
	}
	else
	{
		okFlag = 1;
		return 1;
	}
}

void __attribute__ ((interrupt)) LPTMR0_IRQHandler(void){
	LPTMR0->CSR = LPTMR_CSR_TCF_MASK;	//Clear the LPTMR flag (w1c)
	lptmrIntFlag = 1;
	LPTMR0->CSR &= ~LPTMR_CSR_TEN_MASK;

}

void initLPTMR(void)
{
	SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK; // Clock enable
	LPTMR0->CSR &= ~LPTMR_CSR_TMS_MASK; // Set Time counter mode
	LPTMR0->PSR |= LPTMR_PSR_PRESCALE(6) | LPTMR_PSR_PCS(1); /* Set prescaler and glitch filter ->
																Chip Clock = LPO — 1 kHz clock */
	NVIC_SetVector(LPTMR0_IRQn, (uint32_t)&LPTMR0_IRQHandler);
	NVIC_ClearPendingIRQ(LPTMR0_IRQn);
	NVIC_SetPriority(LPTMR0_IRQn, 2);
	NVIC_EnableIRQ(LPTMR0_IRQn);
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
	ADC0->SC2 |= ADC_SC2_REFSEL(0);		// Select voltage reference 0= default voltage reference pair

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

void deInitADC(void)
{
	ADC0->SC1[0] = ADC_SC1_ADCH(0x1F); // Module disabled
}

float getEnergy(void)
{
	ADC0->SC1[0] = ADC_SC1_ADCH(4);					// measurement
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));		// wait for end of measurement

	uint16_t adcVal = ADC0->R[0];
	float measuredVoltage = VOLTAGE_MCU*((float)adcVal/65535); // convert ADC value to voltage

	measuredVoltage = measuredVoltage/0.375; 		// convert voltage range

	float energyCap = 0.5*measuredVoltage*measuredVoltage; // calculate energy
	float SE;

	if(energyCap > E_MIN){
		SE = (energyCap - E_MIN)/(E_MAX - E_MIN);	// calculate SE
	}else{
		SE = 0;
	}

	return SE;
}

void initPins(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;		// enable clock
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;		// enable clock

	PORTA->PCR[12] |= (PORT_PCR_MUX(1));	// MUX = alt 1 -> GPIO
	PORTA->PCR[13] |= (PORT_PCR_MUX(1));	// MUX = alt 1 -> GPIO
	PORTB->PCR[19] |= (PORT_PCR_MUX(1));	// MUX = alt 1 -> GPIO
	PORTB->PCR[18] |= (PORT_PCR_MUX(1));	// MUX = alt 1 -> GPIO

	//GPIOA->PDDR |= (1 << 13);
	GPIOB->PDDR |= (1 << 9);
	GPIOB->PDDR |= (1 << 1);

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
	int eps = 0;
	if(currentEpoch > EPOCHS){
		eps = EPSILON;
	}else{
		eps = currentEpsilon;
	}

	if(randr(1,100) <= eps){
		// Explorace
		return randr(0,(NUM_ACTIONS-1));
	}else{
		// Exploatace: selecting the action with the highest Q-value
		int32_t bestAction = 0;
		for (int i = 1; i < NUM_ACTIONS; i++) {
			if (Qtable[state][i] > Qtable[state][bestAction]) {
				bestAction = i;
			}
		}
	return bestAction;
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

float getTemp(void)
{
	uint32_t adcValue = adc_read(ADC_MODE_16BIT_SE, 26); // 16- single-ended measurement of temperature

	float voltageValue = adcValue*3300/65536; // in miliVolts
	float m = 0.0;
	//NewValue = (((OldValue - OldMin) * NewRange) / OldRange) + NewMin
	voltageValue = (((voltageValue)*3000)/3300);
	if (voltageValue >= VTEMP25) {
		m = 1.646; // Cold Slope
	}else {
		m = 1.769; // Hot Slope
	}

	float temp = 25 - (((voltageValue/1000) - (VTEMP25/1000))/m);

	return temp;
}

void dataPrep(uint8_t* buffer, float value)
{
	uint32_t uTempValue = (uint32_t)(value * 10);

	buffer[0] = (uint8_t)uTempValue;
	buffer[1] = (uint8_t)(uTempValue >> 8);
	buffer[2] = (uint8_t)(uTempValue >> 16);
	buffer[3] = (uint8_t)(uTempValue >> 24);
}

void initButton(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; // PORTD clock enable
	PORTD->PCR[7] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK;	// MUX = alt 1 -> GPIO ||| mozna pull rezistor???

	NVIC_SetVector(PORTD_IRQn, (uint32_t)&PORTD_IRQHandler);
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	NVIC_SetPriority(PORTD_IRQn, 2);
	NVIC_EnableIRQ(PORTD_IRQn);

	PORTD->PCR[7] |= PORT_PCR_IRQC(0b1010); // Interrupt on falling edge

}

void __attribute__ ((interrupt)) PORTD_IRQHandler(void){
	PORTD->PCR[7] |= PORT_PCR_ISF_MASK;	//Clear the PTD7 flag (w1c)
	//LED_ON(LED_D1);
	//btnIntFlag = 1;
}

void radio_ON(void){
	GPIOA->PSOR |= 1 << 12;
}

void radio_OFF(void){
	GPIOA->PCOR |= 1 << 12;
}
