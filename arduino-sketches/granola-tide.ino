/**
 * Granola Tide
 *
 * Control LEDs and an OLED based on current tide information,
 * as provided by NOAA
 */

// Wifi
#include <ESP8266WiFi.h>

// MQTT
#include <PubSubClient.h>

// OLED setup:
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

#define baud_rate 115200

// LEDs
#define led_0 D0
#define led_2 D3
#define led_4 D5
#define led_6 D6
#define led_8 D7
#define led_10 D8

#define wifi_ssid "SSID"
#define wifi_password "PASSWORD"

#define mqtt_server "MQTT HOST"
#define mqtt_port 8883
#define mqtt_topic "mqtt/topic/here"

// Define the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

WiFiClient espClient;
PubSubClient client(espClient);

String payload_str, payload_str_last;

String timestamp1, timestamp2;
float water_level, water_temperature;

void setup() {
  delay(10);
  Serial.begin(baud_rate);
  setup_oled();
  initialize_oled();
  setup_gpio();
  setup_wifi();
  setup_mqtt();
}

void setup_oled() {
  if (! display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void initialize_oled() {
//  display.clearDisplay();

//  // Normal 1:1 pixel scale
//  display.setTextSize(1);
//  // Draw white text
//  display.setTextColor(WHITE);
//  // Start at top-left corner
//  display.setCursor(0,0);
//  display.println(F("Hello, world!"));
//
//  // Draw 'inverse' text
//  display.setTextColor(BLACK, WHITE);
//  display.println(3.141592);
//
//  // Draw 2X-scale text
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.print(F("0x"));
//  display.println(0xDEADBEEF, HEX);

//  for (int i = 0; i < 5; i++) {
//    for (int j = 0; j < 50; j++) {
//      display.clearDisplay();
//      display.setCursor(0, j);
//      display.setTextSize(2);
//      display.setTextColor(WHITE);
//      display.println(F("Loading..."));
//      display.display();
//      delay(300);
//    }
//  }

  display.clearDisplay();

  display.setCursor(0, 25);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println(F("Loading..."));

  display.display();
}

void setup_gpio() {
  pinMode(LED_BUILTIN, OUTPUT);
//  TODO:
//  pinMode(led_red, OUTPUT);
//  pinMode(led_yellow, OUTPUT);
//  pinMode(led_green, OUTPUT);
//  digitalWrite(led_red, LOW);
//  digitalWrite(led_yellow, LOW);
//  digitalWrite(led_green, LOW);
}

void setup_wifi() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

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
  client.setServer(mqtt_server, mqtt_port);
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
  if (strcmp(topic, mqtt_topic) != 0) {
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
      digitalWrite(led_0, value);
      return;
    case 2:
      digitalWrite(led_2, value);
      return;
    case 4:
      digitalWrite(led_4, value);
      return;
    case 6:
      digitalWrite(led_6, value);
      return;
    case 8:
      digitalWrite(led_8, value);
      return;
    case 10:
      digitalWrite(led_10, value);
      return;
    default:
      Serial.print("Unknown led in update_led(): ");
      Serial.println(led);
  }
}

// TODO:
void update_oled() {
  
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
      client.subscribe(mqtt_topic);
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