
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

#define LCD_WIDTH  132
#define LCD_HEIGHT 132

#define LCD_CS    PORTCbits.RC2
#define LCD_RST   PORTCbits.RC6
#define LCD_CLK   PORTCbits.RC3
#define LCD_DATA  PORTCbits.RC5

#define BLACK  0x000
#define WHITE  0xFFF
#define RED    0xF00
#define GREEN  0x0F0
#define BLUE   0x00F
#define YELLOW 0xFF0
#define CYAN   0x0FF

// 5x7 font stored in 8x8 character cells.
// Supported: space, digits, uppercase A-Z.
const uint8_t font5x7[37][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // space
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x7E,0x11,0x11,0x11,0x7E}, // A
    {0x7F,0x49,0x49,0x49,0x36}, // B
    {0x3E,0x41,0x41,0x41,0x22}, // C
    {0x7F,0x41,0x41,0x22,0x1C}, // D
    {0x7F,0x49,0x49,0x49,0x41}, // E
    {0x7F,0x09,0x09,0x09,0x01}, // F
    {0x3E,0x41,0x49,0x49,0x7A}, // G
    {0x7F,0x08,0x08,0x08,0x7F}, // H
    {0x00,0x41,0x7F,0x41,0x00}, // I
    {0x20,0x40,0x41,0x3F,0x01}, // J
    {0x7F,0x08,0x14,0x22,0x41}, // K
    {0x7F,0x40,0x40,0x40,0x40}, // L
    {0x7F,0x02,0x0C,0x02,0x7F}, // M
    {0x7F,0x04,0x08,0x10,0x7F}, // N
    {0x3E,0x41,0x41,0x41,0x3E}, // O
    {0x7F,0x09,0x09,0x09,0x06}, // P
    {0x3E,0x41,0x51,0x21,0x5E}, // Q
    {0x7F,0x09,0x19,0x29,0x46}, // R
    {0x46,0x49,0x49,0x49,0x31}, // S
    {0x01,0x01,0x7F,0x01,0x01}, // T
    {0x3F,0x40,0x40,0x40,0x3F}, // U
    {0x1F,0x20,0x40,0x20,0x1F}, // V
    {0x7F,0x20,0x18,0x20,0x7F}, // W
    {0x63,0x14,0x08,0x14,0x63}, // X
    {0x07,0x08,0x70,0x08,0x07}, // Y
    {0x61,0x51,0x49,0x45,0x43}  // Z
};

void lcd_trans(uint8_t data)
{
    // 8-bit transfer using MSSP SPI.
    SSPBUF = data;
    while (!SSPSTATbits.BF) {
    }
    (void)SSPBUF;
}

// Send exactly nine bits: D/C followed by eight payload bits.
// The 9th bit is clocked manually so the display does not
// receive seven unwanted extra clock pulses.
void lcd_send9(uint8_t dc, uint8_t value)
{
    uint8_t first = (uint8_t)((dc << 7) | (value >> 1));
    uint8_t last = value & 0x01;

    LCD_CS = 0;
    lcd_trans(first);

    // Disable MSSP temporarily to clock the ninth bit.
    SSPCONbits.SSPEN = 0;

    LCD_CLK = 0;
    LCD_DATA = last;
    __delay_us(2);
    LCD_CLK = 1;
    __delay_us(2);
    LCD_CLK = 0;

    // Return SDO/SCK pins to MSSP.
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC5 = 0;
    SSPCONbits.SSPEN = 1;

    LCD_CS = 1;
}

void lcd_trans_com(uint8_t command)
{
    lcd_send9(0, command);
}

void lcd_trans_data(uint8_t data)
{
    lcd_send9(1, data);
}

void Init_MK(void)
{
    ADCON1 = 0x06;

    LCD_CS = 1;
    LCD_RST = 1;

    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC4 = 1;

    // SPI Master, Fosc/4, mode 0
    SSPSTAT = 0x40;
    SSPCON = 0x20;
}

void LCD_Init(void)
{
    LCD_CS = 1;
    LCD_RST = 0;
    __delay_ms(2);
    LCD_RST = 1;
    __delay_ms(10);

    lcd_trans_com(0x01); // SWRESET
    __delay_ms(10);

    lcd_trans_com(0x11); // SLEEPOUT
    lcd_trans_com(0x03); // BSTRON
    __delay_ms(10);

    lcd_trans_com(0x29); // DISPON
    lcd_trans_com(0x13); // NORON

    lcd_trans_com(0x2B); // PASET
    lcd_trans_data(0);
    lcd_trans_data(131);

    lcd_trans_com(0x2A); // CASET
    lcd_trans_data(0);
    lcd_trans_data(131);

    __delay_ms(5);
    lcd_trans_com(0x2C); // RAMWR
    __delay_ms(50);

    lcd_trans_com(0x21); // INVON

    lcd_trans_com(0x3A); // COLMOD
    lcd_trans_data(0x03); // 12-bit color

    __delay_ms(5);

    lcd_trans_com(0x36); // MADCTL
    lcd_trans_data(0x20);

    lcd_trans_com(0x25); // SETCON
    lcd_trans_data(0x3A);
}

