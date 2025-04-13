#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoLog.h>

const char* ssid = "1N5F-2.4G";
const char* password = "29332502";
const int ledPin = 2;

const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/led";
const char* mqtt_client_id = "esp32_client";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Log.noticeln("Starting WiFi connection...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Log.traceln("Connecting...");
  }
  Log.noticeln("WiFi connected, IP: %s", WiFi.localIP().toString().c_str());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Log.noticeln("Received topic: [%s]", topic);
  
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  if (message == "ON") {
    digitalWrite(ledPin, HIGH);
    Log.noticeln("LED turned ON");
  } else if (message == "OFF") {
    digitalWrite(ledPin, LOW);
    Log.noticeln("LED turned OFF");
  }
}

void reconnect() {
  while (!client.connected()) {
    Log.noticeln("Attempting MQTT connection...");
    if (client.connect(mqtt_client_id)) {
      client.subscribe(mqtt_topic);
      Log.noticeln("Subscribed to: %s", mqtt_topic);
    } else {
      Log.errorln("Failed to connect, state=%d", client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) continue;
  
  Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);
  Log.noticeln("Setup started");
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  Log.noticeln("Setup completed");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(10);
}