#include <SevenSegment.h>
#include <ShiftRegister595.h>
#include <ShiftRegister.h>
#include <ICMPPing.h>
#include <dht.h>
#include <DallasTemperature.h>
#include <OneWire.h> 
#include <SPI.h>
#include <LiquidCrystal.h>
#include <EthernetUdp.h>
#include <Ethernet.h>
#include <util.h>


/*

        PINBELEGUNG
        
        Ethernet Shield
PIN   4      SS SDCard Ethernet PIN 4
PIN  10      SS Ethernet PIN 10
        LCD-Display
PIN  26      LCD Data d4
PIN  27      LCD Data d5
PIN  28      LCD Data d6
PIN  29      LCD Data d7
PIN  30      RS - LCD Pin 4
PIN  31      RW - LCD Pin 5
PIN  32      Enable - LCD Pin 6
PIN  33      Backlight - LCD Pin 15 (Potentiometer)
        7-Segment
PIN  35      DATA 7-Segment
PIN  36      CLK  7-Segment
        1-Wire
PIN  38      LCK Latch Innensensor       (use pullup 4,5K)
PIN  39      LCK Latch Außensensor      (use pullup 4,5K)
        DHT22
PIN  40      Signal DHT Pin 2 
        7-Segment
PIN  42      latchPin (ST_CP)
PIN  43      clockPin (SH_CP)
PIN  44      dataPin  (DS)
        Ethernet Shield
PIN  50      MISO Ethernet
PIN  51      MOSI Ethernet
PIN  52      SCK Ethernet
PIN  53      Hardware SPI Ethernet (outputmode)

*/

#define DHT22_PIN 40

/*
      DECLARATIONS
*/

/*
      DHT
*/

dht DHT;
/*
      1-Wire
*/
OneWire  oneWireInnen(38);
DallasTemperature sensorInnen(&oneWireInnen);
OneWire oneWireAussen(39);
DallasTemperature sensorAussen(&oneWireAussen);

/* 
     LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)  
*/

LiquidCrystal lcd(30, 31, 32, 29, 28, 27, 26);
/* 
    Ethernet Shield
 */
int spiHardware = 53;
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //Change
IPAddress ip(192, 168, 177, 181);
unsigned int localPort = 8888;      // local port to listen on
// buffers for receiving and sending data
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
char ReplyBuffer[] = "acknowledged";       // a string to send back
IPAddress pingAddr(192,168,177,25); // ip address to ping
SOCKET pingSocket=0;
char buffer[256];
ICMPPing ping(pingSocket, (uint16_t)random(0, 255));

// An EthernetUDP instance to let us send and receive packets over UDP
EthernetUDP Udp;
/*
      7-Segment
 */
byte latchPin = 42;
byte clockPin = 43;
byte dataPin = 44;  
ShiftRegister595 shiftRegister = ShiftRegister( latchPin , clockPin , dataPin );
SevenSegment sevenSegment = SevenSegment(shiftRegister);

/*
      Constant
*/
const uint32_t crystalFrequency = 16000000UL; // Frequenz des Oszillators in Hz -> 16 MHz
const uint8_t timerFrequency = 1UL; // Frequenz des Timer 1 in Hz -> 10 Hz
volatile uint8_t timerflag = 0; // Flag zur Anzeige, dass der Timerinterrupt aktiviert wurde


/*
    GLOBAL VARIABLE
*/
int anzeigezaehler = 0; // Zeitliche Festlegung der Anzeigedauer

int anzeigeflag = 0; // Schritte der Anzeigedauer für die unterschiedlichen Darstellungen

int aussentemp = 0;

int innentemp = 0;

int feuchte = 0;

char *temperaturen[3];

/*
    HELPERS
*/

void timerInit() // Initialisierung Timer 1
{
  TCCR1A = (0 << WGM11) | (0 << WGM10); // Modus 4: CTC
  TCCR1B = (0 << WGM13) | (1 << WGM12); // Modus 4: CTC
  TCNT1  = 0; // Counter zurücksetzen
  TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10); // Prescaler: 256
  OCR1A = (uint16_t)(crystalFrequency / (timerFrequency * 256UL)) - 1; // Output Compare Register für 10 Hz bei einem Prescaler von 256 einstellen
  TIFR1 = (1 << OCF1A); // Compare Match Flag zurücksetzen
  TIMSK1 |= (1 << OCIE1A); // Compare Match Interrupt aktivieren
}

 

