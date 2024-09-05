#include <M5Core2.h>
#include <TinyGPS++.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

// WiFi設定
const char* ssid = "Judas";  // あなたのWiFiのSSIDに置き換えてください
const char* password = "KaitoENTP";  // あなたのWiFiのパスワードに置き換えてください

// TinyGPS++ オブジェクト
TinyGPSPlus gps;

// HTTPサーバーオブジェクト
AsyncWebServer server(80);

// 現在の位置情報を保持する変数
double currentLat = 0.0;
double currentLng = 0.0;

// smartDelay関数
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (Serial2.available() > 0) gps.encode(Serial2.read());
  } while (millis() - start < ms);
  M5.Lcd.clear();
}

void displayInfo() {
  // 日付と時刻 (UTC) を表示
  M5.Lcd.setCursor(0, 0, 4);
  M5.Lcd.print(F("UTC: "));
  if (gps.date.isValid() && gps.time.isValid()) {
    // 日付
    M5.Lcd.print(gps.date.month());
    M5.Lcd.print(F("/"));
    M5.Lcd.print(gps.date.day());
    M5.Lcd.print(F("/"));
    M5.Lcd.print(gps.date.year());
    // 時刻
    M5.Lcd.print(F(" "));
    if (gps.time.hour() < 10) M5.Lcd.print(F("0"));
    M5.Lcd.print(gps.time.hour());
    M5.Lcd.print(F(":"));
    if (gps.time.minute() < 10) M5.Lcd.print(F("0"));
    M5.Lcd.print(gps.time.minute());
    M5.Lcd.print(F(":"));
    if (gps.time.second() < 10) M5.Lcd.print(F("0"));
    M5.Lcd.print(gps.time.second());
    M5.Lcd.print(F("."));
    if (gps.time.centisecond() < 10) M5.Lcd.print(F("0"));
    M5.Lcd.print(gps.time.centisecond());
  } else {
    M5.Lcd.print(F("INVALID"));
  }
  M5.Lcd.println();

  // 緯度を表示
  M5.Lcd.print(F("LAT: "));
  if (gps.location.isValid()) {
    M5.Lcd.print(gps.location.lat(), 6);
  } else {
    M5.Lcd.print(F("INVALID"));
  }
  M5.Lcd.println();

  // 経度を表示
  M5.Lcd.print(F("LNG: "));
  if (gps.location.isValid()) {
    M5.Lcd.print(gps.location.lng(), 6);
  } else {
    M5.Lcd.print(F("INVALID"));
  }
  M5.Lcd.println();
}

void setup() {
  // M5Core2を初期化
  M5.begin();
  Serial.println("M5Core2 initialized");

  // メインのシリアルポートをシリアルモニター用に初期化
  Serial.begin(115200);
  Serial.println("Serial initialized");

  // GPSモジュール用のシリアルポートを初期化
  Serial2.begin(9600);
  Serial.println("Serial2 initialized");

  // LCD画面を初期化
  M5.Lcd.begin();
  M5.Lcd.setRotation(1);
  M5.Lcd.setTextSize(1);
  Serial.println("LCD initialized");

  // WiFiに接続
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..."); // Print to Serial Monitor instead of LCD
  }
  M5.Lcd.println("WiFi connected"); // Print to Serial Monitor
  M5.Lcd.println("IP address: "); // Print to Serial Monitor
  M5.Lcd.println(WiFi.localIP()); // Print to Serial Monitor

  // GPSモジュールが衛星に接続するのを待つ
  delay(3000);

  // HTTPサーバーを設定
  server.on("/location", HTTP_GET, [](AsyncWebServerRequest *request){
    String response = "{ \"lat\": " + String(currentLat, 6) + ", \"lng\": " + String(currentLng, 6) + " }";
    AsyncResponseStream *responseStream = request->beginResponseStream("application/json");
    responseStream->addHeader("Access-Control-Allow-Origin", "*");  // CORSヘッダーを追加
    responseStream->print(response);
    request->send(responseStream);
  });

  M5.Lcd.println("Server setup done");

  // HTTPサーバーを開始
  server.begin();
  M5.Lcd.println("Server started");
}

unsigned long lastUpdate = 0;
const unsigned long updateInterval = 5000; // Update every 5000ms (5 second)

void loop() {
  while (Serial2.available() > 0) {
    char c = Serial2.read();
    if (gps.encode(c)) {
      unsigned long currentMillis = millis();
      // Check if it's time to update the display
      if (currentMillis - lastUpdate >= updateInterval) {
        lastUpdate = currentMillis;
        displayInfo(); // エンコードされたデータを表示

        // 現在の位置情報を更新
        if (gps.location.isValid()) {
          currentLat = gps.location.lat();
          currentLng = gps.location.lng();
        }
      }
    }
    Serial.print(c); // Print raw data to the Serial Monitor for debugging
  }
}
