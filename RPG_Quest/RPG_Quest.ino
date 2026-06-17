/*
  RPG_Quest.ino
  Arduino UNO R4 WiFi - Web RPG (Phase 16 / 第一スライス)

  目的:
    - フィールドを歩ける（勇者キャラ・カメラ追従）
    - NPCに話しかけると会話ウィンドウ（ドラクエ風）
    - 物語の導入（魔王討伐）

  設計方針（両取り）:
    - ゲームデータは GAME_DATA に分離。後で /api 配信へ差し替えやすくする。
    - 迷路(MiniGame_Phase12_RPG)とは別ファイル。モード統合は組み込みリファクタ時。

  操作:
    - PC: WASD / 矢印で移動、Space または Enter で話す・会話送り
    - スマホ: 移動したい方向をタップ、会話中はタップで送り
*/

#include <WiFiS3.h>
#include <Adafruit_NeoPixel.h>
#include "Arduino_LED_Matrix.h"   // 本体内蔵 12x8 LEDマトリクス
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
WiFiServer server(80);

// ===== ハードウェア =====
// ジョイスティック: VRx->A0  VRy->A1  SW->D2(INPUT_PULLUP)  VCC->5V  GND->GND
const int PIN_VRX = A0;
const int PIN_VRY = A1;
const int PIN_SW  = 2;
// パッシブブザー: 信号->D9, もう片足->ポテンショメータ(中央)->外側->GND（直列で音量調整）
const int PIN_BUZZ = 9;
// WS2812 RGBテープ: DIN->D6, 5V->5V, GND->GND
const int PIN_NEO = 6;
const int NUM_LED = 8;
Adafruit_NeoPixel strip(NUM_LED, PIN_NEO, NEO_GRB + NEO_KHZ800);
unsigned long ledOffAt = 0;   // LEDを消す時刻(非ブロッキング)

// ===== 本体内蔵 12x8 LEDマトリクス：ミニマップ =====
// ブラウザが現在マップを12x8に縮小し「町/出入口」フレーム(96bit=3xuint32)＋「自分の位置(0..95)」を /led で送る。
// 自分の位置の点滅はArduino側で自走（非ブロッキング）。
ArduinoLEDMatrix matrix;
uint32_t ledFrame[3] = {0,0,0};   // 町/出入口マーカー（点灯）
int ledPlayer = -1;               // 自分の位置 0..95（点滅）。-1=非表示
bool ledBlink = true;
unsigned long ledBlinkAt = 0;
bool ledMapOn = false;
void renderMatrix(){
  uint32_t f[3] = { ledFrame[0], ledFrame[1], ledFrame[2] };
  if(ledPlayer>=0 && ledBlink){ int w=ledPlayer>>5; f[w] |= (1UL << (31-(ledPlayer&31))); }
  matrix.loadFrame(f);
}
// クエリから符号なし32bit整数を取り出す（uint32はlongに収まらないので strtoul）
uint32_t qU32(const String& p, const char* k){
  int i=p.indexOf(k); if(i<0) return 0; i+=strlen(k);
  return (uint32_t) strtoul(p.c_str()+i, nullptr, 10);
}

