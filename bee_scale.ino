//>25 dB – Great signals
//18-25 dB – Very good
//11-18 dB -  Medium
//5-10 dB - Low
//4 dB – Very poor

// during an alarm the INT pin of the RTC is pulled low
//
// this is handy for minimizing power consumption for sensor-like devices, 
// since they can be started up by this pin on given time intervals.
//===============Settings Commands==============
//set number to send sms            
//"1234,NUMBER:00000000"
//change password
//"1234,CHANGE:1821"
//set wake3
//"1234,WAKE3:19:30"
//set wake2
//"1234,WAKE2:13:30"
//set wake1
//"1234,WAKE1:08:30"
//send sms with current settings
//"1234,CONFIG
//send sms
//"1234,SMS
//===============Settings Commands==============

#include <Wire.h>
#include "ds3231.h"
#include <Sleep_n0m1.h>
#include <EEPROMex.h>
#include "Arduino.h"
#include <Vcc.h>
#include "HX711.h"
#include <SoftwareSerial.h>
#include <GPRS_Shield_Arduino.h>

#define intPin 2
#define PIN_TX    7
#define PIN_RX    8
#define BAUDRATE  2400

#define MESSAGE_LENGTH 25

const int maxAllowedWrites = 800000;
const int memBase          = 50;
const float VccMin   = 0.0;           // Minimum expected Vcc level, in Volts.
const float VccMax   = 5.0;           // Maximum expected Vcc level, in Volts.
const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc
 
Vcc vcc(VccCorrection);
HX711 scale(A1, A0);

//first wake up
uint8_t wake_HOUR_1 = 16;
uint8_t wake_MINUTE_1 = 54;
uint8_t wake_SECOND_1 = 15;

//second wake up
uint8_t wake_HOUR_2 = 16;
uint8_t wake_MINUTE_2 = 55;
uint8_t wake_SECOND_2 = 15;

//third wake up
uint8_t wake_HOUR_3 = 16;
uint8_t wake_MINUTE_3 = 56;
uint8_t wake_SECOND_3 = 15;
// how often to refresh the info on stdout (ms)
unsigned long prev = 5000, interval = 5000;

uint8_t wake_1 = 0;
uint8_t wake_2 = 0;
uint8_t wake_3 = 0;

uint8_t first_boot = 0;
uint8_t send_config = 0;
uint8_t send_sms = 0;
int pass;
float temp_1;
float temp_2;
float temp_3;
float weight_1;
float weight_2;
float weight_3;
float p;

//--------------variable for receive sms --------------
char tempChars[20];        // temporary array for use by strtok() function
char value_number1[] = "          ";
char password1[] = "    ";

Sleep sleep;
GPRS gprsTest(PIN_TX,PIN_RX,BAUDRATE);//RX,TX,BaudRate

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    scale.begin(A1, A0);
    scale.set_scale(2280.f);  // this value is obtained by calibrating the scale with known weights; see the README for details           
    pinMode(6, OUTPUT);//buzzer PWM
    pinMode(4, OUTPUT);//step up power module pin
    pinMode(10, OUTPUT);//hx711 power pin
    pinMode(11, OUTPUT);//gsm power pin
    
    digitalWrite(4, HIGH);
    digitalWrite(10, HIGH);
    //digitalWrite(11, HIGH);
    delay(200);
    scale.begin(A1, A0);
    scale.set_scale(2280.f);  // this value is obtained by calibrating the scale with known weights; see the README for details  
    correct_sound(); 
    first_boot = 0;
    //--------------variable for receive sms --------------
    char gprsBuffer[64];
    int i = 0;
    char *s = NULL;
    int inComing = 0;
    char recvChar;
    
    //start_GSM();
    scale.tare();    // reset the scale to 0
    correct_sound();
      
