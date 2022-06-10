#include <WiFi.h>
#include <PubSubClient.h> //Librairie pour la gestion Mqtt 

#define BUZZER_PIN  18
#define LED 14
#define PUSH_BUTTON 32

int lastState = HIGH;
int stateLed = LOW;
const char* ssid = "ESGI";
const char* password = "Reseau-GES";
//MQTT
const char* mqtt_server = "test.mosquitto.org";//Adresse IP du Broker Mqtt
const int mqttPort = 1883; //port utilisé par le Broker 
char gateState = '0';


WiFiClient espClient;
PubSubClient client(espClient);
// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT); 
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
  client.setCallback(callback);//Déclaration de la fonction de souscription
  reconnect();
}

void callback(char* topic, byte *payload, unsigned int length) {
   Serial.println("-------Nouveau message de Gwendalino-----");
   Serial.print("Canal:");
   Serial.println(topic);
   Serial.print("donnee:");
   Serial.write(payload, length);
   gateState = (char)payload[0];
 }

void reconnect(){
  while (!client.connected()) {
    if (!client.connect("ESP32Client")) {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 2s");
      delay(1000);
    }
  }
  client.subscribe("iot-projo-gate-angle");
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
    digitalWrite(LED, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(LED, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
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
    Serial.print("Publish message: ");
    Serial.println(stateLed);
    char msg_out[20];
    sprintf(msg_out, "%d", stateLed);
    reconnect();
    client.publish("dam/presence", msg_out);
  }
  blinkLed();
  blinkBuzzerError();
  lastState = currentState;
  reconnect();
  client.loop();
}
