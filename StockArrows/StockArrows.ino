#include <Arduino.h>
#include <ArduinoJson.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

ESP8266WiFiMulti WiFiMulti;

const int GREEN_RELAY = 14; // D5
const int RED_RELAY = 12; // D6
const char* SSID = "";
const char* PASSWD = "";
const String APIKEY = "";

void setup() {
  pinMode(GREEN_RELAY, OUTPUT);
  pinMode(RED_RELAY, OUTPUT); 
  Serial.begin(115200);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWD);
}

void makeGetRequest(){
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    //client->setFingerprint(fingerprint);
    client->setInsecure();
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    // client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    https.useHTTP10(true);
    String getRequestURL = "https://finnhub.io/api/v1/quote?symbol=SPY&token=" + APIKEY;
    if (https.begin(*client, getRequestURL)) {  // HTTPS

      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = https.GET();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          parseResponse(https.getStream());
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
}

void parseResponse(WiFiClient stream){
  // We have a response
  // Parse response
  StaticJsonDocument<192> doc;
  deserializeJson(doc, stream);
  // get percentage change of stock
  float percentChange = doc["dp"];
  if (signbit(percentChange)){
    StockDown();
  }
  else {
    StockUp();
  }
}

void StockUp(){
  // Stock price has gone up
  Serial.printf("Stock price has gone UP\n");
  digitalWrite(GREEN_RELAY, HIGH);
  digitalWrite(RED_RELAY, LOW);
}

void StockDown(){
  // Stock price has gone down
  Serial.printf("Stock price has gone UP\n");
  digitalWrite(GREEN_RELAY, LOW);
  digitalWrite(RED_RELAY, HIGH);
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    makeGetRequest();    
  }
  delay(500);
}                   