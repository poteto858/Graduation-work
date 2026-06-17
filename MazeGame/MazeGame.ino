/*
  MazeGame.ino
  Arduino UNO R4 WiFi - Web Game Server (Phase 12 / 迷路ゲーム・完成版)

  追加機能:
    - ステージ1〜3
    - ステージ選択画面
    - コイン収集（全コイン取得でゴール解放）
    - 敵キャラ（巡回／接触でスタートに戻る）
    - ジョイスティックでステージ切替・操作（/state エンドポイントをポーリング）

  配線（ジョイスティックモジュール）:
    VRx -> A0   (左右)
    VRy -> A1   (上下)
    SW  -> D2   (押し込みボタン, INPUT_PULLUP)
    VCC -> 5V   GND -> GND

  ジョイスティックが無くても、キーボード(WASD/Enter)とタップで全機能動作します。
*/

#include <WiFiS3.h>
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

WiFiServer server(80);

// ---- ジョイスティック ピン ----
const int PIN_VRX = A0;
const int PIN_VRY = A1;
const int PIN_SW  = 2;

const char page[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Maze RPG Phase12</title>
<style>
body{background:#222;color:#fff;text-align:center;font-family:Arial,sans-serif;}
canvas{border:2px solid #fff;background:#333;touch-action:none;max-width:95vw;}
button{padding:10px 18px;margin:8px 4px;font-size:15px;cursor:pointer;}
#hud{margin-top:8px;}
</style>
</head>
<body>

<h2>Maze RPG Phase12</h2>
<p>PC: WASD移動 / Enter決定 / Rリスタート　スマホ: タップ　ジョイスティック対応</p>

<canvas id="game" width="400" height="400"></canvas>

<div id="hud">
<span id="timer">Time: 0.0s</span> |
<span id="coininfo">Coins: 0/0</span> |
<span id="best">Best: --</span>
</div>

<div>
<button id="restartBtn">Restart Stage</button>
<button id="stagesBtn">Stage Select</button>
</div>

<script>
const canvas=document.getElementById("game");
const ctx=canvas.getContext("2d");
const TILE=40;

// ====== ステージ定義 ======
// 記号: # 壁 / 空白 床 / S スタート / G ゴール / C コイン / E 敵
const stages=[
 { name:"Stage 1", map:[
   "#############",
   "#S #    E   #",
   "#  #C       #",
   "#  #  #  ####",
   "#  #  #     #",
   "#  #  # C   #",
   "# C#######  #",
   "#C       #  #",
   "#        #  #",
   "#######  #  #",
   "#       C  C#",
   "#      E   G#",
   "#############"]},
 { name:"Stage 2", map:[
   "################",
   "#S #    C   C  #",
   "#  #           #",
   "#  #######  #E #",
   "#     #     #  #",
   "#     #C CE #  #",
   "####  #  #  #  #",
   "#  #   C #     #",
   "#  #C    #     #",
   "#  #######  ####",
   "#E #     #     #",
   "#  #     #     #",
   "#C #  #  ####  #",
   "#     #       E#",
   "#    C#       G#",
   "################"]},
 { name:"Stage 3", map:[
   "###################",
   "#S #         C    #",
   "#  #     E        #",
   "#  ####  ####  # C#",
   "# E       E #  # C#",
   "#      C C  #  #  #",
   "##########  ####  #",
   "#  C  CE #     #  #",
   "#        #     #  #",
   "####  #  ####  #  #",
   "#        #  C  #  #",
   "#     C  #     #  #",
   "#  ####  #  ####  #",
   "#        #  #C    #",
   "#        #  #     #",
   "#  #  ####  ####E #",
   "#  #          E   #",
   "#  #             G#",
   "###################"]},
];

// ====== 状態 ======
let screen="select";       // "select" | "playing" | "clear"
let selIndex=0;            // ステージ選択カーソル
let stageIndex=0;
let map=[];
let player, playerStart, goal;
let coins=[], enemies=[];
let totalCoins=0, gotCoins=0;
let target=null;
let keys={};
let startTime=0, clearTime=0;
let cameraX=0, cameraY=0;

// ジョイスティック由来の移動・操作
let joyVX=0, joyVY=0;
let navReady=true, btnReady=true;

// ====== 補助 ======
function bestKey(i){return "mazeBest_stage"+i;}
function getBest(i){return localStorage.getItem(bestKey(i));}

function hitsWall(x,y,size){
  const cols=map[0].length, rows=map.length;
  const l=Math.floor(x/TILE), r=Math.floor((x+size-1)/TILE);
  const t=Math.floor(y/TILE), b=Math.floor((y+size-1)/TILE);
  if(l<0||t<0||r>=cols||b>=rows) return true;
  return map[t][l]==="#"||map[t][r]==="#"||map[b][l]==="#"||map[b][r]==="#";
}
function aabb(ax,ay,as,bx,by,bs){
  return ax<bx+bs && ax+as>bx && ay<by+bs && ay+as>by;
}

function makeEnemy(tx,ty){
  const sx=tx*TILE+8, sy=ty*TILE+8;
  const e={x:sx, y:sy, size:24, vx:0, vy:0, sx:sx, sy:sy, patrol:4*TILE};
  const sp=1.2;
  const hOpen=(map[ty][tx-1]!=="#")||(map[ty][tx+1]!=="#");
  if(hOpen) e.vx=sp; else e.vy=sp;
  return e;
}

function loadStage(i){
  stageIndex=i;
  map=stages[i].map;
  coins=[]; enemies=[]; gotCoins=0; target=null;
  joyVX=0; joyVY=0; cameraX=0; cameraY=0;
  for(let y=0;y<map.length;y++){
    for(let x=0;x<map[y].length;x++){
      const c=map[y][x];
      if(c==="S"){ player={x:x*TILE+5,y:y*TILE+5,size:30};
                   playerStart={x:x*TILE+5,y:y*TILE+5}; }
      if(c==="G")  goal={x:x*TILE+20,y:y*TILE+20,radius:12};
      if(c==="C")  coins.push({x:x*TILE+TILE/2,y:y*TILE+TILE/2,r:7,got:false});
      if(c==="E")  enemies.push(makeEnemy(x,y));
    }
  }
  totalCoins=coins.length;
  startTime=Date.now();
  screen="playing";
}

function respawn(){
  player.x=playerStart.x; player.y=playerStart.y; target=null;
}

// ====== 入力 ======
document.addEventListener("keydown",(e)=>{
  const k=e.key.toLowerCase();
  keys[k]=true;
  // 矢印キーをWASDに正規化
  if(e.key==="ArrowUp")    keys["w"]=true;
  if(e.key==="ArrowDown")  keys["s"]=true;
  if(e.key==="ArrowLeft")  keys["a"]=true;
  if(e.key==="ArrowRight") keys["d"]=true;
  if(screen==="select"){
    if(k==="w"||e.key==="ArrowUp")   selIndex=(selIndex-1+stages.length)%stages.length;
    if(k==="s"||e.key==="ArrowDown") selIndex=(selIndex+1)%stages.length;
    if(e.key==="Enter"||k===" ")     loadStage(selIndex);
  }else if(screen==="playing"){
    if(k==="r") loadStage(stageIndex);
    if(e.key==="ArrowUp"||e.key==="ArrowDown"||e.key==="ArrowLeft"||e.key==="ArrowRight") e.preventDefault();
  }else if(screen==="clear"){
    if(e.key==="Enter"||k==="r") screen="select";
  }
});
document.addEventListener("keyup",(e)=>{
  const k=e.key.toLowerCase();
  keys[k]=false;
  if(e.key==="ArrowUp")    keys["w"]=false;
  if(e.key==="ArrowDown")  keys["s"]=false;
  if(e.key==="ArrowLeft")  keys["a"]=false;
  if(e.key==="ArrowRight") keys["d"]=false;
});

document.getElementById("restartBtn").addEventListener("click",()=>{
  if(screen==="select") loadStage(selIndex); else loadStage(stageIndex);
});
document.getElementById("stagesBtn").addEventListener("click",()=>{ screen="select"; });

function canvasPos(e){
  const rect=canvas.getBoundingClientRect();
  const sx=canvas.width/rect.width, sy=canvas.height/rect.height;
  return { x:(e.clientX-rect.left)*sx, y:(e.clientY-rect.top)*sy };
}
canvas.addEventListener("pointerdown",(e)=>{
  const p=canvasPos(e);
  if(screen==="select"){
    const idx=Math.floor((p.y-95)/70);
    if(idx>=0 && idx<stages.length){ selIndex=idx; loadStage(idx); }
  }else if(screen==="playing"){
    target={x:p.x+cameraX, y:p.y+cameraY};
  }else if(screen==="clear"){
    screen="select";
  }
});

// ====== ジョイスティック ポーリング ======
// しきい値。配線で上下/左右が逆なら INVERT を切り替えてください。
const JOY_LOW=200, JOY_HIGH=820;
const INVERT_X=false, INVERT_Y=false;

async function pollJoy(){
  try{
    const r=await fetch("/state",{cache:"no-store"});
    if(!r.ok) return;
    const j=await r.json();
    applyJoy(j);
  }catch(e){ /* ジョイスティック未接続なら無視 */ }
}
function applyJoy(j){
  // x==0 && y==0 はフローティング（未接続）と判断してニュートラル扱い
  if(j.x===0 && j.y===0) return;
  let xl=j.x<JOY_LOW, xh=j.x>JOY_HIGH;
  let yl=j.y<JOY_LOW, yh=j.y>JOY_HIGH;
  let left =INVERT_X? xh:xl, right=INVERT_X? xl:xh;
  let up   =INVERT_Y? yh:yl, down =INVERT_Y? yl:yh;
  const btn=j.b===1;

  if(screen==="select"){
    if((up||down)&&navReady){
      selIndex=(selIndex+(down?1:-1)+stages.length)%stages.length;
      navReady=false;
    }
    if(!up&&!down) navReady=true;
    if(btn&&btnReady){ loadStage(selIndex); btnReady=false; }
    if(!btn) btnReady=true;
  }else if(screen==="playing"){
    joyVX = left?-1:right?1:0;
    joyVY = up?-1:down?1:0;
    if(btn&&btnReady){ screen="select"; btnReady=false; }
    if(!btn) btnReady=true;
  }else if(screen==="clear"){
    if(btn&&btnReady){ screen="select"; btnReady=false; }
    if(!btn) btnReady=true;
  }
}
setInterval(pollJoy,120);

// ====== 更新 ======
function movePlayer(dx,dy){
  if(dx && !hitsWall(player.x+dx,player.y,player.size)) player.x+=dx;
  if(dy && !hitsWall(player.x,player.y+dy,player.size)) player.y+=dy;
}

function updateEnemies(){
  for(const e of enemies){
    if(e.vx!==0){
      const nx=e.x+e.vx;
      if(hitsWall(nx,e.y,e.size)||Math.abs(nx-e.sx)>e.patrol) e.vx=-e.vx; else e.x=nx;
    }
    if(e.vy!==0){
      const ny=e.y+e.vy;
      if(hitsWall(e.x,ny,e.size)||Math.abs(ny-e.sy)>e.patrol) e.vy=-e.vy; else e.y=ny;
    }
    if(aabb(player.x,player.y,player.size,e.x,e.y,e.size)) respawn();
  }
}

function collectCoins(){
  const cx=player.x+player.size/2, cy=player.y+player.size/2;
  for(const c of coins){
    if(!c.got && Math.hypot(cx-c.x,cy-c.y) < c.r+player.size/2){
      c.got=true; gotCoins++;
    }
  }
}

function checkGoal(){
  if(gotCoins<totalCoins) return;
  const cx=player.x+player.size/2, cy=player.y+player.size/2;
  if(Math.hypot(cx-goal.x,cy-goal.y) < goal.radius+player.size/2){
    screen="clear";
    clearTime=(Date.now()-startTime)/1000;
    const b=getBest(stageIndex);
    if(!b || clearTime<Number(b)) localStorage.setItem(bestKey(stageIndex),clearTime);
  }
}

function update(){
  if(screen!=="playing") return;
  const speed=2;
  let dx=0, dy=0;
  if(keys["w"]) dy-=speed;
  if(keys["s"]) dy+=speed;
  if(keys["a"]) dx-=speed;
  if(keys["d"]) dx+=speed;
  dx+=joyVX*speed; dy+=joyVY*speed;
  if(dx) movePlayer(dx,0);
  if(dy) movePlayer(0,dy);

  if(target){
    const cx=player.x+player.size/2, cy=player.y+player.size/2;
    const tx=target.x-cx, ty=target.y-cy;
    const dist=Math.hypot(tx,ty);
    if(dist>3) movePlayer(tx/dist*speed, ty/dist*speed);
    else target=null;
  }

  updateEnemies();
  collectCoins();
  checkGoal();
  updateCamera();
}

function updateCamera(){
  const mapW=map[0].length*TILE, mapH=map.length*TILE;
  const cx=player.x+player.size/2, cy=player.y+player.size/2;
  cameraX=Math.max(0, Math.min(cx-200, mapW-400));
  cameraY=Math.max(0, Math.min(cy-200, mapH-400));
}

// ====== 描画 ======
function drawSelect(){
  ctx.clearRect(0,0,400,400);
  ctx.textAlign="center";
  ctx.fillStyle="#fff"; ctx.font="28px Arial";
  ctx.fillText("STAGE SELECT",200,60);
  ctx.font="18px Arial";
  for(let i=0;i<stages.length;i++){
    const y=120+i*70;
    ctx.fillStyle = i===selIndex ? "#444" : "#2a2a2a";
    ctx.fillRect(60,y-30,280,55);
    if(i===selIndex){ ctx.strokeStyle="lime"; ctx.lineWidth=3; ctx.strokeRect(60,y-30,280,55); }
    ctx.fillStyle="#fff"; ctx.font="18px Arial";
    ctx.fillText(stages[i].name,200,y-4);
    const b=getBest(i);
    ctx.fillStyle="#9cf"; ctx.font="13px Arial";
    ctx.fillText(b?("Best: "+Number(b).toFixed(1)+"s"):"Best: --",200,y+16);
  }
  ctx.fillStyle="#aaa"; ctx.font="12px Arial";
  ctx.fillText("W/S・ジョイスティック上下: 選択",200,355);
  ctx.fillText("Enter・ボタン・タップ: 決定",200,375);
  ctx.textAlign="left";
}

function drawPlaying(){
  ctx.clearRect(0,0,400,400);
  ctx.save();
  ctx.translate(-cameraX, -cameraY);

  // 壁
  for(let y=0;y<map.length;y++){
    for(let x=0;x<map[y].length;x++){
      if(map[y][x]==="#"){ ctx.fillStyle="steelblue"; ctx.fillRect(x*TILE,y*TILE,TILE,TILE); }
    }
  }
  // コイン
  for(const c of coins){
    if(c.got) continue;
    ctx.fillStyle="gold"; ctx.beginPath(); ctx.arc(c.x,c.y,c.r,0,Math.PI*2); ctx.fill();
    ctx.strokeStyle="#b8860b"; ctx.lineWidth=2; ctx.stroke();
  }
  // ゴール（全コイン未取得なら灰色＝ロック）
  const open=(gotCoins>=totalCoins);
  ctx.fillStyle= open ? "red" : "#777";
  ctx.beginPath(); ctx.arc(goal.x,goal.y,goal.radius,0,Math.PI*2); ctx.fill();
  // 敵
  for(const e of enemies){
    ctx.fillStyle="#e0457b"; ctx.fillRect(e.x,e.y,e.size,e.size);
    ctx.fillStyle="#fff";
    ctx.fillRect(e.x+5,e.y+7,4,4); ctx.fillRect(e.x+e.size-9,e.y+7,4,4);
  }
  // タップ目標
  if(target){
    ctx.strokeStyle="yellow"; ctx.beginPath(); ctx.arc(target.x,target.y,8,0,Math.PI*2); ctx.stroke();
  }
  // プレイヤー
  ctx.fillStyle="lime"; ctx.fillRect(player.x,player.y,player.size,player.size);

  ctx.restore();

  // HUD（スクリーン座標）
  const now=(Date.now()-startTime)/1000;
  document.getElementById("timer").textContent="Time: "+now.toFixed(1)+"s";
  document.getElementById("coininfo").textContent="Coins: "+gotCoins+"/"+totalCoins;
  const b=getBest(stageIndex);
  document.getElementById("best").textContent=b?("Best: "+Number(b).toFixed(1)+"s"):"Best: --";
}

function drawClear(){
  drawPlaying();
  ctx.fillStyle="rgba(0,0,0,0.6)"; ctx.fillRect(0,0,400,400);
  ctx.textAlign="center"; ctx.fillStyle="#fff";
  ctx.font="36px Arial"; ctx.fillText("CLEAR!",200,150);
  ctx.font="20px Arial";
  ctx.fillText(stages[stageIndex].name,200,190);
  ctx.fillText("Time: "+clearTime.toFixed(1)+"s",200,220);
  const b=getBest(stageIndex);
  ctx.fillStyle="#9cf";
  ctx.fillText("Best: "+(b?Number(b).toFixed(1):clearTime.toFixed(1))+"s",200,248);
  ctx.fillStyle="#aaa"; ctx.font="15px Arial";
  ctx.fillText("Enter・ボタン・タップ: ステージ選択へ",200,290);
  ctx.textAlign="left";
}

function draw(){
  if(screen==="select") drawSelect();
  else if(screen==="playing") drawPlaying();
  else drawClear();
}

function gameLoop(){
  update();
  draw();
  requestAnimationFrame(gameLoop);
}

gameLoop();
</script>
</body>
</html>
)rawliteral";

// ===================== Arduino 本体 =====================
void setup() {
  Serial.begin(9600);
  pinMode(PIN_SW, INPUT_PULLUP);

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(5000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected");
  while (WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
    delay(500);
    Serial.println("Waiting for IP...");
  }
  Serial.println(WiFi.localIP());
  server.begin();
}

void sendState(WiFiClient &client) {
  int vx = analogRead(PIN_VRX);
  int vy = analogRead(PIN_VRY);
  int b  = (digitalRead(PIN_SW) == LOW) ? 1 : 0;  // 押し込みで LOW

  String json = "{\"x\":" + String(vx) + ",\"y\":" + String(vy) + ",\"b\":" + String(b) + "}";

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Cache-Control: no-store");
  client.println("Connection: close");
  client.println();
  client.print(json);
}

void sendPage(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("Connection: close");
  client.println();
  client.print(page);
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String reqLine = "";
  bool firstLine = true;

  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      if (firstLine) { reqLine = line; firstLine = false; }
      if (line == "\r") break;   // ヘッダ終端
    }
  }

  // パス取り出し:  "GET /state HTTP/1.1"
  int p1 = reqLine.indexOf(' ');
  int p2 = reqLine.indexOf(' ', p1 + 1);
  String path = (p1 >= 0 && p2 > p1) ? reqLine.substring(p1 + 1, p2) : "/";

  if (path.startsWith("/state")) {
    sendState(client);
  } else {
    sendPage(client);
  }

  client.stop();
}
