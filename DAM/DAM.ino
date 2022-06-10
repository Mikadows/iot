#include <WiFi.h>
#include <PubSubClient.h> //Librairie pour la gestion Mqtt 
#include <ESP32Servo.h>


/**
 * Digital IO
 */
#define RED_LED_PIN 18
#define GREEN_LED_PIN 19
#define POTAR_PIN 33
#define GATE_PIN 21
#define GATE_STEP_DELAY 10
Servo myservo;
int gatePosition = 0;

/**
 * MQTT & NETWORT Definitions
 */
#define MQTT_TOPIC_SUB "iot-projo-gate"
#define MQTT_TOPIC_PUB "iot-projo-water-level"
#define MQTT_SERVER "test.mosquitto.org" //Adresse IP du Broker Mqtt
long tps=0;
const char* ssid = "ESGI";
const char* password = "Reseau-GES";

/**
 * NETWORK & MQTT SETUP
 */
WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi(){
  WiFi.begin();
  while ( WiFi.status() != WL_CONNECTED) {
    delay ( 500 );
    Serial.println ( "no connected" );
  }
  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.print("MAC : ");
  Serial.println(WiFi.macAddress());
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

void setup_mqtt(){
  client.setServer(MQTT_SERVER, 1883);
  client.setCallback(callback); //Déclaration de la fonction de souscription
  reconnect();
}

//Callback doit être présent pour souscrire a un topic et de prévoir une action 
void callback(char* topic, byte *payload, unsigned int length) {
  Serial.println("-------Nouveau message du broker mqtt-----");
  Serial.print("Canal:");
  Serial.println(topic);
  Serial.print("donnee:");
  Serial.write(payload, length);
  Serial.println();

  // Recieve message to close or open the gate
  
  
   if ((char) payload[0] == 'o') {
     openGate();
   } else if ((char) payload[0] == 'c') {
     closeGate();
   }
 }

void reconnect(){
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe(MQTT_TOPIC_SUB);
    }
    else {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 2s");
      delay(2000);
    }
  }
  client.subscribe(MQTT_TOPIC_SUB);//souscription au topic led pour commander une led
}

/*************************************************/

float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // IO Setup
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  // Servo Setup
  myservo.setPeriodHertz(50);
  myservo.attach(GATE_PIN, 500, 2400);
  
  // Gate setup
  closeGate();
  
  // Network & MQTT Setup
  setup_wifi();
  setup_mqtt();
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin GIOP36:
  int analogValue = analogRead(POTAR_PIN);
  // Rescale to potentiometer's voltage (from 0V to 3.3V):
  float voltage = floatMap(analogValue, 0, 4095, 0, 3.3);

  // print out the value you read:
  Serial.print("Analog: ");
  Serial.print(analogValue);
  Serial.print(", Voltage: ");
  Serial.println(voltage);

  manageRoad(analogValue);
  publishWaterLevel(analogValue);
  
  delay(100);


  reconnect();
  client.loop(); 
  //On utilise pas un delay pour ne pas bloquer la réception de messages 
  if (millis()-tps>10000){
     tps=millis();
     float temp = random(30);
   }
   

  Serial.println("---------------------------");


}

/**
 * Manage the road
 */
void manageRoad(int value) {
  if (value <= 800) {
    // Send mqtt message : to open gate
    Serial.   println("Send mqtt message : to open gate");
    // openGate();    
  } 

  if (value <= 1000) {
    // Send mqtt message : play sound (on led)
    Serial.println("Send mqtt message : play sound (on led)");
  } 

  if (value <= 1200) {
    // Turn local led ON
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    Serial.println("ROAD CLOSED => Local LED ON");
  } else {
    // Turn local led OFF
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    Serial.println("ROAD OPEN => Local LED OFF");
//    closeGate();
  }
}

/**
 * Gate control
 */
bool isGateOpened() {
  return gatePosition >= 90;
}
bool isGateClosed() {
  return gatePosition <= 0;
}
 
void openGate() {
  if (isGateClosed()) {
    while(!isGateOpened()){
      gatePosition += 1;
      myservo.write(gatePosition);
      delay(GATE_STEP_DELAY);
    }
  }
}

void closeGate() {
  if (isGateOpened()) {
    while(!isGateClosed()){
      gatePosition -= 1;
      myservo.write(gatePosition);
      delay(GATE_STEP_DELAY);
    }
  }
}

/**
 * Publish water level
 */
void publishWaterLevel(int value) {
    reconnect();
    Serial.print("Publish message: ");
    char msg_out[20];
    sprintf(msg_out, "%d", value);
    client.publish(MQTT_TOPIC_PUB, msg_out);
}
