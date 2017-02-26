/*
||
|| SevenSegment Library 
|| Released into the public domain
|| 
|| @author Nart A.Schinakow, Alexander Brevig
|| @version 1.2
||
*/

#ifndef SevenSegment_h
#define SevenSegment_h

#include "WProgram.h"
#include "../ShiftRegister/ShiftRegister.h"

class SevenSegment{

public:
  SevenSegment();
  SevenSegment( ShiftRegister& userShiftRegister );
  SevenSegment( byte userPins[] ); 
  
  void buffer(byte number);
  void print();
  void print(byte number);
  
  void attachShiftRegister( ShiftRegister& userShiftRegister );
  
private:
  void initialize();
  
  ShiftRegister* shiftRegister;
  byte dataArray[];
  byte pinArray[];
  byte bufferByte;
};

#endif

/*
|| Changelog:
||
|| 1.2 2009-04-13 - Alexander Brevig : ready for ShiftRegisters 
|| 1.1 2009-03-16 - Alexander Brevig : uses binary logic to represent states, not ints. [uses 14 x less ram]
||                                     modified API slightly
||                                     modified datatypes to be as small as possible
|| 1.0 2009-03-13 - Nart : InitialRelease
*/