uint32_t period = 1 * 60000L;       // 5 minutes
for( uint32_t tStart = millis();  (millis()-tStart) < period; ){
  // doStuff();    
  if(gprsTest.readable()) {
       inComing = 1;
   }else{ delay(500);}
   
   if(inComing){
      sim900_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
      //Serial.print(gprsBuffer);
       if(NULL != (s = strstr(gprsBuffer,"+CMTI: \"SM\""))) { //SMS: $$+CMTI: "SM",24$$
          delay(1000);
          char message[MESSAGE_LENGTH];
          int messageIndex = atoi(s+12);
          gprsTest.readSMS(messageIndex, message,MESSAGE_LENGTH);
          delay(1000);
          Serial.print(F("Recv Mess"));
         Serial.println(message);
          strcpy(tempChars, message);
          delay(500);
          parseData();
          Serial.print(F("PrsDTEx"));
          gprsTest.deleteSMS(messageIndex);
          delay(1000);
     }
     sim900_clean_buffer(gprsBuffer,32);  
     delay(1000);
     inComing = 0;
   }
  
}
    digitalWrite(4, LOW);
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);

}
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void correct_sound(){
    beep(50);
    beep(50);
    beep(50);
    beep(50);
    beep(50);
    beep(50);
  }
void wrong_sound(){
    beep(200);
    beep(200);
    beep(200);
    beep(200);
    beep(200);
    beep(200);
    beep(200);
    beep(200);
  
  }
void set_alarm_1(){

  if(first_boot == 1){
    wake3_actions();
    }
    // flags define what calendar component to be checked against the current time in order
    // to trigger the alarm - see datasheet
    // A1M1 (seconds) (0 to enable, 1 to disable)
    // A1M2 (minutes) (0 to enable, 1 to disable)
    // A1M3 (hour)    (0 to enable, 1 to disable) 
    // A1M4 (day)     (0 to enable, 1 to disable)
    // DY/DT          (dayofweek == 1/dayofmonth == 0)
    uint8_t flags[5] = { 0, 0, 0, 1, 1 };
    DS3231_init(DS3231_INTCN);
    DS3231_clear_a1f();
    wake_HOUR_1 = EEPROM.read(50);
    delay(500);
    wake_MINUTE_1 = EEPROM.read(51);
    delay(500);
    // set Alarm1    //second         //minute         //hour
     DS3231_set_a1(wake_SECOND_1, wake_MINUTE_1, wake_HOUR_1, 0, flags);
    // activate Alarm1
    DS3231_set_creg(DS3231_INTCN | DS3231_A1IE);
    Serial.println(F("A1A"));
    wake_1 = 1;
  }

void set_alarm_2(){
   uint8_t flags[5] = { 0, 0, 0, 1, 1 };
   DS3231_init(DS3231_INTCN);
   DS3231_clear_a1f();
   wake_HOUR_2 = EEPROM.read(53);
   delay(500);
   wake_MINUTE_2 = EEPROM.read(54);
   delay(500);
  // set Alarm2
  DS3231_set_a1(wake_SECOND_2, wake_MINUTE_2,wake_HOUR_2, 0, flags);
  // activate Alarm2
  DS3231_set_creg(DS3231_INTCN | DS3231_A1IE);
  Serial.print(F("A2A"));
  wake_2 = 1;
  wake1_actions();
  first_boot = 1;
  }

