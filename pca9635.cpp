#include "Arduino.h"
#include <Wire.h>
#include <pca9635.h>

//Constructor
pca9635::pca9635()
{
}

void pca9635::begin(byte addr)
{
  mode1=0b00;
  mode2=0b00;
  ledout0_3 = 0;
  slave_addr = addr;
  inverted_bits=0;
}
  
//Sends a byte to the device
byte pca9635::sendByte(byte reg, byte data)
{
  byte endCode;
  Wire.beginTransmission(slave_addr);
  byte bytes_returned  = Wire.write(reg);   //Control reg - Set address
       bytes_returned += Wire.write(data);  //send data byte
  endCode = Wire.endTransmission();
  switch (endCode)
  {
    case 0:
    default:
      break;
    case 1:
      //Serial.println("ERR_I2C_PCA9635: Data too long to fit in transmit buffer");
      break;
    case 2:
      //Serial.println("ERR_I2C_PCA9635: Received NACK on transmit of address");
      break;
    case 3:
      //Serial.println("ERR_I2C_PCA9635: Received NACK on transmit data");
      break;
    case 4:
      //Serial.println("ERR_I2C_PCA9635: Other error");
      break;
  }
  //delay(1);
  return (bytes_returned);
}

//Reads a byte from the device
byte pca9635::readByte(byte reg)
{
  Wire.beginTransmission(slave_addr);  //Send START condition 
  Wire.write(reg);                     //Send control register
  Wire.endTransmission(false);         //Send RESTART condition
  Wire.requestFrom(slave_addr,byte(1),byte(false));   //Request byte
  byte rcvd_byte = Wire.read();        //Read the byte
  Wire.endTransmission();              //Send STOP condition
  return rcvd_byte;
}

//Sets a pin to ON, OFF, PWM, or BLINK.
byte pca9635::setMode(byte led, byte pwm_value)
{
  ledout_state state; //on/off/pwm/blink
  byte pwm_value2 = pwm_value;
  
  if (pwm_value==0)
  {
    state = ledout_off;
  }
  else if (pwm_value==255)
  {
    state = ledout_on;
  }
  else
  {
    state = ledout_pwm;
  }
  
  if (((inverted_bits>>led)&0x01)!=0)
  {
    //If the output is inverted, we need to flip the states (OFF becomes ON, and ON becomes OFF)
    if (state==ledout_off) 
    {
      state = ledout_on; 
    }
    else if (state==ledout_on)  
    {
      state = ledout_off;
    }
    
    //Invert the PWM value
    pwm_value2 = ~pwm_value;
  }
	
// 	Switch (led)
// 	{
// 	  case 0:
// 	  case 1:
// 	  case 2:
// 	  case 3:
// 	  break;
// 	  case 4:
// 	  case 5:
// 	  case 6:
// 	  case 7:
// 	  break;
// 	  case 8:
// 	  case 9:
// 	  case 10:
// 	  case 11:
// 	  break;
// 	  case 12:
// 	  case 13:
// 	  case 14:
// 	  case 15:
// 	  break;
// 	  default:
// 	  break;
// 	}
  //clear the two mode bits for the LED register
  //TODO: fix modulo
  ledout0_3 &= (unsigned long)~((unsigned long)0b11<<((unsigned long)led*2));  
  
  //Set the proper bits depending on the LED and the mode, and the invert
  ledout0_3 |= (unsigned long)state<<((unsigned long)led*2);

  byte returnValue = 0;
  returnValue += sendByte(LEDOUT0,byte(ledout0_3));
  returnValue += sendByte(LEDOUT1,byte(ledout0_3>>8));
  returnValue += sendByte(LEDOUT2,byte(ledout0_3>>16));
  returnValue += sendByte(LEDOUT3,byte(ledout0_3>>24));
  returnValue += sendByte(led+PWM0, pwm_value2);
  //printRegisters();
  return(returnValue);
}

//Sets sleep bit in MODE1 register
byte pca9635::sleep(boolean sleep)
{
  if (sleep)
  {
  	mode1 |= MODE1_SLEEP;
  }
  else
  {
    mode1 &= ~MODE1_SLEEP;
  }
  byte returnValue = sendByte(MODE1,mode1);
	
	//as per datasheet, the device takes ~500 ms to fully wake up.  Give 2ms to be sure.
  delay(2);
  return returnValue;
}

//Sets invert bit in MODE2 register.  
byte pca9635::setInvert(boolean invert)
{
  if (invert)
  {
  	mode2 |= MODE2_INVRT;
  }
  else
  {
    mode2 &= ~MODE2_INVRT;
  }
  return sendByte(MODE2,mode2);
}

//Sets open-drain or totem pole in MODE2 register.
byte pca9635::setOutDrv(outdrv_state state)
{
  switch (state)
  {
  case open_drain: //0
    mode2 &= ~MODE2_OUTDRV;      //Clear the bit
    break;
  case totem_pole: //1
    mode2 |= byte(MODE2_OUTDRV); //Set the bit
    break;
  }
  return sendByte(MODE2,mode2);
}

//Sets behavior when /OE=1 (not enabled)
byte pca9635::setOutNotEnable(out_not_enabled outne)
{
  mode2 &= ~MODE2_OUTNE1;
  mode2 &= ~MODE2_OUTNE0;
  mode2 |= outne;
  return sendByte(MODE2,mode2);
}

void pca9635::printRegisters(void)
{
  Serial.println("PCA9635 Byte Map");
  Serial.println("----------------");
  Serial.print("MODE1 ");
  Serial.println(readByte(MODE1),BIN);
  Serial.print("MODE2 ");
  Serial.println(readByte(MODE2),BIN);
  Serial.print("LEDOUT0 ");
  Serial.println(readByte(LEDOUT0),BIN);
  Serial.print("LEDOUT1 ");
  Serial.println(readByte(LEDOUT1),BIN);
  Serial.print("LEDOUT2 ");
  Serial.println(readByte(LEDOUT2),BIN);
  Serial.print("LEDOUT3 ");
  Serial.println(readByte(LEDOUT3),BIN);
  delay(250);

}

