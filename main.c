/*----------------------------------------------------------------------------
 *      Program do manipulatora
 *----------------------------------------------------------------------------
 *    	Prezentacja aplikacji mikrokontrolerów Freescale 
 *    	Kraków 2015
 *----------------------------------------------------------------------------
 *      Bartlomiej Buczek
 *     
 *---------------------------------------------------------------------------*/

#include "MKL25Z4.h"                    // Device header

#define RED_WIRE    0		//definicje wyjsc do silnika
#define BLUE_WIRE   1
#define ORANGE_WIRE 2
#define YELLOW_WIRE 3

const uint32_t led_mask[] = {1UL << 8, 1UL << 2, 1UL << 10, 1UL << 11, 1UL << 0, 1UL << 1,}; //tablica stalych pomocniczych
volatile int temp1, temp2;							//zmienne pozycyjne serw modyfikowane w przerwaniach
const int krok= 1;			
const int min = 15;
const int max=70;

/*----------------------------------------------------------------------------
										Inicjalizacja pinów GPIO
 *----------------------------------------------------------------------------*/
void GPIO_Initialize(void) {
	PORTA->PCR[1] = (1UL <<  PORT_PCR_MUX_SHIFT)|(1UL <<  PORT_PCR_PE_SHIFT)|(1UL <<  PORT_PCR_PS_SHIFT)|(10UL<<PORT_PCR_IRQC_SHIFT);/* switche do serw */
	PORTA->PCR[2] = (1UL <<  PORT_PCR_MUX_SHIFT)|(1UL <<  PORT_PCR_PE_SHIFT)|(1UL <<  PORT_PCR_PS_SHIFT)|(10UL<<PORT_PCR_IRQC_SHIFT);
	PORTD->PCR[0] = (1UL <<  PORT_PCR_MUX_SHIFT)|(1UL <<  PORT_PCR_PE_SHIFT)|(1UL <<  PORT_PCR_PS_SHIFT)|(10UL<<PORT_PCR_IRQC_SHIFT); 
	PORTD->PCR[1] = (1UL <<  PORT_PCR_MUX_SHIFT)|(1UL <<  PORT_PCR_PE_SHIFT)|(1UL <<  PORT_PCR_PS_SHIFT)|(10UL<<PORT_PCR_IRQC_SHIFT);
	

	PORTE->PCR[22] |= (3<<PORT_PCR_MUX_SHIFT);										 /* kanaly PWM*/
	PORTE->PCR[20] |= (3<<PORT_PCR_MUX_SHIFT);										 
	
	
	PORTB->PCR[0] = (1UL <<  PORT_PCR_MUX_SHIFT )|(1UL <<  PORT_PCR_PE_SHIFT )|(1UL <<  PORT_PCR_PS_SHIFT  );        /* switche do silnika */
  PORTB->PCR[1] = (1UL <<  PORT_PCR_MUX_SHIFT )|(1UL <<  PORT_PCR_PE_SHIFT )|(1UL <<  PORT_PCR_PS_SHIFT  );					
	
  PORTB->PCR[8] = (1UL <<  PORT_PCR_MUX_SHIFT );        /* GPIO silnika - wyjscia */
  PORTB->PCR[2] = (1UL <<  PORT_PCR_MUX_SHIFT );				
	PORTB->PCR[10] = (1UL <<  PORT_PCR_MUX_SHIFT );				
	PORTB->PCR[11] = (1UL <<  PORT_PCR_MUX_SHIFT );				
 
  FPTB->PDDR = (led_mask[0] | 				/* Wyjscia silnika*/
               led_mask[1] |
								led_mask[2] |
								led_mask[3] );  
	
	FPTB->PDOR = (led_mask[4] | 				
               led_mask[5] ); /*wysoki stan na wyjsciach  */
	
}


/*--------------------------------------------------/
										Timer2 Initialization
/--------------------------------------------------*/
void Timer2_init(void){    
    SIM->SOPT2 |= (3<< SIM_SOPT2_TPMSRC_SHIFT );   /* use MCGIRCLK as timer counter clock */
    TPM2->SC = 0;               /* disable timer while configuring */

    TPM2->MOD = 0x290;        /* modulo value */
		TPM2->CONTROLS[0].CnSC = 0xE8; /* PWM edge aligned, clear CHF, enable int from channel nad CHF clear */
	
		TPM2->CONTROLS[0].CnV = temp2;
    TPM2->SC |= (1<<TPM_SC_TOF_SHIFT);           /* clear TOF */
    TPM2->SC |= (1<<TPM_SC_CMOD_SHIFT);        /* enable timer free-running mode and interrupt */
}


/*--------------------------------------------------/
										Timer1 Initialization
/--------------------------------------------------*/
void Timer1_init(void){    
    SIM->SOPT2 |= (3<< SIM_SOPT2_TPMSRC_SHIFT );   /* use MCGIRCLK as timer counter clock */
    TPM1->SC = 0;               /* disable timer while configuring */
   
    TPM1->MOD = 0x290;        /* modulo value */
		TPM1->CONTROLS[0].CnSC = 0xE8; /* PWM edge aligned, clear CHF, enable int from channel nad CHF clear */
	
		TPM1->CONTROLS[0].CnV = temp1;
    TPM1->SC |= (1<<TPM_SC_TOF_SHIFT);           /* clear TOF */
    TPM1->SC |= (1<<TPM_SC_CMOD_SHIFT);           /* enable timer free-running mode and interrupt */
}




/*--------------------------------------------------/
										Delay Function
/--------------------------------------------------*/
void delayMs(int n){
    int32_t i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 700; j++)
            {}  /* do nothing for 1 ms */
}