void LCD_Window(uint8_t x0, uint8_t y0,
                uint8_t x1, uint8_t y1)
{
    lcd_trans_com(0x2A);
    lcd_trans_data(x0);
    lcd_trans_data(x1);

    lcd_trans_com(0x2B);
    lcd_trans_data(y0);
    lcd_trans_data(y1);

    lcd_trans_com(0x2C);
}

// Two RGB444 pixels packed into three bytes.
void LCD_WriteTwoPixels(uint16_t color1, uint16_t color2)
{
    lcd_trans_data((uint8_t)(color1 >> 4));
    lcd_trans_data((uint8_t)(((color1 & 0x0F) << 4) |
                              (color2 >> 8)));
    lcd_trans_data((uint8_t)color2);
}

void BOX_DRW(uint8_t x0, uint8_t y0,
             uint8_t x1, uint8_t y1,
             uint16_t color)
{
    uint32_t pixels;

    if (x0 > x1 || y0 > y1 ||
        x1 >= LCD_WIDTH || y1 >= LCD_HEIGHT) {
        return;
    }

    LCD_Window(x0, y0, x1, y1);

    pixels = (uint32_t)(x1 - x0 + 1) *
             (uint32_t)(y1 - y0 + 1);

    while (pixels >= 2) {
        LCD_WriteTwoPixels(color, color);
        pixels -= 2;
    }

    if (pixels) {
        // Last pixel in 12-bit mode
        lcd_trans_data((uint8_t)(color >> 4));
        lcd_trans_data((uint8_t)((color & 0x0F) << 4));
    }
}

void LCD_SetCursor(uint8_t x, uint8_t y)
{
    LCD_Window(x, y, x, y);
}

void LCD_Pixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || y < 0 ||
        x >= LCD_WIDTH || y >= LCD_HEIGHT) {
        return;
    }

    BOX_DRW((uint8_t)x, (uint8_t)y,
            (uint8_t)x, (uint8_t)y, color);
}

void LCD_Line(int16_t x0, int16_t y0,
              int16_t x1, int16_t y1,
              uint16_t color)
{
    int16_t dx = abs(x1 - x0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t dy = -abs(y1 - y0);
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx + dy;

    while (1) {
        int16_t e2;

        LCD_Pixel(x0, y0, color);

        if (x0 == x1 && y0 == y1) {
            break;
        }

        e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void LCD_Circle(int16_t x0, int16_t y0,
                int16_t radius, uint16_t color)
{
    int16_t x = radius;
    int16_t y = 0;
    int16_t err = 1 - radius;

    while (x >= y) {
        LCD_Pixel(x0 + x, y0 + y, color);
        LCD_Pixel(x0 + y, y0 + x, color);
        LCD_Pixel(x0 - y, y0 + x, color);
        LCD_Pixel(x0 - x, y0 + y, color);
        LCD_Pixel(x0 - x, y0 - y, color);
        LCD_Pixel(x0 - y, y0 - x, color);
        LCD_Pixel(x0 + y, y0 - x, color);
        LCD_Pixel(x0 + x, y0 - y, color);

        y++;

        if (err < 0) {
            err += 2 * y + 1;
        } else {
            x--;
            err += 2 * (y - x) + 1;
        }
    }
}

uint8_t LCD_FontIndex(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return (uint8_t)(ch - '0' + 1);
    }

    if (ch >= 'A' && ch <= 'Z') {
        return (uint8_t)(ch - 'A' + 11);
    }

    return 0;
}

void LCD_Char8x8(uint8_t x, uint8_t y,
                 char ch, uint16_t color,
                 uint16_t background)
{
    uint8_t index = LCD_FontIndex(ch);

    for (uint8_t row = 0; row < 8; row++) {
        for (uint8_t col = 0; col < 8; col++) {
            bool on = false;

            if (col < 5 && row < 7) {
                on = (font5x7[index][col] &
                      (1U << row)) != 0;
            }

            LCD_Pixel((int16_t)x + col,
                      (int16_t)y + row,
                      on ? color : background);
        }
    }
}

void LCD_Text(uint8_t x, uint8_t y,
              const char *text,
              uint16_t color,
              uint16_t background)
{
    while (*text && x <= LCD_WIDTH - 8) {
        LCD_Char8x8(x, y, *text,
                    color, background);
        x += 8;
        text++;
    }
}

void main(void)
{
    Init_MK();
    LCD_Init();

    // Background
    BOX_DRW(0, 0, 131, 131, WHITE);

    // Colored header
    BOX_DRW(0, 0, 131, 24, BLUE);
    LCD_Text(12, 8, "PRACTICAL 10",
             WHITE, BLUE);

    // Graphic primitives
    BOX_DRW(10, 36, 45, 60, RED);
    LCD_Line(55, 36, 115, 60, GREEN);
    LCD_Line(55, 60, 115, 36, GREEN);
    LCD_Circle(66, 93, 23, BLUE);

    // Text
    LCD_Text(10, 120, "NOKIA 6610",
             BLACK, WHITE);

    while (1) {
    }
}

