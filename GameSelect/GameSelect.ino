/*
  GameSelect.ino
  Arduino UNO R4 WiFi - ゲーム選択ランチャー（統合）

  1台で「迷路ゲーム」と「RPG_Quest」の2作を配信する統合スケッチ。
  起動してアクセスすると、まずゲーム選択画面が出る（URLパスで出し分け）:
    /        -> 選択画面（launcher.h）
    /maze    -> 迷路（maze_gz.h … MazeGame由来。_sync.py でコピー）
    /rpg     -> RPG （webpage_gz.h … RPG_Quest由来。_sync.py でコピー）
    /state /fx /led /maps.json -> 共通（迷路は /state のみ使用）

  ※ 各ゲームの本体は MazeGame/ と RPG_Quest/ で開発する。
     ここへは _sync.py で最新の maze_gz.h / webpage_gz.h / maps.h を取り込む。

  操作:
    - PC: WASD / 矢印、Space/Enter　スマホ: タップ　実機: ジョイスティック
*/

#include <WiFiS3.h>
#include <Adafruit_NeoPixel.h>
#include "Arduino_LED_Matrix.h"   // 本体内蔵 12x8 LEDマトリクス
#include <Wire.h>                 // I2C（OLED用）
#include <Adafruit_SSD1306.h>     // OLED SSD1306 128x64（QR表示）
#include <qrcode.h>               // QRコード生成（ricmoo QRCode）
// arduino_secrets.h があれば自分のWiFi情報を使う。無ければダミーで通す＝DLしてすぐ書き込める
// （デモはAP「GameSelect」で動くのでダミーは未使用。家WiFiに繋ぐ時だけ .example をコピーして記入）
#if __has_include("arduino_secrets.h")
  #include "arduino_secrets.h"
#else
  #define SECRET_SSID "your-2.4GHz-wifi"
  #define SECRET_PASS "your-wifi-password"
#endif

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
// 外での持ち出しプレイ用: Arduino自身が立てるWi-Fi(アクセスポイント)。
// 「配るための」公開情報（QRに載せる）なので arduino_secrets.h ではなくここに置く。
const char* AP_SSID = "GameSelect";   // パスワード無し(オープン)で立てる＝R4でも確実に接続でき、キャプティブで自動的にゲームが開く
WiFiServer server(80);
WiFiUDP    dnsUdp;                          // キャプティブポータル用の簡易DNS（AP時のみ）
bool       apMode = false;                  // AP(外でプレイ)モードか
Adafruit_SSD1306 oled(128, 64, &Wire, -1);  // QR表示用OLED（I2C SDA/SCL, アドレス0x3C）

// ★デモモード（1=ON / 0=普段）：周辺機器（ジョイ等）を外し、OLEDとArduinoだけで動かす設定。
//   ・起動と同時にAP(GameSelect)を立てる（家Wi-Fiを待たない＝電源を入れたらすぐ使える）
//   ・ジョイ未接続でも端子フローティングで誤入力しないよう /state は中立(512,512,0)を返す
//   ※ブザー/NeoPixelは未接続でも無害、ミニマップは本体内蔵LEDで表示される。普段に戻すなら 0。
#define PORTABLE_DEMO 1

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
void ledClear(){ ledMapOn=false; ledPlayer=-1; ledFrame[0]=ledFrame[1]=ledFrame[2]=0; renderMatrix(); }  // ゲーム切替/選択画面でミニマップを消す（前のRPGマップを持ち越さない）
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

#include "webpage_gz.h" // RPGのHTMLをgzip圧縮したバイト列（PAGE_GZ / PAGE_GZ_LEN）。_gzip_page.py が webpage.h から生成
#include "maps.h"      // /maps.json で配信するマップデータ（webpage.h から分離）
#include "maze_gz.h"   // 迷路ページをgzip圧縮したバイト列（MAZE_GZ / MAZE_GZ_LEN）。_gzip_maze.py が maze.html から生成
#include "launcher.h"  // 起動時のゲーム選択画面（LAUNCHER_HTML, 生文字列・raw配信）

