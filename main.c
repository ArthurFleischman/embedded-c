#define F_CPU 16000000UL
// #define BAUD 9600
// #define MYUBRR F_CPU/16/BAUD-1
#include<stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "serial.h"

double dutyCycle = 0;
void setupADC();
void startConversation();

int main(void)
{   
    // USART_Init(MYUBRR);
    
    DDRD = (1 << PORTD6);  //set port as output
    
    TCCR0A = (1 << COM0A1) | (1 << WGM00) | (1 << WGM01);  //turn pwm on at 8 bit registerTCCRA01, setting coparisson at top, reset at bottom, overflow at max
    TIMSK0 = (1 << TOIE0);
    setupADC();
    sei();
     
    TCCR0B = (1 << CS00)|(1<< CS02);
    
 
    while(1)
    {  
        
    }
}

void setupADC(){
    ADMUX = (1 << REFS0) ;
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1<< ADPS0)| (1<< ADPS1) | (1 << ADPS2);
    DIDR0 = (1 << ADC0D);
    startConversation();
}

void startConversation(){
    ADCSRA |= (1<< ADSC); 
}
ISR(TIMER0_OVF_vect){
    OCR0A = dutyCycle;
}


ISR(ADC_vect){
    dutyCycle = (ADC*0.25);
    startConversation();
}
int ADCsingleREAD(uint8_t adctouse)
{
    int ADCval;

    ADMUX = adctouse;         // use #1 ADC
    ADMUX |= (1 << REFS0);    // use AVcc as the reference
    ADMUX &= ~(1 << ADLAR);   // clear for 10 bit resolution
    
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescale for 8Mhz
    ADCSRA |= (1 << ADEN);    // Enable the ADC

    ADCSRA |= (1 << ADSC);    // Start the ADC conversion

    while(ADCSRA & (1 << ADSC));      // Thanks T, this line waits for the ADC to finish 


    ADCval = ADCL;
    ADCval = (ADCH << 8) + ADCval;    // ADCH is read so ADC can be updated again

    return ADCval;
}