void set_alarm_3(){
  uint8_t flags[5] = { 0, 0, 0, 1, 1 };
  DS3231_init(DS3231_INTCN);
  DS3231_clear_a1f();
  wake_HOUR_3 = EEPROM.read(56);
  delay(500);
  wake_MINUTE_3 = EEPROM.read(57);
  delay(500);
  // set Alarm3
  DS3231_set_a1(wake_SECOND_3, wake_MINUTE_3, wake_HOUR_3, 0, flags);
  // activate Alarm3
  DS3231_set_creg(DS3231_INTCN | DS3231_A1IE);
  Serial.print(F("A3A"));
  wake_1 = 0;
  wake_2 = 0;
  wake2_actions();
  
  }

 void wake1_actions(){
  digitalWrite(4, HIGH);
  digitalWrite(10, HIGH);
  delay(500);
  scale.power_up();
  delay(3000);
  temp_1 = DS3231_get_treg();
  delay(200);
  weight_1 = scale.get_units()*0.109, 2;
  delay(500);
  scale.power_down();             // put the ADC in sleep mode
  delay(1000);
           // put the ADC in sleep mode
  digitalWrite(4, LOW);
  digitalWrite(10, LOW);
 // Serial.println("Temp and weight");
 // Serial.println(temp_1);
 // Serial.println(weight_1);
  }
 void wake2_actions(){
  digitalWrite(4, HIGH);
  digitalWrite(10, HIGH);
  delay(500);
  scale.power_up();
  delay(3000);
  temp_2 = DS3231_get_treg();
  delay(200);
  weight_2 = scale.get_units()*0.109, 2;
  delay(500);
  scale.power_down();             // put the ADC in sleep mode
  delay(1000);
  digitalWrite(4, LOW);
  digitalWrite(10, LOW);
 // scale.power_up();           // put the ADC in sleep mode
  //Serial.println("Temp and weight");
 // Serial.println(temp_2);
 // Serial.println(weight_2);
  }
 void wake3_actions(){
  digitalWrite(4, HIGH);
  digitalWrite(10, HIGH);
  delay(500);
  scale.power_up();
  delay(3000);
  temp_3 = DS3231_get_treg();
  delay(200);
  weight_3 = scale.get_units()*0.109, 2;
  delay(500);
  p = vcc.Read_Perc(VccMin, VccMax);
  scale.power_down();             // put the ADC in sleep mode
  delay(1000);
  digitalWrite(4, LOW);
  digitalWrite(10, LOW);
          // put the ADC in sleep mode
  //Serial.println("Temp and weight");
  //Serial.println(temp_3);
  //Serial.println(weight_3);
  //Serial.println("Battery status");
  //Serial.println(v);
  //Serial.println(p);
  sendSMS();
  //digitalWrite(4, LOW);
  digitalWrite(10, LOW);
 }

void set_alarm(){
  if (wake_1 == 0 && wake_2 == 0){
    set_alarm_1();
  }
   else if(wake_1 == 1 && wake_2 == 0){
    set_alarm_2();
   }
  else if (wake_1 == 1 && wake_2 == 1){
    set_alarm_3();
    }
  }
  
