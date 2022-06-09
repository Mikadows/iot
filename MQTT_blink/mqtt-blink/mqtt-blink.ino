#include <WiFi.h>
#include <PubSubClient.h> //Librairie pour la gestion Mqtt 

#define MQTT_TOPIC "esgi-eltopico-delarduino-AMG"
//MQTT
const char* mqtt_server = "test.mosquitto.org";//Adresse IP du Broker Mqtt
//const int mqttPort = 1883; //port utilisé par le Broker 
long tps=0;
const char* ssid = "ESGI";
const char* password = "Reseau-GES";

WiFiClient espClient;
PubSubClient client(espClient);

const int ledPin = 18;

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  setup_wifi();
  setup_mqtt();
}
void loop() {
  reconnect();
  client.loop(); 
  //On utilise pas un delay pour ne pas bloquer la réception de messages 
  if (millis()-tps>10000){
     tps=millis();
     float temp = random(30);
   }
}
void setup_wifi(){
  //connexion au wifi
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
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);//Déclaration de la fonction de souscription
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
   if ((char)payload[0] == '1') {
     Serial.println("LED ON");
     digitalWrite(ledPin,HIGH); 
   } else {
     Serial.println("LED OFF");
     digitalWrite(ledPin,LOW); 
   }
 }
void reconnect(){
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      client.subscribe(MQTT_TOPIC);
    }
    else {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 2s");
      delay(2000);
    }
  }
  client.subscribe(MQTT_TOPIC);//souscription au topic led pour commander une led
}