// ===== OLEDにQRを表示（白地に黒モジュール＋余白）。右に説明2行＋アドレス（折り返し表示） =====
// addr = 画面が自動で開かない時に手入力するアドレス。10文字ごとに折り返して下に表示する。
void showQR(const char* text, const char* l1, const char* l2, const char* addr){
  QRCode qr; uint8_t qrbuf[qrcode_getBufferSize(3)];
  qrcode_initText(&qr, qrbuf, 3, ECC_MEDIUM, text);
  oled.clearDisplay();
  const int scale=2, sz=qr.size*scale, ox=3, oy=(64-sz)/2;
  oled.fillRect(0, 0, ox+sz+2, 64, SSD1306_WHITE);        // 左側を白ブロックに＝QR周囲に余白(クワイエットゾーン)
  for(int y=0;y<qr.size;y++) for(int x=0;x<qr.size;x++)
    if(qrcode_getModule(&qr, x, y))
      oled.fillRect(ox+x*scale, oy+y*scale, scale, scale, SSD1306_BLACK);
  oled.setTextColor(SSD1306_WHITE); oled.setTextSize(1);
  const int tx = ox+sz+5;                 // 文字開始x=66（QRの右・黒地）。1行=最大10文字(約60px)
  oled.setCursor(tx, 2);  oled.print(l1);
  oled.setCursor(tx, 12); oled.print(l2);
  int alen = strlen(addr);                // アドレスを10文字ずつ折り返して表示（長いURL/IPでも収まる）
  for(int i=0, y=30; i<alen; i+=10, y+=10){
    char buf[11]; int k=0;
    for(; k<10 && (i+k)<alen; k++) buf[k]=addr[i+k];
    buf[k]=0;
    oled.setCursor(tx, y); oled.print(buf);
  }
  oled.display();
}

// ===== キャプティブポータル：どのドメイン問い合わせにも 192.168.4.1 を返す簡易DNS =====
void handleDNS(){
  static uint8_t p[512];
  // 溜まっているDNS問い合わせを毎回まとめて処理（取りこぼし防止＝キャプティブ検出の成功率を上げる）
  while(dnsUdp.parsePacket() > 0){
    int len = dnsUdp.read(p, sizeof(p));
    if(len < 12) continue;
    p[2] = 0x81; p[3] = 0x80;          // フラグ: 応答・再帰可
    p[6] = 0; p[7] = 1;                // ANCOUNT=1
    p[8] = p[9] = p[10] = p[11] = 0;   // NSCOUNT/ARCOUNT=0
    int i = 12;                        // 質問(QNAME)の末尾まで進む（ラベル長+1ずつ）
    while(i < len && p[i] != 0){ i += p[i] + 1; }
    i += 1 + 4;                        // 0終端 + QTYPE(2) + QCLASS(2)
    if(i + 16 > (int)sizeof(p)) continue;
    const uint8_t ans[16] = {0xC0,0x0C, 0,1, 0,1, 0,0,0,0x3C, 0,4, 192,168,4,1}; // A/IN/TTL60/192.168.4.1
    for(int k=0;k<16;k++) p[i+k] = ans[k];
    dnsUdp.beginPacket(dnsUdp.remoteIP(), dnsUdp.remotePort());
    dnsUdp.write(p, i + 16);
    dnsUdp.endPacket();
  }
}

