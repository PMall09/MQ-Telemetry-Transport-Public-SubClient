#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(172, 16, 0, 100);
IPAddress server(172, 16, 0, 2);
void callback(char* topic, byte* payload, unsigned int length) {
}
EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);
void setup()
{
  Ethernet.begin(mac, ip);
  if (client.connect("arduinoClient", "testuser", "testpass")) {
    client.publish("outTopic","hello world");
    client.subscribe("inTopic");
  }
}

void loop()
{
  client.loop();
}

