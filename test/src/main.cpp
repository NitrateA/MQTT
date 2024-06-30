#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define BUZZER_PIN 12

const char* ssid = "Sude Ceren";
const char* password = "mxNuaFXR";
const char* mqtt_server = "2bbc0289811c4a149b59c3b08e8023cf.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "Sergen"; 
const char* mqtt_password = "Z3usyenildi!"; 
const char* topic = "test/topic"; // Ensure this matches with your Android app
int incoming;
int treshold = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
WiFiClientSecure espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.begin(115200);
  Serial.println("Connecting to WiFi..");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(topic); // Subscribe to the correct topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishData(const char* topic, const String& message){
  if (client.connected()) {
    client.publish(topic, message.c_str());
    Serial.print("Message published. Topic: ");
    Serial.print(topic);
    Serial.print(", Message: ");
    Serial.println(message);
  } else {
    reconnect(); // Reconnect if not connected before trying to publish
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  char msg[length + 1];
  
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0'; 
  Serial.println(msg);
  if(msg[0] == 'D'){
    msg[0] = ' ';
    for(int a; a < length; a++){
      msg[a] = msg[a + 1];
    }
    lcd.clear();
    lcd.setCursor(0, 0); // Ensure the cursor is set to the start of the LCD
    length--;

    if(length > 16){
      for(int i = 0; i < length && msg[i] != '\0';i++){
        if(i > 15){
          lcd.setCursor(i - 16, 1);
        }
        else{
          lcd.setCursor(i, 0);
        }
        lcd.print(msg[i]);
      }
    }
    else{
      lcd.print(msg);
    }
     
    digitalWrite(BUZZER_PIN,HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN,LOW);

  }  
}

void setup() {
  Serial.begin(115200); // Initialize serial communication
  setup_wifi();
  espClient.setInsecure(); // Only for testing without CA certificate validation
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); // Set the callback function here

  pinMode(BUZZER_PIN, OUTPUT);
  lcd.begin();                
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Mesaj Yok");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Ensure this is called frequently to handle incoming messages
  //digitalWrite(BUZZER_PIN,HIGH);

}