// LEDを指定色で光らせ、一定時間後に消す予約
void ledFlash(uint8_t r, uint8_t g, uint8_t b, int ms){
  for(int i=0;i<NUM_LED;i++) strip.setPixelColor(i, strip.Color(r,g,b));
  strip.show();
  ledOffAt = millis() + ms;
}
// 非ブロッキングのメロディ再生（delayを使わずloopで進める）
int seqF[8]; int seqT[8]; int seqN=0, seqI=0; unsigned long seqStart=0;
void playSeq(const int* f, const int* t, int n){
  seqN=n; seqI=0; seqStart=millis();
  for(int i=0;i<n && i<8;i++){ seqF[i]=f[i]; seqT[i]=t[i]; }
}
void serviceSeq(){
  if(seqI<seqN && millis()-seqStart >= (unsigned long)seqT[seqI]){
    tone(PIN_BUZZ, seqF[seqI], 110); seqI++;
  }
}
// 効果音＋LED（ブラウザの /fx?s=... から呼ばれる）。delayは使わない＝サーバーを止めない
void playFx(const String& s){
  if(s=="coin"){      tone(PIN_BUZZ,1320,70);  ledFlash(255,200,0,180); }
  else if(s=="hit"){  tone(PIN_BUZZ,160,110);  ledFlash(255,40,40,200); }
  else if(s=="heal"){ tone(PIN_BUZZ,880,90);   ledFlash(40,255,90,200); }
  else if(s=="enc"){  tone(PIN_BUZZ,300,120);  ledFlash(160,80,255,200); }
  else if(s=="win"){  tone(PIN_BUZZ,784,120);  ledFlash(80,160,255,250); }
  else if(s=="level"){
    static const int f[]={523,659,784,1047}; static const int t[]={0,110,220,330};
    playSeq(f,t,4); ledFlash(255,220,0,500);
  }
}

#include "webpage.h"

void setup(){
  Serial.begin(9600);
  pinMode(PIN_SW, INPUT_PULLUP);
  pinMode(PIN_BUZZ, OUTPUT);
  strip.begin(); strip.show();   // 全消灯
  matrix.begin();                // 内蔵LEDマトリクス開始
  while(WiFi.begin(ssid,pass)!=WL_CONNECTED){ delay(2000); Serial.println("Connecting..."); }
  Serial.println("Connected");
  while(WiFi.localIP()==IPAddress(0,0,0,0)){ delay(500); Serial.println("Waiting for IP..."); }
  Serial.println(WiFi.localIP());
  server.begin();
}

// 小さなレスポンスを返して接続を閉じる
void sendShort(WiFiClient& client, const String& body){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
  client.print(body);
  client.stop();
}

void loop(){
  serviceSeq();   // メロディを非ブロッキングで進める
  // LEDの自動消灯（非ブロッキング）
  if(ledOffAt && millis()>ledOffAt){ strip.clear(); strip.show(); ledOffAt=0; }
  // ミニマップの自分の位置を点滅（非ブロッキング）
  if(ledMapOn && millis()-ledBlinkAt>350){ ledBlinkAt=millis(); ledBlink=!ledBlink; renderMatrix(); }

  WiFiClient client=server.available();
  if(client){
    client.setTimeout(40);              // 読み取り待ちを短く（既定1秒だと詰まる）
    // リクエスト1行目だけ読んでパスを取得
    String reqLine = client.readStringUntil('\n');
    // 残りのヘッダを読み飛ばす（最大20行・空行で終了）
    for(int i=0;i<20;i++){
      String h=client.readStringUntil('\n');
      if(h.length()<=1) break;          // "\r" か 空
    }
    // パス抽出: "GET /xxx HTTP/1.1"
    int sp1=reqLine.indexOf(' '), sp2=reqLine.indexOf(' ', sp1+1);
    String path = (sp1>=0&&sp2>sp1) ? reqLine.substring(sp1+1, sp2) : "/";

    if(path.startsWith("/state")){
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
    else if(path.startsWith("/fx")){
      int q=path.indexOf("s=");
      String s = q>=0 ? path.substring(q+2) : "";
      playFx(s);
      sendShort(client, "ok");
    }
    else if(path.startsWith("/led")){
      ledFrame[0]=qU32(path,"a="); ledFrame[1]=qU32(path,"b="); ledFrame[2]=qU32(path,"c=");
      ledPlayer=(int)qU32(path,"p="); ledMapOn=true; ledBlink=true; renderMatrix();
      sendShort(client, "ok");
    }
    else{
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html; charset=UTF-8");
      client.println("Connection: close");
      client.println();
      client.print(page);
      client.stop();
    }
  }
}
