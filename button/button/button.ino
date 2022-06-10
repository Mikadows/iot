#include <WiFi.h>
#include <PubSubClient.h> //Librairie pour la gestion Mqtt 

#define BUZZER_PIN  18
#define LED 19
#define PUSH_BUTTON 32

int lastState = HIGH;
int stateLed = LOW;
const char* ssid = "ESGI";
const char* password = "Reseau-GES";
//MQTT
const char* mqtt_server = "test.mosquitto.org";//Adresse IP du Broker Mqtt
char gateState = '0';
long tps = 0;


WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi(){
  //connexion au wifi
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.print("MAC : ");
  Serial.println(WiFi.macAddress());
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}
void setup_mqtt(){
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);//Déclaration de la fonction de souscription
}

void callback(char* topic, byte *payload, unsigned int length) {
   Serial.println("-------Nouveau message de Gwendalino-----");
   Serial.print("Canal:");
   Serial.println(topic);
   Serial.print("donnee:");
   //Serial.write(payload, length);
   gateState = (char)payload[0];
   Serial.println(gateState);
 }

void blinkLed(){
  if(stateLed==HIGH){
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    delay(200);
  }
}

void blinkBuzzerError(){
  if(stateLed==HIGH && gateState == '9'){
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED, LOW);
    delay(200);
  }
}

void publish(char* msg_out){
    client.publish("dam/presence", msg_out);
//    delay(100);
}

void to_subscribe(){
  client.loop();
  if (millis()-tps>10000){
   tps=millis();
   float temp = random(30);
 }
}

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT); 
  setup_wifi();
  setup_mqtt();
}

// the loop function runs over and over again forever
void loop() {

  if (!client.connected())
  {
    if (client.connect("ESP32_clientID")) {
      client.subscribe("iot-projo-gate-angle");
    }
    else {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 1s");
    }
  }
  
  int currentState = digitalRead(PUSH_BUTTON);
  //print out the value of the pushbutton
  if (lastState == LOW && currentState == HIGH){
    lastState = HIGH;
    if(stateLed == LOW){
      stateLed = HIGH;
    }else{
      stateLed = LOW;
      digitalWrite(LED, stateLed);
    }
    blinkLed();
  }
  Serial.print("Publish message: ");
  Serial.println(stateLed);
  char msg_out[20];
  sprintf(msg_out, "%d", stateLed);
  publish(msg_out);
  blinkLed();
  blinkBuzzerError();
  lastState = currentState;

  client.loop();
  delay(50);
}
