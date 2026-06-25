// ゲーム選択画面（起動時 "/" で配信）。迷路 / RPG を選ぶランチャー。
// HTML/JSは webpage.h と同じく生文字列で分離（.ino直書きはctagsが壊れるため不可）。
// 小さいので gzipせず raw 配信（.ino の "/" 分岐が Content-Length で送る）。
const char LAUNCHER_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="ja">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1.0,maximum-scale=1.0,user-scalable=no">
<title>ゲーム選択</title>
<style>
 *{box-sizing:border-box;-webkit-tap-highlight-color:transparent;}
 body{margin:0;min-height:100vh;background:radial-gradient(circle at 50% -10%,#1d2c52,#0b1020 70%);
      color:#eef2ff;font-family:"Segoe UI","Hiragino Kaku Gothic ProN",system-ui,sans-serif;
      display:flex;flex-direction:column;align-items:center;justify-content:center;gap:8px;padding:24px;}
 h1{margin:0;font-size:30px;font-weight:800;letter-spacing:.12em;text-shadow:0 2px 14px #000;}
 .sub{margin:2px 0 18px;font-size:13px;color:#8aa0d0;letter-spacing:.06em;}
 .cards{display:flex;flex-direction:column;gap:16px;width:min(440px,92vw);}
 .card{display:flex;align-items:center;gap:18px;padding:18px 20px;border-radius:16px;cursor:pointer;
       background:linear-gradient(180deg,#1a2240,#141a30);border:2px solid #2a375f;
       transition:transform .12s,border-color .12s,box-shadow .12s;}
 .card .ico{font-size:40px;line-height:1;filter:drop-shadow(0 2px 6px #000);}
 .card .txt{text-align:left;flex:1;}
 .card .t{font-size:22px;font-weight:700;}
 .card .d{font-size:13px;color:#9fb0d6;margin-top:3px;}
 .card .go{font-size:20px;color:#5f6f9c;}
 .card.sel{border-color:#ffd45e;box-shadow:0 0 0 2px #ffd45e55,0 8px 26px #000a;
           transform:translateY(-2px) scale(1.02);}
 .card.sel .go{color:#ffd45e;}
 .hint{margin-top:18px;font-size:12px;color:#6c7ba6;line-height:1.8;text-align:center;}
 kbd{background:#222c4a;border:1px solid #3a4870;border-radius:5px;padding:1px 6px;font-size:11px;}
</style>
</head>
<body>
<h1>ゲームをえらぶ</h1>
<div class="sub">Arduino UNO R4 WiFi &middot; Game Collection</div>
<div class="cards" id="cards"></div>
<div class="hint">
 タップ / クリックで決定　｜　<kbd>W</kbd><kbd>S</kbd> または <kbd>↑</kbd><kbd>↓</kbd> で選択・<kbd>Enter</kbd>で決定<br>
 実機：ジョイスティック上下で選択、ボタン押し込みで決定
</div>
<script>
const items=[
 {t:"迷路ゲーム", d:"3ステージ・コイン集め・タイムアタック", u:"/maze", ico:"🧩"},
 {t:"RPG_Quest", d:"仲間と隊列で冒険・ターン制バトル・魔王討伐", u:"/rpg", ico:"⚔️"}
];
let sel=0;
const wrap=document.getElementById("cards");
items.forEach((it,i)=>{
  const c=document.createElement("div");
  c.className="card"+(i===0?" sel":"");
  c.innerHTML='<div class="ico">'+it.ico+'</div><div class="txt"><div class="t">'+it.t+
              '</div><div class="d">'+it.d+'</div></div><div class="go">▶</div>';
  c.addEventListener("click",()=>{sel=i;enter();});
  wrap.appendChild(c);
});
function render(){[...wrap.children].forEach((c,i)=>c.classList.toggle("sel",i===sel));}
function move(d){sel=(sel+d+items.length)%items.length;render();}
function enter(){location.href=items[sel].u;}
addEventListener("keydown",e=>{
  const k=e.key.toLowerCase();
  if(k==="arrowup"||k==="w"){move(-1);e.preventDefault();}
  else if(k==="arrowdown"||k==="s"){move(1);e.preventDefault();}
  else if(k==="enter"||k===" "){enter();e.preventDefault();}
  else if(k==="1"){sel=0;enter();}
  else if(k==="2"){sel=1;enter();}
});
// ===== ジョイスティック（/state を約120msでポーリング） =====
const JOY_LOW=200, JOY_HIGH=820;
let navReady=true, btnReady=false, joyArmed=false;
async function pollJoy(){
  try{
    const r=await fetch("/state",{cache:"no-store"});
    if(!r.ok) return;
    const j=await r.json();
    if(j.x===0 && j.y===0) return;          // 未接続フローティングは無視
    const up=j.y<JOY_LOW, down=j.y>JOY_HIGH, btn=j.b===1;
    // 起動直後の「押しっぱなし(強制APのボタン等)」で勝手に決定しないよう、一度ニュートラルを見るまで無効化
    if(!joyArmed){ if(!up && !down && !btn) joyArmed=true; return; }
    if((up||down)&&navReady){ move(down?1:-1); navReady=false; }
    if(!up&&!down) navReady=true;
    if(btn&&btnReady){ btnReady=false; enter(); }
    if(!btn) btnReady=true;
  }catch(e){ /* 未接続は無視 */ }
}
setInterval(pollJoy,120);
</script>
</body>
</html>
)rawliteral";
