#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_

char lcd_display_symbols[2][16];

void lcd_interface_init();

void display_text(char line1[], char line2[]);
void display_numbers(uint16_t line1, uint16_t line2);


uint16_t symbol_request(uint8_t id, uint16_t data);

#endif