ISR(TIMER1_COMPA_vect) // Interrupt-Routine des Timer 1, welcher im 0,1s-Takt auslöst
{
  timerflag = 1; // Timerflag setzen, so dass die loop-Routine erkennt, dass 0,1 s vergangen sind
}

/*
      SETUP
*/

void setup() {
  Serial.begin(9600);
    // Timer initialisieren
  noInterrupts();
  timerInit();
  interrupts();
  
  // start the Ethernet and UDP:
  pinMode(spiHardware, OUTPUT);
  Ethernet.begin(mac,ip);
  Udp.begin(localPort);
  Serial.println("Ethernet initiated");
  
  // Liquid Crystal  
  lcd.begin(20,4);              // columns, rows.  use 16,2 for a 16x2 LCD, etc.
  lcd.display();
  lcd.clear();                  // start with a blank screen
  Serial.println("LCD initiated");

  
  // 1-Wire
  
  sensorInnen.begin();
  sensorAussen.begin();
   

  }




void loop()
{  
  //Auslesen DHT22
  float DHT22_feuchte = DHT.humidity;
  int DHT22_feuchte_innen_int=(int)(DHT22_feuchte+.5); // Float in Integer wandeln
  feuchte = DHT22_feuchte_innen_int;
  char DHT22_feuchte_innen_char[4]; // String Variable anlegen
  sprintf(DHT22_feuchte_innen_char,"%d", DHT22_feuchte_innen_int); // Integer in String wandeln, Text für Zeile 3 (2)
  temperaturen[3] = DHT22_feuchte_innen_char;
  Serial.print(DHT22_feuchte_innen_char);
 
  //1-Wire abfragen
  sensorInnen.requestTemperatures(); // Temperaturen 1Wire-Sensor Innen
    
  float onewire_sauna_aussen_float = sensorAussen.getTempCByIndex(0); // Erster 1Wire-Sensor (außerhalb der Sauna)
  int onewire_sauna_aussen_int=(int)(onewire_sauna_aussen_float+.5); // Float in Integer wandeln mit kaufmännischer Rundung
  aussentemp = onewire_sauna_aussen_int;
  char onewire_sauna_aussen_char[4]; // String Variable anlegen
  sprintf(onewire_sauna_aussen_char,"%d", onewire_sauna_aussen_int); // Integer in String wandeln, Text für Zeile 3 (2)
  temperaturen[0] = onewire_sauna_aussen_char;
  //Serial.print("1-Wire Aussen:");
  //Serial.println(onewire_sauna_aussen_char); 
  
  sensorAussen.requestTemperatures(); // Temperatur 1Wire-Sensor Außen
  
  float onewire_sauna_innen_float = sensorInnen.getTempCByIndex(0); // Zweiter 1Wire-Sensor (in der Sauna)
  int onewire_sauna_innen_int=(int)(onewire_sauna_innen_float+.5); // Float in Integer wandeln mit kaufmännischer Rundung
  innentemp = onewire_sauna_innen_int;
  char onewire_sauna_innen_char[4]; // String Variable anlegen
  sprintf(onewire_sauna_innen_char,"%d", onewire_sauna_innen_int); // Integer in String wandeln, Text für Z
  temperaturen[1] = onewire_sauna_innen_char;
  //Serial.print("1-Wire Innen:");
  //Serial.println(onewire_sauna_innen_char);
  
  if(timerflag == 1)
  {

    timerflag = 0;
    anzeigezaehler++;
    if(anzeigezaehler == 1) // Saunagangdauer (schwitzzeit)
    {
      anzeigeflag = 1;
    }

    if(anzeigezaehler == 15) // "t °C"
    {
      anzeigeflag = 2;
    }

    if(anzeigezaehler == 16)  // Temp Sauna
    {
      anzeigeflag = 3;
    }

    if(anzeigezaehler == 20) // F %
    {
      anzeigeflag = 4;
    }

    if(anzeigezaehler == 21) // Feuchte Sauna
    {
      anzeigeflag = 5;
    }

    if(anzeigezaehler == 25) // ZEIT
    {
      anzeigeflag = 0;
      anzeigezaehler = 0;
    }

      char *test[3];
      test[0]=onewire_sauna_aussen_char;
      String zeile[3];
      zeile[0]="Aussen: ";
      zeile[1]="Sauna: ";
      zeile[2]="H2O:   ";
      lcd.clear();
      for(int row = 0; row <=2; row++) 
      {
        lcd.setCursor(0,row);
        lcd.print(zeile[row]);
        lcd.setCursor(8,row);
        lcd.print(temperaturen[row]); 
        Serial.println(temperaturen[row]);   
      }

      
    }
 
  
  
  
  
}


