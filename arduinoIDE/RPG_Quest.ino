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

// LEDを指定色で光らせ、一定時間後に消す予約
void ledFlash(uint8_t r, uint8_t g, uint8_t b, int ms){
  for(int i=0;i<NUM_LED;i++) strip.setPixelColor(i, strip.Color(r,g,b));
  strip.show();
  ledOffAt = millis() + ms;
}
// 効果音＋LED（ブラウザの /fx?s=... から呼ばれる）
void playFx(const String& s){
  if(s=="coin"){      tone(PIN_BUZZ,1320,70);  ledFlash(255,200,0,180); }
  else if(s=="hit"){  tone(PIN_BUZZ,160,110);  ledFlash(255,40,40,200); }
  else if(s=="heal"){ tone(PIN_BUZZ,880,90);   ledFlash(40,255,90,200); }
  else if(s=="enc"){  tone(PIN_BUZZ,300,120);  ledFlash(160,80,255,200); }
  else if(s=="win"){  tone(PIN_BUZZ,784,120);  ledFlash(80,160,255,250); }
  else if(s=="level"){            // レベルアップ：上昇アルペジオ＋金色
    tone(PIN_BUZZ,523,90); delay(100);
    tone(PIN_BUZZ,659,90); delay(100);
    tone(PIN_BUZZ,784,90); delay(100);
    tone(PIN_BUZZ,1047,160);
    ledFlash(255,220,0,500);
  }
}

