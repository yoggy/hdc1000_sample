//
//  mqtt_pub_hdc1000_s9648_100.ino - mqtt publish sample for Arduino Yun
//
#include <YunClient.h>
#include <PubSubClient.h>
#include <Wire.h>

char *server   = "mqtt.example.com";
int  port      = 1883;
char *username = "username";
char *password = "password";
char *topic0    = "topic_hdc1000";
char *topic1    = "topic_s9648";

#define LED_PIN         13
#define S9648_100_PIN   A0

YunClient client;
PubSubClient mqtt_pub_client(server, port, NULL, client);

void setup() {
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
  }
  Serial.println("boot...");

  // for Wire (I2C)
   Wire.begin();

  // for Arduino Yun
  Bridge.begin();

  digitalWrite(LED_PIN, HIGH);

  if (mqtt_pub_client.connect("mqtt_pub_hdc1000_s9648_100", username, password) == false) {
    Serial.println("Failed to connect MQTT broker...");
    delay(3000);
    reboot();
  }

  digitalWrite(LED_PIN, LOW);

  Serial.print("connection start...");
  Serial.print("mqtt_server=");
  Serial.print(server);
  Serial.print(", port=");
  Serial.print(port);
  Serial.println();

  hdc1000_setup();
}

void loop() {
  publish_hdc1000();
  delay(1000);

  publish_s9648_100();
  delay(1000);

  // blink led...
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);

  delay(2000);
}

void publish_hdc1000() {
  // get temperature & humidity from HDC1000
  float t, h;
  hdc1000_get(t, h);

  // build payload
  String payload = "";
  payload += "{\"temperature\":";
  payload += t;
  payload += ", \"humidity\":";
  payload += h;
  payload += ", \"tick_count\":";
  payload += millis();
  payload += "}";
  
  // publish message
  mqtt_pub(topic0, (char*)payload.c_str());
}

void publish_s9648_100() {
  // get analog value
  int val = analogRead(0);
  
  // build payload
  String payload = "";
  payload += "{\"cds\":";
  payload += val;
  payload += ", \"tick_count\":";
  payload += millis();
  payload += "}";
  
  // publish message
  mqtt_pub(topic1, (char*)payload.c_str());
}

void mqtt_pub(char *topic, char *payload){
  Serial.print("pub() : topic=");
  Serial.print(topic);
  Serial.print(", payload=");
  Serial.print(payload);
  Serial.println();

  if (mqtt_pub_client.publish(topic, payload) == false) {
    Serial.print("pub() : error...publish failed...");
    reboot();
  }
  mqtt_pub_client.loop();
}

void reboot() {
  Serial.println("now rebooting...");
  for (int i = 0; i < 10; ++i) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }

  // for Arduino Yun
  Process p;
  p.runShellCommand("/usr/bin/reset-mcu");
  p.runShellCommand("chmod +x /bin/reboot");
  p.runShellCommand("/sbin/reboot");
}



///////////////////////////////////////////////////////
//
// functions for HDC1000
//
#define HDC1000_ADDR 0x40

void hdc1000_setup() {
  delay(100);
  Wire.beginTransmission(HDC1000_ADDR);
  Wire.write(0x20);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(100);
}

void hdc1000_get(float &temperature, float &humidity) {
  unsigned int t = 0, h = 0;

  Wire.beginTransmission(HDC1000_ADDR);
  Wire.write(0x00); // temperature
  Wire.endTransmission();

  delay(100);

  Wire.requestFrom(HDC1000_ADDR, 4);

  delay(10);
  while (Wire.available() < 4) {
    Serial.println(Wire.available());
    delay(10);
  }

  t = Wire.read() << 8;
  t |= Wire.read();

  h = Wire.read() << 8;
  h |= Wire.read();

  temperature = t / 65536.0 * 165.0 - 40.0;
  humidity = h / 65536.0 * 100.0;
}

