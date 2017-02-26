/*
||
|| @author Alexander Brevig
|| @version 1.0
||
|| @description
|| Demonstrates the functionality of the SevenSegment class
||
*/

#include <SevenSegment.h>

byte pins[] = {6,7,8,9,10,11,12,};
SevenSegment sevenSegment = SevenSegment(pins);


void setup(){ /*nothing*/ }

void loop()
{
  for (byte i=0;i<=9;i++){
    sevenSegment.print(i);// prints selected values from 0 to 9
    delay(1000);
  }
}
