#include "UniversalTelegramBot.h"
#include "Arduino.h"
#include "Air_Quality_Sensor.h"
#include "WiFiClientSecure.h"
#include "ThingsBoard.h"
#include "math.h"
#include "DHT.h"
#include "ArduinoJson.h"
#include "Wire.h"

#define pinSensorAir        34
#define pinSensorLoudness   35
#define pinSensorLight      32
#define pinSensorUV         33
#define pinSensorDHT        25
#define LED                 15


#define TOKEN               "kPGAcPz5LDGrZbwIMYyp"
#define THINGSBOARD_SERVER  "iot.etsisi.upm.es"


#define SERIAL_DEBUG_BAUD   115200
#define DEBUG               false
#define DELAY               10


#define BOT_TOKEN           "1684815494:AAESXBIUxecb7LXcSJ1uV_RxEzVjPV02vuc"
#define CHAT_ID             "512655357"




const char* ssid = "SBC";
const char* password = "sbc$2020";

WiFiClient cliente;
WiFiClientSecure client;
ThingsBoard tb(cliente);
AirQualitySensor sensor1(pinSensorAir);
DHT sensorDHT(pinSensorDHT, DHT22);
UniversalTelegramBot bot(BOT_TOKEN, client);

int botRequestDelay = 2000;
unsigned long lastTimeBotRan;

String readAirQuality() {

  int quality = sensor1.slope();
  String message;
  Serial.println(sensor1.getValue());

  if (quality == AirQualitySensor::FORCE_SIGNAL) {
    Serial.println("High pollution! Force signal active.");
    tb.sendTelemetryString("AirQuality", "High pollution! Force signal active.");
    message = "AirQuality: High pollution! Force signal active \n";
  } else if (quality == AirQualitySensor::HIGH_POLLUTION) {
    Serial.println("High pollution!");
    tb.sendTelemetryString("AirQuality", "High pollution!");
    message= "AirQuality: High pollution!\n";    
  } else if (quality == AirQualitySensor::LOW_POLLUTION) {
    Serial.println("Low pollution!");
    tb.sendTelemetryString("AirQuality", "Low pollution!");
    message= "AirQuality: Low pollution!.\n";

  } else if (quality == AirQualitySensor::FRESH_AIR) {
    Serial.println("Fresh air.");
    tb.sendTelemetryString("AirQuality", "Fresh air");
    message= "AirQuality: Fresh air.\n";

  }
  
  Serial.println("Sending data...");
  

  tb.loop();

  return message;
}


String readLoudness () {
  
    int loudness;
    String message;
    
    loudness = analogRead(pinSensorLoudness);
    Serial.println(loudness);

    message= "Loudness: " + String(loudness) + "\n";
    Serial.println("Sending data...");
    tb.sendTelemetryInt("Loudness level", loudness);
    tb.loop();
    return message;

}

String readLightSensorData () {
    int sensorValue;
    String message;
    
    sensorValue = analogRead(pinSensorLight);
    Serial.println(sensorValue);
    
    message = "Light Value: " + String(sensorValue) + "\n";

    if(sensorValue >= 0 && sensorValue < 700){
      digitalWrite(LED,HIGH);
    }else{
      digitalWrite(LED,LOW);
      }

    Serial.println("Sending data...");
    tb.sendTelemetryInt("Light value", sensorValue);
    tb.loop();
    return message;
}

String readUVSensor () {
    int uvRaw;
    String message;

    uvRaw = analogRead(pinSensorUV);
    Serial.println(uvRaw);

    message = "UV Value: " + String(uvRaw) + "\n";

    Serial.println("Sending data...");
    tb.sendTelemetryInt("UV value", uvRaw);
    tb.loop();
    return message;
}

String readDHTSensor() {
  
    float humidity, temperature;
    String message;
    
    humidity = sensorDHT.readHumidity();
    temperature = sensorDHT.readTemperature();

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("ºC / Humidity: ");
    Serial.print(humidity);
    Serial.print("%\n");

    message = "Temperatura: " + String(temperature) +  "ºC / Humedad: " + String(humidity) + "% " +"\n";

    Serial.println("Sending data..");
    tb.sendTelemetryInt("Temperature value", temperature);
    tb.sendTelemetryInt("Humidity value", humidity);
    tb.loop(); 
    return message;
  }

String getReadings() {

    String message = readDHTSensor() + " \n";
           message += readUVSensor() + "  \n";
           message += readLightSensorData() + "  \n";
           message += readLoudness() + "  \n";
           message += readAirQuality() + "  \n";

 return message;
 
}


void handleNewMessages (int numNewMessages)  {
      
      Serial.println("handleNewMessages");
      Serial.println(String(numNewMessages));

      for(int i=0; i < numNewMessages; i++){
          
          String chat_id = String (bot.messages[i].chat_id);

          if (chat_id != CHAT_ID){
              bot.sendMessage(chat_id, "Unauthorized user", "");
              continue;
          }
        
          // Imprimir el mensaje recibido

          String text = bot.messages[i].text;
          Serial.println(text);

          String from_name = bot.messages[i].from_name;

          if(text == "/Iniciar"){
              
              String welcome = "Bienvenido, " + from_name + ".\n";
              welcome += "Use los siguientes comandos para interactuar con la EstMeteoESP . \n\n";
              welcome += "/readings \n";

              bot.sendMessage(chat_id, welcome, "");
          }

          if(text == "/readings") {        
              String message = getReadings();
              bot.sendMessage(chat_id, message, "");
 
          }
      }

 }



void setup() {

    Serial.begin(115200);
    WiFi.mode (WIFI_STA);
    
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

    //Led
    pinMode (LED, OUTPUT);
 
}

void loop() {
  
    if (millis() > lastTimeBotRan + botRequestDelay)  {
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  
      while(numNewMessages) {
        Serial.println("got response");
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
     }
     lastTimeBotRan = millis();
   
    }

    
    
    if (tb.connected()) {
      // Conectar a ThingsBoard
      Serial.print("Connecting to: ");
      Serial.print(THINGSBOARD_SERVER);
      Serial.print(" with token ");
      Serial.println(TOKEN);
      if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
        Serial.println("Failed to connect");
        return;
      }
    }

    Serial.print("\nSensor Air Quality value: ");
    readAirQuality();
    Serial.println("\n");
    Serial.print("Sensor Loudness value: ");
    readLoudness();
    Serial.println("\n");
    Serial.print("Sensor Light value: ");
    readLightSensorData();
    Serial.println("\n");
    Serial.print("Sensor UV value: ");
    readUVSensor();
    Serial.println("\n");
    Serial.print("Sensor DHT values: ");
    readDHTSensor();
    Serial.println("\n");

    Serial.println("----------------------------------------");
    
    delay(5000);

    
}