const char page[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Quest</title>
<style>
body{margin:0;padding:14px 0;background:radial-gradient(circle at 50% 0%,#243a5e,#0c1424 70%);
     color:#d8e0f0;text-align:center;font-family:'Segoe UI','Hiragino Kaku Gothic ProN',sans-serif;}
h2{letter-spacing:6px;margin:4px 0 12px;font-weight:700;color:#ffe9a8;text-shadow:0 0 10px #b58a2a,0 2px 4px #000;}
.frame{display:inline-block;padding:8px;border-radius:10px;
       background:linear-gradient(145deg,#caa24a,#6e5018);box-shadow:0 10px 40px rgba(0,0,0,.7);}
canvas{display:block;border-radius:4px;background:#2a3a2a;touch-action:none;max-width:94vw;
       box-shadow:inset 0 0 0 2px #0a0a12;}
#hint{color:#6f6890;font-size:13px;margin-top:12px;}
</style>
</head>
<body>
<h2>― Q U E S T ―</h2>
<div class="frame"><canvas id="game" width="800" height="800"></canvas></div>
<div id="hint">WASD / 矢印で移動　Space / Enter で話す　スマホはタップ</div>

<script>
const canvas=document.getElementById("game");
const ctx=canvas.getContext("2d");
const TILE=80, VIEW=800;
const CELL=16, SCALE=TILE/CELL;        // タイル用：16px原画を5倍
const CHAR=4;                          // キャラ/敵用：24px原画を4倍(=96px)でタイルとバランス
ctx.imageSmoothingEnabled=false;       // アンチエイリアス無効＝カクカクのドット感

// ===== ドット絵データ（_sprites.py で検証済み。キャラは16x24・人間型） =====
const HERO_DOWN=["........................",".....y............y.....",".....yy..........yy.....","...XyyiiiiI..Iiiiiyyaa..","...XiIIIIIIiiIIIIIIiaa..","..yyyyyyyyyyyyyyyyyyaa..","...iiibbssssssssbbiiaa..","...iiirrssssssssrriiaa..","...iiitsssssssssstiiaa..","..oouuuuuuuUUuuuuuuuaa..",".occoyyyyyyyyyyyyyyyaa..",".occoUUUUUUUUUUUUUUuaa..",".oCcoUUUUUUUUUUUUUUuaa..",".ocCoyyyyyyyyyyyyyyyaa..",".occouuuuuUUUUuuuuuyyyy.",".occouuuub....buuuu.AA..","..ooXuuuub....buuuuXAA..","....yyyyyy....yyyyyy....","....Xkkkkb....bkkkkX....","....XkkkbX....XbkkkX....",".....XXXX......XXXX.....","........................","........................","........................"];
const HERO_DOWN_A=["........................",".....y............y.....",".....yy..........yy.....","...XyyiiiiI..Iiiiiyyaa..","...XiIIIIIIiiIIIIIIiaa..","..yyyyyyyyyyyyyyyyyyaa..","...iiibbssssssssbbiiaa..","...iiirrssssssssrriiaa..","...iiitsssssssssstiiaa..","..oouuuuuuuUUuuuuuuuaa..",".occoyyyyyyyyyyyyyyyaa..",".occoUUUUUUUUUUUUUUuaa..",".oCcoUUUUUUUUUUUUUUuaa..",".ocCoyyyyyyyyyyyyyyyaa..",".occouuuuuUUUUuuuuuyyyy.",".occyyyyyy....buuuu.AA..","..ooXkkkkb....buuuuXAA..","....XkkkbX....yyyyyy....",".....XXXX.....bkkkkX....","..............XbkkkX....","...............XXXX.....","........................","........................","........................"];
const HERO_DOWN_B=["........................",".....y............y.....",".....yy..........yy.....","...XyyiiiiI..Iiiiiyyaa..","...XiIIIIIIiiIIIIIIiaa..","..yyyyyyyyyyyyyyyyyyaa..","...iiibbssssssssbbiiaa..","...iiirrssssssssrriiaa..","...iiitsssssssssstiiaa..","..oouuuuuuuUUuuuuuuuaa..",".occoyyyyyyyyyyyyyyyaa..",".occoUUUUUUUUUUUUUUuaa..",".oCcoUUUUUUUUUUUUUUuaa..",".ocCoyyyyyyyyyyyyyyyaa..",".occouuuuuUUUUuuuuuyyyy.",".occouuuub....yyyyyyAA..","..ooXuuuub....bkkkkXAA..","....yyyyyy....XbkkkX....","....Xkkkkb.....XXXX.....","....XkkkbX..............",".....XXXX...............","........................","........................","........................"];
const HERO_UP=["........................",".....y............y.....",".....yy..........yy.....","...XyyiiiiI..Iiiiiyyaa..","...XiIIIIIIiiIIIIIIiaa..","..yyyyyyyyyyyyyyyyyyaa..","...iiiIIIIIIIIIIIIiiaa..","...iiiIIIIIIIIIIIIiiaa..","...iiiIIIIIIIIIIIIiiaa..","..oouuuuuuuUUuuuuuuuaa..",".occoyyyyyyyyyyyyyyyaa..",".occoUUUUUUUUUUUUUUuaa..",".oCcoUUUUUUUUUUUUUUuaa..",".ocCoyyyyyyyyyyyyyyyaa..",".occouuuuuUUUUuuuuuyyyy.",".occouuuub....buuuu.AA..","..ooXuuuub....buuuuXAA..","....yyyyyy....yyyyyy....","....Xkkkkb....bkkkkX....","....XkkkbX....XbkkkX....",".....XXXX......XXXX.....","........................","........................","........................"];
const HERO_UP_A=["........................",".....y............y.....",".....yy..........yy.....","...XyyiiiiI..Iiiiiyyaa..","...XiIIIIIIiiIIIIIIiaa..","..yyyyyyyyyyyyyyyyyyaa..","...iiiIIIIIIIIIIIIiiaa..","...iiiIIIIIIIIIIIIiiaa..","...iiiIIIIIIIIIIIIiiaa..","..oouuuuuuuUUuuuuuuuaa..",".occoyyyyyyyyyyyyyyyaa..",".occoUUUUUUUUUUUUUUuaa..",".oCcoUUUUUUUUUUUUUUuaa..",".ocCoyyyyyyyyyyyyyyyaa..",".occouuuuuUUUUuuuuuyyyy.",".occyyyyyy....buuuu.AA..","..ooXkkkkb....buuuuXAA..","....XkkkbX....yyyyyy....",".....XXXX.....bkkkkX....","..............XbkkkX....","...............XXXX.....","........................","........................","........................"];
const HERO_UP_B=["........................",".....y............y.....",".....yy..........yy.....","...XyyiiiiI..Iiiiiyyaa..","...XiIIIIIIiiIIIIIIiaa..","..yyyyyyyyyyyyyyyyyyaa..","...iiiIIIIIIIIIIIIiiaa..","...iiiIIIIIIIIIIIIiiaa..","...iiiIIIIIIIIIIIIiiaa..","..oouuuuuuuUUuuuuuuuaa..",".occoyyyyyyyyyyyyyyyaa..",".occoUUUUUUUUUUUUUUuaa..",".oCcoUUUUUUUUUUUUUUuaa..",".ocCoyyyyyyyyyyyyyyyaa..",".occouuuuuUUUUuuuuuyyyy.",".occouuuub....yyyyyyAA..","..ooXuuuub....bkkkkXAA..","....yyyyyy....XbkkkX....","....Xkkkkb.....XXXX.....","....XkkkbX..............",".....XXXX...............","........................","........................","........................"];
const HERO_LEFT=["........................","...........Xyy..........","..........Xiyy..........","....XiiiiiiyyX..........","...XiIIIIIIIiX..........","..yyyyyyyyyyy...........","..XrssssiiiiX...........","..XssssssiiiX...........","..XtsssssiiiX...........",".oouuuuuuuuuX...........","occoyyyyyyyy............","occoUUUUUUUUaa..........","oCcoUUUUUUUUaa..........","occoyyyyyyyyaa..........",".ooUuuuuuuyyyy..........",".XXuuuuuuuAAX...........","...yyyyyyyy.............","...Xkkkbkkkb............","...Xkkkbkkkb............","...XXXXXXXXX............","........................","........................","........................","........................"];
const HERO_LEFT_A=["........................","...........Xyy..........","..........Xiyy..........","....XiiiiiiyyX..........","...XiIIIIIIIiX..........","..yyyyyyyyyyy...........","..XrssssiiiiX...........","..XssssssiiiX...........","..XtsssssiiiX...........",".oouuuuuuuuuX...........","occoyyyyyyyy............","occoUUUUUUUUaa..........","oCcoUUUUUUUUaa..........","occoyyyyyyyyaa..........",".ooUuuuuuuyyyy..........",".XXuuuuuuuAAX...........","...Xkkkyyyy.............","...XXXXbkkkb............",".......bkkkb............",".......XXXXX............","........................","........................","........................","........................"];
const HERO_LEFT_B=["........................","...........Xyy..........","..........Xiyy..........","....XiiiiiiyyX..........","...XiIIIIIIIiX..........","..yyyyyyyyyyy...........","..XrssssiiiiX...........","..XssssssiiiX...........","..XtsssssiiiX...........",".oouuuuuuuuuX...........","occoyyyyyyyy............","occoUUUUUUUUaa..........","oCcoUUUUUUUUaa..........","occoyyyyyyyyaa..........",".ooUuuuuuuyyyy..........",".XXuuuuuuuAAX...........","...yyyybkkkb............","...XkkkXXXXX............","...Xkkk.................","...XXXX.................","........................","........................","........................","........................"];
const VILLAGER=["........................","........................","....HHHHHHHHHHHHHHHH....","...HHHHHHHHHHHHHHHHHH...","...HssssssssssssssssH...","...HssssssssssssssssH...","...HsseesssssssseessH...","...HssssssssssssssssH...","...HtsssssssssssssstH...","....ssssssssssssssss....","....qppppppppppppppq....","...qppPPPPPPPPPPPPppq...","..qpppPPPPPPPPPPPPpppq..","..qpppPPPPPPPPPPPPpppq..","..qpppPPPPPPPPPPPPpppq..","..qppppPPPPPPPPPPppppq..",".qppppppPPPPPPPPppppppq.",".qppppppppPPPPppppppppq.",".qppppppppPPPPppppppppq.",".qppppppppPPPPppppppppq.",".qppppppppPPPPppppppppq.",".XppppppppXPPXppppppppX.","..XXmmmmXX....XXmmmmXX..","........................"];
const ELDER=["........................",".....WWW........WWW.....","....WWWWWW....WWWWWW....","...WWWWWWWWWWWWWWWWWW...","...WWssssssssssssssWW...","...WsseesssssssseessW...","...WssssssssssssssssW...","...WWWWWWWWWWWWWWWWWW...","..WWWWWWWWWWWWWWWWWWWW..","..WWWWWWWWWWWWWWWWWWWW..","..WWWWWWWWWWWWWWWWWWWW..","...qWWWWWWooooWWWWWWq...","..qppppppssoossppppppq..","..qpppppppPkkPpppppppq..",".qpppppppPPkkPPpppppppq.",".qpppppppPPkkPPpppppppq.",".qpppppppPPkkPPpppppppq.",".qpppppppPPkkPPpppppppq.",".qpppppppPPkkPPpppppppq.",".qpppppppPPkkPPpppppppq.",".qpppppppPPkkPPpppppppq.",".XppppppppXkkXppppppppX.","..XXmmmmXX.KK.XXmmmmXX..","........................"];
const ELDER_PAL={'.':null,X:'#1a1422',W:'#eef0f4',s:'#e8c89a',t:'#c8a070',e:'#3a2a2a',p:'#6b5a8c',P:'#8a78ac',q:'#4a3c66',o:'#c9a13a',k:'#6b4a2a',K:'#4a3018',m:'#3a2a1a'};
const TREE=["......XXXX......",".....XkkkkX.....","....XkllllkX....","...XkllLLllkX...","...XklLLLllkX...","..XkllllLlllkX..","..XkllLllllllX..","..XklllllLllkX..","...XkllllllkX...","...XkllLllkX....","....XkllkX......",".....XmMX.......","......mMX.......",".....XmMmX......","....XmmMmmX.....",".....XXXXX......"];
const ROCK=["................","................","................",".....XXXX.......","...XXddddXX.....","..XdddRRdddX....",".XddRRRRRRddX...",".XdRRRRrrRRdX...",".XdRRrrrrrRdX...",".XddRrrrrrRdX...","..XddRrrrddX....","...XXddddXX.....",".....XXXX.......","................","................","................"];
const HOUSE=[".......XX.......","......XEEX......",".....XEEEEX.....","....XEEEEEEX....","...XEEEEEEEEX...","..XEEEEEEEEEEX..",".XEEEEEEEEEEEEX.",".XooooooooooooX.",".XoWWoooooWWooX.",".XoWWoooooWWooX.",".XoooooDDoooooX.",".XoooooDDoooooX.",".XoooooDDoooooX.",".XoooooDDoooooX.",".XXXXXXXXXXXXXX.","................"];
const DEADTREE=[".......X........","......XmX.......",".....XmmX.......","...XmmMmX.......","...XmMmmmX......","....XmMmXmX.....",".....XmMmmX.....","......XmMX......",".....XmMmX......","....XmMmX.......","......mMX.......","......mMX.......",".....XmMX.......","....XmmMmX......","...XmmmMmmX.....","....XXXXXX......"];

// ===== 敵スプライト（戦闘用・16x16） =====
const SLIME=["........................","..........XXXX..........","........XXLLLLXX........",".......XLLLLLLLLX.......","......XLLLLLLLLLLX......",".....XllllllllllllX.....",".....XllllllllllllX.....","....XllllllllllllllX....","....XbbbbbbbbbbbbbbX....","...XbbbbbbbbbbbbbbbbX...","...XbbwwbbbbbbbbwwbbX...","...XbbwebbbbbbbbewbbX...","...XbbeebbbbbbbbeebbX...","...XbbbbbbbbbbbbbbbbX...","...XbbbbbbbbbbbbbbbbX...","...XbbbmbbbbbbbbmbbbX...","....XbbbmmmmmmmmbbbX....","....XddddddddddddddX....",".....XddddddddddddX.....",".....XddddddddddddX.....","......XXddddddddXX......","........XXddddXX........","..........XXXX..........","........................"];
const BAT=["........................","........................","..........X..X..........",".........XPXXPX.........",".....XX..XPPPPX..XX.....","...XXppX.XPPPPX.XppXX...","..XppppX.rPPPPr.XppppX..",".XppppX.PrPPPPrP.XppppX.","XppdppX.PPPPPPPP.XppdppX","XpdpdpX.PwPPPPwP.XpdpdpX","XppppppXPPPPPPPPXppppppX",".XpppppXPPPPPPPPXpppppX.","..XpppXPPPPPPPPPPXpppX..","...XXXPPPPP..PPPPPXXX...",".....XPPPP....PPPPX.....",".....XPPP......PPPX.....","......XPP......PPX......","......XPX......XPX......",".....XX.X......X.XX.....","........................","........................","........................","........................","........................"];
const BONE=["........................","......yXy......yXy......","....XiiiiiiIIiiiiiiX....","....XIIiiiiIIiiiiIIX....","....yyyyyyyyyyyyyyyyaa..","....XWWWWWWWWWWWWWWXaa..","....XWWWWWWWWWWWWWWXaa..","....XWddrrWWWWrrddWXaa..","....XWddrrWWWWrrddWXaa..","....XWWWddWWWWddWWWXaa..",".....XWWWWWWWWWWWWX.aa..",".....XWtttWWWWtttWX.aa..",".oo...XWtWWWWWWtWX..aa..","ossoWWWWWWWWWWWWWWWWaa..","oSsoWXddWWdWWdWWddXyyyy.","ossoWXWddWWWWWWddWXWAA..","ossoWXWddWWWWWWddWXWAA..","ossoWXWWWWWWWWWWWWXWX...",".oo.XWWXWWWWWWWWXWWX....",".....XWWX......XWWX.....",".....XWWX......XWWX.....",".....XWWX......XWWX.....","....XXWWXX....XXWWXX....","........................"];
const SLIME_PAL={'.':null,X:'#0a1428',L:'#bfe0ff',l:'#5fa8f8',b:'#2f7fe0',d:'#1f53b0',w:'#ffffff',e:'#0e2244',m:'#163a78'};
const BAT_PAL={'.':null,X:'#160a26',p:'#6a3aa0',P:'#8a5ac0',d:'#3f2068',r:'#ff4632',w:'#ffffff'};
const BONE_PAL={'.':null,X:'#10121a',i:'#4a5470',I:'#6a7490',y:'#d9b24a',W:'#e8ecf2',w:'#9298a4',d:'#2a2e38',r:'#ff4632',t:'#c8ccd4',a:'#d2d7df',A:'#7a818c',s:'#7a4a22',S:'#a87a3a',o:'#3a2410'};

// ===== パレット（暗めの世界観） =====
const HERO_PAL={'.':null,X:'#16182a',i:'#3a5a9e',I:'#6f9fe0',y:'#f0cc3a',s:'#f0c290',t:'#c89060',r:'#46d8ff',u:'#2e57c8',U:'#4f7ce8',b:'#1c356e',k:'#22407a',c:'#c0c6d2',C:'#8a93a6',o:'#d9b24a',a:'#e6ebf5',A:'#8a93a6'};
const TREE_PAL={X:'#184018',k:'#2f7a30',l:'#3da53d',L:'#6fd06f',m:'#6b3b1a',M:'#8a5a2a'};
const ROCK_PAL={X:'#10131a',d:'#3a3f47',R:'#6a7078',r:'#878d96'};
const HOUSE_PAL={X:'#0f0c08',E:'#7a2a2a',o:'#8a6a40',W:'#d8b850',D:'#241608'};
const DEAD_PAL={X:'#0d0b08',m:'#2e2010',M:'#48341c'};
function shadeHex(hex,f){
  const n=parseInt(hex.slice(1),16); const r=(n>>16)&255,g=(n>>8)&255,b=n&255;
  const c=v=>('0'+Math.min(255,Math.max(0,Math.round(v*f))).toString(16)).slice(-2);
  return '#'+c(r)+c(g)+c(b);
}
function npcPal(color){ return {'.':null,X:'#1a1422',s:'#f0c290',t:'#c89060',e:'#3a2a2a',H:'#5a4632',m:'#3a2a1a',p:color,P:shadeHex(color,1.28),q:shadeHex(color,0.66)}; }

// ===== スプライト生成（原画サイズから自動でキャンバスを作る） =====
function makeSprite(rows,pal,sc){
  sc=sc||SCALE;
  const w=rows[0].length, h=rows.length;
  const c=document.createElement('canvas'); c.width=w*sc; c.height=h*sc;
  const g=c.getContext('2d');
  for(let y=0;y<h;y++)for(let x=0;x<rows[y].length;x++){
    const col=pal[rows[y][x]]; if(!col) continue;
    g.fillStyle=col; g.fillRect(x*sc,y*sc,sc,sc);
  }
  return c;
}
function mirror(src){
  const c=document.createElement('canvas'); c.width=src.width; c.height=src.height;
  const g=c.getContext('2d'); g.translate(src.width,0); g.scale(-1,1); g.drawImage(src,0,0); return c;
}
function makeGrass(seed){
  const c=document.createElement('canvas'); c.width=TILE; c.height=TILE; const g=c.getContext('2d');
  g.fillStyle="#3aa83a"; g.fillRect(0,0,TILE,TILE);   // 明るいクラシックな草地
  for(let i=0;i<26;i++){
    const cx=(i*97+seed*4137)%CELL, cy=(i*53+seed*911)%CELL;
    g.fillStyle=(i%3===0)?"#4cc24c":"#2e8f2e";
    g.fillRect(cx*SCALE,cy*SCALE,SCALE,SCALE);
  }
  return c;
}
function makeWater(frame){
  const c=document.createElement('canvas'); c.width=TILE; c.height=TILE; const g=c.getContext('2d');
  g.fillStyle="#2f6fd8"; g.fillRect(0,0,TILE,TILE);   // 明るい水
  g.fillStyle="#7ab0f0";
  for(let i=1;i<CELL;i+=4){ const off=(frame*2+i)%CELL; g.fillRect(off*SCALE,i*SCALE,3*SCALE,SCALE); }
  g.fillStyle="#1f54b0";
  for(let i=3;i<CELL;i+=4){ const off=(CELL-frame*2+i)%CELL; g.fillRect(off*SCALE,i*SCALE,2*SCALE,SCALE); }
  return c;
}

const SPR={};
function buildSprites(){
  // 勇者：方向ごとに [立ち, 歩行A, 歩行B] の3コマ（CHARスケール）
  const hp=HERO_PAL;
  SPR.heroDown =[makeSprite(HERO_DOWN,hp,CHAR), makeSprite(HERO_DOWN_A,hp,CHAR), makeSprite(HERO_DOWN_B,hp,CHAR)];
  SPR.heroUp   =[makeSprite(HERO_UP,hp,CHAR),   makeSprite(HERO_UP_A,hp,CHAR),   makeSprite(HERO_UP_B,hp,CHAR)];
  SPR.heroLeft =[makeSprite(HERO_LEFT,hp,CHAR), makeSprite(HERO_LEFT_A,hp,CHAR), makeSprite(HERO_LEFT_B,hp,CHAR)];
  SPR.heroRight=SPR.heroLeft.map(mirror);
  SPR.tree=makeSprite(TREE,TREE_PAL);
  SPR.rock=makeSprite(ROCK,ROCK_PAL);
  SPR.house=makeSprite(HOUSE,HOUSE_PAL);
  SPR.dead=makeSprite(DEADTREE,DEAD_PAL);
  SPR.grass=[makeGrass(0),makeGrass(1)];
  SPR.water=[makeWater(0),makeWater(1)];
  SPR.slime=makeSprite(SLIME,SLIME_PAL,CHAR);
  SPR.bat=makeSprite(BAT,BAT_PAL,CHAR);
  SPR.bone=makeSprite(BONE,BONE_PAL,CHAR);
}

// ====== ゲームデータ（後でAPI配信へ差し替えやすいよう分離） ======
const GAME_DATA={
  // 記号: . 草地 / T 木 / W 水 / R 岩 / H 家 / P 開始位置
  // フィールド（魔物が出る）。G=街の入口
  field:{
    safe:false,
    exit:{ to:"town", tx:9, ty:11 },
    map:[
      "TTTTTTTTTTTTTTTTTTTT",
      "T..................T",
      "T..TT....RR........T",
      "T..TT...HHHH...WWW.T",
      "T.......HHHH...WWW.T",
      "T.....P..G.........T",
      "T..................T",
      "T...........TT.....T",
      "T...........TT.....T",
      "T...........TT.....T",
      "T..RR..............T",
      "T..RR........WWWW..T",
      "T............WWWW..T",
      "T.....TT...........T",
      "T.....TT...........T",
      "T..................T",
      "T....R.......TT....T",
      "T............TT....T",
      "T..................T",
      "TTTTTTTTTTTTTTTTTTTT"],
    npcs:[]
  },
  // 街（魔物が出ない・安全）。G=フィールドへの出口。建物は buildings で配置
  town:{
    safe:true,
    exit:{ to:"field", tx:10, ty:6 },
    map:[
      "TTTTTTTTTTTTTTTTTT",
      "T................T",
      "T................T",
      "T................T",
      "T................T",
      "T................T",
      "T................T",
      "T................T",
      "T................T",
      "T................T",
      "T................T",
      "T................T",
      "TTTTTTTTGGTTTTTTTT",
      "TTTTTTTTTTTTTTTTTT"],
    buildings:[
      { type:"inn",    x:2,  y:2, name:"やどや",   color:"#c06a3a" },
      { type:"weapon", x:7,  y:2, name:"ぶきや",   color:"#8a8f9a" },
      { type:"item",   x:12, y:2, name:"どうぐや", color:"#5a9a5a" },
      { type:"church", x:7,  y:8, name:"きょうかい", color:"#d8d2c0" }
    ],
    npcs:[
      { x:4, y:7, name:"長老", color:"#7a5e8c", kind:"elder",
        lines:[
          "おお、勇者よ！　よくぞ来た。",
          "魔王が　復活してしまったのじゃ。",
          "宿屋で 休み、武器を 整え、",
          "教会で 祈りを 捧げてから 旅立つがよい。"
        ] },
      { x:12, y:7, name:"町の人", color:"#5a7a6a",
        lines:[
          "東の洞窟には　強い魔物が出るらしい。",
          "宿屋で セーブして おくといいよ。"
        ] }
    ]
  },
  // 敵（戦闘）。spr はスプライトキー
  enemies:[
    { name:"ブルン",     spr:"slime", maxhp:8,  atk:7,  def:3, exp:3,  gold:2 },
    { name:"ヤミバット", spr:"bat",   maxhp:13, atk:10, def:4, exp:6,  gold:5 },
    { name:"ホネナイト", spr:"bone",  maxhp:20, atk:15, def:7, exp:13, gold:11 }
  ]
};

// ====== 商品・装備データ ======
const SHOP_WEAPON=[
  {name:"どうのつるぎ", atk:6,  price:80},
  {name:"はがねのけん", atk:14, price:350},
  {name:"ほのおのけん", atk:24, price:1200}
];
const SHOP_ARMOR=[
  {name:"かわのよろい", def:5,  price:70},
  {name:"てつのよろい", def:12, price:300},
  {name:"はがねのよろい", def:20, price:1000}
];
const SHOP_ITEM=[
  {name:"やくそう",   key:"herb",   price:8},
  {name:"エリクサー", key:"elixir", price:500}
];
const INN_COST=10;

// ====== 状態 ======
let mode="field";          // "field" | "talk" | "battle" | "status" | "service"
let darkArea=false;
let currentMap="field";
let buildings=[];
const TILE_WALL={ "T":1, "W":1, "R":1, "H":1, "d":1 };  // 通れないタイル
let map=[], npcs=[];
let player={x:0,y:0,size:52,dir:"down",step:0};
let cameraX=0, cameraY=0;
let respawn={ map:"town", tx:9, ty:11 };   // 教会で更新するリスポーン地点

// 会話状態
let dialog={active:false, lines:[], idx:0, name:"", shown:0, t:0};

// プレイヤー能力値・装備
let stats={ lv:1, hp:22, maxhp:22, mp:10, maxmp:10, atk:12, def:7, exp:0, gold:0,
            herb:3, elixir:0, weapon:null, armor:null };
function atkTotal(){ return stats.atk + (stats.weapon? stats.weapon.atk:0); }
function defTotal(){ return stats.def + (stats.armor? stats.armor.def:0); }

// 戦闘状態
let battle=null;
let encCooldown=0;
const BATTLE_CMDS=["たたかう","じゅもん","どうぐ","にげる"];
const SPELLS=[{name:"かえん",mp:3},{name:"いやし",mp:4},{name:"もどる",mp:0}];

// 施設（宿屋/武器屋/道具屋/教会）
let service=null;

let keys={};

// ====== 初期化 ======
function loadMap(name, sx, sy){
  currentMap=name;
  const m=GAME_DATA[name];
  map=m.map;
  buildings=(m.buildings||[]);
  darkArea=!!m.dark;
  npcs=(m.npcs||[]).map(n=>({...n}));
  npcs.forEach(n=>{
    n.spr = n.kind==="elder" ? makeSprite(ELDER, ELDER_PAL, CHAR)
                             : makeSprite(VILLAGER, npcPal(n.color), CHAR);
  });
  if(sx!==undefined){
    player.x=sx*TILE+(TILE-player.size)/2; player.y=sy*TILE+(TILE-player.size)/2;
  }else{
    for(let y=0;y<map.length;y++)for(let x=0;x<map[y].length;x++)
      if(map[y][x]==="P"){ player.x=x*TILE+(TILE-player.size)/2; player.y=y*TILE+(TILE-player.size)/2; }
  }
  mode="field";
}
function loadField(){ loadMap("field"); }

// 建物の足元当たり判定（ドア以外は壁）。ドア=(x+1, y+1)
function buildingWall(tx,ty){
  for(const b of buildings){
    if(tx>=b.x && tx<b.x+3 && ty>=b.y && ty<b.y+2){
      if(tx===b.x+1 && ty===b.y+1) return false;  // ドアは通れる
      return true;
    }
  }
  return false;
}
function doorAt(tx,ty){
  for(const b of buildings) if(tx===b.x+1 && ty===b.y+1) return b;
  return null;
}

function isWall(px,py){
  const s=player.size;
  const pts=[[px,py],[px+s-1,py],[px,py+s-1],[px+s-1,py+s-1]];
  for(const [x,y] of pts){
    const tx=Math.floor(x/TILE), ty=Math.floor(y/TILE);
    if(ty<0||tx<0||ty>=map.length||tx>=map[0].length) return true;
    if(TILE_WALL[map[ty][tx]]) return true;
    if(buildingWall(tx,ty)) return true;
  }
  // NPCにも当たる
  for(const n of npcs){
    const nx=n.x*TILE+8, ny=n.y*TILE+8, ns=TILE-16;
    if(px<nx+ns && px+s>nx && py<ny+ns && py+s>ny) return true;
  }
  return false;
}

function onGrass(){
  const tx=Math.floor((player.x+player.size/2)/TILE), ty=Math.floor((player.y+player.size/2)/TILE);
  if(ty<0||tx<0||ty>=map.length||tx>=map[0].length) return false;
  const c=map[ty][tx];
  return c==="."||c==="P";   // 草地（床）のみ
}

function move(dx,dy){
  if(dx<0)player.dir="left"; else if(dx>0)player.dir="right";
  else if(dy<0)player.dir="up"; else if(dy>0)player.dir="down";
  let moved=false;
  if(dx && !isWall(player.x+dx,player.y)){ player.x+=dx; moved=true; }
  if(dy && !isWall(player.x,player.y+dy)){ player.y+=dy; moved=true; }
  if(moved) player.step++;
  else player.walk=0;
  if(moved) player.walk=1;
}

// プレイヤーの正面のタイルにいるNPCを探す
function npcInFront(){
  const cx=player.x+player.size/2, cy=player.y+player.size/2;
  let fx=cx, fy=cy;
  const d=TILE*0.8;
  if(player.dir==="left")fx-=d; if(player.dir==="right")fx+=d;
  if(player.dir==="up")fy-=d; if(player.dir==="down")fy+=d;
  for(const n of npcs){
    const nx=n.x*TILE+TILE/2, ny=n.y*TILE+TILE/2;
    if(Math.hypot(fx-nx,fy-ny) < TILE) return n;
  }
  return null;
}

function talkTo(n){
  dialog.active=true; dialog.lines=n.lines; dialog.idx=0;
  dialog.name=n.name; dialog.shown=0; dialog.t=0;
  mode="talk";
}
function advanceDialog(){
  const full=dialog.lines[dialog.idx];
  if(dialog.shown<full.length){ dialog.shown=full.length; return; } // 一気に表示
  dialog.idx++;
  if(dialog.idx>=dialog.lines.length){ dialog.active=false; mode="field"; }
  else { dialog.shown=0; dialog.t=0; }
}

// ====== 戦闘 ======
function calcDmg(atk,def){ const base=atk-def/2; return Math.max(1, Math.floor(base*(0.85+Math.random()*0.4))); }
function needExp(lv){ return lv*lv*4 + lv*6; }
function pickEnemy(){ const r=Math.random(); const i=r<0.6?0:r<0.9?1:2; return GAME_DATA.enemies[i]; }

function startBattle(){
  const e=pickEnemy();
  battle={ enemy:{...e, hp:e.maxhp}, state:"msg", msg:[], msgIdx:0, shown:0,
           after:null, cmd:0, spell:0, shake:0, shakeP:0, flash:0, rects:null };
  mode="battle"; fx("enc");
  queueMsg([e.name+"が あらわれた！"], enterCommand);
}
function queueMsg(lines, after){
  battle.msg=Array.isArray(lines)?lines:[lines];
  battle.msgIdx=0; battle.shown=0; battle.state="msg"; battle.after=after||null;
}
function advanceBattleMsg(){
  const b=battle, full=b.msg[b.msgIdx]||"";
  if(b.shown<full.length){ b.shown=full.length; return; }
  b.msgIdx++;
  if(b.msgIdx>=b.msg.length){ const cb=b.after; b.after=null; if(cb)cb(); }
  else b.shown=0;
}
function enterCommand(){ battle.state="command"; battle.cmd=0; }
function confirmCommand(){
  const b=battle;
  if(b.cmd===0) playerAttack();
  else if(b.cmd===1){ b.state="spell"; b.spell=0; }
  else if(b.cmd===2) useHerb();
  else tryFlee();
}
function afterPlayerAction(){ if(battle.enemy.hp<=0) winBattle(); else enemyTurn(); }
function playerAttack(){
  const d=calcDmg(atkTotal(), battle.enemy.def);
  battle.enemy.hp-=d; battle.shake=12; fx("hit");
  queueMsg(["勇者の こうげき！", battle.enemy.name+"に "+d+"の ダメージ！"], afterPlayerAction);
}
function confirmSpell(){
  const b=battle, s=SPELLS[b.spell];
  if(s.name==="もどる"){ enterCommand(); return; }
  if(stats.mp<s.mp){ queueMsg(["MPが たりない！"], enterCommand); return; }
  stats.mp-=s.mp;
  if(s.name==="かえん"){
    const d=calcDmg(atkTotal()+8+stats.lv*2, battle.enemy.def);
    battle.enemy.hp-=d; battle.flash=10; fx("hit");
    queueMsg(["勇者は かえんを となえた！", battle.enemy.name+"に "+d+"の ダメージ！"], afterPlayerAction);
  }else{ // いやし
    const h=18+Math.floor(Math.random()*8); stats.hp=Math.min(stats.maxhp, stats.hp+h); fx("heal");
    queueMsg(["勇者は いやしを となえた！","HPが "+h+" かいふくした！"], enemyTurn);
  }
}
function useHerb(){
  if(stats.herb<=0){ queueMsg(["やくそうが ない！"], enterCommand); return; }
  stats.herb--; const h=20+Math.floor(Math.random()*8); stats.hp=Math.min(stats.maxhp, stats.hp+h); fx("heal");
  queueMsg(["やくそうを つかった！","HPが "+h+" かいふくした！"], enemyTurn);
}
function tryFlee(){
  if(Math.random()<0.6) queueMsg(["勇者は にげだした！"], endBattle);
  else queueMsg(["まわりこまれて しまった！"], enemyTurn);
}
function enemyTurn(){
  const d=calcDmg(battle.enemy.atk, defTotal());
  stats.hp-=d; battle.shakeP=12; fx("hit");
  queueMsg([battle.enemy.name+"の こうげき！","勇者は "+d+"の ダメージ！"],
    ()=>{ if(stats.hp<=0) loseBattle(); else enterCommand(); });
}
function gainExp(){
  const msgs=[];
  while(stats.exp>=needExp(stats.lv)){
    stats.exp-=needExp(stats.lv); stats.lv++;
    stats.maxhp+=6; stats.maxmp+=3; stats.atk+=2; stats.def+=1;
    stats.hp=stats.maxhp; stats.mp=stats.maxmp; fx("level");
    msgs.push("レベルが あがった！　Lv"+stats.lv+"！");
  }
  return msgs;
}
function winBattle(){
  const e=battle.enemy; fx("win");
  stats.exp+=e.exp; stats.gold+=e.gold;
  const lines=[e.name+"を たおした！","けいけんち "+e.exp+"を かくとく！", e.gold+"ゴールド てにいれた！"];
  lines.push(...gainExp());
  queueMsg(lines, endBattle);
}
function loseBattle(){
  queueMsg(["勇者は ちからつきた…","きょうかいで めをさました…"], ()=>{
    stats.hp=stats.maxhp; stats.mp=stats.maxmp;
    battle=null; encCooldown=120;
    loadMap(respawn.map, respawn.tx, respawn.ty);   // 教会(リスポーン地点)で復活
  });
}
function endBattle(){ mode="field"; battle=null; encCooldown=100; }

// ====== 施設（宿屋/武器屋/道具屋/教会）＆セーブ ======
function openService(b){
  mode="service";
  service={ type:b.type, name:b.name, b:b, phase:"menu", labels:[], actions:[],
            cursor:0, msgs:[], msgIdx:0, shown:0, after:null, rects:[] };
  if(b.type==="inn")        svcMenu(["とまる ("+INN_COST+"G)","やめる"], [innStay, closeService]);
  else if(b.type==="church")svcMenu(["いのる","やめる"], [churchPray, closeService]);
  else if(b.type==="weapon")openWeaponShop();
  else if(b.type==="item")  openItemShop();
}
function svcMenu(labels, actions){ service.phase="menu"; service.labels=labels; service.actions=actions; service.cursor=0; }
function svcMsg(lines, after){ service.phase="msg"; service.msgs=lines; service.msgIdx=0; service.shown=0; service.after=after||null; }
function advanceSvc(){
  const s=service, full=s.msgs[s.msgIdx]||"";
  if(s.shown<full.length){ s.shown=full.length; return; }
  s.msgIdx++;
  if(s.msgIdx>=s.msgs.length){ const cb=s.after; s.after=null; if(cb)cb(); } else s.shown=0;
}
function confirmSvc(){ const s=service; if(s.phase==="menu"){ const a=s.actions[s.cursor]; if(a)a(); } else advanceSvc(); }
function closeService(){
  const b=service.b;
  player.x=(b.x+1)*TILE+(TILE-player.size)/2; player.y=(b.y+2)*TILE+(TILE-player.size)/2; // ドアの前へ
  mode="field"; service=null;
}
function innStay(){
  if(stats.gold<INN_COST){ svcMsg(["おかねが たりないようだ。"], ()=>openService(service.b)); return; }
  stats.gold-=INN_COST; stats.hp=stats.maxhp; stats.mp=stats.maxmp; saveGame();
  svcMsg(["ゆっくり おやすみなさい…","HPとMPが かいふくし、","ぼうけんを セーブした！"], closeService);
}
function churchPray(){
  stats.hp=stats.maxhp; stats.mp=stats.maxmp;
  respawn={ map:currentMap, tx:service.b.x+1, ty:service.b.y+2 };
  saveGame();
  svcMsg(["女神に いのりを ささげた。","ここが ふっかつの ちと なった。","HP・MPが かいふくした。"], closeService);
}
function openWeaponShop(){
  const list=SHOP_WEAPON.concat(SHOP_ARMOR).slice().sort((a,b)=>a.price-b.price);
  const labels=list.map(it=> it.name+" "+it.price+"G"); labels.push("やめる");
  const actions=list.map(it=>()=>buyEquip(it)); actions.push(closeService);
  svcMenu(labels, actions);
}
function buyEquip(it){
  if(stats.gold<it.price){ svcMsg(["おかねが たりない！"], openWeaponShop); return; }
  stats.gold-=it.price;
  if(it.atk!==undefined) stats.weapon=it; else stats.armor=it;
  svcMsg([it.name+"を そうびした！"], openWeaponShop);
}
function openItemShop(){
  const list=SHOP_ITEM.slice().sort((a,b)=>a.price-b.price);
  const labels=list.map(it=> it.name+" "+it.price+"G"); labels.push("やめる");
  const actions=list.map(it=>()=>buyItem(it)); actions.push(closeService);
  svcMenu(labels, actions);
}
function buyItem(it){
  if(stats.gold<it.price){ svcMsg(["おかねが たりない！"], openItemShop); return; }
  stats.gold-=it.price; stats[it.key]=(stats[it.key]||0)+1;
  svcMsg([it.name+"を かった！"], openItemShop);
}
function saveGame(){ try{ localStorage.setItem("rpgQuestSave", JSON.stringify({stats, respawn})); }catch(e){} }
function loadGame(){
  try{ const s=JSON.parse(localStorage.getItem("rpgQuestSave"));
    if(s&&s.stats){ Object.assign(stats, s.stats); if(s.respawn) respawn=s.respawn; return true; }
  }catch(e){} return false;
}

function updateBattle(){
  const b=battle; if(!b) return;
  if(b.shake>0)b.shake--; if(b.shakeP>0)b.shakeP--; if(b.flash>0)b.flash--;
  if(b.state==="msg"){
    const full=b.msg[b.msgIdx]||"";
    if(b.shown<full.length) b.shown++;
  }
}

// ====== 入力 ======
document.addEventListener("keydown",(e)=>{
  const k=e.key.toLowerCase();
  keys[k]=true;
  if(e.key==="ArrowUp")keys["w"]=true; if(e.key==="ArrowDown")keys["s"]=true;
  if(e.key==="ArrowLeft")keys["a"]=true; if(e.key==="ArrowRight")keys["d"]=true;
  if(["arrowup","arrowdown","arrowleft","arrowright"," "].includes(k)) e.preventDefault();
  if(mode==="battle"){
    const b=battle; if(!b) return;
    if(b.state==="msg"){ if(k===" "||e.key==="Enter") advanceBattleMsg(); }
    else if(b.state==="command"){
      if(k==="w"||e.key==="ArrowUp") b.cmd=(b.cmd+3)%4;
      if(k==="s"||e.key==="ArrowDown") b.cmd=(b.cmd+1)%4;
      if(k===" "||e.key==="Enter") confirmCommand();
    }else if(b.state==="spell"){
      if(k==="w"||e.key==="ArrowUp") b.spell=(b.spell+2)%3;
      if(k==="s"||e.key==="ArrowDown") b.spell=(b.spell+1)%3;
      if(k===" "||e.key==="Enter") confirmSpell();
    }
    return;
  }
  if(mode==="service"){
    const s=service; if(!s) return;
    if(s.phase==="menu"){
      const n=s.labels.length;
      if(k==="w"||e.key==="ArrowUp")    s.cursor=Math.max(0, s.cursor-2);
      if(k==="s"||e.key==="ArrowDown")  s.cursor=Math.min(n-1, s.cursor+2);
      if(k==="a"||e.key==="ArrowLeft")  s.cursor=Math.max(0, s.cursor-1);
      if(k==="d"||e.key==="ArrowRight") s.cursor=Math.min(n-1, s.cursor+1);
      if(k===" "||e.key==="Enter") confirmSvc();
    }else{ if(k===" "||e.key==="Enter") advanceSvc(); }
    return;
  }
  if(mode==="status"){
    if(k==="m"||k==="escape"||k==="i"||k===" "||e.key==="Enter") mode="field";
    return;
  }
  if(mode==="field" && (k==="m"||k==="i")){ mode="status"; return; }
  if(k===" "||e.key==="Enter"){
    if(mode==="field"){ const n=npcInFront(); if(n) talkTo(n); }
    else if(mode==="talk"){ advanceDialog(); }
  }
});
document.addEventListener("keyup",(e)=>{
  const k=e.key.toLowerCase(); keys[k]=false;
  if(e.key==="ArrowUp")keys["w"]=false; if(e.key==="ArrowDown")keys["s"]=false;
  if(e.key==="ArrowLeft")keys["a"]=false; if(e.key==="ArrowRight")keys["d"]=false;
});

function canvasPos(e){
  const r=canvas.getBoundingClientRect();
  return { x:(e.clientX-r.left)*(canvas.width/r.width), y:(e.clientY-r.top)*(canvas.height/r.height) };
}
let tapDir=null;
canvas.addEventListener("pointerdown",(e)=>{
  if(mode==="talk"){ advanceDialog(); return; }
  if(mode==="battle"){
    const b=battle; if(!b) return; const p=canvasPos(e);
    if(b.state==="msg"){ advanceBattleMsg(); return; }
    if(b.rects){ for(const r of b.rects){
      if(p.x>=r.x&&p.x<=r.x+r.w&&p.y>=r.y&&p.y<=r.y+r.h){
        if(b.state==="command"){ b.cmd=r.i; confirmCommand(); }
        else { b.spell=r.i; confirmSpell(); }
        return;
      } } }
    return;
  }
  if(mode==="service"){
    const s=service; if(!s) return; const p=canvasPos(e);
    if(s.phase==="msg"){ advanceSvc(); return; }
    if(s.rects){ for(const r of s.rects){ if(p.x>=r.x&&p.x<=r.x+r.w&&p.y>=r.y&&p.y<=r.y+r.h){ s.cursor=r.i; confirmSvc(); return; } } }
    return;
  }
  if(mode==="status"){ mode="field"; return; }
  const p=canvasPos(e);
  if(p.x>VIEW-72 && p.y<72){ mode="status"; return; }   // 右上メニューボタン
  // 画面中央(プレイヤー)から見たタップ方向へ動く
  const dx=p.x-VIEW/2, dy=p.y-VIEW/2;
  if(Math.abs(dx)>Math.abs(dy)) tapDir=dx<0?"a":"d"; else tapDir=dy<0?"w":"s";
});
canvas.addEventListener("pointerup",()=>{ tapDir=null; });
canvas.addEventListener("pointerleave",()=>{ tapDir=null; });

// ====== ハードウェア効果音（Arduinoのブザー＋WS2812 LED） ======
function fx(s){ try{ fetch("/fx?s="+s,{cache:"no-store"}); }catch(e){} }

// ====== ジョイスティック（/state をポーリング） ======
let joyState={x:512,y:512,b:0}, joyNav=true, joyBtn=true;
async function pollJoy(){
  try{ const r=await fetch("/state",{cache:"no-store"}); if(!r.ok) return; applyJoy(await r.json()); }catch(e){}
}
function applyJoy(j){
  joyState=j;
  if(j.x===0&&j.y===0) return;                 // 未接続(値0)は無視
  const left=j.x<350,right=j.x>700,up=j.y<350,down=j.y>700, btn=j.b===1;
  if(mode!=="field"){                          // メニュー類：傾けたエッジで1回だけ
    if((up||down||left||right)&&joyNav){ joyNav=false; joyNavDo(up,down,left,right); }
    if(!up&&!down&&!left&&!right) joyNav=true;
  }
  if(btn&&joyBtn){ joyBtn=false; joyButton(); } // ボタンは押した瞬間だけ
  if(!btn) joyBtn=true;
}
function joyNavDo(up,down,left,right){
  if(mode==="battle"){ const b=battle; if(!b)return;
    if(b.state==="command"){ if(up)b.cmd=(b.cmd+3)%4; if(down)b.cmd=(b.cmd+1)%4; }
    else if(b.state==="spell"){ if(up)b.spell=(b.spell+2)%3; if(down)b.spell=(b.spell+1)%3; }
  }else if(mode==="service"){ const s=service; if(!s||s.phase!=="menu")return; const n=s.labels.length;
    if(up)s.cursor=Math.max(0,s.cursor-2); if(down)s.cursor=Math.min(n-1,s.cursor+2);
    if(left)s.cursor=Math.max(0,s.cursor-1); if(right)s.cursor=Math.min(n-1,s.cursor+1);
  }
}
function joyButton(){
  if(mode==="field"){ const n=npcInFront(); if(n) talkTo(n); }
  else if(mode==="talk"){ advanceDialog(); }
  else if(mode==="status"){ mode="field"; }
  else if(mode==="battle"){ const b=battle; if(!b)return;
    if(b.state==="msg") advanceBattleMsg(); else if(b.state==="command") confirmCommand(); else if(b.state==="spell") confirmSpell(); }
  else if(mode==="service"){ const s=service; if(!s)return;
    if(s.phase==="msg") advanceSvc(); else confirmSvc(); }
}
setInterval(pollJoy,120);

// ====== 更新 ======
function update(){
  if(encCooldown>0) encCooldown--;
  if(mode==="field"){
    player.walk=0;
    const sp=3; let dx=0,dy=0;
    if(keys["w"])dy-=sp; if(keys["s"])dy+=sp;
    if(keys["a"])dx-=sp; if(keys["d"])dx+=sp;
    if(tapDir==="w")dy-=sp; if(tapDir==="s")dy+=sp;
    if(tapDir==="a")dx-=sp; if(tapDir==="d")dx+=sp;
    const j=joyState;                          // ジョイスティック
    if(!(j.x===0&&j.y===0)){
      if(j.x<350)dx-=sp; if(j.x>700)dx+=sp;
      if(j.y<350)dy-=sp; if(j.y>700)dy+=sp;
    }
    if(dx)move(dx,0); if(dy)move(0,dy);
    updateCamera();
    // 立っているタイル
    const ptx=Math.floor((player.x+player.size/2)/TILE), pty=Math.floor((player.y+player.size/2)/TILE);
    // 出口(G)で別マップへ
    if((dx||dy) && map[pty] && map[pty][ptx]==="G"){
      const ex=GAME_DATA[currentMap].exit;
      if(ex){ loadMap(ex.to, ex.tx, ex.ty); return; }
    }
    // 施設のドアに入ると施設メニュー
    if((dx||dy)){ const b=doorAt(ptx,pty); if(b){ openService(b); return; } }
    // 草地を歩くとランダムエンカウント（安全マップ=街では出ない）
    const safe=GAME_DATA[currentMap].safe;
    if(!safe && (dx||dy) && encCooldown<=0 && onGrass() && Math.random()<0.004){ startBattle(); }
  }
  if(mode==="battle") updateBattle();
  if(mode==="talk" && dialog.active){
    dialog.t++;
    const full=dialog.lines[dialog.idx];
    if(dialog.shown<full.length && dialog.t%2===0) dialog.shown++; // 1文字ずつ表示
  }
}
function updateCamera(){
  const mapW=map[0].length*TILE, mapH=map.length*TILE;
  cameraX=Math.max(0,Math.min(player.x+player.size/2-VIEW/2, mapW-VIEW));
  cameraY=Math.max(0,Math.min(player.y+player.size/2-VIEW/2, mapH-VIEW));
}

// ====== 描画 ======
function roundRect(x,y,w,h,r){
  ctx.beginPath();
  ctx.moveTo(x+r,y); ctx.arcTo(x+w,y,x+w,y+h,r); ctx.arcTo(x+w,y+h,x,y+h,r);
  ctx.arcTo(x,y+h,x,y,r); ctx.arcTo(x,y,x+w,y,r); ctx.closePath();
}
// タイル座標から安定した擬似乱数（草の模様を固定するため）
function hash(x,y){ let h=(x*374761393+y*668265263)>>>0; h=(h^(h>>13))*1274126177>>>0; return (h>>>0)/4294967296; }

function drawTile(c,x,y){
  const gx=x*TILE, gy=y*TILE;
  if(c==="W"){ ctx.drawImage(SPR.water[Math.floor(Date.now()/350)%2], gx, gy); return; }
  ctx.drawImage(SPR.grass[(x+y)&1], gx, gy);       // 草地ベース
  if(c==="T") ctx.drawImage(SPR.tree, gx, gy);
  else if(c==="R") ctx.drawImage(SPR.rock, gx, gy);
  else if(c==="H") ctx.drawImage(SPR.house, gx, gy);
  else if(c==="d") ctx.drawImage(SPR.dead, gx, gy);
  else if(c==="G"){ // 村の入口（家々の前の土の道。文字は出さない）
    ctx.fillStyle="#b8945a"; ctx.fillRect(gx+TILE*0.28,gy,TILE*0.44,TILE);                // 土の道
    ctx.fillStyle="#a8824a"; ctx.fillRect(gx+TILE*0.28,gy,4,TILE); ctx.fillRect(gx+TILE*0.72-4,gy,4,TILE);
  }
}

// 街の建物（大きめ・3タイル幅×2.5タイル高）。種類で看板を変える
function drawBuildings(){
  for(const b of buildings){
    const x=b.x*TILE, y=(b.y-1)*TILE, w=3*TILE, h=3*TILE;  // 上に1タイル分ぼ屋根
    // 影
    ctx.fillStyle="rgba(0,0,0,0.18)"; ctx.fillRect(x+8,y+h-6,w-16,8);
    // 壁
    ctx.fillStyle=b.color; ctx.fillRect(x+6,y+h*0.42,w-12,h*0.58);
    ctx.fillStyle="rgba(0,0,0,0.12)"; ctx.fillRect(x+6,y+h*0.42,12,h*0.58);
    // 屋根
    ctx.fillStyle="#7a2f2f"; ctx.beginPath();
    ctx.moveTo(x,y+h*0.46); ctx.lineTo(x+w/2,y+8); ctx.lineTo(x+w,y+h*0.46); ctx.closePath(); ctx.fill();
    ctx.fillStyle="#5e2222"; ctx.fillRect(x,y+h*0.43,w,8);
    // ドア（下中央）
    const dw=44, dx=x+w/2-dw/2, dy=y+h-78;
    ctx.fillStyle="#3a2410"; ctx.fillRect(dx,dy,dw,78);
    ctx.fillStyle="#ffd23f"; ctx.beginPath(); ctx.arc(dx+dw-10,dy+40,3,0,Math.PI*2); ctx.fill();
    // 看板（アイコンで分かりやすく）
    const sx=x+w/2-30, sy=y+h*0.49, sw=60, sh=44;
    ctx.fillStyle="#efe6cf"; roundRect(sx,sy,sw,sh,5); ctx.fill();
    ctx.strokeStyle="#5e2222"; ctx.lineWidth=2.5; roundRect(sx,sy,sw,sh,5); ctx.stroke();
    drawSignIcon(b.type, x+w/2, sy+sh/2);
    // 教会は屋根に十字
    if(b.type==="church"){ ctx.strokeStyle="#ffe9a8"; ctx.lineWidth=4; ctx.lineCap="round";
      ctx.beginPath(); ctx.moveTo(x+w/2,y+12); ctx.lineTo(x+w/2,y+h*0.4); ctx.moveTo(x+w/2-12,y+h*0.18); ctx.lineTo(x+w/2+12,y+h*0.18); ctx.stroke(); }
    ctx.textAlign="left";
  }
}
// 施設アイコン（宿=ベッド/武器=剣/道具=ビン/教会=十字）
function drawSignIcon(type, cx, cy){
  if(type==="inn"){            // ベッド
    ctx.fillStyle="#7a4a2a"; ctx.fillRect(cx-18,cy-2,36,10); ctx.fillRect(cx-18,cy-8,6,16);
    ctx.fillStyle="#e8e8f0"; ctx.fillRect(cx-12,cy-7,12,7);   // 枕
    ctx.fillStyle="#c0303a"; ctx.fillRect(cx-1,cy-7,18,7);    // 掛け布団
  }else if(type==="weapon"){   // 剣
    ctx.fillStyle="#c8ced8"; ctx.fillRect(cx-2,cy-15,5,22);    // 刃
    ctx.fillStyle="#d9b24a"; ctx.fillRect(cx-9,cy+4,19,4);     // つば
    ctx.fillStyle="#6b4a2a"; ctx.fillRect(cx-1,cy+8,3,7);      // 柄
  }else if(type==="item"){     // ポーション
    ctx.fillStyle="#9aa0a8"; ctx.fillRect(cx-3,cy-14,6,5);     // 栓
    ctx.fillStyle="#cfe2f5"; ctx.beginPath(); ctx.arc(cx,cy+1,11,0,Math.PI*2); ctx.fill();
    ctx.fillStyle="#2f8fd0"; ctx.beginPath(); ctx.arc(cx,cy+3,8,0,Math.PI*2); ctx.fill();
    ctx.fillStyle="#cfe2f5"; ctx.fillRect(cx-4,cy-12,8,6);
  }else if(type==="church"){   // 十字
    ctx.fillStyle="#d9b24a"; ctx.fillRect(cx-3,cy-15,6,30); ctx.fillRect(cx-12,cy-7,30,6);
  }
}

function drawHero(){
  const dir=player.dir;
  const set = dir==="up"?SPR.heroUp : dir==="left"?SPR.heroLeft
            : dir==="right"?SPR.heroRight : SPR.heroDown;
  // 歩行中は[歩行A,歩行B]を交互に。止まると[立ち]
  let frame=0;
  if(player.walk){ frame = (Math.floor(Date.now()/200)%2) ? 2 : 1; }  // 200ms毎に足を入れ替え
  const spr=set[frame];
  const bob=(player.walk && Math.floor(Date.now()/200)%2)? CHAR:0;    // わずかな上下
  const dx=Math.round(player.x+player.size/2-spr.width/2);
  const dy=Math.round(player.y+player.size-spr.height)+12-bob;        // 足を地面寄りに
  ctx.drawImage(spr, dx, dy);
}

function drawNpc(n){
  const dx=n.x*TILE+TILE/2-n.spr.width/2;
  const dy=n.y*TILE+TILE-n.spr.height+8;   // 足をタイル下端付近に合わせる
  ctx.drawImage(n.spr, dx, dy);
  // 「！」（ふわふわ上下）
  const bob=Math.sin(Date.now()*0.005)*3;
  ctx.fillStyle="#ffd23f"; ctx.font="bold 24px sans-serif"; ctx.textAlign="center";
  ctx.fillText("!", n.x*TILE+TILE/2, dy-8+bob); ctx.textAlign="left";
}

function drawDialog(){
  const m=20, h=170, y=VIEW-h-m, w=VIEW-m*2;
  // 影
  ctx.fillStyle="rgba(0,0,0,0.4)"; roundRect(m+4,y+6,w,h,14); ctx.fill();
  // 枠（金フレーム＋濃紺の中身）
  const g=ctx.createLinearGradient(0,y,0,y+h);
  g.addColorStop(0,"#d9b24a"); g.addColorStop(1,"#9c7a28");
  ctx.fillStyle=g; roundRect(m,y,w,h,14); ctx.fill();
  ctx.fillStyle="#101830"; roundRect(m+6,y+6,w-12,h-12,10); ctx.fill();
  // 名前タグ
  ctx.fillStyle="#d9b24a"; roundRect(m+18,y-2,140,34,8); ctx.fill();
  ctx.fillStyle="#101830"; ctx.font="bold 18px sans-serif"; ctx.textAlign="center";
  ctx.fillText(dialog.name, m+18+70, y+21); ctx.textAlign="left";
  // 本文（1文字ずつ）
  ctx.fillStyle="#fff"; ctx.font="24px 'Hiragino Kaku Gothic ProN',sans-serif";
  const text=dialog.lines[dialog.idx].slice(0,dialog.shown);
  ctx.fillText(text, m+30, y+80);
  // 送り三角（全部表示後に点滅）
  const full=dialog.lines[dialog.idx];
  if(dialog.shown>=full.length && Math.floor(Date.now()/400)%2===0){
    ctx.fillStyle="#ffd23f"; ctx.beginPath();
    ctx.moveTo(VIEW-m-40,y+h-30); ctx.lineTo(VIEW-m-24,y+h-30); ctx.lineTo(VIEW-m-32,y+h-18); ctx.closePath(); ctx.fill();
  }
}

// 金枠ウィンドウ
function winBox(x,y,w,h){
  ctx.fillStyle="rgba(0,0,0,0.45)"; roundRect(x+4,y+5,w,h,12); ctx.fill();
  const g=ctx.createLinearGradient(0,y,0,y+h); g.addColorStop(0,"#d9b24a"); g.addColorStop(1,"#9c7a28");
  ctx.fillStyle=g; roundRect(x,y,w,h,12); ctx.fill();
  ctx.fillStyle="#101830"; roundRect(x+6,y+6,w-12,h-12,8); ctx.fill();
}
function drawMenu(items){
  const b=battle; b.rects=[];
  const x=70, y0=VIEW-150, lh=40, sel=b.state==="command"?b.cmd:b.spell;
  ctx.font="26px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="left";
  for(let i=0;i<items.length;i++){
    const iy=y0+i*lh;
    if(i===sel){ ctx.fillStyle="#ffd23f"; ctx.fillText("▶", x-34, iy); }
    ctx.fillStyle = i===sel ? "#fff" : "#9aa0b0";
    ctx.fillText(items[i], x, iy);
    b.rects.push({x:x-40,y:iy-30,w:240,h:36,i});
  }
}
function drawBattle(){
  const b=battle;
  // 背景（暗い戦闘空間）
  let g=ctx.createLinearGradient(0,0,0,VIEW);
  g.addColorStop(0,"#0b0b1c"); g.addColorStop(0.55,"#15152e"); g.addColorStop(1,"#05050c");
  ctx.fillStyle=g; ctx.fillRect(0,0,VIEW,VIEW);
  ctx.fillStyle="#1c1838"; ctx.fillRect(0,VIEW*0.50,VIEW,3);
  // 敵
  const ew=192, float=Math.sin(Date.now()*0.003)*7;   // 96px原画を2倍で綺麗に
  const shakeX=b.shake>0?(Math.random()-0.5)*b.shake:0;
  const ex=VIEW/2-ew/2+shakeX, ey=VIEW*0.16+float;
  if(!(b.flash>0 && Math.floor(Date.now()/60)%2)) ctx.drawImage(SPR[b.enemy.spr], ex, ey, ew, ew);
  ctx.fillStyle="#fff"; ctx.font="22px sans-serif"; ctx.textAlign="center";
  ctx.fillText(b.enemy.name, VIEW/2, ey-14); ctx.textAlign="left";
  // ステータス（左上）
  winBox(20,20,270,128);
  ctx.fillStyle="#fff"; ctx.font="22px sans-serif"; ctx.fillText("勇者", 42, 54);
  ctx.font="20px monospace";
  ctx.fillStyle = stats.hp<=stats.maxhp*0.25 ? "#ff6a6a":"#fff";
  ctx.fillText("HP "+stats.hp+"/"+stats.maxhp, 42, 90);
  ctx.fillStyle="#9fd0ff"; ctx.fillText("MP "+stats.mp+"/"+stats.maxmp, 42, 118);
  ctx.fillStyle="#ffd23f"; ctx.font="18px monospace"; ctx.fillText("Lv "+stats.lv, 200, 54);
  // プレイヤー被弾フラッシュ
  if(b.shakeP>0 && Math.floor(Date.now()/50)%2){ ctx.fillStyle="rgba(200,30,30,0.18)"; ctx.fillRect(0,0,VIEW,VIEW); }
  // 下部ウィンドウ
  const my=VIEW-200;
  winBox(20,my,VIEW-40,180);
  if(b.state==="command") drawMenu(BATTLE_CMDS);
  else if(b.state==="spell") drawMenu(SPELLS.map(s=> s.mp? s.name+"("+s.mp+")":s.name));
  else{
    ctx.fillStyle="#fff"; ctx.font="26px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="left";
    const line=(b.msg[b.msgIdx]||"").slice(0,b.shown);
    ctx.fillText(line, 50, my+70);
    const full=b.msg[b.msgIdx]||"";
    if(b.shown>=full.length && Math.floor(Date.now()/400)%2){
      ctx.fillStyle="#ffd23f"; ctx.beginPath();
      ctx.moveTo(VIEW-58,my+150); ctx.lineTo(VIEW-42,my+150); ctx.lineTo(VIEW-50,my+162); ctx.fill();
    }
  }
}

// 右上メニューボタン
function drawMenuButton(){
  const x=VIEW-66, y=10, w=56, h=56;
  winBox(x,y,w,h);
  ctx.strokeStyle="#ffd23f"; ctx.lineWidth=3; ctx.lineCap="round";
  for(let i=0;i<3;i++){ const ly=y+18+i*10; ctx.beginPath(); ctx.moveTo(x+14,ly); ctx.lineTo(x+w-14,ly); ctx.stroke(); }
}

// ステータス／持ち物 画面
function drawStatus(){
  ctx.fillStyle="rgba(0,0,8,0.6)"; ctx.fillRect(0,0,VIEW,VIEW);
  const x=50, y=40, w=VIEW-100, h=VIEW-90;
  winBox(x,y,w,h);
  ctx.textAlign="left";
  // タイトル
  ctx.fillStyle="#ffd23f"; ctx.font="28px 'Hiragino Kaku Gothic ProN',sans-serif";
  ctx.fillText("勇者の そうび・じょうたい", x+34, y+46);
  // ステータス
  ctx.font="24px monospace";
  let ly=y+90; const lx=x+36, lh=38;
  const rows=[
    ["レベル", stats.lv],
    ["HP", stats.hp+" / "+stats.maxhp],
    ["MP", stats.mp+" / "+stats.maxmp],
    ["こうげき力", atkTotal()+(stats.weapon?" (+"+stats.weapon.atk+")":"")],
    ["しゅび力", defTotal()+(stats.armor?" (+"+stats.armor.def+")":"")],
    ["ぶき", stats.weapon? stats.weapon.name:"なし"],
    ["よろい", stats.armor? stats.armor.name:"なし"],
    ["ゴールド", stats.gold+" G"],
    ["つぎのLvまで", Math.max(0, needExp(stats.lv)-stats.exp)]
  ];
  ctx.fillStyle="#fff";
  for(const [k,v] of rows){
    ctx.fillStyle="#9fb0d0"; ctx.fillText(k, lx, ly);
    ctx.fillStyle="#fff";     ctx.fillText(""+v, lx+260, ly);
    ly+=lh;
  }
  // もちもの（持っているものだけ）
  ly+=8;
  ctx.fillStyle="#ffd23f"; ctx.font="22px 'Hiragino Kaku Gothic ProN',sans-serif";
  ctx.fillText("もちもの", lx, ly); ly+=34;
  const items=[];
  if(stats.herb>0)   items.push("やくそう ×"+stats.herb);
  if(stats.elixir>0) items.push("エリクサー ×"+stats.elixir);
  ctx.fillStyle="#fff"; ctx.font="22px monospace";
  ctx.fillText(items.length? items.join("   ") : "なし", lx+20, ly);
  // 覚えている呪文
  ly+=40;
  ctx.fillStyle="#ffd23f"; ctx.font="22px 'Hiragino Kaku Gothic ProN',sans-serif";
  ctx.fillText("じゅもん", lx, ly); ly+=34;
  ctx.fillStyle="#9fd0ff"; ctx.font="22px monospace";
  ctx.fillText("かえん(MP3)   いやし(MP4)", lx+20, ly);
  // 閉じる案内
  ctx.fillStyle="#9aa0b0"; ctx.font="18px sans-serif"; ctx.textAlign="center";
  ctx.fillText("M キー / タップ でとじる", VIEW/2, y+h-20);
  ctx.textAlign="left";
}

function draw(){
  if(mode==="battle"){ drawBattle(); return; }
  ctx.clearRect(0,0,VIEW,VIEW);
  ctx.save(); ctx.translate(-Math.round(cameraX),-Math.round(cameraY));
  // タイル（画面内のみ）
  const x0=Math.floor(cameraX/TILE), y0=Math.floor(cameraY/TILE);
  for(let y=y0;y<=y0+VIEW/TILE+1 && y<map.length;y++)
    for(let x=x0;x<=x0+VIEW/TILE+1 && x<map[0].length;x++)
      drawTile(map[y][x],x,y);
  drawBuildings();
  for(const n of npcs) drawNpc(n);
  drawHero();
  ctx.restore();
  // 周辺減光は暗いエリアのみ（通常エリアは明るいクラシックな見た目）
  if(darkArea){
    const vg=ctx.createRadialGradient(VIEW/2,VIEW/2,VIEW*0.25,VIEW/2,VIEW/2,VIEW*0.7);
    vg.addColorStop(0,"rgba(0,0,0,0)"); vg.addColorStop(1,"rgba(8,4,18,0.72)");
    ctx.fillStyle=vg; ctx.fillRect(0,0,VIEW,VIEW);
  }
  if(mode==="talk" && dialog.active) drawDialog();
  if(mode==="field"||mode==="status") drawMenuButton();
  if(mode==="status") drawStatus();
  if(mode==="service") drawService();
}

function drawService(){
  const s=service;
  const mh=240, my=VIEW-mh-20, mw=VIEW-40;
  // 看板タイトル
  winBox(20,30,300,56);
  ctx.fillStyle="#fff"; ctx.font="26px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="left";
  ctx.fillText(s.name, 44, 68);
  // 所持金
  winBox(VIEW-220,30,200,56);
  ctx.fillStyle="#ffd23f"; ctx.font="24px monospace"; ctx.fillText(stats.gold+" G", VIEW-200, 68);
  // 下ウィンドウ
  winBox(20,my,mw,mh);
  if(s.phase==="menu"){
    s.rects=[];
    const PER=8, page=Math.floor(s.cursor/PER), start=page*PER, end=Math.min(start+PER,s.labels.length);
    const colX=[60, 60+(mw-80)/2], y0=my+44, lh=44;
    ctx.font="24px 'Hiragino Kaku Gothic ProN',sans-serif";
    for(let i=start;i<end;i++){
      const k=i-start, col=k%2, row=Math.floor(k/2);
      const bx=colX[col], by=y0+row*lh;
      if(i===s.cursor){ ctx.fillStyle="#ffd23f"; ctx.fillText("▶",bx-28,by); }
      ctx.fillStyle = i===s.cursor ? "#fff":"#9aa0b0";
      ctx.fillText(s.labels[i], bx, by);
      s.rects.push({x:bx-30,y:by-30,w:(mw-80)/2-8,h:36,i});
    }
    const pages=Math.ceil(s.labels.length/PER);
    if(pages>1){
      ctx.fillStyle="#9aa0b0"; ctx.font="20px sans-serif"; ctx.textAlign="center";
      ctx.fillText("ページ "+(page+1)+" / "+pages, VIEW/2, my+mh-18); ctx.textAlign="left";
    }
  }else{
    ctx.fillStyle="#fff"; ctx.font="26px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="left";
    const line=(s.msgs[s.msgIdx]||"").slice(0,s.shown);
    ctx.fillText(line, 50, my+70);
    const full=s.msgs[s.msgIdx]||"";
    if(s.shown>=full.length && Math.floor(Date.now()/400)%2){
      ctx.fillStyle="#ffd23f"; ctx.beginPath();
      ctx.moveTo(VIEW-58,my+170); ctx.lineTo(VIEW-42,my+170); ctx.lineTo(VIEW-50,my+182); ctx.fill();
    }
  }
}

// 施設メッセージの1文字送り
function updateService(){
  const s=service; if(!s||s.phase!=="msg") return;
  const full=s.msgs[s.msgIdx]||""; if(s.shown<full.length) s.shown++;
}

function loop(){ update(); if(mode==="service") updateService(); draw(); requestAnimationFrame(loop); }
buildSprites(); loadGame(); loadMap(respawn.map, respawn.tx, respawn.ty); loop();
</script>
</body>
</html>
)rawliteral";

void setup(){
  Serial.begin(9600);
  pinMode(PIN_SW, INPUT_PULLUP);
  pinMode(PIN_BUZZ, OUTPUT);
  strip.begin(); strip.show();   // 全消灯
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
  // LEDの自動消灯（非ブロッキング）
  if(ledOffAt && millis()>ledOffAt){ strip.clear(); strip.show(); ledOffAt=0; }

  WiFiClient client=server.available();
  if(client){
    // リクエスト1行目だけ読んでパスを取得
    String reqLine = client.readStringUntil('\n');
    // 残りのヘッダを読み飛ばす
    while(client.connected()){
      String h=client.readStringUntil('\n');
      if(h=="\r"||h.length()==0) break;
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
