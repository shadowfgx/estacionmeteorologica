#include "UniversalTelegramBot.h"
#include "Arduino.h"
#include "Air_Quality_Sensor.h"
#include "WiFiClientSecure.h"
#include "ThingsBoard.h"
#include "math.h"
#include "DHT.h"
#include "ArduinoJson.h"
#include "Wire.h"
#include "WiFiClient.h"
#include "WebServer.h"
#include "ESPmDNS.h"
#include "Update.h"

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


const char* host = "esp32";

const char* ssid = "SBC";
const char* password = "sbc$2020";

WiFiClient cliente;
WiFiClientSecure client;
ThingsBoard tb(cliente);
AirQualitySensor sensor1(pinSensorAir);
DHT sensorDHT(pinSensorDHT, DHT22);
UniversalTelegramBot bot(BOT_TOKEN, client);
WebServer server(80);


int botRequestDelay = 1000;
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


// UVSensor Implementacion
float readUV(){
  int s = 1000;
  float v = 5.0;
  float sum = 0;
  for(int i=0; i<s; i++){
    float v = analogRead(pinSensorUV);
    sum = v + sum;
    delay(2);
  }
  float sensor_value_average = sum / s;
  float mv = sensor_value_average * v;

  return mv;
}

float getIndex (float read_mV){
  float voltage = read_mV / 1024.0;
  int uv_index = voltage / 0.1;

  return uv_index;
}


String readUVSensor () {
  
    float uvRaw;
    float uvIndex;
    String message;

    uvRaw = readUV();
    uvIndex = getIndex(uvRaw);
    Serial.println(uvIndex);
    
    message = "UV Index: " + String(uvIndex) + "\n";

    Serial.println("Sending data...");
    tb.sendTelemetryFloat("UV Index", uvIndex);
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
    tb.sendTelemetryFloat("Temperature value", temperature);
    tb.sendTelemetryFloat("Humidity value", humidity);
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


/*
 * Login page
 */
const char* loginIndex = 
 "<form name='loginForm'>"
      "<table width='20%' bgcolor='A09F9F' align='center'>"
          "<tr>"
              "<td colspan=2>"
                  "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                  "<br>"
              "</td>"
              "<br>"
              "<br>"
          "</tr>"
          "<td>Username:</td>"
          "<td><input type='text' size=25 name='userid'><br></td>"
          "</tr>"
          "<br>"
          "<br>"
          "<tr>"
              "<td>Password:</td>"
              "<td><input type='Password' size=25 name='pwd'><br></td>"
              "<br>"
              "<br>"
          "</tr>"
          "<tr>"
              "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
          "</tr>"
      "</table>"
  "</form>"
  "<script>"
      "function check(form)"
      "{"
      "if(form.userid.value=='admin' && form.pwd.value=='admin')"
      "{"
      "window.open('/serverIndex')"
      "}"
      "else"
      "{"
      " alert('Error Password or Username')/*displays error message*/"
      "}"
      "}"
"</script>";
 
/*
 * Server Index Page
 */
 
const char* serverIndex = 
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
   "<div id='prg'>progress: 0%</div>"
   "<script>"
    "$('form').submit(function(e){"
    "e.preventDefault();"
    "var form = $('#upload_form')[0];"
    "var data = new FormData(form);"
    " $.ajax({"
    "url: '/update',"
    "type: 'POST',"
    "data: data,"
    "contentType: false,"
    "processData:false,"
    "xhr: function() {"
    "var xhr = new window.XMLHttpRequest();"
    "xhr.upload.addEventListener('progress', function(evt) {"
    "if (evt.lengthComputable) {"
    "var per = evt.loaded / evt.total;"
    "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
    "}"
    "}, false);"
    "return xhr;"
    "},"
    "success:function(d, s) {"
    "console.log('success!')" 
   "},"
   "error: function (a, b, c) {"
   "}"
   "});"
   "});"
 "</script>";


void setup() {

    Serial.begin(115200);
    WiFi.mode (WIFI_STA);
    
    WiFi.begin (ssid, password);
    
    while (WiFi.status () != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.print("Wifi conectado: ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    /*use mdns for host name resolution*/
    if (!MDNS.begin(host)) { //http://esp32.local
      Serial.println("Error setting up MDNS responder!");
      while (1) {
      delay(1000);
     }
    }
    
    //Led of sensor Light
    pinMode(pinSensorLight,OUTPUT);

    //DHT
    sensorDHT.begin();

    //Led
    pinMode (LED, OUTPUT);

    //GUVA
   

    Serial.println("mDNS responder started");
    /*return index page which is stored in serverIndex */
      server.on("/", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", loginIndex);
      });
      server.on("/serverIndex", HTTP_GET, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/html", serverIndex);
      });
      /*handling uploading firmware file */
      server.on("/update", HTTP_POST, []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
      }, []() {
        HTTPUpload& upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
          Serial.printf("Update: %s\n", upload.filename.c_str());
          if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          /* flashing firmware to ESP*/
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          } else {
            Update.printError(Serial);
          }
        }
      });
      server.begin();
 
}

void loop() {

    server.handleClient();
    delay(1);
  
    if (millis() - lastTimeBotRan > botRequestDelay)  {
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  
      while(numNewMessages) {
        Serial.println("got response");
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
     }
     lastTimeBotRan = millis();
    }

    
    
    if (!tb.connected()) {
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
    
    delay(10000);

    
    
}
