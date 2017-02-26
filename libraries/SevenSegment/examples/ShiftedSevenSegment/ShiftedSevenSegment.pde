/*
||
|| @author Alexander Brevig
|| @version 1.0
||
|| @description
|| Demonstrates the functionality of the SevenSegment class and ShiftRegister595 class
||
*/

#include <ShiftRegister595.h>
#include <SevenSegment.h>

ShiftRegister595 shiftRegister = ShiftRegister595(10,11,12);
SevenSegment sevenSegment = SevenSegment(shiftRegister);

void setup(){ /*nothing*/ }

void loop()
{
  for (byte i=0;i<=9;i++){
    sevenSegment.print(i);// prints selected values from 0 to 9
    delay(1000);
  }
}

