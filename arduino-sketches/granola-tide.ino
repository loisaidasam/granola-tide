/**
 * Granola Tide
 *
 * Control LEDs and an OLED based on current tide information,
 * as provided by NOAA
 */

// For rounding
#include <math.h>

// Wifi
#include <ESP8266WiFi.h>

// MQTT
#include <PubSubClient.h>

// OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display width, in pixels
#define SCREEN_WIDTH 128
// OLED display height, in pixels
#define SCREEN_HEIGHT 64
//#define SCREEN_HEIGHT 32

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// Reset pin # (or -1 if sharing Arduino reset pin)
//#define OLED_RESET     4
// Use builtin for NodeMCU ESP8266
#define OLED_RESET     LED_BUILTIN

// Other OLED connection notes:
// GND -> GND
// VCC -> 3V3
// SCL -> D1
// SDA -> D2

#define BAUD_RATE 115200

// LEDs
#define LED_0 D7
#define LED_2 D6
#define LED_4 D8
#define LED_6 D5
#define LED_8 D3
#define LED_10 D0

#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

#define MQTT_SERVER "MQTT HOST"
#define MQTT_PORT 8883
#define MQTT_TOPIC "mqtt/topic/here"

#define LOCATION "Portland Maine"

WiFiClient espClient;
PubSubClient client(espClient);

// Define the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String payload_str, payload_str_last;

String timestamp1, timestamp2;
float water_level, water_temperature;
int water_temperature_int;

void setup() {
  delay(10);
  Serial.begin(BAUD_RATE);
  setup_oled();
  initialize_oled();
  setup_gpio();
  setup_wifi();
  setup_mqtt();
}

void setup_oled() {
  // "0x3C" is written on back of OLED
  if (! display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    // Don't proceed, loop forever
    for(;;);
  }
}

/**
 * Notes about F() function:
 *
 * http://forum.arduino.cc/index.php?topic=91314.0
 * http://forum.arduino.cc/index.php?topic=91314.msg685761#msg685761
 * 
 * Has something to do with referencing "data" space vs. "code" space
 */
void initialize_oled() {
  display.clearDisplay();

  display.setCursor(0, 25);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(F("Loading..."));

  display.display();
}

void setup_gpio() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(LED_0, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_6, OUTPUT);
  pinMode(LED_8, OUTPUT);
  pinMode(LED_10, OUTPUT);
  
  digitalWrite(LED_0, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_4, LOW);
  digitalWrite(LED_6, LOW);
  digitalWrite(LED_8, LOW);
  digitalWrite(LED_10, LOW);
}

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_mqtt() {
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  payload_str = "";
  for (int i = 0; i < length; i++) {
    payload_str += (char)payload[i];
  }
  Serial.println(payload_str);
  if (strcmp(topic, MQTT_TOPIC) != 0) {
    Serial.print("Unknown topic");
    return;
  }
  if (payload_str == payload_str_last) {
    // Repeat data point
    return;    
  }
  parse_payload(payload_str);
  printCurrentData();
  update_leds();
  update_oled();
  payload_str_last = payload_str;
}

/**
 * Sample payload:
 *
 * "2018-12-14 22:18,1.016,2018-12-14 22:18,40.6"
 */
void parse_payload(String payload) {
  int i = 0;
  String buffer;
  timestamp1 = parse_next_piece(payload, i);
  i += timestamp1.length() + 1;
  buffer = parse_next_piece(payload, i);
  water_level = buffer.toFloat();
  i += buffer.length() + 1;
  timestamp2 = parse_next_piece(payload, i);
  i += timestamp2.length() + 1;
  buffer = parse_next_piece(payload, i);
  water_temperature = buffer.toFloat();
  water_temperature_int = (int) round(water_temperature);
}

String parse_next_piece(String payload, int i) {
  String buffer = "";
  for (; i < payload.length(); i++) {
    if (payload[i] == ',') {
      return buffer;
    }
    buffer += payload[i];
  }
  return buffer;
}

void printCurrentData() {
  Serial.print("timestamp1=");
  Serial.println(timestamp1);
  Serial.print("water_level=");
  Serial.println(water_level);
  Serial.print("timestamp2=");
  Serial.println(timestamp2);
  Serial.print("water_temperature=");
  Serial.println(water_temperature);
}

void update_leds() {
  int max_led = get_max_water_level_led();
  Serial.print("max_led=");
  Serial.println(max_led);
  for (int led = 10; led >= 0; led -= 2) {
    int value = led <= max_led ? HIGH : LOW;
    update_led(led, value);
  }
}

int get_max_water_level_led() {
  int max_led = -1;
  float closest = 100;
  for (int led = 0; led <= 10; led += 2) {
    if (max_led == -1 || abs(led - water_level) < closest) {
      max_led = led;
      closest = abs(led - water_level);
    }
    else {
      break;
    }
  }
  return max_led;
}

void update_led(int led, int value) {
  switch(led) {
    case 0:
      digitalWrite(LED_0, value);
      return;
    case 2:
      digitalWrite(LED_2, value);
      return;
    case 4:
      digitalWrite(LED_4, value);
      return;
    case 6:
      digitalWrite(LED_6, value);
      return;
    case 8:
      digitalWrite(LED_8, value);
      return;
    case 10:
      digitalWrite(LED_10, value);
      return;
    default:
      Serial.print("Unknown led in update_led(): ");
      Serial.println(led);
  }
}

/**
 * Characters at size 1 are 5x8 pixels
 * 
 * https://cdn-learn.adafruit.com/downloads/pdf/adafruit-gfx-graphics-library.pdf
 */
void update_oled() {
  display.clearDisplay();

  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(timestamp1);

  display.setCursor(0, 9);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F(LOCATION));
  
  display.setCursor(0, 25);
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.print(water_temperature_int);
  // char 247 is the degrees symbol
  // via https://forum.arduino.cc/index.php?topic=170670.msg2389431#msg2389431
  display.print((char)247);
  display.println(F("F"));

  display.setCursor(0, 56);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print(F("Tide: "));
  display.print(water_level);
  display.println(F(" ft."));

  display.display();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void reconnect() {
  // Turn the LED on by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  // Turn the LED off by making the voltage HIGH
  digitalWrite(LED_BUILTIN, HIGH);
}