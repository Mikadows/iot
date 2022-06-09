#include <ESP32Servo.h>
#define GATE_PIN 21
#define GATE_STEP_DELAY 10


Servo myservo;
int gatePosition = 0;
 
void setup() {
    myservo.setPeriodHertz(50);
    myservo.attach(GATE_PIN, 500, 2400);
}
 
void loop() {
  closeGate();
  delay(2000);
  openGate();
  delay(2000);
}

void openGate() {
  for (gatePosition = 0; gatePosition <= 90; gatePosition += 1) {
        myservo.write(gatePosition);
        delay(GATE_STEP_DELAY);
    }
}


void closeGate() {
  for (gatePosition = 90; gatePosition >= 0; gatePosition -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(gatePosition);    // tell servo to go to position in variable 'pos'
        delay(GATE_STEP_DELAY);             // waits 15ms for the servo to reach the position
    }
}