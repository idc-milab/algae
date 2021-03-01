#include <EEPROM.h>
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <string.h>
#include "AS726X.h"
#include <DS1307.h>
#include <Wire.h>



DS1307 rtc;
#define STATE_NONE 0
#define STATE_GROWING 10
#define STATE_HARVESTING 20
#define STATE_FEEDING 30

AS726X sensor;

int handleGrowing();
int handleHarvesting();
int handleFeeding();
void handleStateTransition(int old, int neww);
void logToFile(String msg);
int restorePreviousState();
void logToFile(String);
void get_Time();

//varabales we want to adjust
File data_file;
int newState = STATE_NONE;
int state;
float harvesteTimeDelay = 5000;
float feedingTimeDelay = 5000;
int airPressure = 40;
int goalGreenValue = 0;         //need to update
float LightSampleInterval = 600000; // ten minuts
unsigned long startMillis;        //start time of the program
unsigned long currentMillis;      // update every call to millis()

// t_time lastStateTransition;
const int SD_pin = 10;
const int PUMP1 = 6;
const int PUMP2 = 7;
const int AIR = 9; // the Arduino pin, which connects to the IN pin of relay
// the setup function runs once when you press reset or power the board
uint8_t sec;
uint8_t min;
uint8_t hour;
uint8_t day;
uint8_t month;
uint16_t year;


void setup()
{
  Serial.begin(9600);
  state = restorePreviousState();
  pinMode(SD_pin, OUTPUT);
  pinMode(PUMP1, OUTPUT);
  pinMode(PUMP2, OUTPUT);
  pinMode(AIR, OUTPUT);
  EEPROM.write(202, harvesteTimeDelay);
  EEPROM.write(303, harvesteTimeDelay);
  //data_file = SD.open("data.txt", FILE_WRITE);
  rtc.set(0, 0, 8, 24, 12, 2014); //08:00:00 24.12.2014 //sec, min, hour, day, month, year  - only set one time
  rtc.start();
}

void loop()
{
  Serial.println("start monitor");
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

// 1. check how much time passed since growing state started - for detecting HW problems or fatalities
// 2. check (millis() - lastSpectometerCheckTstamp):
// 2.1 if more than  10 minutes - check, logToFile() and update lastSpectometerCheckTstamp = millis();
// 2.1.1 if spectometer green enough, return STATE_HARVESTING
// 2.2 if not, return STATE_GROWING


int handleGrowing()
{
 
  currentMillis = millis();
  if (currentMillis - startMillis > LightSampleInterval) // cheack if we waiting more then LightSampleInterval
  {
    sensor.takeMeasurementsWithBulb(); //take sensor sample
    float sensorSample = sensor.getCalibratedOrange();
    Serial.print("Sampling light: green value is ");
    Serial.print(sensorSample);
    logToFile(String("the green value is: ") + sensorSample); //write to log file
    if (sensorSample > goalGreenValue)
    {
      startMillis = currentMillis;
      return STATE_HARVESTING;
    }
    Serial.println("Air Pump Off"); // give one minute rest for the air pump
    logToFile("air pumped off");
    while (millis() < currentMillis + 60000) // 60000= one mimute
    {
      analogWrite(AIR, 0);
    }
    analogWrite(AIR, airPressure); //Turn airPump On
    Serial.println("Air Pump On");
    logToFile("air pumped on");
    startMillis = currentMillis;
  }

  return STATE_HARVESTING;
}

// activate harvesting pump for preconfigued time

int handleHarvesting()
{
  int harvestingDurationSeconds = EEPROM.read(202);
  analogWrite(AIR, 0);              //Turn airPump Off
  Serial.println("Air Pump Off");
  digitalWrite(PUMP2, HIGH); // activate pump2
  Serial.println("Pump2 Pump On");
  delay(harvestingDurationSeconds);// delay(harvestingDurationSeconds * 1000L);
  digitalWrite(PUMP2, LOW);
  Serial.println("Pump2 Pump Off"); // stop activate pump2 
  analogWrite(AIR, airPressure);    //Turn airPump On
  Serial.println("Air Pump On");
  
  return STATE_FEEDING;             //next step State
}

 //  activate feeding pump for preconfigued time
int handleFeeding()
{

  int feedingDurationSeconds = EEPROM.read(303);
  digitalWrite(PUMP1, HIGH); // activate pump1
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

  return STATE_GROWING; //next step State
}

void handleStateTransition(int old, int neww)
{
  logToFile(String("Old State: ") + old + ", new state: " + neww);
  // 1. persist new state to EEPROM: which, start time
  EEPROM.write(0, neww);
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

    data_file.println(msg); // check how to write rtc.now
    // String time = getTime();         //need to implement
    // data_file.println("At" + time); // data_file.println(rtc.now()); // cheack how to wright rtc.now to show exact time
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

