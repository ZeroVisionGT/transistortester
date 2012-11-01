// Interfacing a HD44780 compatible LCD with  4-Bit-Interface mode
// http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial
//
// Pin selection can be changed in lcd-routines.h with define
 
#include <avr/io.h>
#include "config.h"
#include "lcd-routines.h"
#include "wait1000ms.h"
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#ifdef STRIP_GRID_BOARD
 #warning "strip-grid-board layout selectet!"
#endif


 

// sends numeric character (Pin Number) to the LCD 
// from binary 0 we send ASCII 1 ....
void lcd_testpin(unsigned char temp) {
   lcd_data(temp + '1');
}

// send space character to LCD
void lcd_space(void) {
   lcd_data(' ');
}

// sends data byte to the LCD 
void lcd_data(unsigned char temp1) {
 lcd_write_data(temp1);		// set RS to 1
#ifdef WITH_UART
 switch(temp1) {
    case LCD_CHAR_DIODE1:
    	uart_putc('>');
    	uart_putc('|');
    	break;
    case LCD_CHAR_DIODE2:
    	uart_putc('|');
    	uart_putc('<');
    	break;
    case LCD_CHAR_CAP:
    	uart_putc('|');
    	uart_putc('|');
    	break;
    case LCD_CHAR_RESIS1:
    case LCD_CHAR_RESIS2:
    	uart_putc('R');
    	break;
    case 225:	//�
    	uart_putc('�');
    	break;
    case 228:	//�
    	uart_putc('�');
    	break;
    case 244:	//Omega
    	break;
    default:
    	uart_putc(temp1);
 }
#endif
}
 
// sends a command to the LCD
 
void lcd_command(unsigned char temp1) {
        lcd_write_cmd(temp1);		// set RS to 0
#ifdef WITH_UART
	if((temp1 == 0x80) || (temp1 == 0xC0)) {
		uart_newline();
	}
#endif
}
 
// Initialise: 
// Must be called first .
 
void lcd_init(void) {
   _lcd_hw_init();
   wait30ms();
   // to initialise, send 3 times
   lcd_write_init(1);
   wait5ms();
   
   lcd_write_init(1);
   wait1ms();

   lcd_write_init(1);
   wait1ms();

   lcd_write_init(0);
   wait10ms();

   // 4Bit / 2 rows / 5x7
   lcd_command(CMD_SetIFOptions | 0x08);

   // Display on / Cursor off / no Blinking
   lcd_command(CMD_SetDisplayAndCursor | 0x04);

   // increment / no Scroll    
   lcd_command(CMD_SetEntryMode | 0x02);	
   lcd_clear();
}
 
// send the command to clear the display 
 
void lcd_clear(void) {
   lcd_command(CLEAR_DISPLAY);
   wait10ms();
#ifdef WITH_UART
   uart_newline();
#endif
}


#ifdef WITH_UART
void uart_newline(void) {
   uart_putc('\r');
   uart_putc('\n');
}
#endif
 
 
// writes a string to the LCD 
 
void lcd_string(char *data) {
    while(*data) {
        lcd_data(*data);
        data++;
    }
}

#ifdef use_lcd_pgm
//Load string from PGM  and send to LCD 
void lcd_pgm_string(const unsigned char *data) {
   unsigned char cc;
   while(1) {
      cc = pgm_read_byte(data);
      if((cc==0) || (cc==128)) {
         return;
      }
      lcd_data(cc);
      data++;
   }
}
#endif

//Load string from PGM or EEprom and send to LCD 
void lcd_fix_string(const unsigned char *data) {
   unsigned char cc;
   while(1) {
      cc = MEM_read_byte(data);
      if((cc==0) || (cc==128)) {
         return;
      }
      lcd_data(cc);
      data++;
   }
}

// load custom character from PGM or EEprom and send to LCD
void lcd_fix_customchar(const unsigned char *chardata) {	
    for(uint8_t i=0;i<8;i++) {
        lcd_data(MEM_read_byte(chardata));
        chardata++;
    }
}