void beep(unsigned char delayms){
  
  analogWrite(6, 5);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(6, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
}  

void start_GSM(){
  digitalWrite(11, HIGH);
  digitalWrite(4, HIGH);
  delay(32000);
  while(!gprsTest.init()) {
     //delay(3000);
      //Serial.print(F("init error\r\n"));
      wrong_sound();

  }  
 Serial.println(F("gprs init success"));
  //Serial.println("start to send message or receive ...");
  correct_sound();
  }

void parseData() {
  char password[5] = {0};
  char command[10] = {0};
  char value[10] = {0};
  char value1[10] = {0}; 
  EEPROM.readBlock<char>(91, password1, 4);
  pass = atoi(password1);
    // split the data into its parts
    
  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars,",");      // get the first part - the string
  strcpy(password, strtokIndx); // copy it to messageFromPC
  //Serial.println(password);
  

  strtokIndx = strtok(NULL, ":"); 
  strcpy(command, strtokIndx);  
 // Serial.println(command);


  strtokIndx = strtok(NULL, ":"); 
  strcpy(value, strtokIndx);  
 //Serial.println(value);

  strtokIndx = strtok(NULL, ":"); 
  strcpy(value1, strtokIndx); 
 // Serial.println(value1);
  

  
  if (atoi(password) == pass){
    //Serial.println("Password Correct");
    if(strcmp(command, "CONFIG") == 0){
      send_config = 1;
      correct_sound();
      }
    if(strcmp(command, "SMS") == 0){
      send_sms = 1;
      correct_sound();    
      }
    if (strcmp(command, "NUMBER") == 0){
     // Serial.println("chg num");
      EEPROM.writeBlock<char>(71, value, 10);
      //Serial.println("Value from sms");
      //Serial.print(value);
     // EEPROM.readBlock<char>(59, value_number1, 10);
      //Serial.println("Value from Eeprom");
      //Serial.print(value_number1);
      correct_sound();
      }
    if (strcmp(command, "CHANGE") == 0){
      //Serial.println("I change password with ");
      EEPROM.writeBlock<char>(91, value, 4);
      //Serial.println("Value from sms");
      //Serial.print(value);
      //EEPROM.readBlock<char>(81, password1, 4);
      //Serial.println("Value from Eeprom");
      //Serial.print(password1);
      correct_sound();
      }
     if (strcmp(command, "WAKE3") == 0){
      //Serial.println("I set WAKE3 to ");
    wake_HOUR_3 = (uint8_t)atoi(value);
    wake_MINUTE_3 = (uint8_t)atoi(value1);
      EEPROM.write(56,wake_HOUR_3);
      delay(500);
      EEPROM.write(57,wake_MINUTE_3);
      delay(500);
      EEPROM.writeInt(67, atoi(value));
      delay(500);
      EEPROM.writeInt(69, atoi(value1));
      //wake_HOUR_3 = EEPROM.read(56);
      //delay(500);
      //wake_MINUTE_3 = EEPROM.read(57);
      //Serial.println("Values from Eeprom byte variables");
      //Serial.print(wake_HOUR_3);
      //Serial.print(":");
      //Serial.print(wake_MINUTE_3);
      correct_sound();
   }
     if (strcmp(command, "WAKE2") == 0){
      //Serial.println("I set WAKE2 to ");
    wake_HOUR_2 = (uint8_t)atoi(value);
    wake_MINUTE_2 = (uint8_t)atoi(value1);
      EEPROM.write(53,wake_HOUR_2);
      delay(500);
      EEPROM.write(54,wake_MINUTE_2);
      delay(500);
      EEPROM.writeInt(63, atoi(value));
      delay(500);
      EEPROM.writeInt(65, atoi(value1));
    //Serial.println(wake_HOUR_2);
    //Serial.print(":");
    //Serial.print(wake_MINUTE_2);
    //wake_HOUR_2 = EEPROM.read(53);
    //wake_MINUTE_2 = EEPROM.read(54);
    //Serial.println("Values from Eeprom byte variables");
      //Serial.print(wake_HOUR_2);
      //Serial.print(":");
      //Serial.print(wake_MINUTE_2);
      correct_sound();
      
     }
     if (strcmp(command, "WAKE1") == 0){
      //Serial.println("I set WAKE1 to ");
      wake_HOUR_1 = (uint8_t)atoi(value);
      wake_MINUTE_1 = (uint8_t)atoi(value1);
      EEPROM.write(50,wake_HOUR_1);
      delay(500);
      EEPROM.write(51,wake_MINUTE_1);
      delay(500);
      EEPROM.writeInt(59, atoi(value));
      delay(500);
      EEPROM.writeInt(61, atoi(value1));
      //Serial.println(wake_HOUR_1);
     // Serial.print(":");
     // Serial.print(wake_MINUTE_1);
    //  wake_HOUR_1 = EEPROM.read(50);
    //  wake_MINUTE_1 = EEPROM.read(51);
   // Serial.println("Values from Eeprom byte variables");
     // Serial.println(wake_HOUR_1);
     // Serial.print(":");
      //Serial.print(wake_MINUTE_1);
      correct_sound();
     }
  }
  else{
      //Serial.print("Wrong Password");
      wrong_sound();
      }
  /*strtokIndx = strtok(NULL, ","); 
  integerFromPC = atoi(strtokIndx);  // convert this part to a float
  Serial.println(integerFromPC);
  
  strtokIndx = strtok(NULL, ","); 
  floatFromPC = atof(strtokIndx);  // convert this part to a float
  Serial.println(floatFromPC);

  strtokIndx = strtok(NULL, ","); 
  strcpy(anotherstring, strtokIndx);  // convert this part to a float
  Serial.println(anotherstring); */
}
void sendSMS(){
  
  char sms[55];
  char temp_string[7];
  memset(sms, '\0', sizeof(sms));
  memset(temp_string, '\0', sizeof(temp_string));
  
  strcpy(sms, "T1:");
  dtostrf(temp_1, 4, 1, temp_string);
  strcat(sms, temp_string);
  dtostrf(weight_1, 5, 2, temp_string);
  strcat(sms, " W1:");
  strcat(sms, temp_string);
  strcat(sms, "\n");

  dtostrf(temp_2, 4, 1, temp_string);
  strcat(sms, "T2:");
  strcat(sms, temp_string);
  dtostrf(weight_2, 5, 2, temp_string);
  strcat(sms, " W2:");
  strcat(sms, temp_string);
  strcat(sms, "\n");

  dtostrf(temp_3, 4, 1, temp_string);
  strcat(sms, "T3:");
  strcat(sms, temp_string);
  dtostrf(weight_3, 5, 2, temp_string);
  strcat(sms, " W3:");
  strcat(sms, temp_string);
  strcat(sms, "\n");

  dtostrf(p, 3, 0, temp_string);
  strcat(sms, "Batt:" );
  strcat(sms, temp_string);
  strcat(sms, "%" );
  
  
  Serial.println(sms);
  EEPROM.readBlock<char>(71, value_number1, 10);
  delay(500);
  //Serial.println(value_number1);
  //start_GSM();
  //delay(1000);
  //gprsTest.sendSMS(value_number1, sms); //define phone number and text
  delay(2000);
  Serial.print(F("S OK"));
  digitalWrite(4, LOW);
  }
