//***************************************************************************
//  pca9635.h
//  Programmer:  Neil Jansen (njansen1@gmail.com)
//  Date:        June 11 2012
//  Platform:    Arduino v1.01
//  Description: Library for NXP PCA9635.  Currently doesn't support blink
//               functions.  Make sure to place a #include <Wire.h> in your
//               main sketch.
//***************************************************************************
#ifndef pca9635_h
#define pca9635_h

#include "Arduino.h"

//PCA9635 I2C registers
const byte MODE1   = 0x00;
const byte MODE2   = 0x01;
const byte PWM0    = 0x02; // pin 6
const byte PWM1    = 0x03; // pin 7
const byte PWM2    = 0x04; // pin 8
const byte PWM3    = 0x05; // pin 9
const byte PWM4    = 0x06; // pin 10
const byte PWM5    = 0x07; // pin 11
const byte PWM6    = 0x08; // pin 12
const byte PWM7    = 0x09; // pin 13
const byte PWM8    = 0x0A; // pin 15
const byte PWM9    = 0x0B; // pin 16
const byte PWM10   = 0x0C; // pin 17
const byte PWM11   = 0x0D; // pin 18
const byte PWM12   = 0x0E; // pin 19
const byte PWM13   = 0x0F; // pin 20
const byte PWM14   = 0x10; // pin 21
const byte PWM15   = 0x11; // pin 22
const byte LEDOUT0 = 0x14; // PWM0,1,2,3     (pins 6,7,8,9)
const byte LEDOUT1 = 0x15; // PWM4,5,6,7     (pins 10,11,12,13)
const byte LEDOUT2 = 0x16; // PWM8,9,10,11   (pins 15,16,17,18)
const byte LEDOUT3 = 0x17; // PWM12,13,14,15 (pins 19,20,21,22)

//PCA9635 bit names for MODE1 and MODE2
const byte MODE1_SLEEP   = 0b00010000; //0=normal,     1=sleep
const byte MODE2_INVRT   = 0b00010000; //0=no invert,  1=invert
const byte MODE2_OUTDRV  = 0b00000100; //0=open-drain, 1=totem pole
const byte MODE2_OUTNE1  = 0b00000010; //when not enabled:
const byte MODE2_OUTNE0  = 0b00000001; //00=GND, 01=1(totem) or Hi-z(open-drain), 10=Hi-Z

const byte bcdTable[] = {0b00000000, 0b00000001, 0b00000100, 0b00000101, 0b00010000, 0b00010001, 0b00010100, 0b00010101, 0b01000000, 0b01000001};
//                       0: 0 0 0 0, 1: 0 0 0 1, 2: 0 0 1 0, 3: 0 0 1 1, 4: 0 1 0 0, 5: 0 1 0 1, 6: 0 1 1 0, 7: 0 1 1 1, 8: 1 0 0 0, 9: 1 0 0 1


enum ledout_state
{
  ledout_off   = 0,
  ledout_on    = 1,
  ledout_pwm   = 2,
  ledout_blink = 3
};

enum outdrv_state
{
  open_drain = 0,
  totem_pole = 1
};

enum out_not_enabled
{
  outne_not_enabled = 0,
  outne_normal      = 1,
  outne_hi_z        = 2
};

byte pca_setup();


class pca9635
{
  public:
    //Constructor
    pca9635();
    void begin(byte addr);
  
    //Sends a byte to the device
    byte sendByte(byte reg, byte data);
    
    //Reads a byte from the device
    byte readByte(byte reg);

    //Sets a pin to ON, OFF, PWM, or BLINK.
    byte setMode(byte led, byte pwm_value);
    
    //Sets sleep bit in MODE1 register
    byte sleep(boolean sleep);
    
    //Sets invert bit in MODE2 register.  
    byte setInvert(boolean invert);
    
    //Sets open-drain or totem pole
    byte setOutDrv(outdrv_state state);
    
    //Sets behavior when /OE=1 (not enabled)
    byte setOutNotEnable(out_not_enabled outne);
    
    //debug - reads registers and prints to debug console
    void printRegisters(void);
    
    unsigned long inverted_bits;
  private:
    byte mode1;   //we could read-modify-write these, but keeping a 
    byte mode2;   //   copy is safer and only costs two bytes,
    byte ledout0;
    byte ledout1;
    byte ledout2;
    byte ledout3;
    unsigned long ledout0_3; //   and is faster.
    byte slave_addr;
};


#endif