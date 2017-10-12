/* Flash this program first to store eeprom variables in atmega328 eeprom
Variables ->Address:Value
*/
#include <EEPROMex.h>
#include "Arduino.h"
void issuedAdresses();
const int maxAllowedWrites = 8000000;
const int memBase          = 50;

//first wake up
uint8_t wake_HOUR_1 = 8;
uint8_t wake_MINUTE_1 = 30;
uint8_t wake_SECOND_1 = 15;

//second wake up
uint8_t wake_HOUR_2 = 13;
uint8_t wake_MINUTE_2 = 30;
uint8_t wake_SECOND_2 = 15;

//third wake up
uint8_t wake_HOUR_3 = 15;
uint8_t wake_MINUTE_3 = 30;
uint8_t wake_SECOND_3 = 15;

int AddressByteHour1;
int AddressByteMinute1;
int AddressByteSecond1; 
int AddressByteHour2;
int AddressByteMinute2;
int AddressByteSecond2;
int AddressByteHour3;
int AddressByteMinute3;
int AddressByteSecond3;
int AddressNumber1Char;
int AddressNumber2Char;
int AddressPasswordChar;

int AddressIntHour1;
int AddressIntMinute1;
int AddressIntHour2;
int AddressIntMinute2;
int AddressIntHour3;
int AddressIntMinute3;


char Number1[]   = "0000000000";// change to your mobile number
char Number2[]   = "0000000000";// second number not implemented yet 
char Password[]  = "1234";
char input[]     = "Arduino";

void issuedAdresses() {
    Serial.println("-----------------------------------");     
    Serial.println("Following adresses have been issued");     
    Serial.println("-----------------------------------");      
    
    Serial.println("adress \t\t size");
    
    Serial.print(AddressByteHour1);            Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (Byte)");
    Serial.print(AddressByteMinute1);          Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (Byte)");
    Serial.print(AddressByteSecond1);          Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (Byte)");
    Serial.print(AddressByteHour2);            Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (Byte)");
    Serial.print(AddressByteMinute2);          Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (Byte)");
    Serial.print(AddressByteSecond2);          Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (Byte)");
    Serial.print(AddressByteHour3);            Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (Byte)");
    Serial.print(AddressByteMinute3);          Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (Byte)");
    Serial.print(AddressByteSecond3);          Serial.print(" \t\t "); Serial.print(sizeof(byte)); Serial.println(" (Byte)");

    Serial.print(AddressIntHour1);            Serial.print(" \t\t "); Serial.print(sizeof(int)); Serial.println(" (int)");
    Serial.print(AddressIntMinute1);          Serial.print(" \t\t "); Serial.print(sizeof(int)); Serial.println(" (int)");
    Serial.print(AddressIntHour2);            Serial.print(" \t\t "); Serial.print(sizeof(int)); Serial.println(" (int)");
    Serial.print(AddressIntMinute2);          Serial.print(" \t\t "); Serial.print(sizeof(int)); Serial.println(" (int)");
    Serial.print(AddressIntHour3);            Serial.print(" \t\t "); Serial.print(sizeof(int)); Serial.println(" (int)");
    Serial.print(AddressIntMinute3);          Serial.print(" \t\t "); Serial.print(sizeof(int)); Serial.println(" (int)");
    
    Serial.print(AddressNumber1Char);          Serial.print(" \t\t "); Serial.print(sizeof(char)*10); Serial.println(" (array of 10 chars)");
    Serial.print(AddressNumber2Char);          Serial.print(" \t\t "); Serial.print(sizeof(char)*10); Serial.println(" (array of 10 chars)"); 
    
    Serial.print(AddressPasswordChar);         Serial.print(" \t\t "); Serial.print(sizeof(char)*4); Serial.println(" (array of 4 chars)");   
  
}  

void setup() {
Serial.begin(9600);  

EEPROM.setMemPool(memBase, EEPROMSizeUno);
EEPROM.setMaxAllowedWrites(maxAllowedWrites);
 delay(100);
AddressByteHour1         = EEPROM.getAddress(sizeof(byte));
delay(100);
AddressByteMinute1       = EEPROM.getAddress(sizeof(byte));
delay(100);
AddressByteSecond1       = EEPROM.getAddress(sizeof(byte)); 
delay(100);
AddressByteHour2         = EEPROM.getAddress(sizeof(byte));
delay(100);
AddressByteMinute2       = EEPROM.getAddress(sizeof(byte));
delay(100);
AddressByteSecond2       = EEPROM.getAddress(sizeof(byte));
delay(100);
AddressByteHour3         = EEPROM.getAddress(sizeof(byte));
delay(100);
AddressByteMinute3       = EEPROM.getAddress(sizeof(byte));
delay(100);
AddressByteSecond3       = EEPROM.getAddress(sizeof(byte));
delay(100);
AddressIntHour1         = EEPROM.getAddress(sizeof(int)); 
delay(100);
AddressIntMinute1         = EEPROM.getAddress(sizeof(int));
delay(100);
AddressIntHour2         = EEPROM.getAddress(sizeof(int)); 
delay(100);
AddressIntMinute2         = EEPROM.getAddress(sizeof(int)); 
delay(100);
AddressIntHour3         = EEPROM.getAddress(sizeof(int)); 
delay(100);
AddressIntMinute3         = EEPROM.getAddress(sizeof(int));
delay(100);
AddressNumber1Char       = EEPROM.getAddress(sizeof(char)*10); //Must be one byte more than i need..
delay(100);
AddressNumber2Char       = EEPROM.getAddress(sizeof(char)*10);
delay(100); 
AddressPasswordChar      = EEPROM.getAddress(sizeof(char)*4);
delay(100);

issuedAdresses(); 

EEPROM.write(AddressByteHour1,wake_HOUR_1);
delay(100);
EEPROM.write(AddressByteMinute1,wake_MINUTE_1);
delay(100);
EEPROM.write(AddressByteSecond1,15);
delay(100);
EEPROM.write(AddressByteHour2,wake_HOUR_2);
delay(100);
EEPROM.write(AddressByteMinute2,wake_MINUTE_2);
delay(100);
EEPROM.write(AddressByteSecond2,15);
delay(100);
EEPROM.write(AddressByteHour3,wake_HOUR_3);
delay(100);
EEPROM.write(AddressByteMinute3,wake_MINUTE_3);
delay(100);
EEPROM.write(AddressByteSecond3,15);
delay(100);
EEPROM.writeBlock<char>(AddressNumber1Char, Number1, 10);
delay(100);
EEPROM.writeBlock<char>(AddressNumber2Char, Number2, 10);
delay(100);
EEPROM.writeBlock<char>(AddressPasswordChar, Password, 4);
delay(100);


}


void loop() {
  // put your main code here, to run repeatedly:

}

