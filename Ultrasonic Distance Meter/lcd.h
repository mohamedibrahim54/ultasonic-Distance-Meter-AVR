
#ifndef LCD_Control_Port
#error "LCD_Control_Port must defined ex: #define LCD_Control_Port PORTD"
#endif

#ifndef LCD_Data_Port
#error "LCD_Data_Port must defined ex: #define LCD_Data_Port PORTD"
#endif

#ifndef RS
#error "RS must defined ex: #define RS PORTD0"
#endif

#ifndef EN
#error "EN must defined ex: #define EN PORTD1"
#endif

#if !defined(D4) || !defined(D5) || !defined(D6) || !defined(D7)
#error "D4..D7 must defined ex: #define D5 PORTD3"
#endif

#include<util/delay.h>

//LCD 4 Bit Interfacing Functions

void set_LCD_data_pin(char pin){
	LCD_Data_Port |= 1 << pin; 
}

void clear_LCD_data_pin(char pin){
	LCD_Data_Port &= ~(1 << pin); 
}

void set_LCD_control_pin(char pin){
	LCD_Control_Port |= 1 << pin;
}

void clear_LCD_control_pin(char pin){
	LCD_Control_Port &= ~(1 << pin);
}

void Lcd4_Port(char a)
{
	if(a & 1)
	set_LCD_data_pin(D4);
	else
	clear_LCD_data_pin(D4);
	
	if(a & 2)
	set_LCD_data_pin(D5);
	else
	clear_LCD_data_pin(D5);
	
	if(a & 4)
	set_LCD_data_pin(D6);
	else
	clear_LCD_data_pin(D6);
	
	if(a & 8)
	set_LCD_data_pin(D7);
	else
	clear_LCD_data_pin(D7);
}
void Lcd4_Cmd(char a)
{
	clear_LCD_control_pin(RS);             // => RS = 0
	Lcd4_Port(a);
	set_LCD_control_pin(EN);            // => E = 1
	_delay_ms(1);
	clear_LCD_control_pin(EN);             // => E = 0
	_delay_ms(1);
}

void Lcd4_Clear()
{
	Lcd4_Cmd(0);
	Lcd4_Cmd(1);
}

void Lcd4_Set_Cursor(char a, char b)
{
	char temp,z,y;
	if(a == 1)
	{
		temp = 0x80 + b;
		z = temp>>4;
		y = (0x80+b) & 0x0F;
		Lcd4_Cmd(z);
		Lcd4_Cmd(y);
	}
	else if(a == 2)
	{
		temp = 0xC0 + b;
		z = temp>>4;
		y = (0xC0+b) & 0x0F;
		Lcd4_Cmd(z);
		Lcd4_Cmd(y);
	}
}

void Lcd4_Init()
{
	Lcd4_Port(0x00);
	_delay_ms(20);
	///////////// Reset process from data sheet /////////
	Lcd4_Cmd(0x03);
	_delay_ms(5);
	Lcd4_Cmd(0x03);
	_delay_ms(11);
	Lcd4_Cmd(0x03);
	/////////////////////////////////////////////////////
	Lcd4_Cmd(0x02);
	Lcd4_Cmd(0x02);
	Lcd4_Cmd(0x08);
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x0C);
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x06);
}

void Lcd4_Write_Char(char a)
{
	char temp,y;
	temp = a&0x0F;
	y = a&0xF0;
	set_LCD_control_pin(RS);             // => RS = 1
	Lcd4_Port(y>>4);             //Data transfer
	set_LCD_control_pin(EN);
	_delay_ms(1);
	clear_LCD_control_pin(EN);
	_delay_ms(1);
	Lcd4_Port(temp);
	set_LCD_control_pin(EN);
	_delay_ms(1);
	clear_LCD_control_pin(EN);
	_delay_ms(1);
}

void Lcd4_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	Lcd4_Write_Char(a[i]);
}

void Lcd4_Shift_Right()
{
	Lcd4_Cmd(0x01);
	Lcd4_Cmd(0x0C);
}

void Lcd4_Shift_Left()
{
	Lcd4_Cmd(0x01);
	Lcd4_Cmd(0x08);
}
//End LCD 4 Bit Interfacing Functions