void setup(){
  Serial.begin(9600);
  pinMode(PIN_SW, INPUT_PULLUP);
  pinMode(PIN_BUZZ, OUTPUT);
  strip.begin(); strip.show();   // 全消灯
  matrix.begin();                // 内蔵LEDマトリクス開始
  Wire.begin();
  // OLEDのI2Cアドレスは 0x3C が多いが 0x3D の個体もあるので両方試す
  if(oled.begin(SSD1306_SWITCHCAPVCC, 0x3C) || oled.begin(SSD1306_SWITCHCAPVCC, 0x3D)){
    Serial.println("[OLED] ready");
  } else {
    Serial.println("[OLED] not found at 0x3C/0x3D - check SDA/SCL pins(not A4/A5), VCC, GND");
  }
  oled.clearDisplay(); oled.setTextColor(SSD1306_WHITE); oled.setTextSize(1);
  oled.setCursor(0, 28); oled.print("Wi-Fi setting up..."); oled.display();

  // --- Wi-Fi: 家ではルーターに接続(STA)、外ではArduino自身がアクセスポイント(AP)になる ---
  // ボタン(ジョイスティックSW)を押しながらリセットすると、待たずに即AP（外での持ち出し用）。
#if PORTABLE_DEMO
  bool forceAP = true;                          // デモモード：常にAP（家Wi-Fiを待たない）
#else
  bool forceAP = (digitalRead(PIN_SW) == LOW);
#endif
  if(!forceAP){
    Serial.println("Connecting to home Wi-Fi...");
    unsigned long t0 = millis();
    while(WiFi.begin(ssid, pass) != WL_CONNECTED && millis() - t0 < 12000){
      delay(1000); Serial.print(".");
    }
    Serial.println();
  }
  if(!forceAP && WiFi.status() == WL_CONNECTED){
    // 家: ルーター経由（スマホ・PCを同じWi-Fiに）
    while(WiFi.localIP() == IPAddress(0,0,0,0)){ delay(500); }
    Serial.print("[STA] Connected  ->  http://");
    Serial.print(WiFi.localIP()); Serial.println("/");
    showQR((String("http://")+WiFi.localIP().toString()+"/?go").c_str(), "SCAN or", "open:", (String("http://")+WiFi.localIP().toString()).c_str());
  } else {
    // 外: Arduinoが自前のWi-Fi(AP)を立てる。スマホをAP_SSIDにつなぎ http://192.168.4.1/
    Serial.println(forceAP ? "[AP] Forced by button" : "[AP] No home Wi-Fi -> Access Point");
    int st = WiFi.beginAP(AP_SSID);   // パスワード無し(オープン)＝R4でも確実に接続できる
    if(st != WL_AP_LISTENING){ Serial.println("[AP] start failed"); }
    delay(2000);
    Serial.print("[AP] SSID: ");  Serial.print(AP_SSID);  Serial.println("  (open / no password)");
    Serial.print("[AP] join then  ->  http://");
    Serial.print(WiFi.localIP()); Serial.println("/");
    apMode = true;
    dnsUdp.begin(53);                 // 全ドメインを 192.168.4.1 へ＝キャプティブポータル
    Serial.println("[AP] captive DNS on :53");
    showQR((String("WIFI:T:nopass;S:")+AP_SSID+";;").c_str(), "Wi-Fi:", AP_SSID, "http://192.168.4.1/?go");  // OLEDにAP名＋開くアドレス(http付き)を表示
  }
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

// gzip圧縮済みHTMLを配信（ブラウザが自動展開。WiFiS3が詰まらないよう1KBずつ送り切る）
void sendGzipPage(WiFiClient& client, const unsigned char* data, size_t len){
  size_t off = 0; unsigned long t0 = millis();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("Cache-Control: no-store");   // 古い版をキャッシュ表示させない
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

// 生(非圧縮)HTMLを配信（選択画面など小さいページ用）
void sendHtml(WiFiClient& client, const char* data){
  size_t len = strlen(data), off = 0; unsigned long t0 = millis();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("Cache-Control: no-store");   // 選択画面を常に最新で
  client.println("Content-Length: " + String((unsigned long)len));
  client.println("Connection: close");
  client.println();
  while(off < len && client.connected()){
    size_t chunk = len - off; if(chunk > 1024) chunk = 1024;
    size_t n = client.write((const uint8_t*)(data + off), chunk);
    if(n > 0){ off += n; t0 = millis(); }
    else { if(millis() - t0 > 5000) break; delay(1); }
  }
  client.flush();
  client.stop();
}

void loop(){
  if(apMode) handleDNS();   // キャプティブポータルのDNS応答（AP時のみ）
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
    if(reqLine.length()==0){ client.stop(); return; }   // 空接続(プローブ等)は即切断
    // 残りのヘッダを読み飛ばす（最大20行・空行で終了）
    for(int i=0;i<20;i++){
      String h=client.readStringUntil('\n');
      if(h.length()<=1) break;          // "\r" か 空
    }
    // パス抽出: "GET /xxx HTTP/1.1"
    int sp1=reqLine.indexOf(' '), sp2=reqLine.indexOf(' ', sp1+1);
    String path = (sp1>=0&&sp2>sp1) ? reqLine.substring(sp1+1, sp2) : "/";

    if(path.startsWith("/state")){
#if PORTABLE_DEMO
      int vx=512, vy=512, b=0;                  // デモ:ジョイ未接続→中立を返し端子フローティングの誤入力を防ぐ
#else
      int vx=analogRead(PIN_VRX), vy=analogRead(PIN_VRY);
      int b=(digitalRead(PIN_SW)==LOW)?1:0;
#endif
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
    else if(path.startsWith("/favicon.ico")){
      // faviconは持たない。空応答(204)で即返す。
      // これをしないと未知パス扱いで120KBのページを返してしまい、起動時の/maps.json取得を妨げる。
      client.println("HTTP/1.1 204 No Content");
      client.println("Connection: close");
      client.println();
      client.stop();
    }
    else if(path.startsWith("/maps.json")){
      // マップデータ(JSON)を配信。ブラウザは起動時に1回だけ取得する。
      // page と同様、WiFiS3が詰まらないよう【1KBずつ刻んで】送り、Content-Lengthで総量を伝える。
      size_t len = strlen(MAPS_JSON), off = 0; unsigned long t0 = millis();
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: application/json; charset=UTF-8");
      client.println("Content-Length: " + String((unsigned long)len));
      client.println("Connection: close");
      client.println();
      while(off < len && client.connected()){
        size_t chunk = len - off; if(chunk > 1024) chunk = 1024;
        size_t n = client.write((const uint8_t*)(MAPS_JSON + off), chunk);
        if(n > 0){ off += n; t0 = millis(); }
        else { if(millis() - t0 > 5000) break; delay(1); }
      }
      client.flush();
      client.stop();
    }
    else if(path.startsWith("/maze")){
      ledClear();                                   // RPGのミニマップを消してから迷路へ
      sendGzipPage(client, MAZE_GZ, MAZE_GZ_LEN);   // 迷路ゲーム
    }
    else if(path.startsWith("/rpg")){
      sendGzipPage(client, PAGE_GZ, PAGE_GZ_LEN);   // RPG_Quest
    }
    else if(path=="/" || path=="/index.html" || path.startsWith("/?")){
      ledClear();                        // 選択画面ではミニマップを消す
      sendHtml(client, LAUNCHER_HTML);   // ゲーム選択画面（?付きURLでもOK＝キャッシュ回避用）
    }
    else if(apMode){
      // キャプティブポータル：未知URL（OSのネット接続確認など）は選択画面へ302リダイレクト
      // → スマホがAPに繋いだ直後、自動でゲーム選択画面が開く。?go付きで古いキャッシュを確実に回避
      client.println("HTTP/1.1 302 Found");
      client.println("Location: http://192.168.4.1/?go");
      client.println("Cache-Control: no-store");
      client.println("Connection: close");
      client.println();
      client.stop();
    }
    else{
      sendHtml(client, LAUNCHER_HTML);   // STA時の未知URLは選択画面
    }
  }
}
