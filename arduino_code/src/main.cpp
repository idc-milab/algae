#include <Arduino.h>

// constants won't change
const int PUMP1 = D5;
const int PUMP2 = D7;
const int AIR = D1;  // the Arduino pin, which connects to the IN pin of relay
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin A5 as an output.
  pinMode(PUMP1, OUTPUT);
  pinMode(PUMP2, OUTPUT);
  pinMode(AIR, OUTPUT);

}
// the loop function runs over and over again forever
void loop() {
  digitalWrite(PUMP1, LOW);  // turn off pump1 5 seconds
  digitalWrite(PUMP2, HIGH);  // turn on pump2 5 seconds
  delay(5000);
  digitalWrite(PUMP1, HIGH); // turn on pump1 5 seconds
  digitalWrite(PUMP2, LOW); // turn off pump2 5 seconds
  delay(5000);
  anlogWrite(AIR,HIGH);
  analogWrite(AIR, 100) // to change the volume change the value in the right side
  delay(5000);
  