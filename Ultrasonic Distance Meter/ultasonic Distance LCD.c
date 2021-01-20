/*
 * ultasonic_Distance_LCD.c
 * MCU	  : at90s2313
 * F-CPU  : 4 Mhz
 * Created: 20/03/2019 06:59:39 م
 *  Author: Mohamed Ibrahim
 */ 


#define F_CPU 4000000ul
#define F_CPU_Mhz 4
#define ICP 6				//input capture pin is pin 6 at PORTD for at90s2313.
#define TRIGGER_PIN 5

#define LCD_Control_Port PORTD
#define LCD_Data_Port PORTB
#define RS PD3
#define EN PD4
#define D4 PB0
#define D5 PB1
#define D6 PB2
#define D7 PB3

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/wdt.h> 
#include <stdlib.h>
#include "lcd.h"


unsigned long capture_register = 0;
int overflow_counter = 0;
volatile char start_capture = 0;
unsigned long first_capture = 0 , last_capture = 0; 
double capture_us = 0, distance = 0;
char distance_string [5];

ISR(TIMER1_CAPT1_vect){
	capture_register = ICR1;
	if (start_capture == 0)
	{
		first_capture = capture_register;
		TCCR1B &= ~(1 << ICES1);			// clear =  Timer/Counter1 contents are transferred to the (ICR1) on the  falling edge.
		
		start_capture = 1;
	} else{
		last_capture = capture_register;
		TCCR1B |= (1 << ICES1);			// set =  Timer/Counter1 contents are transferred to the (ICR1) on the  rising edge.
		start_capture = 0;
		calc_capture_us();
		display_distance();
		overflow_counter = 0;
	}
}

ISR(TIMER1_OVF1_vect){
	if(start_capture == 1){
		overflow_counter += 1;
	}
}

void calc_capture_us(){
	last_capture += overflow_counter *  65536 ;
	unsigned long delta = last_capture - first_capture;
	capture_us = delta / F_CPU_Mhz;		// Input Capture1 Noise Canceler (4 XTAL clock frequency) for both low and high state give us the right pulse duratoin.
}

void display_distance(){
	cli();
	distance = capture_us * 0.017;		//time in micro seconds * [speed of sound in (cm / microsecond) / 2 = 0.017 cm/us]  
	Lcd4_Clear();
	Lcd4_Write_String("Distance: ");
	Lcd4_Set_Cursor(2, 2);
	ltoa(distance, distance_string, 10);
	Lcd4_Write_String(distance_string);
	Lcd4_Write_String(" cm");
	sei();
}

int main(void)
{
	cli();
	//disable watchdog if enabled
   	wdt_reset();
	WDTCR |= 1 << WDTOE | 1 << WDE ;
	WDTCR  &= ~(1 << WDE);

	// PD6 is Input Capture Pin in 2313 uC.
	// PD 5 is Trigger Pin.
	// PD3, PD4 is used as RS, EN for LCD Control.
	DDRD = (1 << PD5) | (1 << PD4) | (1 << PD3);
	
	//PB[0..3] is used as data lines for LCD.
	DDRB = 1 << PB3 | 1 << PB2 | 1 << PB1 | 1 << PB0 ;
	
	// Timer/Counter1 Control Register B (TCCR1B) :
	//  ICNC1: Input Capture1 Noise Canceler (4 CKs) = enable.
	//  ICES1: Input Capture1 Edge Select : set (one) =  Timer/Counter1 contents are transferred to
	//													 the Input Capture Register (ICR1) on the rising edge.
	//  CS12, CS11, CS10: Clock Select1, Bits 2, 1 and 0 = XTAL clock frequency.
	TCCR1B = 1 << ICNC1 | 1 << ICES1 | 0 << CS12 | 0 << CS11 | 1<< CS10 ; 
	
	// Timer/Counter Interrupt Mask Register (TIMSK) :
	//  TOIE1: Timer/Counter1 Overflow Interrupt Enable = true
	//  TICIE1: Timer/Counter1 Input Capture Interrupt Enable = true 
	TIMSK = 1 << TOIE1 | 1 << 3 ;      // ICIE1 for at tiny 2313 ,TICIE1 for at90s2313 (both at bit 3);
	
	Lcd4_Init();		//this function do LCD initialization and set LCD to work on in 4 mode
	Lcd4_Clear();
	Lcd4_Write_String("Distance: ");
	_delay_ms(1000);
	Lcd4_Write_String("1");
	_delay_ms(1000);
	Lcd4_Write_String("2");
	_delay_ms(1000);
	Lcd4_Write_String("3");
	_delay_ms(500);
	sei();	
    while(1)
    {
		//In order to generate the ultrasound you need to set the Trig on a High State for 10 µs.
		// That will send out an 8 cycle sonic burst which will travel at the speed sound and it will be received in the Echo pin. 
		// The Echo pin will output the time in microseconds the sound wave traveled.
		
		PORTD |= 1 << TRIGGER_PIN;
		_delay_us(10);
		PORTD &= ~(1 << TRIGGER_PIN);
		_delay_ms(500); 
    }
}