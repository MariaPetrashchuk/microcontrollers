
#include <xc.h>

// ========================================
// Конфігурація мікроконтролера PIC16F877A
// ========================================

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

// Тактова частота — 4 МГц
#define _XTAL_FREQ 4000000UL

// ========================================
// Підключення рідкокристалічного індикатора
// ========================================

// Керуючі контакти LCD
#define LCD_RS PORTBbits.RB0
#define LCD_RW PORTBbits.RB1
#define LCD_E  PORTBbits.RB2

// Шина даних LCD: DB0–DB7 -> RD0–RD7

// ========================================
// Налаштування портів мікроконтролера
// ========================================

void ports_init(void)
{
    // Цифровий режим входів
    ADCON1 = 0x06;

    // RB0, RB1, RB2 — виходи
    TRISB = 0xF8;

    // Усі контакти PORTD — виходи
    TRISD = 0x00;

    // Початкові значення
    PORTB = 0x00;
    PORTD = 0x00;
}

// ========================================
// Передача байта до LCD
// ========================================

void lcd_write(unsigned char value, unsigned char rs)
{
    LCD_RS = rs;
    LCD_RW = 0;
    LCD_E = 0;

    PORTD = value;

    __delay_us(2);

    LCD_E = 1;
    __delay_us(2);
    LCD_E = 0;

    // Час на виконання команди або запису даних
    __delay_ms(2);
}

// ========================================
// Передача команди до LCD
// ========================================

void lcd_command(unsigned char command)
{
    lcd_write(command, 0);
}

// ========================================
// Передача символу до LCD
// ========================================

void lcd_data(unsigned char data)
{
    lcd_write(data, 1);
}

// ========================================
// Ініціалізація LCD
// ========================================

void lcd_init(void)
{
    // Очікування після подачі живлення
    __delay_ms(40);

    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E = 0;

    // Початкова послідовність для 8-бітного режиму
    lcd_command(0x30);
    __delay_ms(5);

    lcd_command(0x30);
    __delay_us(150);

    lcd_command(0x30);

    // 8-бітний інтерфейс, 2 рядки, матриця 5x8
    lcd_command(0x38);

    // Вимкнення дисплея
    lcd_command(0x08);

    // Очищення дисплея
    lcd_command(0x01);

    // Автоматичне переміщення курсора вправо
    lcd_command(0x06);

    // Увімкнення дисплея без курсора
    lcd_command(0x0C);
}

// ========================================
// Виведення текстового рядка
// ========================================

void lcd_print(const char *text)
{
    while (*text != '\0')
    {
        lcd_data((unsigned char)*text);
        text++;
    }
}

// ========================================
// Встановлення позиції курсора
// ========================================

void lcd_set_cursor(unsigned char row,
                    unsigned char column)
{
    unsigned char address;

    if (row == 1)
    {
        address = 0x80 + column;
    }
    else
    {
        address = 0xC0 + column;
    }

    lcd_command(address);
}

// ========================================
// Головна функція програми
// ========================================

void main(void)
{
    // Налаштування портів
    ports_init();

    // Ініціалізація індикатора
    lcd_init();

    // Перший рядок — ім'я
    lcd_set_cursor(1, 0);
    lcd_print("MARIIA");

    // Другий рядок — прізвище
    lcd_set_cursor(2, 0);
    lcd_print("PETRASHCHUK");

    // Нескінченний цикл
    while (1)
    {
        // Текст залишається на дисплеї
    }
}
