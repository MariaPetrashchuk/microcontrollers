
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

// Configuration bits
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

#define EEPROM_WRITE 0xA0
#define EEPROM_READ  0xA1
#define EEPROM_ADDR  0x00
#define TEST_DATA    0xA5

volatile uint8_t read_data = 0;
volatile uint8_t write_success = 0;
volatile uint8_t read_success = 0;

// Wait until the MSSP module is idle
void I2C_Wait(void)
{
    while ((SSPCON2 & 0x1F) || SSPSTATbits.R_nW)
    {
    }
}

// Initialize I2C at 100 kHz
void I2C_Init(void)
{
    TRISCbits.TRISC3 = 1; // SCL
    TRISCbits.TRISC4 = 1; // SDA

    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;

    SSPSTAT = 0x80;
    SSPCON = 0x28;
    SSPCON2 = 0x00;

    // Fosc / (4 * (SSPADD + 1))
    // 4 MHz / (4 * 10) = 100 kHz
    SSPADD = 9;
}

// Generate START condition
void I2C_Start(void)
{
    I2C_Wait();
    SSPCON2bits.SEN = 1;

    while (SSPCON2bits.SEN)
    {
    }
}

// Generate repeated START
void I2C_Restart(void)
{
    I2C_Wait();
    SSPCON2bits.RSEN = 1;

    while (SSPCON2bits.RSEN)
    {
    }
}

// Generate STOP condition
void I2C_Stop(void)
{
    I2C_Wait();
    SSPCON2bits.PEN = 1;

    while (SSPCON2bits.PEN)
    {
    }
}

// Transmit a byte and check ACK
bool I2C_Trans(uint8_t data)
{
    I2C_Wait();

    PIR1bits.SSPIF = 0;
    SSPBUF = data;

    while (!PIR1bits.SSPIF)
    {
    }

    PIR1bits.SSPIF = 0;

    return (SSPCON2bits.ACKSTAT == 0);
}

// Receive one byte, then send ACK or NACK
uint8_t I2C_Receive(bool ack)
{
    uint8_t data;

    I2C_Wait();

    SSPCON2bits.RCEN = 1;

    while (!SSPSTATbits.BF)
    {
    }

    data = SSPBUF;

    I2C_Wait();

    SSPCON2bits.ACKDT = ack ? 0 : 1;
    SSPCON2bits.ACKEN = 1;

    while (SSPCON2bits.ACKEN)
    {
    }

    return data;
}

uint8_t RECIEVE_ACK(void)
{
    return I2C_Receive(true);
}

uint8_t RECIEVE_NACK(void)
{
    return I2C_Receive(false);
}

// Write one byte to EEPROM address 0x00
bool EEPROM_WriteByte(uint8_t address, uint8_t data)
{
    I2C_Start();

    if (!I2C_Trans(EEPROM_WRITE))
    {
        I2C_Stop();
        return false;
    }

    if (!I2C_Trans(address))
    {
        I2C_Stop();
        return false;
    }

    if (!I2C_Trans(data))
    {
        I2C_Stop();
        return false;
    }

    I2C_Stop();

    // Wait for internal EEPROM write cycle
    __delay_ms(10);

    return true;
}

// Read one byte from EEPROM
bool EEPROM_ReadByte(uint8_t address, uint8_t *data)
{
    I2C_Start();

    if (!I2C_Trans(EEPROM_WRITE))
    {
        I2C_Stop();
        return false;
    }

    if (!I2C_Trans(address))
    {
        I2C_Stop();
        return false;
    }

    I2C_Restart();

    if (!I2C_Trans(EEPROM_READ))
    {
        I2C_Stop();
        return false;
    }

    *data = RECIEVE_NACK();

    I2C_Stop();

    return true;
}

void main(void)
{
    TRISD = 0x00;
    PORTD = 0x00;

    I2C_Init();

    __delay_ms(100);

    // Write 0xA5 to EEPROM address 0x00
    write_success = EEPROM_WriteByte(
        EEPROM_ADDR,
        TEST_DATA
    );

    if (write_success)
    {
        read_success = EEPROM_ReadByte(
            EEPROM_ADDR,
            (uint8_t *)&read_data
        );
    }

    while (1)
    {
        if (write_success && read_success)
        {
            PORTD = read_data;
        }
        else
        {
            PORTD = 0x00;
        }

        __delay_ms(100);
    }
}

