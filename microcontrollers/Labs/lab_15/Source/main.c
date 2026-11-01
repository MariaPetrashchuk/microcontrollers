
#include <xc.h>
#include <stdint.h>

// =======================================
// LAB 15: EEPROM 24C16 / I2C
// PIC16F877A, XC8, 4 MHz
// =======================================

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// 24C16: block 0, write/read addresses
#define EEPROM_WRITE 0xA0
#define EEPROM_READ  0xA1

#define MEMORY_ADDRESS 0x00
#define TEST_DATA      0xA5

volatile uint8_t written_data = TEST_DATA;
volatile uint8_t received_data = 0;
volatile uint8_t write_verified = 0;
volatile uint8_t i2c_error = 0;

// =======================================
// Wait until MSSP is idle
// =======================================

void I2C_Wait(void)
{
    while ((SSPCON2 & 0x1F) ||
           SSPSTATbits.R_nW)
    {
    }
}

// =======================================
// Initialize I2C master, 100 kHz
// =======================================

void I2C_Init(void)
{
    TRISCbits.TRISC3 = 1; // SCL
    TRISCbits.TRISC4 = 1; // SDA

    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;

    SSPCON = 0x28;   // MSSP enabled, I2C Master
    SSPCON2 = 0x00;
    SSPSTAT = 0x80;  // SMP=1, CKE=0
    SSPADD = 9;      // 4MHz/(4*(9+1)) = 100kHz

    PIR1bits.SSPIF = 0;
}

// =======================================
// I2C START
// =======================================

void I2C_Start(void)
{
    I2C_Wait();

    SSPCON2bits.SEN = 1;

    while (SSPCON2bits.SEN)
    {
    }
}

// =======================================
// I2C repeated START
// =======================================

void I2C_Restart(void)
{
    I2C_Wait();

    SSPCON2bits.RSEN = 1;

    while (SSPCON2bits.RSEN)
    {
    }
}

// =======================================
// I2C STOP
// =======================================

void I2C_Stop(void)
{
    I2C_Wait();

    SSPCON2bits.PEN = 1;

    while (SSPCON2bits.PEN)
    {
    }
}

// =======================================
// Transmit byte; return 1 if ACK
// =======================================

uint8_t I2C_Trans(uint8_t data)
{
    I2C_Wait();

    SSPBUF = data;

    while (SSPSTATbits.R_nW)
    {
    }

    return (SSPCON2bits.ACKSTAT == 0);
}

// =======================================
// Receive byte and send ACK / NACK
// =======================================

uint8_t I2C_Receive(uint8_t send_ack)
{
    uint8_t data;

    I2C_Wait();

    SSPCON2bits.RCEN = 1;

    while (!SSPSTATbits.BF)
    {
    }

    data = SSPBUF;

    I2C_Wait();

    // ACKDT = 0: ACK
    // ACKDT = 1: NACK
    SSPCON2bits.ACKDT = send_ack ? 0 : 1;
    SSPCON2bits.ACKEN = 1;

    while (SSPCON2bits.ACKEN)
    {
    }

    return data;
}

// =======================================
// Write one byte to EEPROM
// =======================================

uint8_t EEPROM_WriteByte(uint8_t address,
                         uint8_t data)
{
    uint8_t ok = 1;

    I2C_Start();

    if (!I2C_Trans(EEPROM_WRITE))
        ok = 0;

    if (ok && !I2C_Trans(address))
        ok = 0;

    if (ok && !I2C_Trans(data))
        ok = 0;

    I2C_Stop();

    if (!ok)
        return 0;

    // Wait for internal EEPROM write cycle
    __delay_ms(10);

    return 1;
}

// =======================================
// Read one byte from EEPROM
// =======================================

uint8_t EEPROM_ReadByte(uint8_t address,
                        uint8_t *data)
{
    I2C_Start();

    if (!I2C_Trans(EEPROM_WRITE))
    {
        I2C_Stop();
        return 0;
    }

    if (!I2C_Trans(address))
    {
        I2C_Stop();
        return 0;
    }

    I2C_Restart();

    if (!I2C_Trans(EEPROM_READ))
    {
        I2C_Stop();
        return 0;
    }

    *data = I2C_Receive(0); // Last byte: NACK

    I2C_Stop();

    return 1;
}

// =======================================
// Main
// =======================================

void main(void)
{
    ADCON1 = 0x06;

    TRISD = 0x00;
    PORTD = 0x00;

    I2C_Init();

    __delay_ms(100);

    while (1)
    {
        i2c_error = 0;
        write_verified = 0;

        // Write 0xA5 to address 0x00
        if (!EEPROM_WriteByte(MEMORY_ADDRESS,
                              written_data))
        {
            i2c_error = 1;
            PORTD = 0x00;
        }
        else
        {
            // Read byte back
            if (EEPROM_ReadByte(MEMORY_ADDRESS,
                                (uint8_t *)&received_data))
            {
                PORTD = received_data;

                if (received_data == written_data)
                {
                    write_verified = 1;
                }
            }
            else
            {
                i2c_error = 2;
                PORTD = 0x00;
            }
        }

        __delay_ms(1000);
    }
}
