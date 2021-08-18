#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const char* ssid = "........";
const char* password = "........";
const char* mqtt_server = "broker.mqtt-dashboard.com";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  String topicStr(topic);
  int bottleCount = 0; 
  if (topicStr.indexOf('/') >= 0) {
    topicStr.remove(0, topicStr.indexOf('/')+1);
    bottleCount = topicStr.toInt();
  }

  if (bottleCount > 0) {
    int msgLen = 0;
    for (int i = bottleCount; i > 0; i--) {
      String numBottles(i);
      msgLen += 2*numBottles.length();
      if (i == 1) {
        msgLen += 2*String(" green bottle, standing on the wall\n").length();
      } else {
        msgLen += 2*String(" green bottles, standing on the wall\n").length();
      }
      msgLen += String("And if one green bottle should accidentally fall\nThere'll be ").length();
      switch (i) {
      case 1:
        msgLen += String("no green bottles, standing on the wall\n\n").length();
        break;
      case 2:
        msgLen += String("1 green bottle, standing on the wall\n\n").length();
        break;
      default:
        numBottles = i-1;
        msgLen += numBottles.length();
        msgLen += String(" green bottles, standing on the wall\n\n").length();
        break;
      };}
    client.beginPublish("greenBottles/lyrics", msgLen, false);
    for (int i = bottleCount; i > 0; i--) {
      for (int j = 0; j < 2; j++) {
        client.print(i);
        if (i == 1) {
          client.print(" green bottle, standing on the wall\n");
        } else {
          client.print(" green bottles, standing on the wall\n");
        }
      }
      client.print("And if one green bottle should accidentally fall\nThere'll be ");
      switch (i) {
      case 1:
        client.print("no green bottles, standing on the wall\n\n");
        break;
      case 2:
        client.print("1 green bottle, standing on the wall\n\n");
        break;
      default:
        client.print(i-1);
        client.print(" green bottles, standing on the wall\n\n");
        break;
      };
    }
    client.endPublish();
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("outTopic", "hello world");
      client.subscribe("greenBottles/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);    
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

