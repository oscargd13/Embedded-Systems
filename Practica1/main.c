// DSPIC33FJ32MC204 Configuration Bit Settings

// 'C' source line config statements

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Oscillator Mode (Primary Oscillator (XT, HS, EC) w/ PLL)
#pragma config IESO = ON                // Internal External Switch Over Mode (Start-up device with FRC, then automatically switch to user-selected oscillator source when ready)

// FOSC
#pragma config POSCMD = EC              // Primary Oscillator Source (EC Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow Only One Re-configuration)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = ON              // Watchdog Timer Enable (Watchdog timer always enabled)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)
#pragma config ALTI2C = OFF             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)
#pragma config LPOL = ON                // Motor Control PWM Low Side Polarity bit (PWM module low side output pins have active-high output polarity)
#pragma config HPOL = ON                // Motor Control PWM High Side Polarity bit (PWM module high side output pins have active-high output polarity)
#pragma config PWMPIN = ON              // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PORT register at device Reset)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include<xc.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>

#define baud_9600 1041
#define led_rojo LATAbits.LATA1
#define led_verde LATAbits.LATA0

void delay_ms(unsigned long time_ms)
{
    unsigned long u;
    for(u=0; u < time_ms*900; u++)
    {
        asm("NOP");
    }
}

void uart_config(unsigned int baud){
    
    //Configuracion de pines tx y rx
    TRISCbits.TRISC3 = 1;   //Pin de recepcion de uart establecido como entrada
    RPINR18bits.U1RXR = 19; //Pin de recepcion RC0 trabajndo con el modulo uart (RP16)
    RPOR10bits.RP20R = 3;    //U1TX conectado con el pin RC1 (RP17)
    
    //Configuración de registro de U1MODE
    U1MODEbits.UARTEN=0;  //Deshabilitar UART.
    U1MODEbits.USIDL=0;   //Continuar operacion en modo IDLE
    U1MODEbits.IREN=0;    //IR no usado
    U1MODEbits.RTSMD=1;   //Control de flujo desactivado
    U1MODEbits.UEN=0;     //Solo usamos pin de Tx y pin de Rx
    U1MODEbits.WAKE=0;    //No quiero que la UART despierte del modo sleep
    U1MODEbits.LPBACK=0;  //Loopback deshabilitado
    U1MODEbits.ABAUD=0;   //Automedicion de baudios (bps ) deshabilitada
    U1MODEbits.URXINV=0;  //En estado de reposo, el receptor mantiene un estado alto, high
    U1MODEbits.BRGH=1;    //Modo High-Speed
    U1MODEbits.PDSEL=0;   //8 Bits de datos y paridad Nula (8N)
    U1MODEbits.STSEL=0;   //1-bit de stop al final de la trama de datos (8N1)

    //Configuración de registro de U1STA
    U1STAbits.UTXISEL0=0; //Tema interrupciones (no mirar)
    U1STAbits.UTXISEL1=0; //Tema interrupciones (no mirar)
    U1STAbits.UTXINV=0;   //El estado en reposo del pin de transmision en high
    U1STAbits.UTXBRK=0;   //No usamos trama de sincronización
    U1STAbits.UTXEN=1;    //El transmisor
    U1STAbits.URXISEL=0;  //Tema interrupciones
    U1STAbits.ADDEN=0;    //No usamos direccionamiento
    //U1STAbits.RIDLE =0;
    U1STAbits.OERR=0;     //Reseteamos buffer de recepcion

    //Configuramos la velocidad de transmisión/recepción de los datos
    U1BRG = baud;
    U1MODEbits.UARTEN=1;  // Uart habilitada por completo
}

int main(void) {
    
     PLLFBD = 38;                      //M = 40
    CLKDIVbits.PLLPOST = 0;           //N1 = 2
    CLKDIVbits.PLLPRE = 0;            //N2 = 2
    //RCONbits.SWDTEN = 0;            //Disable watch dog timer
    while(OSCCONbits.LOCK != 1);      //Wait fot PLL 
    
    AD1PCFGL = 0xFFFF; //Todos los pines configurados como digitales
    
    TRISAbits.TRISA0 = 0;
    LATAbits.LATA0 = 0;
    
    TRISAbits.TRISA1 = 0;
    LATAbits.LATA1 = 0;
    
    
    uart_config(baud_9600);
    
    unsigned int cont = 0;
    bool boton_verde = false;
   
    printf("Oscar Gonzalez y Jorge Gil -- ");
    printf("Contador:\r");
    
    while(1){
        
        
        
        if(U1STAbits.URXDA == 0){
            cont = 1;
            
            while(1){
                //U1TXREG = cont; //bits que transmites por la uart
                printf("%d", cont);
                cont = cont + 1;
                U1TXREG = '\r';
                delay_ms(500);
                
                 
            if(U1STAbits.URXDA == 1){
                if(U1RXREG == 69 || U1RXREG == 101){ //E/e
                    led_rojo = 1;
                }else if(U1RXREG == 65 || U1RXREG == 97){ //A/a
                    led_rojo = 0;
                }
                
                if(U1RXREG == 72 || U1RXREG == 104){ //H/h
                    boton_verde = !boton_verde;  
                }
                if(U1RXREG == 32){
                    cont = 0;
                }
            }
                
            if(boton_verde == true){
                led_verde = !PORTAbits.RA0;
            }else if(boton_verde == false){
                led_verde = 0;
            }delay_ms(250);
        }  
    }
}
  
    return 0;
}
