#include <WiFi.h>
#include <PubSubClient.h> //Librairie pour la gestion Mqtt 

#define LED 14
#define PUSH_BUTTON 32

int lastState = HIGH;
int stateLed = LOW;
const char* ssid = "ESGI";
const char* password = "Reseau-GES";
//MQTT
const char* mqtt_server = "test.mosquitto.org";//Adresse IP du Broker Mqtt
const int mqttPort = 1883; //port utilisé par le Broker 


WiFiClient espClient;
PubSubClient client(espClient);
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  setup_wifi();
  setup_mqtt();
}

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
  client.setServer(mqtt_server, mqttPort);
  reconnect();
}

void reconnect(){
  while (!client.connected()) {
    if (!client.connect("ESP32Client")) {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 2s");
    delay(2000);
    }
  }
}

// the loop function runs over and over again forever
void loop() {
  int currentState = digitalRead(PUSH_BUTTON);
  //print out the value of the pushbutton
  
  if (lastState == LOW && currentState == HIGH){
    lastState = HIGH;
    if(stateLed == LOW){
      stateLed = HIGH;
      digitalWrite(LED, stateLed);
    }else{
      stateLed = LOW;
      digitalWrite(LED, stateLed);
    }
    reconnect();
    Serial.print("Publish message: ");
    Serial.println(stateLed);
    char msg_out[20];
    sprintf(msg_out, "%d", stateLed);
    client.publish("dam/presence", msg_out);
}
  lastState = currentState;
  
}
