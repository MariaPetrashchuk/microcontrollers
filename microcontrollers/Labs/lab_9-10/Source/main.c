
#include <xc.h>
#include <stdint.h>

// =============================================
// LAB 9-10: ADC, VOLTMETER, VOLTAGE RELAY
// PIC16F877A / XC8 / 4 MHz
// =============================================

// Select laboratory part: 1, 2 or 3
#define LAB_PART 1

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Supply and ADC reference: 5 V
#define VREF_MV 5000UL

// Part 3: voltage threshold = 3.00 V
#define THRESHOLD_MV 3000UL

// LCD control (parts 2 and 3)
#define LCD_RS PORTBbits.RB0
#define LCD_RW PORTBbits.RB1
#define LCD_E  PORTBbits.RB2

// Relay indicators (part 3)
#define LED_GREEN PORTBbits.RB3
#define LED_RED   PORTBbits.RB4

// =============================================
// ADC initialization
// =============================================

void adc_init(void)
{
    // RA0 is input
    TRISAbits.TRISA0 = 1;

    // AN0 analog, other channels digital
    // Right-justified ADC result
    // VREF+ = VDD, VREF- = VSS
    ADCON1 = 0x8E;

    // ADC clock Fosc/32
    // Channel AN0
    // ADC enabled
    ADCON0 = 0x81;

    __delay_ms(2);
}

// =============================================
// Read 10-bit ADC result
// =============================================

uint16_t adc_read(void)
{
    uint16_t result;

    // Acquisition time
    __delay_us(40);

    // Start conversion
    ADCON0bits.GO_nDONE = 1;

    // Wait until conversion finishes
    while (ADCON0bits.GO_nDONE != 0)
    {
    }

    result = ((uint16_t)ADRESH << 8) | ADRESL;

    return result;
}

// =============================================
// Convert ADC value to millivolts
// =============================================

uint16_t adc_to_mv(uint16_t adc_value)
{
    uint32_t mv;

    mv = ((uint32_t)adc_value * VREF_MV + 511UL)
         / 1023UL;

    return (uint16_t)mv;
}

// =============================================
// LCD functions
// LCD D0-D7 -> RD0-RD7
// =============================================

void lcd_write(uint8_t value, uint8_t rs)
{
    LCD_RS = rs;
    LCD_RW = 0;
    LCD_E = 0;

    PORTD = value;

    __delay_us(2);

    LCD_E = 1;
    __delay_us(2);
    LCD_E = 0;

    __delay_ms(2);
}

void lcd_command(uint8_t command)
{
    lcd_write(command, 0);
}

void lcd_data(uint8_t value)
{
    lcd_write(value, 1);
}

void lcd_init(void)
{
    TRISB = 0x00;
    PORTB = 0x00;

    TRISD = 0x00;
    PORTD = 0x00;

    __delay_ms(40);

    lcd_command(0x30);
    __delay_ms(5);

    lcd_command(0x30);
    __delay_us(150);

    lcd_command(0x30);
    lcd_command(0x38);

    lcd_command(0x08);
    lcd_command(0x01);
    lcd_command(0x06);
    lcd_command(0x0C);
}

void lcd_set_cursor(uint8_t row, uint8_t col)
{
    if (row == 1)
    {
        lcd_command(0x80 + col);
    }
    else
    {
        lcd_command(0xC0 + col);
    }
}

void lcd_print(const char *text)
{
    while (*text != '\0')
    {
        lcd_data((uint8_t)*text);
        text++;
    }
}

// Show voltage in format 0.000 V ... 5.000 V
void lcd_print_voltage(uint16_t mv)
{
    uint8_t volts;
    uint16_t fraction;

    volts = (uint8_t)(mv / 1000U);
    fraction = mv % 1000U;

    lcd_data((uint8_t)('0' + volts));
    lcd_data('.');

    lcd_data((uint8_t)('0' + fraction / 100U));
    lcd_data((uint8_t)('0' + (fraction / 10U) % 10U));
    lcd_data((uint8_t)('0' + fraction % 10U));

    lcd_print(" V");
}

// =============================================
// PART 1: ADC + 10 LEDs
// =============================================

void part1_init(void)
{
    // RB0-RB7 = ADC bits 0..7
    TRISB = 0x00;
    PORTB = 0x00;

    // RC0-RC1 = ADC bits 8..9
    TRISC = 0xFC;
    PORTC = 0x00;
}

void part1_run(void)
{
    uint16_t adc_value;

    while (1)
    {
        adc_value = adc_read();

        // Lower 8 bits
        PORTB = (uint8_t)(adc_value & 0xFFU);

        // Upper 2 bits
        PORTC = (uint8_t)((adc_value >> 8) & 0x03U);

        __delay_ms(100);
    }
}

// =============================================
// PART 2: DIGITAL VOLTMETER
// =============================================

void part2_run(void)
{
    uint16_t adc_value;
    uint16_t voltage_mv;

    lcd_init();

    lcd_set_cursor(1, 0);
    lcd_print("DIGITAL VOLTMETER");

    while (1)
    {
        adc_value = adc_read();
        voltage_mv = adc_to_mv(adc_value);

        lcd_set_cursor(2, 0);
        lcd_print("U = ");
        lcd_print_voltage(voltage_mv);
        lcd_print("   ");

        __delay_ms(200);
    }
}

// =============================================
// PART 3: VOLTAGE RELAY SIMULATION
// =============================================

void part3_run(void)
{
    uint16_t adc_value;
    uint16_t voltage_mv;

    lcd_init();

    LED_GREEN = 0;
    LED_RED = 0;

    while (1)
    {
        adc_value = adc_read();
        voltage_mv = adc_to_mv(adc_value);

        lcd_set_cursor(1, 0);
        lcd_print("U=");
        lcd_print_voltage(voltage_mv);
        lcd_print("       ");

        lcd_set_cursor(2, 0);

        if (voltage_mv <= THRESHOLD_MV)
        {
            // Voltage is within selected limit
            LED_GREEN = 1;
            LED_RED = 0;

            lcd_print("RELAY ON        ");
        }
        else
        {
            // Voltage exceeds the limit
            LED_GREEN = 0;
            LED_RED = 1;

            lcd_print("RELAY OFF       ");
        }

        __delay_ms(200);
    }
}

// =============================================
// MAIN
// =============================================

void main(void)
{
    adc_init();

#if LAB_PART == 1

    part1_init();
    part1_run();

#elif LAB_PART == 2

    part2_run();

#elif LAB_PART == 3

    part3_run();

#else

#error "LAB_PART must be 1, 2 or 3"

#endif
}