void send_config_sms(){
  
 char sms[70];
 char temp[5];
 int *sgnlstr;
 int sgnl;
 memset(sms, '\0', sizeof(sms));
 memset(temp, '\0', sizeof(temp));
 start_GSM();
 delay(1000);
 p = vcc.Read_Perc(VccMin, VccMax);
 gprsTest.getSignalStrength(sgnlstr);
 delay(500);
 sgnl = *sgnlstr;
 strcpy(sms, "Num:");
 delay(50);
 EEPROM.readBlock<char>(71, value_number1, 10);
 delay(50);
 strcat(sms, value_number1);
 strcat(sms, "\n");
 strcat(sms, "W1:");
 sprintf(temp, "%d", EEPROM.readInt(59));
 delay(50);
 strcat(sms, temp);
 strcat(sms, ":");
 sprintf(temp, "%d", EEPROM.readInt(61));
 delay(50);
 strcat(sms, temp);
 strcat(sms, "\n");
 strcat(sms, "W2:");
 sprintf(temp, "%d", EEPROM.readInt(63));
 delay(50);
 strcat(sms, temp);
 strcat(sms, ":");
 sprintf(temp, "%d", EEPROM.readInt(65));
 delay(50);
 strcat(sms, temp);
 strcat(sms, "\n");   
 strcat(sms, "W3:");
 sprintf(temp, "%d", EEPROM.readInt(67));
 delay(50);
 strcat(sms, temp);
 strcat(sms, ":");
 sprintf(temp, "%d", EEPROM.readInt(69));
 delay(50);
 strcat(sms, temp);
 strcat(sms, "\n");   
 dtostrf(p, 3, 0, temp);
 strcat(sms, "Batt:" );
 strcat(sms, temp); 
 strcat(sms, "%" );
 strcat(sms, "\n");  
 strcat(sms, "SGNL:");
 sprintf(temp, "%d", sgnl);
 strcat(sms, temp);
 strcat(sms, "\n");
 strcat(sms, "WGHT:");
 weight_3 = scale.get_units()*0.109, 2;
 dtostrf(weight_3, 5, 2, temp);
 strcat(sms, temp);
 
Serial.println(sms);
 
  EEPROM.readBlock<char>(71, value_number1, 10);
  delay(100);
  //Serial.println(value_number1);

  //delay(2000);
  //gprsTest.sendSMS(value_number1, sms); //define phone number and text
  delay(2000);
  Serial.println(F("S OK"));   
  send_config = 0; 
  digitalWrite(4, LOW); 
}
void sleep_function(){
  scale.power_down();
  sleep.pwrDownMode(); //set sleep mode

  //Sleep till interrupt pin equals a particular state.
  //In this case "low" is state 0.
  
  sleep.sleepPinInterrupt(intPin,FALLING); //(interrupt Pin Number, interrupt State)
  
  }
void loop()
{
  if (send_config == 1){
    send_config_sms();
    }
  Serial.print(F("loop"));
  set_alarm();     //
  
  delay(100); ////delays are just for serial print, without serial they can be removed
  //Serial.println("execute your code here");
  Serial.print(F("Sleep"));

  delay(1000); //delay to allow serial to fully print before sleep
  sleep_function();

}
