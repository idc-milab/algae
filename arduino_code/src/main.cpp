#include <Arduino.h>
#include <SD.h>
#include <SPI.h>


// constants won't change
File data_file;
int CS_pin = 10;
long sec=00;
long mint=00;
long hr=00;


const int PUMP1 = 6;
const int PUMP2 = 7;
const int AIR = 9;  // the Arduino pin, which connects to the IN pin of relay
// the setup function runs once when you press reset or power the board
void setup() {
//
  Serial.begin(9600);
  pinMode(CS_pin, OUTPUT);
  pinMode(PUMP1, OUTPUT);
  pinMode(PUMP2, OUTPUT);
  //pinMode(AIR, OUTPUT);

  // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is initialized and it is ready to use");
  } else
  {
    Serial.println("SD card is not initialized");
    return;
  }

}
// the loop function runs over and over again forever
void loop() {

   data_file = SD.open("data.txt", FILE_WRITE);

   // if the file opens, run and log:
  if (data_file) {   
       for(long sec = 00; sec < 60; sec=sec+2){
          data_file.print(hr);
          data_file.print(":");
          data_file.print(mint);
          data_file.print(":");
          data_file.print(sec);
          data_file.print(",  ");
          Serial.print(hr);
          Serial.print(":");
          Serial.print(mint);
          Serial.print(":");
          Serial.print(sec);
          Serial.print(",    ");
          if(sec>=58){
            mint= mint + 1;
          }
          if (mint>59){
            hr = hr + 1;
            mint = 0;
            }
     // analogWrite(AIR , 40);//Turn airPump on

      data_file.println("Air Pump On");
      Serial.println("Air Pump On");
      digitalWrite(PUMP2, HIGH);  // Turn on pump2
      data_file.println("Pump2 Pump On");
      Serial.println("Pump2 Pump On");
      delay(5000); // How much time Pump2 would be on 
      digitalWrite(PUMP2, LOW);// Turn Pump2 off
      data_file.println("Pump2 Pump Off");
      Serial.println("Pump2 Pump Off");
      digitalWrite(PUMP1, HIGH);  //Turn on Pump1
      data_file.println("Pump1 Pump On");
      Serial.println("Pump1 Pump On");
      delay(5000);// How much time Pump1 would be on
      digitalWrite(PUMP1, LOW);//  Turn off pump1 until next loop
      data_file.println("Pump1 Pump Off");
      Serial.println("Pump1 Pump Off");
      data_file.flush(); //saving the file
      delay(2000);
      data_file.close(); //closing the file
      Serial.println("file closed");
      delay(166666);// Delay Bbetween loops
    }
  }
  
  // if the file didn't open, print an error and run anyway:
  else
  {
    Serial.println("error opening data.txt");

      // analogWrite(AIR , 40);//Turn airPump on
      Serial.println("Air Pump On");
      digitalWrite(PUMP2, HIGH);  // Turn on pump2
      Serial.println("Pump2 - Pump On");
      delay(5000); // How much time Pump2 would be on 
      digitalWrite(PUMP2, LOW);// Turn Pump2 off
      Serial.println("Pump2 - Pump Off");
      digitalWrite(PUMP1, HIGH);  //Turn on Pump1
      Serial.println("Pump1 - Pump On");
      delay(5000);// How much time Pump1 would be on
      digitalWrite(PUMP1, LOW);//  Turn off pump1 until next loop
      Serial.println("Pump1 - Pump Off");
   

    delay(200000);// Delay Bbetween loops;
}
}
  