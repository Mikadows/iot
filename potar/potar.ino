#define RED_LED 18

float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  pinMode(RED_LED, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin GIOP36:
  int analogValue = analogRead(15);
  // Rescale to potentiometer's voltage (from 0V to 3.3V):
  float voltage = floatMap(analogValue, 0, 4095, 0, 3.3);

  // print out the value you read:
  Serial.print("Analog: ");
  Serial.print(analogValue);
  Serial.print(", Voltage: ");
  Serial.println(voltage);

  isRoadClosed(analogValue);
  
  delay(100);

  Serial.println("---------------------------");
}

void isRoadClosed(int value) {
  if (value <= 800) {
    // Send mqtt message : to open gate
    Serial.println("Send mqtt message : to open gate");

  } 

  

  if (value <= 1000) {
    // Send mqtt message : play sound (on led)
    Serial.println("Send mqtt message : play sound (on led)");

  } 

  if (value <= 1200) {
    // Turn local led ON
    digitalWrite(18, HIGH);
    Serial.println("ROAD CLOSED => Local LED ON");
  } else {
    // Turn local led OFF
    digitalWrite(18, LOW);
    Serial.println("ROAD OPEN => Local LED OFF");
  }
}
