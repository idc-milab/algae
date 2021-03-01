#include <EEPROM.h>
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <string.h>
#include <DS1307.h>
#include <Wire.h>


DS1307 rtc;
#define STATE_NONE 0
#define STATE_GROWING 10
#define STATE_HARVESTING 20
#define STATE_FEEDING 30

int handleGrowing();
int handleHarvesting();
int handleFeeding();
void handleStateTransition(int old, int neww);
void logToFile(String msg);
int restorePreviousState();
void logToFile(String);
void getTime();

File data_file;
int newState = STATE_NONE;
int state;
int harvesteTimeDelay = 5000;
int feedingTimeDelay = 5000;
int airPressure = 40;
// t_time lastStateTransition;
const int SD_pin = 10;
const int PUMP1 = 6;
const int PUMP2 = 7;
//const int AIR = 9; // the Arduino pin, which connects to the IN pin of relay
// the setup function runs once when you press reset or power the board
uint8_t sec, min, hour, day, month;
uint16_t year;


void setup()
{
  state = restorePreviousState();
  Serial.begin(9600);
  pinMode(SD_pin, OUTPUT);
  pinMode(PUMP1, OUTPUT);
  pinMode(PUMP2, OUTPUT);
  //pinMode(AIR, OUTPUT);
  //EEPROM.write(202, harvesteTimeDelay);
  //EEPROM.write(303, harvesteTimeDelay);
  //data_file = SD.open("data.txt", FILE_WRITE);
  //rtc.set(0, 0, 8, 24, 12, 2014); //08:00:00 24.12.2014 //sec, min, hour, day, month, year  - only set one time
  //rtc.start();
}

void loop()
{
  
  switch (state)
  {
  case STATE_GROWING:
    newState = handleGrowing();
    break;
  case STATE_HARVESTING:
    newState = handleHarvesting();
    break;
  case STATE_FEEDING:
    newState = handleFeeding();
    break;
  }
  if (newState != state)
  {
    handleStateTransition(state, newState);
  }
  state = newState;
}

long lastSpectometerCheckTstamp;

int handleGrowing()
{
  // TODO:
  // 1. check how much time passed since growing state started - for detecting HW problems or fatalities
  // 2. check (millis() - lastSpectometerCheckTstamp):
  // 2.1 if more than  10 minutes - check, logToFile() and update lastSpectometerCheckTstamp = millis();
  // 2.1.1 if spectometer green enough, return STATE_HARVESTING
  // 2.2 if not, return STATE_GROWING

  return STATE_HARVESTING;
}

int handleHarvesting()
{
  // TODO: activate harvesting pump for preconfigued time
  int harvestingDurationSeconds = EEPROM.read(202);
  //analogWrite(AIR, 0);              //Turn airPump Off
  Serial.println("Air Pump Off");
  digitalWrite(PUMP2, HIGH);        // activate pump2
  Serial.println("Pump2 Pump On");
  delay(harvestingDurationSeconds);// delay(harvestingDurationSeconds * 1000L);
  digitalWrite(PUMP2, LOW);
  Serial.println("Pump2 Pump Off"); // stop activate pump2 
  //analogWrite(AIR, airPressure);    //Turn airPump On
  Serial.println("Air Pump On");
  
  return STATE_FEEDING;             //next step State
}

int handleFeeding()
{
  // TODO: activate feeding pump for preconfigued time
  int feedingDurationSeconds = EEPROM.read(303);
  digitalWrite(PUMP1, HIGH);         // activate pump1
  Serial.println("Pump1 Pump On");
  Serial.println("Time: ");
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(min, DEC);
  Serial.print(":");
  delay(feedingDurationSeconds);
  digitalWrite(PUMP1, LOW);         // stop activate pump1
  Serial.println("Pump1 Pump Off"); // delay(feedingDurationSeconds * 1000L);
  Serial.println("Time: ");
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(min, DEC);
  Serial.print(":");

  return STATE_GROWING;             //next step State
}

void handleStateTransition(int old, int neww)
{
  logToFile(String("Old State: ") + old + ", new state: " + neww);
  // 1. persist new state to EEPROM: which, start time
  EEPROM.write(0, neww);
  // TODO:
  // 1.1. get current time using rtc.now();

  // 1.2. serialize time to bytes
  // 1.3. persist times bytes to eeprom at address 1 upwards
  // 1.4. lastStateTransition = now;

  // 2. write transition details to log file
}

int restorePreviousState()
{
  return EEPROM.read(0);
}

void logToFile(String msg)
{
  if (data_file)
  {
    data_file.println(msg);       // cheack how to right rtc.now
   // data_file.println(rtc.now()); // cheack how to wright rtc.now to show exact time
  }
  else
  {
    return;
  }
  // t_time = now();
  // log time + msg
}
void get_Time()
{
  uint8_t sec, min, hour, day, month;
  uint16_t year;
  rtc.get(&sec, &min, &hour, &day, &month, &year);
  Serial.print("\nTime: ");
  Serial.print(hour,DEC);
  Serial.print(":");
  Serial.print(min,DEC);
  Serial.print(":");
  Serial.print(sec,DEC);

  Serial.print("\nDate: ");
  Serial.print(day,DEC);
  Serial.print("/");
  Serial.print(month,DEC);
  Serial.print("/");
  Serial.print(year,DEC);
  
}

