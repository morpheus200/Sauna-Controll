#include "SevenSegment.h"

//<<constructor>>
SevenSegment::SevenSegment(){
	byte c[]={2,3,4,5,6,7,8};
	initialize();
	for (byte i=0;i<7;i++){
		pinArray[i]=c[i];
		pinMode(pinArray[i],OUTPUT);
	}
}

//<<constructor>>
SevenSegment::SevenSegment( ShiftRegister& userShiftRegister ){
	shiftRegister = &userShiftRegister;
	initialize();
}

//<<constructor>> parameter userPins defines the pins to use in order to drive the display
SevenSegment::SevenSegment(byte userPins[]){
	initialize();
	for (byte set=0;set<7;set++){
		pinArray[set]=userPins[set];
		pinMode(pinArray[set],OUTPUT);
	}
}

//buffer a number to display
void SevenSegment::buffer(byte number){
	bufferByte = number;
}

//print a number to the display
void SevenSegment::print(){
	if(shiftRegister){
		shiftRegister->write( bufferByte );
	}else{
		for (byte col=0;col<8;col++){
			digitalWrite(pinArray[col],bitRead(bufferByte,7-col));
		}
	}
}

//print a number to the display
void SevenSegment::print(byte number){
	if(shiftRegister){
		shiftRegister->write( dataArray[number] );
	}else{
		for (byte col=0;col<8;col++){
			digitalWrite(pinArray[col],bitRead(dataArray[number],7-col));
		}
	}
}

void SevenSegment::attachShiftRegister( ShiftRegister& userShiftRegister ){
	shiftRegister = &userShiftRegister;
}

//initialize data
void SevenSegment::initialize(){
	bufferByte = 0;
	shiftRegister = 0;
	byte data[10] = {
		B01111110,
		B00110000,
		B01101101,
		B01111001,
		B00110011,
		B01011011,
		B00011111,
		B01110000,
		B01111111,
		B01110011
	};
	for (byte row=0; row<10; row++){
		dataArray[row]=data[row];
	}
}

/*
|| Changelog:
||
|| 2009-04-13 - Alexander Brevig : implemented ShiftRegister related logic
|| 2009-03-16 - Alexander Brevig : implemented all changes noted in the .h file
*/