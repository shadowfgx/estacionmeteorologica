#include "ThingsBoard.h"
#include "Air_Quality_Sensor.h"
#include "WiFi.h"
#include "ThingsBoard.h"
#include "math.h"
#include "DHT.h"


#define pinSensorAir 34
#define pinSensorLoudness 35
#define pinSensorLight 32
#define pinSensorUV 33
#define pinSensorDHT 25
#define TOKEN               "kPGAcPz5LDGrZbwIMYyp"
#define THINGSBOARD_SERVER  "iot.etsisi.upm.es"
#define SERIAL_DEBUG_BAUD   115200
#define DEBUG false
#define DELAY 10


const char* ssid = "SBC";
const char* password = "sbc$2020";

WiFiClient cliente;
ThingsBoard tb(cliente);
AirQualitySensor sensor1(pinSensorAir);
DHT sensorDHT(pinSensorDHT, DHT22);


void setup() {
  //put your setup code here, to run once:

  
  
  Serial.begin(115200);
  while (!Serial);
  WiFi.begin (ssid, password);
  while (WiFi.status () != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Wifi conectado!");
  
  //Led of sensor Light
  pinMode(pinSensorLight,OUTPUT);

  //DHT
  sensorDHT.begin();
 
}

void loop() {

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }

  Serial.print("Sensor Air Quality value: ");
  readAirQuality();

  Serial.println("-----------------------");
  Serial.println("-----------------------");

  Serial.print("Sensor Loudness value: ");
  readLoudness();

  Serial.println("-----------------------");
  Serial.println("-----------------------");

  Serial.print("Sensor Light value: ");
  readLightSensorData();

  Serial.println("-----------------------");
  Serial.println("-----------------------");

  Serial.print("Sensor UV value: ");
  readUVSensor();

  Serial.println("-----------------------");
  Serial.println("-----------------------");

  Serial.print("Sensor DHT values: ");
  readDHTSensor();
  
  Serial.println("-----------------------");
  Serial.println("-----------------------");

  

}

void readAirQuality() {

  

  int quality = sensor1.slope();

  Serial.println(sensor1.getValue());

  if (quality == AirQualitySensor::FORCE_SIGNAL) {
    Serial.println("High pollution! Force signal active.");
  } else if (quality == AirQualitySensor::HIGH_POLLUTION) {
    Serial.println("High pollution!");
  } else if (quality == AirQualitySensor::LOW_POLLUTION) {
    Serial.println("Low pollution!");
  } else if (quality == AirQualitySensor::FRESH_AIR) {
    Serial.println("Fresh air.");
  }
  Serial.println("Sending data...");
  tb.sendTelemetryInt("AirQuality", quality);
  tb.loop();
  delay(2000);
}


void readLoudness () {
  int loudness;
  int db;
  loudness = analogRead(pinSensorLoudness);

  db = 10 * log10 (10 / 6) + 20 * log10 (loudness);
  Serial.println(db);


  Serial.println("Sending data...");
  tb.sendTelemetryInt("Loudness", db);
  tb.loop();
  delay(2000);
}

void readLightSensorData () {
  int sensorValue;

  sensorValue = analogRead(pinSensorLight);
  Serial.println(sensorValue);
  
  tb.sendTelemetryInt("Light value", sensorValue);
  tb.loop();
  delay(2000);
}

void readUVSensor () {
  int uvRaw;
  uvRaw = analogRead(pinSensorUV);
  Serial.println(uvRaw);

  Serial.println("Sending data...");
  tb.sendTelemetryInt("UV value", uvRaw);
  tb.loop();
  delay(2000);
}

void readDHTSensor() {
  
  float humidity, temperature;
  
  humidity = sensorDHT.readHumidity();
  temperature = sensorDHT.readTemperature();

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("ºC / Humidity: ");
  Serial.print(humidity);
  Serial.print("%");

  Serial.println("Sending data..");
  tb.sendTelemetryInt("Temperature value", temperature);
  tb.sendTelemetryInt("Humidity value", humidity);
  tb.loop();
  delay(2000);
}
