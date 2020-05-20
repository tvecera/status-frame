/**
 * Dad's status V1.0
 *
 * Copyright (C) 2020 Tomas Vecera tomas@vecera.dev
 *
 * This code is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this code.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <main.h>

FirebaseData fireBaseData;
FirebaseJsonData jsonObj;

void updateLastCheckData() {
  Serial.println(F("Set timestamp..."));
  if (Firebase.setTimestamp(fireBaseData, LASTCHECK_PATH)) {
    Serial.print(F("TIMESTAMP (milliSeconds): "));
    Serial.println(fireBaseData.payload());
    Serial.println("ETag: " + Firebase.getETag(fireBaseData, LASTCHECK_PATH));
    Serial.println(F("------------------------------------"));
    Serial.println();
  } else {
    Serial.println(F("FAILED"));
    Serial.println("REASON: " + fireBaseData.errorReason());
    Serial.println(F("------------------------------------"));
    Serial.println();
  }
}

void getData() {
  Serial.println(F("Get data from Firebase..."));
  if (Firebase.get(fireBaseData, STATUS_PATH)) {
    if (fireBaseData.dataType() == "json") {
      FirebaseJson& json = fireBaseData.jsonObject();
      String jsonStr;
      json.toString(jsonStr, true);
      Serial.println("Payload: " + jsonStr);

      json.get(jsonObj, ACTIVITY_PATH);
      String activity = jsonObj.stringValue;
      json.get(jsonObj, LOCATION_PATH);
      String location = jsonObj.stringValue;

      if (activity.equals("DND")) {
        digitalWrite(LED_3_PIN, HIGH);
        digitalWrite(LED_2_PIN, LOW);
        digitalWrite(LED_1_PIN, LOW);
      } else {
        digitalWrite(LED_3_PIN, LOW);
        digitalWrite(LED_2_PIN, location == "WORK" ? HIGH : LOW);
        digitalWrite(LED_1_PIN, location == "TRANSPORT" ? HIGH : LOW);
      }
    }
  } else {
    Serial.println(F("FAILED"));
    Serial.println("REASON: " + fireBaseData.errorReason());
    Serial.println(F("------------------------------------"));
    Serial.println();
  }
}

void preinit() {
  ESP8266WiFiClass::preinitWiFiOff();
}

void initWiFi() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("ESP-FRAME");
  WiFi.setOutputPower(10);
  WiFi.setSleepMode(WIFI_LIGHT_SLEEP, 255);
  Serial.print(F("Connecting to Wi-Fi..."));
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t wifiStart = millis();
  while ((WiFi.status() != WL_CONNECTED) && (millis() - wifiStart < 10000)) {
    yield();
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print(F("Connected with IP: "));
    Serial.println(WiFi.localIP());
    Serial.println();
  } else {
    Serial.println(F("WiFi timed out and didn't connect"));
  }

  WiFi.setAutoReconnect(true);
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);

  initWiFi();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(false);
  fireBaseData.setBSSLBufferSize(1024, 1024);
  fireBaseData.setResponseSize(1024);
  Firebase.setwriteSizeLimit(fireBaseData, "tiny");
}

void loop() {
  updateLastCheckData();
  getData();
  Serial.flush();
  delay(DATA_CHECK_INTERVAL);
}
