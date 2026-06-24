/*
  MazeGame.ino
  Arduino UNO R4 WiFi - 迷路ゲーム（単体）

  ブラウザ側JavaScript(Canvas)の迷路ゲームを配信するWebサーバー。
  ゲーム本体は maze.html（gzip圧縮した maze_gz.h を配信）。
  Arduino は HTML配信とジョイスティック値(/state)の応答だけを担当する。

  操作:
    - PC: WASD / 矢印で移動、Enterで決定、Rでリスタート
    - スマホ: タップ
    - 実機: ジョイスティック（VRx->A0 / VRy->A1 / SW->D2 押し込みで決定）

  ※ ブザー/LEDは使わない（効果音はブラウザのWeb Audioで鳴らす）。
  ※ 迷路ページを編集したら _gzip_maze.py で maze_gz.h を作り直すこと。
*/

#include <WiFiS3.h>
#include "arduino_secrets.h"
#include "maze_gz.h"   // 迷路ページをgzip圧縮したバイト列（MAZE_GZ / MAZE_GZ_LEN）。_gzip_maze.py が maze.html から生成

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
// 外での持ち出しプレイ用: Arduino自身が立てるWi-Fi(アクセスポイント)。配る前提の公開情報。
const char* AP_SSID = "MazeGame";
const char* AP_PASS = "mazegame";   // WPA2のため8文字以上。スマホはこのSSIDにつなぐ
WiFiServer server(80);

// ジョイスティック: VRx->A0  VRy->A1  SW->D2(INPUT_PULLUP)  VCC->5V  GND->GND
const int PIN_VRX = A0;
const int PIN_VRY = A1;
const int PIN_SW  = 2;

void setup(){
  Serial.begin(9600);
  pinMode(PIN_SW, INPUT_PULLUP);

  // --- Wi-Fi: 家ではルーターに接続(STA)、外ではArduino自身がAPになる ---
  // ボタン(ジョイスティックSW)を押しながらリセットすると、待たずに即AP（外での持ち出し用）。
  bool forceAP = (digitalRead(PIN_SW) == LOW);
  if(!forceAP){
    Serial.println("Connecting to home Wi-Fi...");
    unsigned long t0 = millis();
    while(WiFi.begin(ssid, pass) != WL_CONNECTED && millis() - t0 < 12000){
      delay(1000); Serial.print(".");
    }
    Serial.println();
  }
  if(!forceAP && WiFi.status() == WL_CONNECTED){
    while(WiFi.localIP() == IPAddress(0,0,0,0)){ delay(500); }
    Serial.print("[STA] Connected  ->  http://");
    Serial.print(WiFi.localIP()); Serial.println("/");
  } else {
    Serial.println(forceAP ? "[AP] Forced by button" : "[AP] No home Wi-Fi -> Access Point");
    int st = WiFi.beginAP(AP_SSID, AP_PASS);
    if(st != WL_AP_LISTENING){ Serial.println("[AP] start failed"); }
    delay(2000);
    Serial.print("[AP] SSID: ");  Serial.print(AP_SSID);
    Serial.print("  PASS: ");     Serial.println(AP_PASS);
    Serial.print("[AP] join then  ->  http://");
    Serial.print(WiFi.localIP()); Serial.println("/");
  }
  server.begin();
}

// gzip圧縮済みHTMLを配信（ブラウザが自動展開。WiFiS3が詰まらないよう1KBずつ送り切る）
void sendGzipPage(WiFiClient& client, const unsigned char* data, size_t len){
  size_t off = 0; unsigned long t0 = millis();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("Content-Encoding: gzip");
  client.println("Content-Length: " + String((unsigned long)len));
  client.println("Connection: close");
  client.println();
  while(off < len && client.connected()){
    size_t chunk = len - off; if(chunk > 1024) chunk = 1024;
    size_t n = client.write(data + off, chunk);
    if(n > 0){ off += n; t0 = millis(); }
    else { if(millis() - t0 > 5000) break; delay(1); }
  }
  client.flush();
  client.stop();
}

void loop(){
  WiFiClient client = server.available();
  if(client){
    client.setTimeout(40);                       // 読み取り待ちを短く
    String reqLine = client.readStringUntil('\n');
    for(int i=0;i<20;i++){ String h=client.readStringUntil('\n'); if(h.length()<=1) break; }
    int sp1=reqLine.indexOf(' '), sp2=reqLine.indexOf(' ', sp1+1);
    String path = (sp1>=0&&sp2>sp1) ? reqLine.substring(sp1+1, sp2) : "/";

    if(path.startsWith("/state")){
      // ジョイスティック値を JSON で返す（ブラウザが約120msごとに取得）
      int vx=analogRead(PIN_VRX), vy=analogRead(PIN_VRY);
      int b=(digitalRead(PIN_SW)==LOW)?1:0;
      String json="{\"x\":"+String(vx)+",\"y\":"+String(vy)+",\"b\":"+String(b)+"}";
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json");
      client.println("Connection: close");
      client.println();
      client.print(json);
      client.stop();
    }
    else if(path.startsWith("/favicon.ico")){
      // faviconは持たない。空応答(204)で即返す（未知パス扱いでページを返さないため）。
      client.println("HTTP/1.1 204 No Content");
      client.println("Connection: close");
      client.println();
      client.stop();
    }
    else{
      sendGzipPage(client, MAZE_GZ, MAZE_GZ_LEN);   // 迷路ゲーム本体
    }
  }
}
