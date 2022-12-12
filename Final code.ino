#include "UbidotsEsp32Mqtt.h"

const char *UBIDOTS_TOKEN = "BBFF-9wQgFJSSKwuBUSN43HATW762XeW6LU";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Project g12";                                        // Put here your Wi-Fi SSID
const char *WIFI_PASS = "555555555";                                 // Put here your Wi-Fi password
const char *DEVICE_LABEL = "58bf259faf74";                           // Put here your Device label to which data  will be published
const char *VARIABLE_LABEL = "butane-gas";                          // Put here your Variable label to which data  will be published
const char *VARIABLE_LABEL2 = "temperature";                        // Put here your Variable label to which data  will be published

const int PUBLISH_FREQUENCY = 5000;  // Update rate in milliseconds

unsigned long timer;
uint8_t analogPin = 34;  // Pin used to read data from GPIO34 ADC_CH6.
int sensorValue;
Ubidots ubidots(UBIDOTS_TOKEN);
#include <OneWire.h>
#include <DallasTemperature.h>
const int oneWireBus = 2;
float temperatureC ;
float temperatureF;
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/****************************************
   Main Functions
 ****************************************/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sensors.begin();

  // ubidots.setDebug(true);  // uncomment this to make debug messages available
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  timer = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!ubidots.connected()) {
    ubidots.reconnect();
  }
  if (millis() + PUBLISH_FREQUENCY > timer) {
    sensorValue = analogRead(39);  // read analog input pin 0
    Serial.print("AirQua=");
    Serial.print(sensorValue, DEC);  // prints the value read
    Serial.println(" PPM");
    delay(100);  // wait 100ms for next reading
    sensors.requestTemperatures();
    temperatureC = sensors.getTempCByIndex(0);
    temperatureF = sensors.getTempFByIndex(0);
    
    Serial.print(temperatureC);
    Serial.println("ºC");
   // Serial.print(temperatureF);
   // Serial.println("ºF");
  }
  ubidots.add(VARIABLE_LABEL, sensorValue);   // Insert your variable Labels and the value to be sent
  ubidots.add(VARIABLE_LABEL2, temperatureC);  // Insert your variable Labels and the value to be sent
  ubidots.publish(DEVICE_LABEL);
  timer = millis();
  ubidots.loop();
}