/*----------------------------------------------------------------------------
  Funkcja w lewo (silnik krokowy)
*----------------------------------------------------------------------------*/
void lewo(int opoznienie) {
	
	FPTB->PCOR   = led_mask[RED_WIRE];
	FPTB->PCOR   = led_mask[YELLOW_WIRE];
  FPTB->PSOR   = led_mask[BLUE_WIRE];
	FPTB->PSOR   = led_mask[ORANGE_WIRE];
	delayMs(opoznienie);
	FPTB->PCOR   = led_mask[RED_WIRE];
	FPTB->PSOR   = led_mask[YELLOW_WIRE];
  FPTB->PSOR   = led_mask[BLUE_WIRE];
	FPTB->PCOR   = led_mask[ORANGE_WIRE];
	delayMs(opoznienie);
	FPTB->PSOR   = led_mask[RED_WIRE];
	FPTB->PSOR   = led_mask[YELLOW_WIRE];
  FPTB->PCOR   = led_mask[BLUE_WIRE];
	FPTB->PCOR   = led_mask[ORANGE_WIRE];
	delayMs(opoznienie);
	FPTB->PSOR   = led_mask[RED_WIRE];   /* RED LED Off*/
	FPTB->PCOR   = led_mask[YELLOW_WIRE];   /* RED LED Off*/
  FPTB->PCOR   = led_mask[BLUE_WIRE];    /* blue LED On*/
	FPTB->PSOR   = led_mask[ORANGE_WIRE];    /* blue LED On*/
	delayMs(opoznienie);
	
}

/*----------------------------------------------------------------------------
  Funkcja w prawo(silnik krokowy)
 *----------------------------------------------------------------------------*/
void prawo(int opoznienie) {
	FPTB->PSOR   = led_mask[RED_WIRE];   
	FPTB->PSOR   = led_mask[YELLOW_WIRE];   
  FPTB->PCOR   = led_mask[BLUE_WIRE];    
	FPTB->PCOR   = led_mask[ORANGE_WIRE];    
	delayMs(opoznienie);
	FPTB->PCOR   = led_mask[RED_WIRE];   
	FPTB->PSOR   = led_mask[YELLOW_WIRE];   
  FPTB->PSOR   = led_mask[BLUE_WIRE];   
	FPTB->PCOR   = led_mask[ORANGE_WIRE];   
	delayMs(opoznienie);
	FPTB->PCOR   = led_mask[RED_WIRE];   
	FPTB->PCOR   = led_mask[YELLOW_WIRE];  
  FPTB->PSOR   = led_mask[BLUE_WIRE];   
	FPTB->PSOR   = led_mask[ORANGE_WIRE];    
	delayMs(opoznienie);
	FPTB->PSOR   = led_mask[RED_WIRE];   
	FPTB->PCOR   = led_mask[YELLOW_WIRE];   
  FPTB->PCOR   = led_mask[BLUE_WIRE];   
	FPTB->PSOR   = led_mask[ORANGE_WIRE];    
	delayMs(opoznienie);
}

/*----------------------------------------------------------------------------
  Inicjalizacja Zegarów
 *----------------------------------------------------------------------------*/
void Clock_Initialize(void){
SIM->SCGC6 |= (1UL<<SIM_SCGC6_TPM1_SHIFT)|(1UL<<SIM_SCGC6_TPM2_SHIFT);   /* enable clock to TPM2 and TPM1 */
SIM->SCGC5 |= (1UL <<SIM_SCGC5_PORTB_SHIFT)|(1UL <<SIM_SCGC5_PORTA_SHIFT)|(1UL <<SIM_SCGC5_PORTE_SHIFT)|(1UL <<SIM_SCGC5_PORTD_SHIFT); /* Enable Clock to PORTB and PORTA */
}
int main (void)
{
	
	int start_position = 42; //zmienna pomocniczne do sterowania serwami	
	__disable_irq();	
	temp1=temp2=start_position;
	Clock_Initialize();
	GPIO_Initialize();
	Timer1_init();
  Timer2_init();

  __enable_irq();
	FPTB->PDOR =0;	
	NVIC->ICER[0]|=(1UL<<(30%32))|(1UL<<(31%32));
	NVIC->ISER[0]|=(1UL<<(30%32))|(1UL<<(31%32));
	
	
	
	
  while(1){
		
		if( !(FPTB->PDIR&(1<<0))){
			prawo(700);
			}
		if(!(FPTB->PDIR&(1<<1))){
			lewo(700);
		}
	}
}






void PORTA_IRQHandler(){

	if(!(FPTA->PDIR&(1<<1))){
	  if(TPM1->CONTROLS[0].CnV<=max){
			TPM1->CONTROLS[0].CnV=TPM1->CONTROLS[0].CnV+krok;
			
			TPM1->SC |= 0x80;
	
		}
	}else if(!(FPTA->PDIR&(1<<2))){
			
		if(TPM1->CONTROLS[0].CnV>min){
			TPM1->CONTROLS[0].CnV=TPM1->CONTROLS[0].CnV-krok;
		
			TPM1->SC |= 0x80;
		}
	
	 
	}
		PORTA->ISFR=0xFFFFFFFF;



}

void PORTD_IRQHandler(){
	
	if(!(FPTD->PDIR&(1<<0))){
			  if(TPM2->CONTROLS[0].CnV<=max){
			TPM2->CONTROLS[0].CnV=TPM2->CONTROLS[0].CnV+krok;
			
					TPM2->SC |= 0x80;

		}
	}else if(!(FPTD->PDIR&(1<<1))){
		  if(TPM2->CONTROLS[0].CnV>=min){
			TPM2->CONTROLS[0].CnV=TPM2->CONTROLS[0].CnV-krok;
		
				TPM2->SC |= 0x80;
		}
	}
	PORTD->ISFR=0xFFFFFFFF;
}
