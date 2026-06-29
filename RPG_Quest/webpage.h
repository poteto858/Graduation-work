#pragma once
// ゲームのHTML/JS（C++11 生文字列）。
// ※Arduinoのプロトタイプ生成器がJSの function を誤認し、巨大なbase64行で解析が壊れるため、
//   生文字列はこの別ヘッダに分離して #include する（gccは正しく処理する）。
const char page[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no, viewport-fit=cover">
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
#loading{position:fixed;inset:0;z-index:99;display:flex;flex-direction:column;
  align-items:center;justify-content:center;background:radial-gradient(circle at 50% 40%,#243a5e,#0c1424 75%);}
#loading .t{color:#ffe9a8;font-size:30px;letter-spacing:8px;font-weight:700;text-shadow:0 0 12px #b58a2a;}
#loading .s{color:#9fb0d0;font-size:16px;margin-top:18px;animation:blink 1s infinite;}
#loading .bar{width:200px;height:8px;margin-top:18px;border-radius:4px;background:#1a2540;overflow:hidden;}
#loading .bar>i{display:block;height:100%;width:40%;border-radius:4px;background:#ffd23f;animation:slide 1.1s infinite;}
@keyframes blink{0%,100%{opacity:1}50%{opacity:.3}}
@keyframes slide{0%{margin-left:-40%}100%{margin-left:100%}}
/* スマホ用 仮想ゲームパッド（十字キー＋A/B） */
#pad{display:flex;justify-content:space-between;align-items:flex-end;gap:10px;
     width:min(800px,94vw);margin:14px auto 0;padding:0 4px;touch-action:none;
     -webkit-user-select:none;user-select:none;}
.dpad{position:relative;width:162px;height:162px;flex:0 0 auto;}
.db{position:absolute;width:54px;height:54px;border:none;border-radius:12px;padding:0;
    background:rgba(26,34,64,.9);color:#cfe0ff;font-size:20px;line-height:54px;
    box-shadow:0 3px 0 #0a1228,inset 0 0 0 2px #3a4870;cursor:pointer;touch-action:none;}
.db:active{transform:translateY(2px);background:#27365e;color:#fff;
    box-shadow:0 1px 0 #0a1228,inset 0 0 0 2px #ffd23f;}
.db.up{left:54px;top:0}.db.left{left:0;top:54px}.db.right{left:108px;top:54px}.db.down{left:54px;top:108px}
.ab{display:flex;align-items:center;gap:16px;flex:0 0 auto;padding-bottom:6px;}
.ab button{border:none;border-radius:50%;color:#1a1228;font-weight:700;cursor:pointer;
    touch-action:none;box-shadow:0 4px 0 rgba(0,0,0,.45);}
#btnB{width:58px;height:58px;font-size:20px;background:#8fb3ff;}
#btnA{width:74px;height:74px;font-size:26px;background:#ffd23f;}
.ab button:active{transform:translateY(3px);box-shadow:0 1px 0 rgba(0,0,0,.45);}
@media (max-width:640px){
  h2{display:none;} canvas{max-height:58vh;}
  body{padding:8px 0 max(10px,env(safe-area-inset-bottom));display:flex;flex-direction:column;
       align-items:center;min-height:100vh;box-sizing:border-box;}
  #pad{margin-top:auto;}            /* パッドを画面下（親指が届く位置）へ */
  #hint{font-size:11px;margin-top:8px;}
}
</style>
</head>
<body>
<div id="loading"><div class="t">⚔ Q U E S T ⚔</div><div class="bar"><i></i></div><div class="s">よみこみちゅう…</div></div>
<h2>― Q U E S T ―</h2>
<div class="frame"><canvas id="game" width="800" height="800"></canvas></div>
<div id="pad">
  <div class="dpad">
    <button class="db up"    data-k="ArrowUp">▲</button>
    <button class="db left"  data-k="ArrowLeft">◀</button>
    <button class="db right" data-k="ArrowRight">▶</button>
    <button class="db down"  data-k="ArrowDown">▼</button>
  </div>
  <div class="ab">
    <button id="btnB">B</button>
    <button id="btnA">A</button>
  </div>
</div>
<div id="hint">十字キーで移動／A＝決定・話す／B＝メニュー　｜　PCは WASD・矢印・Space　｜　マップのタップ移動も可</div>

<script>
/*IMG_EMBED_START*/
const CHARIMG={};
(function(){const D={"hero":{"d":"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","u":"iVBORw0KGgoAAAANSUhEUgAAAEAAAABACAMAAACdt4HsAAAAflBMVEUAAAANTacfGxpOJQIMLGLm6emapKlXXF4EN41tbW1UdY3///9uip2XYxTspAwxTGZmSyUmJilYWFunt8GenpW1xc8jJSimlWK0ji0VZcb6xyWSdEtKSEblzawgNE3IeAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACZsTZSAAAAIHRSTlMA/vf+/v3+/v4O/gb+/v7+/KZa/h39Zv7//f7+mP1P/f28VdYAAATpSURBVHjalZeJlqsqEEWlRBAERTNPnf//y3eKIYlpte/j3rU6Rmt7qqiBVNXXmohMZaq1ZYimjdtVtaeHPuZnGXcf7/f7lK946SZsvaAbVZMAJuHSmt5PhEaN+y0BTQJEeyLdKKUOmkY2MgnQ0IYHpk4AU+3vJAWpgwNBSymp6rKCRm9EgRQD+FGYXy5SHdQOCqQQreQXdwxQGxJGlRXQcBFCSNUUAC5iKOq/AYq6PcmL+AZEAqm/AY2mbH+ZA0AYdfMPABCSQQY0BSCI7TcBdIoAuQKQEXDaABzD40/AI2zkASQ8mge1xeAL0MrHtgDkRzg9TuX5XwAhcTdMZquapvoU5gC302lTWEE40U+1vVDPb4B2Fv8+XJgVVrUshQZZEHJw1lqXL1tUxLDZLQpgGFA+HEiKABsBLWpTD4KqD8CagsE6yNZQkgADoRylhjdiBjhy9/rdEkYmQPihaQZpowJsv3JgAUn7XNeGu5MeF5sK1uAcOgEAQkC2PiiIYvOPVxrOKlrchrrGTthoAoCAAoXXwxw38iZ2VYfmdVgCHPFYzQhe0iKiQ+6M6WuTO+JRN+63AsNpkAhvyuc3dQkC2u/BLrnQzQjfizv03vCLDB2UW6nsidbt7/uiFB6gSqr/BcD7u9dWoUjggVmaLcsu5BiOOQuocfBg7BYqOgYhbtn87TkEZQ8OVvGEWGoJ93nsEyp/VWRGD74FdIarAwL0ixCFUPof84J46hgegfCgjOGSWzmCWjU7Z7m3JxWFZu2uUZbG1L0PuJqVdleiq7mzK6e4Al+JxB8V39g5aDDwQFl48J1BNStFbBrkyC6WsMzm/KmJM0NppCJCGD34qCy8uu77/uoDSg8zHQAPKxFtBbpLSICdsnIsadzN7H3vb7dbf73WEvq0Igq69MZ2CESN3UEZqovYAwj43INuDD4+PngmwB6NUOqbSwA0akkKBW61VWrMSfDuTbx3zyFNkzZcr+QU90H0YH86nc5nH+ADDSoFYRhGtbNoBaU3xbzwukwj4a+eYmtv2zbvA3fYFl2KDyt87TT24CMJuiOFVrSna/Kiv+KZFw+YMtxTciHO8Hc3S4IxsAMZIG4soX0RXktSuN2Io31WXtOsEYcBD5z75IfuPUdgERCQB74XF7xinLoSwp+aLdsISB9qKRYWHFDNgaiXsvf+TDPAEBVIVrIKgAQANMLg/fP6nAP6vK66XQVgQDjlbj7WZqirRUC/BcCs09LX4VajsUzvPGYAUp6NMVQ3AIjCwCUHEbMe9FMHNuQg8gKJWrEiAfbB37grmDfAUOCiO0cFsuVtFGKVAICvv4+HtWxlVMB//aoHaSu9J/ruxuyD1D7GAHu85sEroT/qqBSTbGVemwJQU75GIzC/ZoH3b/siYFGHrMPiMKgTwT+fdazeVQCc2P86Z+G4UEFD/3wiwMn+8gX4+CwXBhoTxjTBZGoAF+4E4rMpXC4ZI6lbO2ofKfbh3Mk/Aa8eD3kM6BbjgITgX1f3ikorex9QkcRyoGka8ci9Wz1rmjH+yCQcsJTj/hfVwBhliC8o/RhdFDA7YAwuziAt7/noaeO8w8Cc/jjmoi7iIYkHLD+fph/OuA6D1Vk+H3TV5vv5/FeH10TGten2rxlLgcbqzxUnegh8rEi/1Dse8gHf8J1/AbzXWMr9ve5fj/8HWchD+pMZVMAAAAAASUVORK5CYII=","r":"iVBORw0KGgoAAAANSUhEUgAAAEIAAABACAMAAACZQlHRAAAAflBMVEUAAAAfGxoPUqnq7OxRKQZXYGUOLFmYo6piYmIzS2Fsip8gICBlSiFXdIv6zJL///+YYxcFOYvip2daWlqdnZ0qKirkuozemQ+ujWGetcm3xM8SYsOSdlFIRkY3Qky2jDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACpUPK2AAAAIHRSTlMA+f79/v3+/g38/qH8/v4G/v7+Wxpn/v7+/f7+/5xh/9zdSIQAAARkSURBVHjalZfpdqs6DIURRjY2ZgiEKVPf/y3Pls2hnHtXEqMfNKutv8gaN1n21iqiAT/Ka5mV8jhvIytTtYdflOcoZQsCEPh4zXB0GK7nnaBJRS9IE5G1PAKWnXGkJAUEkdb5Pc/dutbEPLRnGJUJiEu+m1sd0/UEIyCU078El2tzihERio9usF1PMNpqCQhlD4zc1itX6Rd5RITiAyLX7kZjohdXkqT+j4HcevZlm+SEpqXZGCGk98igbm58Sk3AiVzTFBlWjl50MBDmjso0J3AIfjRqofx+z7U1xkwNF/NMOaP7ygTE5QJGQYUUp9aM8kSWuZtRbOqRkpUKha2D88gCsTPOrErdIuJeJyG09BbF/jLOGdaWmYlmYVASwiJ/dV3jsMHD0m5dR5Rd0xCwwHA1AEUw6jwYc5FWWXaz2sL7ndDjQUC0KYUhg/OvFRuh6Lw8KbnT/p7c7SG0HohXm0Qoxx0R3aC+9zf96In+GcmYyW95w+6/b+Kz7819mejZ/nvjt+NCWnVDLL2H90vTN+t9afx/irschw8FuiM6IHogTD41PR8ZZYV6exeLar8IooCHb5p+mpqpWcbjtkLjvEWEbNLOKPqpgcELuh73nXKG3sWiHZ6BQcLwvXx/QDxeeyQHrJravfcChRH82DqjbyZco2+Wvyktw7JyTqkPPfd8xuM6FOgiTjQL7YSWjDK1UTx+LC/sUzQak8X0KTyMf2u7YrUKofrYMC0I+K+11mwlJMOhmEBQ+Jupvqwj+BD3ItXkUQG/W3nYCPhV+6FbS7IbwmB0kbe8HYihRDIMhnn5eRsJ4gYCxhZGr3H8bPdQ1u4m+fzctMQYnIAYDC6tJq/MyqFmZGcbEHj4KlKYcV21Omsd4yY1BMbYHpIxftdrwog3qVEblxy5rWIobyGdCfpgWwSWMYCxku6XkNcQypjO9sv4HOkidelDkUeV0XBFGULpJBnfJ2g7aiyyDsNiR5jG+54cCDfzfTdLm2GtSp/BZ7mI1rbpsEXY1/WqaExZA5Rf5CYmaEf0SDfNP7IRl45VmlwSfQDEMoGA8GNosCyzYs4fXicRrpVcn/xjYdw+ekLEEt9u9jSkaCWRR1q+myBPwPA/2CFofOyDee7peyiuVUQE/Q19gRZpFhaWD+u9+h7MkoI40zpq+LqmuS+sEplSvF7QB99WM1Sh3hDQS0isRRg9sRCsH7OU1X4NTlyAsPwDVRDHsJHhUxSJb0RadKYuusfyg8v7sE7ClMLnNAVOUS7/agxJJ4nYsr5KUq4xEhCLl3jc8KaytPY0pBA2J3LJBz8kBKjOThDeFymytcye4gSKU2dWM9v4fhTC4emV+GanrRCAqEjkpqytEJekbIZgXgfZg0BAIsj4BcUBW9tzr4iDCGC0qw1VHYYnUUooj6/FA4xqplqqIag2BPTVpnsRHQ4thu6EOPjB0Fk6ys6YvGZv2tV33TI9gi/n3tvbKJcE4QHxVI3jmGUn/RjoaG/i8AdxPDgeaSyGRwAAAABJRU5ErkJggg==","l":"iVBORw0KGgoAAAANSUhEUgAAAEEAAABACAMAAAByderSAAAAflBMVEUAAAAiHBkPT6UQL2RRKAXloBPq7OxUXGFxcXEGN4qXo6syTGaaYxH///9Sc435yJBqSBlvjKItLCxYWFjgpWWioqImJiZMTEylt8bluYuqjWAMXcSzxtWZeFjHegqxiCwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMan5hAAAAIHRSTlMA9/7+/v79/Qn+/v7+Bf7++/6dV/4WX5b9/v7//f/+/rfaND8AAARgSURBVHjanVeLkqsqEHQERQERfOf9/395e1CTbO0mcu5UNru1FTo9D6bbLPsUVVaN41jxHz0Rjfj9L1FV71CTKlWX/c+oGrxN57JUffNvJKYMzAUJMWd9Nt3K8kTp35plfaeUWJYQ8jwXgrpOlSAxJgM0OH/hw3vUpIGgEyvRMMDihHMvBMEUUhEiA+3UDwqi/AeELJvBwOTvsQIkI0ykg8kvvxiU5a1rEmoweuWl+UFBbQAn6o6nEh+gtiiovvxKoWzPJKpDiCobydvCYo5ixGRUuwHUIqEQVadywRD61GoMlYgsTmXLDOqaskMSuEGXmgorFpReB2UIPC6CeMBrIEzHjVCXnBF4BhcdXHDIpa63pAQQDmtJDFBIIqNIGCAERSBCMUwCB0CQtfxpKSN151wAF4y4McYdc8BIgwGhH2QlB3+xi8EIeUIls4qQgrc3S1JuGMoZDoFXPyZlIckO1kt6YezRJ6003qmDx9tN0npe7kjd5536nlszE7U3t6hh2Ai8EJrvF+J1u+k0XJZhwBji2PlMhxz6afqB1fv2fLm2QzugJHJoaS/Hx3tFSnX9mzT4U3vK9Ybgh+GJ8DEJ4jX8ojHdWkCc8OOBMLSetnpyL/4mQdfwDjHi1KnlQDdQTno1tP/IIbiyfGXZq3aDaAfL47FGl33MotMMod+U9YYiII02zsUzmm87Qbvw3MTQi/m8EogzrdSagao+yubYMYSGsr6vW+/5PP69aGdonqvmyzw1nIdZShqf2xJjRUJKfY2LBjTmo/UKCBBBqs1+yXnFBM1yCQxsu/7bvUYXqFxcuG4mA2lw/lbpEPf8FXkYag43LKq57B2fyVwHiVWrI0BwINQfLBeUQisdW4oPKoPCem0C8oAKo5TOdIfbTV+DWkIkOykRwrpdDdOAkJoDBKSOUlyduyouerdu5wLhia2AQh2OrVy/lYL/xPFHsUXc3JDBBB/WKeShHfWU8fc/VA3lVw/e3QTN3Fv9zfygFI5nuGAAxc4Br1oBQhIUbzzWioq0FsY/LJ2LqNpFAYRacCJ1niLc2Yi255AsX6hoIFYEsAAHqOGRg2mqcWb6FnpRRO9hIoJgEuwCDuWC/csQRdcPKvonILDkC+ELmICUNMYoulLaoo4UUMNoaUHCC7al1UE3mARaaYGALhTPyDeEXCQ4Od6azCFW8TdCl2Llsp4LyYMgBNdhjRUhRfx5cGeSheEC1k8EtHO1ZAmWNF4QkBBsnWIvGOHuKSIkOZBYCh9P7hziRK0IoktBqKD/G4mIsE71ipBIoomjqXYHqO53flrKdxJpDymdvBd2xWCDS3NOdbyqueizxCBpbXG/3y2uiTArAgN8F47XeHNPhWTdwnFetDnxhIBMlvq0NpHkXSOcXrDqUQWsOUo+jWflTp0f0G1wUIs2xJLV983z+TVlpoazZwP0QBUCZFOkP6zuCGxIh7NF+CPz8PfCnKaJm2HZArCHOpS8P58Z3+3Ld4D/AOrfPGsuT+qgAAAAAElFTkSuQmCC"},"mage":{"d":"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","u":"iVBORw0KGgoAAAANSUhEUgAAAGwAAABACAMAAADI1mKyAAAAflBMVEUAAABxLJLkYgQlFyJSIGvtlhNZKQyoUAs1FFCEN6BpaWmROAX///+KRKIoKiyjX1iRINxkRyCZmZnYplxPVVLy0ZleV1gZIVohKSuxhR7jcfHtx2EzOERGR0agoKD29vcAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAChgyMpAAAAIHRSTlMA/v7z/v78/v3+Ef4F/qT+/vgU/lr+/h9k/hL+pJb//oOhPA8AAASnSURBVHjatZmLdts2DIZFggSoi0VTkm+x077/W+4HaTtyt6XrRKNJ7OT06MsPAiCANM0fWkdqn13zZhvwj8gZmCO8fzNrJLdrjWl3u50bp/dK+wytsmCghTf7kdqdKdaqtOadriTzYGVp1HTdG1ntk/WQ9iZbyCnLfdFaepOwYSkhb4JbSaP6IPz+xIXhQlDOU9r5HcIunFHOkXs9taF+QLLvKQAV9OsK1lLWXdWLAzE3Y66J7iUed+E8vMA+tuPGWcrRDeMKhlRrzSpEhhoO7ZZ0uIxLVrBWBmmmpaWuH0fx16tnfei4OrICe03s7dpG8o0XQo3/JPeEtfkTfpyytHxYp2nzuQ3kvU+nppkooICsHQleQBnpPrp1Um6riUKclkFPzL2iNPc4ZcLHMx4/tgCH8zidbx2KY/gVBl8Gz+UW7Rqz25tqRZ/DK6ttTQgXa7210GX2+5970IbtcTItRJZXsDZfAeHiQ2iN9wNgPwGroc16L9ZaSccHDx49utlb0R+0VoUV2xaPKFfWa2FkeMyn5NTC8Sge/uvLdeBd2GecMZvyrWvECyIetwvj+bB5PlxnX1Ch6PQ2tQDhw9BGFyoqyyHu/d3gU/dAwezV7fdI8r2/bGkWJnmwnq8pwXthHZkeDvYmeoSKp2/d9H0Ojtb81nY+JGCu7Gek3f+GIZflP7Bsc2Sb3cv0XaM8/S4+EPTfotoWLC1qVy9MW4tj9++4FrltveQCg1caK5RiqBMr+dGvJCuK0P+R7OFqtZHcqizHCbMXDfj2YcDAb9zbDMOZITjOlXqeXnpWQ+GCTJtB3Iv0lksHBlatFplQL+DKO1FNJFN7a1O59QQNSZXum1QMPpR3N2Xb/FP+kWF8q9Gyds2NbXm0vX9dvYE0nNoPtrU6VsIj5QsmTyIugr63/REwoW6oA0PQsV7Jv6hSmO17wI4ilca1rhtF+IuVYc9vuaejHtlYr1NlzbKVOKvXTP5kFKmGhU/VhgsiKU774hWYWCY6Homp3vB0Bqx/MFZUTWrjRKIktHRDlXULusb+AfNrWI+QN2a+HA6HWqHfndGBaAmxd2f6R5ygojgTf0SFVVr5jPjtqYeKDHzGJKQyYQKOs8SsrIKdGolotKnXc+u/wh9wHX+NwYnFGSW/hhdP6RoxwASoUMBDFso+hOVGKCaRSl48kb9GXRUQaHSnAYVROwRSGPqB+6C2PRZHvqBRdYRHA1EciccHHdhUmkSultJaG5M+3mRfaqHEN6GMa/nFYZ1QbbhOc6R7Wx9yZ7wan7CvcCiP1WC4O2eh0my7PCqt2uF8lNxMXaXCuOCwXhYuL0bajldcTzgj/8xq2xjR6Fx4qcbiGIW8xPg3VsTsFAP7dKu1nj6hZYSyOM8xrg8rZpSjhHmCaoX+NPZebiG4XdRZMO60R9W3PiIRbrg4/SXVy7MxJeT2DVESIsbP+QBJ9yzXJatchLt6S6xlzEu54PIiULM8h6ejT0QrJ6bPqerCDI1aXrs4WgbYGMrkC2VohVL9XXtOaFe6bL3iyh9lmoVxvdRe3+KI4LlQdoyjVpQsDQ3Kqam/Bqfy56x8PFPZ4/7RdvMvYc4zvgFPXX8AAAAASUVORK5CYII=","r":"iVBORw0KGgoAAAANSUhEUgAAADwAAABACAMAAABxwuT6AAAAflBMVEUAAABzLZUnFiLfYwVRH2lbKg+lUw/mlhQ0E0+DOZ7coWEpLi6ROAVpaWn///+cX1prTB330JxiWVWUIOKOR6RSW1gjKiirjl+XmZebppvhafD2zFXMck/ftJA3S1b4+PkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADP/9p9AAAAIHRSTlMA/vX+/f3+/v7+/qL+EQb+/f7+/v1Xbf7/FxH+/v9c/kExBoMAAAS/SURBVHjalZeJeuogEIXDwAyELGBcotXevv9b3jMQra1raT9tjX8OszAzaZo7a2hI13Zo/ryGLQWD5Sj8ER+aLTmzaU3bbjbg9ZN319gcyAEtC7ij8S/SnduY84K66/5Ab6k13wva4X2DselrGLR53+yuCrsr2tHhLXrsyCjs3LX2G2bj5l1HrgbYme/Nb9qXGx9xfRF0IVybbV6bjYsVcS5U/fb+xvWvj1u4MJAlUqPbb75tXXN4ZTYFJTsKy/7bSrbqhaFu/KP5GIc7NjeSu67DX4AXslXSwZCJcWF4Znfm8oY8WYQLSfPKex/z7iJzaG7N7mIu7zsqOVJ3uz/6ePRexJLuexg/bt0FZMcrKXZ1oSaYcxmYtyJOVykPtKvurnZe7tIJvuhJs0y/asKUAVrm8p8usV68RdBMY0zzI/Jb9o1nvTFMDiGJaipZvOat/iAY0jXGfX2tjfsRecJ9i9HkQoaKZfW6Wg7Oi+45UJDJrMGu12b7cQWzEKu7B2KxtuclzVQVJPf6ifN+Y9Z1ues477pix0ACsqJKqiYzvG0t9mYbvaMKA77jdRbmUFXxZQYIf0dW2DbN3q2VNuaH8uI+nGhaDnPR7AFZCENZ1CPTDN2C53vV4Ix6VVVU+hRj6m0/Keynuu8Q6aYclGPZioapF1Fd9RTFmQvbTHLkPIc1RU+/dUM5EYrqRgsrQiECnpaTJ/Z4lPno+bqijwuriVS9ywSaY95PzSotbDOx1czLyo4/yn1BiaQ6KM2A+8+4ioD3Z5jUG7T796s6kLPitR4UXWjaorxSeLUo78VHRGE73jYLsXryyZYFJ/fW94B1pfOu48qXRPyN7zSHPHGFhTNgKxU+K6tNWlZu12FEIVqElcZWhIt0PtvMiBFtH1XikS9wjwRjOcXPz2m6OAa3HB5U/lIVFhZfQ6JPyJHprIsksfSkDLL/3rULqCQxUbq6+ki4af41XBOr5IhLyaGGz8mdlffs+Ulr73tZIsWSkcYJlRB1YqGnXrqhGR/2Z0RHf3GqUYYCrJ4T582iR/Z+mM5XEdkiXM51a9D1KE/RTHsVxx62D5t1d3aX9HUWM8FsNizI2cB5QkGih112KDH2JcTnPg95lwXzVcwp5um77dw4G7CeJ0EFWLo1lW6RMW5EPsW4mrsncL/AS7eDyY7pM04uRsnyuZqfKWux7LX01FJGaBonl3h23p44rVayeziWUK9lu1dhY5LCum2CqBPc+LQ68eMm/Q/VFlVT3ZWQl8mQtkwkKk4i+g2nJ2EekJ+EfqGs9cdjTBkDYYtDSK7XzkHdq+ETOhByMu8dWoZ2Dtbd40PuXo6uKPxBZRBUvGIOi4xYaW17DUMbAxVpAUGt01hHtFbaf2luvpxCS14EjjFm1mAnT+Cn6Q3hobBaRLlmGZIyGJthMr2SPaD01zHEhaXnIb3MhNYv9Ep4x1YuM0wRhjJeUEVjfmVyp7OMSHIX9jNSOiHzkNzdq6G7JASaWcJQkRLqLgKFUYb0lt07zyh6A/A4gD6JRhjdgPJrby9G0XlBU5/MUM7nJxXoFz6W6ReOw0Nh6KAsnrfvP2Et0+/G4WDtdC/bt58Mi3ItgpDWsfJPz4Xj9mJ4fbK61f0PKbw5ikeCjrEAAAAASUVORK5CYII=","l":"iVBORw0KGgoAAAANSUhEUgAAADkAAABACAMAAACX6y++AAAAflBMVEUAAAByLZUlFiFRIGikVRHfYQVbLBE0ElCDOJ////9paWmiY13klhX81aKSOQMoKSjbn2RpSiGTIt9lXlxNU1aNRaYlLyjesIz2zGeTp52mkF54jqW1hxb/AP8AVf+dl5oAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADo62N9AAAAIHRSTlMA/vb+/v78/v4EEf79/v6h/v3+/ln9bf/+Gv4i/QH//+f2kEsAAAShSURBVHjalZeJVuMwDEVjWZGTOIvdpElbYPj/v5wnZ+kCaYvhwKH0RtuTrGbZ0zNkpCc7Z387Q+esORZHY2ks/0RaWxyPxhiwLnsbrbIzmcTpORb2DxY7vN+YDaV3uTKzm8WE2u5NcqRiBWfLBQ1vBTmQBVgUM2UTT+e3smNTUouZpYRaqt5IsJ1dXVDnbDLtRjz1+aHZzcUklGDnkF31G3r1pEIlzV16iKzVaJ8muNQgC1PcoZbIgTXujvy6WvtafNX3r3lVzlrKOnKOaM/X9PqZpLe2T+ksNpswNo4SH6ivqzlVHdc4pwu5hVWjrtOnShzuqfscSc5tXp9O9WkSxLagpP+U+lG95ex+5zQUzYW1kbnO6xA1LylQ1GOMJ3kA9b/ww2r9jsfkIRwkyfO6Zk5PQla7TzjC463YzAHHZB2eTBrdJlcRzuucRdmuHKTOHtw94PtgTO8svBXQdktrMqwsbH5IzbXcqGgwhywZPWZARd9HSTOr8qRlvNadkVoSyVYSv5KzsGkQKSEvuTDnopYX1ok+LysHZH+46aiZNOZg+yzrWW1OnFwUBL2xTD+1Y5PBw8HHVM289cyt2lZ2aRIErJPzXnzxYtTiwdZKRjgaQoAY9LR37KNiJfTO9J9Uc5/+hLOeiBOLHy18tqsEH7qYT4FDw/nyTKEpNI5msygog00aRpof7gASaPxUS78+amr+Nf1lQopZtZB+JcMYJ7eTqNRrByytZB9ATuoFkuUjfOa2pSRo48avO7MfQEO9qLn3Dcim0dK0HGKe4kVfQ2DufpykzmuajQxNOkELFPycZE4dBHZ8HLAa0xq/b6bZZJ1LFE6kaopQ0+FRDWVGdVh7T92FSUiwJRXiXFnqurEsfw6hkm7k1fspBM+SywLCPO/Oy4H42u89XBML/2l1Nq/b/UHLN5runfdQubbrgiLm3YtW8qu7mJoou/Xeps5ODqMBqp1rZXO3j1GwIniOFipeUwQd7ZBbjvrDMcqFvY4fuLuhKMmvIAYUzyOcDkaH1tzWGGrtEil3e9enDq+eBVeesMV2YZdrrF3ze51B9zIaY983wqFwKBCuQeuWhm5TUTVF9PHrWtRNDWZBnLyhiK5AiOY6+2Z0j7w0k/gYvO2/LxDCfFFrktqE4sceyQ2c5U9PAXHqVeIWo23yt5UdsqwkNJGjeOJPJuhgWw5EZyG6RcY9FUXW4c4oRLRprfDeB+PVqKTZ/XtVSiw7Wheohhx7rQdzOJ1yFT4E1HZDtb/TkF4paH/H31ic4KS/RPbqJ74+nqwlZ1ihHoJzl2+16QQCRscQ/B+erV+VCpB0Vs/eWgpBZ4OOIHqx7lXqrpIxzDdvVBAFwiJUPtuqkSO0Uhahok8/X2J6GdoZfbHVamHYFnTxZttPTWKle7q3dRxYau/pMvnrPq0g6jnsFgWvx3BCDTzEcIIKlvXNW1/n0MNzEkuMpA83yE3NgAuPhYx1a8HHnfLJKh0BDljirWaGw1RjqdE00cs1HGrXGwBrvMLz6bpz+XINLxNZKujWa3qGqurl5xvVp/aYfsz5ecG/tJx6U3ub/kJWGKC0nd0PGv8BO00221MRPgAAAAAASUVORK5CYII="},"warrior":{"d":"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","u":"iVBORw0KGgoAAAANSUhEUgAAAGcAAABACAMAAAAwNplFAAAAflBMVEUAAAAiHhxVMRlXWl6Ym55paWk6PUVxRin///88QUcrKyuYVytUV1ff4OKhoaHejlEmJihKSkrJcTS5u8J3e4OIbFa+wcY7REw4Qkibm5t9gHx6gIhzmZmog2i7wL/Gw7wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABt4S69AAAAIHRSTlMA/P79/hH+/gf+oP5h/R7+ZZX+/v7//x5gQv//FP///6l8iZ0AAATASURBVHjavZiHdusoEEA9DAgERLZckld3//8vdwqoPMcJcrSPc+KiWFxNH+ZwaFnOHQ6B/r5HiO7w/yzeN0Om12EEgMjXQo7Ec7uTCJAJJiuEzDy6sCuHVeZ43wFWaww7SwOuStItOYQJe6rsRJIoB9fyRBCj7ch5uLLbz0QfcL7vp7egPvBgjWEvE+U4wIdr2McbInyydhJo5nSLT930ZSc/cBnuQYsw2s+xP1Zc3i3pZPgboPyZG5C//R3O+Fyh+TNMP1PbsFvi2d887sRr7Qbuk0A9bY3TMEaUFeNpoUk3foiJ49Z0GcC+yLIWV61GNOYBhK5v11mI9uXVXizTLMRQ0mN4g2N6xEnwbWuPQBqCiwp0uVwEVP3gMecMpycqDVh7vV7t5YWEmkFvkM4POCmZX5t7nhAGSZBdj4SxCCwjs76ZlB6ozeTwVOHEjlaPQsJu4Gc9FYO/x+HLm0lsoCtzyLVBQNQHDhEQ+4de3aNUoG2qI71dexangBBDABYNe3xA4X9uADnKYpk2BawcAYGLaCWe6LsRRZWl3Rxj/mXQ0Ao6oaXEC3RP32lS6IBZoC7R4w+Q3ScMv1gB0SuMrbXhxDcAUnxC4aBwdJ+eH8AYz/t7zxh5RRFJfttqGjE35R2Ecm8HwlFbkVQoiFT8TmH8APLjvr3Wiff+JHGUwE94ucgHKBgGGQUl7+Xdiid27HWNFqKA1O1JEuRbrSQcVZtQihukYhsNHlQT4ZZmsdZNNUwB0U4/sIQky0ZG8kLoNUhBKe0nLrdonwjzoqZdVQDRIYOKadZV1TViKEbnYET7D8EOJbmo8ftecEb9rn+2epNnY9eX29lStJs/ckWYq5zKMsVpyaXn89nEZut0jOHs1pXG1ni4HY/H87yn8QowC5I502/oaVpSD8VzZkfraVHa6Sb9HWUPUy1jzGylCkqJQOdz06kBJIF1ZTHH5XyqHHpYShV165pRlWNVbefUxpGGoC+YnrZiHVCbkxhDm3Dye2/xdaqBUgWb5Hm9vCIs5Cnx5FmaZGbXph7Faq2F1fnbNHEy5c/L5AYkT8eTlV8UUKSUgjESoZIeJEFMBqpO0cAZgGsMgYTErgBTyjOrIo01P99V8pb8FiJS48ZlBsTjOIzy3MF/ekBt5RwGrHrvud0hFsjwK8QYaYgzqDQSor4UOrOWKLbkUYqxCrJXVopWnbw+OPLmhWI8/NEKt3UI9KsofYCifuo7lpNAVkjS8iYo/rySp73hGWEmKS5W66lTKYdLnJfPyyyX9YzWJpOEh7S8VtreqaLzjsgZNM164/Rn5szwtqF949lXhVBzHZSeVRr0UDheZZsiqHjFyelAsLE6RFnDLGUuxU37AuTSgMIBPyftyvzC4XRYnLir3ny677Q5WfD0Kjx34HaHYXU+SOwIySynSGFUx3+x+JVZAmuIwslMpa7aBcU3ZBg7jhp07Q3wnThUAY3//TupNfQ8rpq63aSLE+NzjuJkEp6dNNMskTvuK/YCqiKSPfB2k64n1qT/zFloyVn4sLYbFLnmfs43OPe8dYJEqU9FbUMtJKWRU10+GtlsOhqb5ZlnOYmZLsZpcuK+yPFeDwhVRaHUVwXFPYbK745c11dbJwj/AeIiLQ3h889gAAAAAElFTkSuQmCC","r":"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","l":"iVBORw0KGgoAAAANSUhEUgAAAFUAAABACAMAAABcMMtAAAAAflBMVEUAAAAhHh1VWFxSMRqeoabdonFjY2NoSDA5PUT///88Q0zZ2duUWTBXV1eecU4qLConKiqbm5u0h2Jjd4ZMTEzstYV5gom1ucFye5Wfr98uSVu9wcbDekO+wb+RkY7Ddj4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC7roijAAAAIHRSTlMA+/3+/v4S/f4H/v3+Xv6iYRz+/57+/v4dEBz+/v5I/xGg12AAAASeSURBVHjarZjpcqMwEIQZSSAEQuLwkWSv93/L7RkJYjvZWAuZqvyIy/7ctOaSq6oomqqaTjRVTVN9T2TQSETTdzFX9ImYCsXjOH4HcBjwRxJuEPg3YB3FqiJSoMXEJkh37hg10ikrXWM4qnOQQyJq77Bxcke0DpSo99AUU9XsfnoJeNqqB+iwP8viP6GnqjpIRXQfDBiaow6sB9Y9GFtam819nUf6KoadTcE9cFpqb9LhVJRdE1t1//XjA/QmitsBKcTdu3NVhXkOmnZWmOoRDF57qkBtLWE/2Fp4XGQQfW9UbATaMDMkaPjktMpSIKrLyt2k1mdG1vUn0MJWMFK3clXl2FVFrBOW1vMDM5b6OrSqa3FigqXGVVM0qk4ywwexBSUbub9DWttxGjDWsLdILIHqORtr388sPjVgVEahN0OqUEldTa8Gh0ojPnsV/FmSINyeWoGh6hdYbaskOhIP5AQtOKqH1rNCjmn/Tm0K2rMiw+AUBBP69DE6z6CmhA1eb1qLyhV95GI2sYoWczX4YMPpamfJ2WBr79diGIr2iAFJBYUqYbtWxEKOBvBc5wjhvW5L2pWTnsenn00gZIFQ65vIg6GEygn9O8q7WWFSi+6kDPGQtXV6/jMXg0XOyXfiC7/GUs/vEhBbwEeF/weIj5IeoT4j6tljFhhOZcNJ8qwNUt//wDG10o8x9YzKvDXrtLU5+QFFjzD8jqcO9NdeUQdqx3OpNZA83Dqep5WmC/vem8tFJexX4JPqr4Y6lFWbxwfq6iXvrEi55A9pTaaTYr6wS90TKsTyGYHZJbWcCGhzr6/8MZcPXXPhgr4scLUr2F2gtV+oFa2iVnXa62SdE5vxjzaimFvPD6jtnmWBG1VvrquzaUnRFiBvm2wAZ5mW02LmwsznLUv1yQMOQCbefBN2cCcr/S8rzudfshm7COyVm4BIlc06cP8XfR55qiyflv6fndABayQLOQ0ULzpvVOuZm5M2M3nAlWVqwuow8zc9ywGepYJduKvwGb2Rr2tfM0alR6dNaJi52fiS/XUQayUVaXLNRNxUbNieWiWjma5llP+hl4L+OuZahA/YTKvfeE6IJS3PmhRL+HCetWWtb2VLJXMZLBWOTcD+sZxiyFwpKVpdPeMRfnp6LVtfR6SmgLmtVtu1RXuuKohFw26X5dJZfkFT6W3DjaOUPGYrY+OG9XmD7zrMnYW9PXA7gtzYrFhZ482C6PIoKNyI3YdLNl6aElbzfEVtLSiU/6N+nssu9db5PPPBLRkKB5pjF851hw3cc7YLh6vcfqmNdFZ0A+T+bPXt8npAbNSKuVom+KxpK954ADox0vL8xdrys/apaHXR8vaFVJmuUq/Wo6IkV/XR+/yUTLR27Vso4bXTuANa/cOKLZ2BjlkQQ3jHxqR2rbX9Fgw3g2WqTlmulvUp7v2BRK6LNqfpC7ucOu3SoTXGvVD8uoI+HqwIHt3EG0hq4Fd0zJ3V5W5/HOK032722EkvO8W6kw5hm60E4RhaIfnBW9G+6hplSFudwVBu5/4cfDJkJ5SpWt9oxSbrZ6/tWrI7td7nv7v/Ce7LResvwmEu7dfTzt4AAAAASUVORK5CYII="},"sister":{"d":"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","u":"iVBORw0KGgoAAAANSUhEUgAAAEgAAABACAMAAACOYMEYAAAAflBMVEUAAAD7+ffloCHp0qQoIBadZhVmSiLMtphOMQ+qkGndrVpoYVn6ymJtbW3///8kJCSuopOTd1ZPUlgeISOZmaHEeh65hzJLSkh4h5YxTmxoeIecquMAAP+z5v8zPUYqKmMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAArlNkkAAAAIHRSTlMA/v7+8P79/vv+/vr+Dgak/v9XZR7+/5gRGiASAgppElGtLYIAAARkSURBVHjarZiJVus4DEBtS5a3xM7SDRge//+XIzkp7WPituGMDktJm4t2yVGqKb1SeZoSVknTdOC/1e9kLinpVVI6zxW+X/qSBn0nQyq/4kz4F2ZB9ft12uCw0GEfJav3TY4e8LLXz5scrf2fXW5WfdrmaIfv/4tCGvBtD+dQmqA05f51y+aWZWJb3uGipmUC2uPrWbVBfkfBZaIWBwKV10Ef3tgGKFi7B4TG2LBl1zjuAv3DIBNNdH+Xx2gjK7Qb1HXArDCOIYSRf50i8NVfgEyMzALoIiwSzQ9QTcz8KIifFSSsDrrOgOmMfC2g/0TtASknEttWnaIzlbcI4RV06KUZ54yq3aRS8sTe7kQMa8TAuCjkvS+rXYX+FCL0gVTfKL+JnHOJIy22gejSyU9rE6Uh4NpvS/QWOhsjfrYacBkAarwpccysyDgSySxwAdK89mLDH+IgHLFMDcuKA30bQ4tIeciNAUIFHZDLCMyJvWf9uQXSsFmvVc0K+jgrr3XHWhL7zzc0ygVgu2IrLcyZ7bJRA5cRnIhVwsbEK60WUkHjLC2UPRSIMaOPklx+s/Mj6KZG2g3YZ5XRdJFGZnhJCtwyLmO7P1a1iLNQ8f2dJXNEK5yP7XTUD0EwFPV5RuJ8PXnmtBRSX00QLN52fF9WaDtYqo9zfaNI+jw3+6wTb4PjlMzibhslVb2lRhZh06qhOi/AgJnzkYxFZLO4/DZGncTjzo4fGoVFL0hfyoOOHpHwILvcVhqd0wMQLLaFktkwTsRpmpq96Br8jVxywenV3bmwRbbuJpvLV//5HfxNjVYQkMJoEMt7o4H06s0baIL0CnLBoAWL06ONOMFK2iqTUM121gACu/rRynfBwL26oRCDpCUxZyRWaM6P9od3DJyw0W3W7RAGJ32X0HUen2wiTOLOZ45hA8WJzBjnsSr0fDGmINPQxh+VFs1pjOAIBx5Xvjxfb7hR85g51dnKNy4io4S3LC6IBBq8FPyTFbBIzTo+y5CT7n4V8TIPMUzsb+h8sM80mn3XcXjYPOLvcBcwizTwRR5Q0fpI+NRD7CD+mFvVuo4jXzV0/B+kQx/Bz4+PDh8lWYKI46JGndFVjK3JyOYR1rQ+PPKRBJ/HhKe1/Qdzk9N6KSWyfsTH67bUGseXR7W+A9Xpb8K6v3k0Hufp2WL8hmIXra0kxDvQmqAjdTg9P15d1BhH8utNww3UrRoBeIv9C7t6PrOHrj1JNKqrA9xAOj3PRVkfBYbuTqPIO2SM3yAdnXrpQMr5WpK+hijGm2nXSzq8tNfmeuhzdaHlDiYMkELpbKirrSzfaX5pOeYzVh0WAqrOXtZH7mUg/uKqSa+dkHr06buRwXFZQrkprj4KlJ6H/7po1bqqtRbZGBGQMl7O2cjR/3rtuCYolIci3HbSOIhIB5FHJPzGIau3V0/9cjLIs7dS6FjmuaCv+wL+6ikLg45HS3KsvqCV1wto50ONvj8sD3wKP3u4nJfXql3y/wIt1DRfO3vZ3QAAAABJRU5ErkJggg==","r":"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","l":"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"}};for(const k in D){CHARIMG[k]={};for(const dd in D[k]){const im=new Image();im.src="data:image/png;base64,"+D[k][dd];CHARIMG[k][dd]=im;}}})();
/*IMG_EMBED_END*/
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
// 強敵・中ボスは画像(ENEMYIMG)に置き換え済み（_embed_enemies.py が 敵素材/ から生成）

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
function makeStone(seed){   // 洞窟の床（暗い石畳）
  const c=document.createElement('canvas'); c.width=TILE; c.height=TILE; const g=c.getContext('2d');
  g.fillStyle="#3a3a46"; g.fillRect(0,0,TILE,TILE);
  for(let i=0;i<22;i++){
    const cx=(i*97+seed*4137)%CELL, cy=(i*53+seed*911)%CELL;
    g.fillStyle=(i%3===0)?"#46465a":"#2e2e38";
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

const ENEMYIMG={};
(function(){const E={"rotcorpse":"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","soulflame":"iVBORw0KGgoAAAANSUhEUgAAADEAAABQCAMAAACH6m3RAAAAY1BMVEX//v76/f39/e7o+fzB9v3L5/iw8fyN5f1x3P1g2/1jy/tppu+Ag9c/hfNmZ9A7Z/cscPorYPFXU89MQb83S+Q6NsMoT+snO9dCIbswJb0qJMMjJsAOCj8AAM8AAQAAAAAAAACkuAGLAAAAIXRSTlP//////////////////////////////////////////wCfwdAhAAAE/ElEQVR42pVX7ZajKhDEoxGFEDUKXiEnmfd/ylsFaDL5mDX+muxadFV3dTeKn4+Pff/P4tP758V/iRhna/chpu0vP9tdiHmDLD7sQXRTtwrZF+M0uC2Cd/Ny3sGq2xDTMIVxf67GZXbD5PcjFg8AWO2rx8lbALoeIf7bhTjPc5jnoR/m+R2pN4hlxuMA8G8Br4glzM4NfT/Nm69c9xaxZJURYAw4rRXvhveIdOIJmiFBGSQq5JK79wikP1sDjHplkKnZ54p33aNHN4RzNmaWIXTb6B5BQPRuk8FNT4jehYigbNW2hhA7bvnqwPO3DuuIIC1k1kQIEhz8fPoQ48fO3epARNGyaZMWvzlleK7HdE7DwKPgAxGQ3zk3++WPCi5kTVq9bqWMxMDef0ag3IEm7DvdKimbBmUhxP4aRQ8ICwU+xAhNK6u6USA2IGWsi7evCFYb3nBdb9q2qatKKtUYepideN7kiB93rzr+e3K9UU0jq4hoDQ+xp5iQDBHObQ6EPYap6wGoNwDrOKbpFRJEdG7ICNQNGKOUqqtyRWyG9GsPi+j+MZIyLSEKlOqyjMpBakov2gkOsHflNiGYz95QRFlUdR1/wm4LmgcGdUm8SGli2Mkgq+BEGaUoKkTJNbSxqolWJ7ZaR1qNbFkLIERRI4pipzj2vckxImL07FDriahlI2siigLqE2ZwMY3enjPiDJJ+GS3am6IBqKtC4IH6mo5kBtH4fs0Vi42IAQCmqWItIqAQ+AsnNIopT5xsQqDYwzz1EN7UeL8oCxEhoizLpB/UHhCYaAgLh6hGyfX87Smpv22VHpgpjiiR/NTTsFRRFb8QSX/TghmCjOxIInC8GXSjmiT5AVKkKA1qOUAq5jEQcYzDDwoHgRPeeASUZealWPyZPSwWaKZdEQJ5FeKgD8VGqdCavKiEfoG3AhCeojFtomxR6Ku+h6gvY8WyxA7u2cJEzGAFWooI0LhcL4esvihP1+tRMEhL7fQYWDFX/dqoMNP1dj1lgJCX6+1YFvBxk4NQx08yplayriFTX2+3S1RSxBDXUYrsfN0zX0DQiFwAqEYpDhYvXTSMmELgxzEZP7Ki4eldG1gSeiohoCSSOt3445Rj6Gj7hPixdupUy+QeRrC6XlIMkkqIKoZI+yHPK+8wOBsol9RhEuJ4QZBTLWKb9FgHwdv7hEM7wSUHQbWXMnqlKI9ItSxoE9ho6ub7ZIg9Cysm7RctirWAtyMAlYRJOFXiMFlnCfyI/oCUotaHzbf6WNJWbZwpecTdNyemIXYAopR3++aWSoB71+a1zDZBo8vqUOauSs0hY7Wx4MbnG4CnJbEAU18RwTaXaQBNflletgFuVFMOk8YPZ69sNJfu42oT95vbuHAFYkDETqHkRAgKwvK81TqX11qYpoHtwhGUJqJbc/SM6NZVGJ0cLSOZVLyfJQ+vrPLmSZuwTYW+BxjWW+qbW1+8aGhZt6zzdssY8n3mHSJeQzG9WrMumfiYt3e4dMzIftGxI+bXG+8vRF6JabxgfuGakQrRfUTElNk05mNjQ8jzve+V1egJkNw0LN4/EGlh9QSw2CyG/ZNVSm7cCyaej3H+7/suZ5FGYqcQFmt33fTHgJY3w5Ob/rzpo+bGGBf2f91hrMaF7+3ur7vT+RwCbz3jN9+DC1I2ffcF+R8+ir5DYBOdv0IAMH73XRs+RPiDlf+E+B/D+F+5gh77YwAAAABJRU5ErkJggg==","gravespider":"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","greatswordsoldier":"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","infernolion":"iVBORw0KGgoAAAANSUhEUgAAAFYAAABQCAMAAAC00XLYAAAAY1BMVEX8+Mn101/fqjr5fhShf0v5XQreXCrhSBqTUStMSETbMhG2MiGjLB+RKxiGIRhKOS1dIg83MCwsLS0nJiUgHx6lDw1ZFQ80EwwvCQcWFxcPDw4QBwYDBAQABQwAAQAAAAAAAACPWjxhAAAAIXRSTlP//////////////////////////////////////////wCfwdAhAAAKt0lEQVR42rVZi3abOBS0eQkQEkJSdPWAOP//lTtX2KnTJm12T5f22NjGw2ju3Iecy9u/OYbhmxde3v4X3H8JK+b/A1b0/wPsMPd/H1bMjPq3tBXigSp+Rp3/O+zwQJr7thcfP/sN9T/BzvM7fiuGDx4b5vm/wj4ZCsqKp7ek/F38/gA7PxY+iL5t57rs+vAHV/wedhCnoNUFbdvPeDEw3CB+74rLV5Fq3q0qqgB92wB3eDtZSgRQ/geDDWd82ACzZAEYFSowqqz51t9FFv9KBHGuca6S9u3YMFk+q5LCF+fJ2+d2+ApWntLNTJRlTStw+aQuXlYXD8/p8j1tRV1iDRQfxuaRn5VRWPwwi7a6eJBz/23Ygdlw0HnNTUVNU77x83GYU4pKGvCn0t+CbWqmzs0wyxPW+Glype1DmlKFvWuBy8TwLSekR5w46ib27IAA1NUXRW5awbaiLnKeJfJZfstgNp9mrUtXmdqmGcgC1aU9bdNkD1PV7l/ND1v8EXba9h+BUqsF7Ilq0+6BurlMCJ0gk01f8+47ImCxgJUvNVLGb56a9oFqk1s3n8seRCC3JnO67cQVv4NNUwe2qHcwa2PcNCVqDTgyyVu3Zu9z2mwhv46ArVpw/P7A1m7dlHZIsIAIB39LsadtXTfr9+ynNYPw6o9tBOpuPqAOX8N6kPWdnE2B2aPtJpuyYaoupc3vXZdwK5vLigBmc2bfGbJfC/ozbDd5e5nb6KnviRV17sCj3xPDdZPLFrcq1jq/qypszXD5Sem9PCvLsH1LCFWEsm7b8N/aDFQEq4NEGQvIPuc9KgRggcleJKN+DWsrrL+2gIV022QtmE7eQdWJSXacaT5BZQhcPA7Yms3bfxK2d1iXAbtxgDmZ8rQhVuu0rX6vfk3QFtZIIMza2hpQUYXo51/7xEdY0OjJbYCx2zACeQDStm7O5xWwCFx2rAhssfkjsHMXZPNvDMYiICq7ivgO7ClGLN30G1CtS5nJgvMtwW9A3/AewQi9KnTCDl/A5gm421bgTpecUHbbVNt7B9ciTkwWyfbqGDV5iAG79EuhzdRM+0neJ1h8ATrYckM6+dY4DxMJj3vksgHUsl13B0FSxluM9gKhFWmIO88vXxnMVbpQ4Jacb3syQ9svUDXjbteuWz3IQh3HFtvJEJWEhDPpVfT9z9X8KR248DFfZHBOotUSRczBqFh/t3a1MmTnPGSBQ1I+9t1O44rqI8X88k72LGlPsM5XFXAUAixHOXm/YwmMyTTh1sSLWvF/4xvy423PBf8+zpHPbA/H5QrIRSiG7VUyxVfU1XEVwyrAlQ82x2G5CJXTKukHLPN9wG4UTCgJXwLGfhbyXpm+sO04+vkgYwzxFagVULdQSNwzauXJP02A72yTkVJHZCYSi2RbxwvZi4K7wPppJy2VkuHwziKKeQ960YU7UZVjet8Jidqzfxgs6hhCzFjtrhGElockISmBjPOFFh2KARD8ldhhBNywZ8sZ+GrfR6H7xPcEq+hWgJtjWCTGLoFqvihdMuogbIoPtTS75RxjYx83IzU8xkEEf3cOTo8K/CNkG6lQSryRVlIAV4pBqkWCZC6FPzFyiZCWCwfYZogmFWhwj3PpPrqzdAOkeC7jBhB7kMOyCMYdFi0HKZcQ9wJgaBt3j/oFt72WqPnOi6bDWU6+x74N2g2N+AC7x0BY6TAsgBOCUQd8MwTasQZEaE/JbgycCnSSy7JggRnxnN59UGfr4UkEiHSUcisGmEow3tAOiwkxBkM5EsG1HDBYYdsSyC5aMy7cA3XvRWzGJMBV/akwjqhQOQJ3GGAFqQfQDjteK00spkdk4FROCaxb4YrA2CEmv5dzQ4VJsqm4D1hrxMj+jKahABVg0gE+pmaMWvFXubhbr0fr9nwc+QiAjXydDnuxjx1BRe17+Q5LqldYYhivDfHqdFhMMdfrENWillCza0RF21Z3g8H1IgWCyW5Adj5MW1HZY+8iTEYi7cMSm2sTg9Y6lh2ojWFaWhMMihtz/Zo84soHgghx4e3g7mnLM9ZH2Lduh8fDoGkkpBlrdjNNYwisNKefd0T5rDUeRhAtwyosjG3mHhPhkMaPbDfuJByEcKvRWrShQmfEdTySW5gsYncgEdnZuILVwAXKROSDWNBOkif0lPl5tJsQMYWrjJJVM35ioxk4gYi7t0LAIoUzFVh/zhKUKNyWq1Vah9GZVhIBtmm68Sxi3pagQIFwxNtJBMs0sKxCvTPoH2smZLTEjM+LByxn+iJDqjMBcK1Rxw4RugaB6dgMDjN3MWWk0A2joXEMCgIYpB5qyyLoYGEtL4hzENHSBpWNYcGFcSfnbW3Ub5euu+JowNevGDXQWC5XiiGW7nLtSHMn5KaAdrOxsMkmqi6oljWvgSvSYky8V+2ETEH8Lkz22jBdRx4N4NZdLteVPJ6AG2OsXB0bYPU3LjrxXg+wEohQzyGufzREzzX9DtsB1h7I4G2veO56qbDI5FbFZC1ooN/mEoJhSy2sLMhyIYMG8MS+nbgU+AYnLNMFcDNRKgTcJk1XRiXDW12VMNZ71IQ1Ra2VOs26cHpFXalrXeKZE2+08AmH7ASGyM04jt2IwreVgslgJeJwtiMdm1A7N4WM/GO61cyIZTDLXQTSsVYGkkwXBuvGsbleQLjCoyTcOh9NvE0T8UuwpXKoPu5Qdi+1HCBcNfVgW8jB52gemoDr0OHusHe86xk85NZlAqkwTS2jDsJgOorwF5oPBXYto572ApjGGd8CBQoWRLWnjWEbpvo4oPEtEMy5CBmxswNXiRHEpddICV2dQTlvZb9o5gsVoAa8XRNOaRBnzmfILhUQ+BB4itrsXQo6dt3NjK0wMBiPXxC2BLYr+hsGCGBw5zBc7DTAA8olwokBpcK+3Y3LXmD77mAR01SPcb0Zwz274mIjwnYaQJYbHRd3TAAYdUxgzWJKSHlKj4nxjos8G7u3CevU1bvIv86UA6ULndWjJ5TXSpaPoUob8LYPFNjLeDVNx7Hn90F07Jjr2J3Zx22P1hrEbisB2YokB2P/xhrIU1q2lK7V0BZUtGD4JuXn+XZkv565ZwR/jQjbnBW2pNvubIZaYOG1ODf9SlXYUCcvlFxjqtD5y43/VvmADYLBIyE6Lrfw++wHVwxYVy9qYww01RmLUWvYyH8Oi2FUwSnKsE8QB/iHDV6zHVs9JQ2oodSCMYeIk9URJzMyglNi/3zPq1HmUeUC6hhfhZaTfzRQXoHm8ShQRFPS9ZZvidisWnG2mU9hXcCkxjwxw6kakfyodvxtCG7Odoi7Q6sKgukVc80L2gxX/PzZLtJ0KtCIOo+w1sHLvdnu/mFUI1Thm2kCb+AqfcP7tzC0c4HcAb+T6ONXWEtNM4fm2oeAE07y8j6vodhd25fIn0JSDKCMW9ke6LVzbPo5mGurPxHBGXxnurZ9HK/4iQcbiWzfhQ3NpZ+LuF57GTH29C8Qs6YTtr0zFTnrwif2M1jccm3EHMd2BrNQni5ar/jxq1zx+LJjmOol6bv5c8QWGXuTaT399ou22/yCDedL3FWPOSmmJ9RpbeY53ngU1Lfuit8monk26e9+ApoovqFUWDyUn66ZQv9CaRe4abndF5P/wp8zVloRVP2C2SWrl4Iekaa/8VeSCrJVYXL5zl9J/gGZpTyosBQzFQAAAABJRU5ErkJggg==","darkknight":"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","hellgeneral":"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","slime":"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","bat":"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","bone":"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","maoh":"iVBORw0KGgoAAAANSUhEUgAAAK0AAAB4CAMAAAC3goLtAAAAw1BMVEX//vrP2+HFtLidq82rkZ2Uir16htt5hI6JdqiBaJVtZb5daHx6VrBxU1dcUoJ8OaBnOn9ZP2g9RH5DQUc6M2c8MjllK5JaJ4tQI39EIHo/JlA+GW40HGoxE18yKUUxJzEvGkAjHWYcIUMgHjUjEVokEEsjISUgGyYbFyQbEyIWEiEQER4zC1UfDE0dC0gZCS8UCCwTBSkSBCgRDBgQBSALCxcLBBoIBwwFAwkJARcEAQoDAQQBAQEAATsAAQAAAAAAAAADF16OAAAAQXRSTlP/////////////////////////////////////////////////////////////////////////////////////ADBV7McAACc/SURBVHja1byJgqLY0jWaplOKoiKgMiNbBsEElGEzdZPv/1T/2phZlTX0UOc7p/teu7oqR13EjmHFisCnt//JYzr9nzzt0/8G7RvH/f8I7ez/g2hFS5bFn0CdrWY/9QRRlD3x30LrZfZ6KVreD3bdrcY/+fFIlNeCFXr/FlovKNTF2rK+84Gj8jPLeuJCIJkXeP+e34YZWS8+vz43nh5Pq2+d9oHdehH8yPt3o8wLrYUgf/18NeNOp8P0G2M/wC6Wfij+yznBynxR9D5nrt35rHOzt7fxxxceaEPRDLJ/O4N5RSguPsHluL1yPivAOHt4wTtYb70ws38drVwQYaFmn427119f9dU7Sm7/+Ndfsxj799AOvmmRwBdNOxyAf8BVznGm7x+hxu2l4euBaZoR8f41tGN21HLoq0vRZzlM5t+vglNeTfF2OrzD1T32XQup2QxI8O+ghWMijCyfBKJJfHbU/KR45IDVay2+5LGyG7IBZ/I8g3uxmXHDx/X8s2insxXHPEGGH6zVC86aC/nnoUrM9m4ZimJQJTEz7uwkP8vW2wtSnSiYoc/cgvtnbYvDPgwvGZLIVMmFmTqUJxMqvnFKBheV5cLMCnPxNpayOe9ZIA4ovKpN/Yqhnc24fw7tjFsrq4Gl8CQ018xtV6uLLE/mVFKKTBIiceQRYkbm2qTPE89j3/cCUSCtxyPQuNXqPb/9E2i5lf7ISyJfR0QVBm+ULQ/WlfuemMR7fpoEmRn0zXIAK79xUgVHIEU2YVmMO6y4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"};for(const k in E){const im=new Image();im.src="data:image/png;base64,"+E[k];ENEMYIMG[k]=im;}})();
const SPR={};
const NPCIMG={};
(function(){const D={"villager":"iVBORw0KGgoAAAANSUhEUgAAAFIAAACCCAMAAADizG+GAAAAk1BMVEX+/Oz+0Y/9x4f9w3/y5ZX2x4Xc0H3hsmDcnE21omCMn11apAJZpABRmAHFgDO3bCOidS2hYCGdWRuXWB+TVR14dlx+WCuJTRZ9SBZBgQEzbQNRUkgkVhV8QAtwOgplNgpeLARBOytDKxJCIwc1GQQcMg8bHBMeEgcPEQwTCAMKBwUFBAQBBg4AAAsBAAAAAAAAAACrb6aMAAAAMXRSTlP///////////////////////////////////////////////////////////////8AH5pOIQAAEqFJREFUeNqlmomCojAShkVRwINbQWnuI5B0k3n/p9u/ErTt6Wt2N3PYrfBZqSuVCqs/P43s7/HnH8bqx0/lX0P8/0ghWNe1zTK6bjjv99n/h5xlm+dh6KkRFHknhRz/J+TOcfY0zvPctVWSh0EQRFGc51XXdWyos/8eOQzDqMY8i7kriiRPgigOfJCTvBJ8+O+QZzkO0zjxZUy8K8o8z6M4ifwoCtK0nNh4xtdmZxryH5CZGHlV5PEykqKcurIsqyKiQRooirnGLLQP8PFX5LkWo6jyBNbw3CAM4hxSVRhltDCjvJhmwGYhBB95/RuyllNVFgnZA0gvCDHhJC0qqLPINTMGcppYW6VFmqZJB+2MPyEHObVlGi9I1wUSmLxKo6QsY9gohuELMZUQOyB++Nmv/kJm54Ez1hZFnhIVd0Wk0DSmV/olgbhF28JkqVJ1nJZVy4bh/JN5JJesaiGEAkYLa/kJA7YqcgqAmESOwxBMyX9A1rWAlLhLWziOguh9xEqxhWIqnvqt7brX8VtkDd+YEoXSwHcJ1e9pmhdpnJKU5KrQaMW4EFIO3+hyv4d1GEuS+M5T487DX+gYgZQUMD9cPybnmriU9e4783DZVaUXuHCeQClKS5vEwbMul2+hFzgRZl61zYdE+owUsstzIG0gIWocP+tR2+lJcFKsGvBNyYfa+YysMyHbOIIzukEU3F0oelestjD+JY9oxYj8NC+bTrzbfPXk5bOowsBWMga2GwShZiXJ+6SflJxoKOWnKCyeEv4TUiDGAmjS9xQy1P5D935EPqk0RnRRxksrIbK/kfXIpyj0bZ++mkLRi2O8xFEKEwcLIngi4s0kjJUp8XFK3nT+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","woman":"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","elder":"iVBORw0KGgoAAAANSUhEUgAAAF8AAACCCAMAAAAXMuQ2AAAAk1BMVEX////39vPq6urq3cXzzYf1xH3gxYPNysf8u3j8uXT8t2zgvXXtsmi4tbTLsHbtnUzVnFW0oX2vlWfYiDuqh0qseTmmZCaLgW+MZi90a1snbWuGUiJ9Sh9wTShFT0t0QhdnOhRcOBZYLw1EPjVDMRxCJQsgKy4iGxQgEgcPEA4PCAQIBgUDAwMAEx0AAAQAAAAAAACE+B8tAAAAMXRSTlP///////////////////////////////////////////////////////////////8AH5pOIQAAFJFJREFUeNqtmol2ozqzhT1gYxszmJlg5kGAsNXv/3R3l4SdpDtO5/R/tXp1Eg8fpVKpapdg9euHo/9i/OBrq5/yx5mLT+M+8/8vft70/TgP2afBpqlvmiaP/md+I2B75nna7n0cDl4Y0ix4/j/xk6bpWVvFsXM6HY6n0+l8onE87M6201RV102M9dG/8+dxFs7pSBYfjkcFPx7xB17Z7U6eN9xn8c/8iImxqRyYe5D8x1j+Op09r2Idy81/40/zVMXa7njSttrxA/4xaDa253mM9//EZ2JQHgf/9LT9KG0/LBeQ/HvzD/wkb9PwDLymadutpn3wzdNXtOIeQolN0X/ni8K2QThr6gIIoOPj3zv/iDW27U78V37eNzw4ng+nw4742uIXhX5fC/DPZ89Oq7z/b/yGj5OvAampsYQlXeS3lT7sbNsLuzn6MT/Hxp9E6tnk6dMyJP8D9UOw0hpXgjE2suQnfCYzWGrbytav8NJdy9/Et0v5nd/TxRf8nPMmSZq6CLC6v43PF1AXxKVhvpf2SRRFyfh5K6y+ChsRmWaUhbZtn07HL3bVgj+pQMXciB8m5tbUIyHG7/hmM/ZNout6lCTOwj8cHiFP0fh0lnKdzHfE96JI3+hmnjOefGM/g3PM1XplJplzVKTjmWayU6Y+FxthS8vq0Q6RfDfS6XtmL/Jv+LPITX1N/OAsXQGTgbfPJ/rzrAZdAT/wKv2n/O+57na1XpsXNjbRK/44JxdzhY+tLo2zO8rYtM/wjXYmUx+DLoG/D7LSnGX8YDiwf73emHozshd8LszVZr0hfuaflEfs81GWE9uzzx8ugD8PammOtuT7Mb672WKRm/c1Xv3uncQ0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"};for(const k in D){const im=new Image();im.src="data:image/png;base64,"+D[k];NPCIMG[k]=im;}})();
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
  SPR.stone=[makeStone(0),makeStone(1)];
  SPR.water=[makeWater(0),makeWater(1)];
  SPR.mageField=makeSprite(VILLAGER, npcPal("#8a4fc0"), CHAR);  // 救出シーン用の魔法使い
  SPR.slime=ENEMYIMG.slime;   // 初期3体も本格ドット絵画像に差し替え（旧:手描き makeSprite）
  SPR.bat=ENEMYIMG.bat;
  SPR.bone=ENEMYIMG.bone;
  SPR.rotcorpse=ENEMYIMG.rotcorpse;
  SPR.soulflame=ENEMYIMG.soulflame;
  SPR.gravespider=ENEMYIMG.gravespider;
  SPR.greatswordsoldier=ENEMYIMG.greatswordsoldier;
  SPR.infernolion=ENEMYIMG.infernolion;
  SPR.darkknight=ENEMYIMG.darkknight;
  SPR.hellgeneral=ENEMYIMG.hellgeneral;
  SPR.maoh=ENEMYIMG.maoh;
}

// ====== ゲームデータ（後でAPI配信へ差し替えやすいよう分離） ======
// マップは maps.json に分離し、起動時に /maps.json から読み込む（下の MAPS）。
// 記号: . 草地 / T 木 / W 水 / R 岩 / H 家 / P 開始 / G,C,N,B,D,K=出入口 / E=救出 / Z=玉座 / c=赤絨毯 / f,l,o,s=装飾
let MAPS={};   // {field,cave,area2,town,town2,road,castle} を /maps.json から取得
// 敵：spr=スプライトキー / grp=最大出現数 / moves=敵固有の特殊行動 / moveChance=特殊行動の確率
//   moves の kind: "heavy"(強単体) / "all"(全体・mult倍率) / "healSelf"(自己回復・power)
//   tint=色変え, evasion=回避率, fleeChance=毎ターン逃走率（レアモンスター用）
const GAME_DATA={
  enemies:[
    { name:"ブルン",     spr:"slime", maxhp:8,  atk:7,  def:3, exp:3,  gold:2,  weak:["火"], resist:[],     grp:3 },
    { name:"ヤミバット", spr:"bat",   maxhp:13, atk:10, def:4, exp:6,  gold:5,  weak:["氷"], resist:["火"], grp:2 },
    { name:"ホネナイト", spr:"bone",  maxhp:20, atk:15, def:7, exp:13, gold:11, weak:["火"], resist:["氷"], grp:2 }
  ]
};
// レアモンスター：ゴールドブルン（少し逃げやすい＝素早く倒せば大量経験値。序盤レベリング用に物理でも倒せる調整）
const GOLD_SLIME={ name:"ゴールドブルン", spr:"slime", tint:"#ffd23f", maxhp:16, atk:6, def:6, exp:500, gold:150,
                   weak:[], resist:[], grp:1, evasion:0.12, fleeChance:0.2 };
// 魔王（ラスボス）。専用ドット絵(maoh)＋固有行動
const MAOH={ name:"まおう", spr:"maoh", maxhp:120, atk:28, def:12, exp:200, gold:300, weak:[], resist:[], big:true, moveChance:0.6,
  moves:[{name:"あんこくは",kind:"all",mult:0.9},{name:"こんとんのいちげき",kind:"heavy"},{name:"やみのちから",kind:"healSelf",power:25}] };
// 中ボス：ゲヘナ将（洞窟最奥の関門）＋固有行動
const GEHENA={ name:"ゲヘナ将", spr:"hellgeneral", maxhp:64, atk:23, def:17, exp:85, gold:100, weak:["氷"], resist:["火","雷"], big:true, moveChance:0.55,
  moves:[{name:"じごくのほのお",kind:"all",mult:0.8},{name:"おたけび",kind:"heavy"}] };
// エリアの強敵（敵固有の行動つき）
const E_AREA=[
  { name:"クサレビト",   spr:"rotcorpse",   maxhp:32, atk:16, def:9,  exp:19, gold:16, weak:["火"],       resist:["氷"], grp:2, moves:[{name:"どくのいき",kind:"all",mult:0.6}] },
  { name:"ヒトダマギ",   spr:"soulflame",   maxhp:27, atk:18, def:8,  exp:24, gold:22, weak:["火"],       resist:["氷"], grp:2, moves:[{name:"おにび",kind:"heavy"}] },
  { name:"ハカモリグモ", spr:"gravespider", maxhp:38, atk:19, def:10, exp:33, gold:38, weak:["火","氷"], resist:["雷"], grp:2, moves:[{name:"ねばつくいと",kind:"heavy"}] }
];
// 道の強敵（魔王城へ近づくほど強い・敵固有の行動つき）
const E_ROAD=[
  { name:"ザンギル斬兵", spr:"greatswordsoldier", maxhp:46, atk:22, def:11, exp:48, gold:50,  weak:["氷"], resist:["火"],       moves:[{name:"だいかいてん",kind:"all",mult:0.7}] },
  { name:"ガオウレオン", spr:"infernolion",       maxhp:58, atk:24, def:13, exp:70, gold:80,  weak:["氷"], resist:["火"],       moves:[{name:"ほのおのいぶき",kind:"all",mult:0.7}] },
  { name:"ノクスナイト", spr:"darkknight",        maxhp:70, atk:26, def:16, exp:92, gold:115, weak:["火"], resist:["氷","雷"], moveChance:0.5, moves:[{name:"あんこくぎり",kind:"heavy"}] }
];

// ====== 商品・装備データ ======
// jobs = 装備できる職業(role)。武器はatk、防具はdefを持つ
const SHOP_WEAPON=[
  // 剣（勇者）
  {name:"どうのつるぎ",   atk:6,  price:80,   wtype:"剣",   jobs:["hero"]},
  {name:"はがねのけん",   atk:14, price:350,  wtype:"剣",   jobs:["hero"]},
  {name:"ほのおのけん",   atk:24, price:1200, wtype:"剣",   jobs:["hero"]},
  // 斧（戦士）
  {name:"いしのオノ",     atk:9,  price:120,  wtype:"斧",   jobs:["warrior"]},
  {name:"バトルアックス", atk:19, price:520,  wtype:"斧",   jobs:["warrior"]},
  // 杖（魔法使い）
  {name:"かしのつえ",     atk:3,  price:60,   wtype:"杖",   jobs:["mage"]},
  {name:"まどうのつえ",   atk:8,  price:300,  wtype:"杖",   jobs:["mage"]},
  // メイス（修道女）
  {name:"せいなるメイス", atk:5,  price:90,   wtype:"メイス", jobs:["priest"]},
  {name:"ぎんのメイス",   atk:12, price:340,  wtype:"メイス", jobs:["priest"]}
];
const SHOP_ARMOR=[
  // 鎧（勇者・戦士）
  {name:"かわのよろい",   def:5,  price:70,   atype:"鎧",   jobs:["hero","warrior"]},
  {name:"てつのよろい",   def:12, price:300,  atype:"鎧",   jobs:["hero","warrior"]},
  {name:"はがねのよろい", def:20, price:1000, atype:"鎧",   jobs:["hero","warrior"]},
  // ローブ（魔法使い・修道女）
  {name:"ぬののローブ",   def:3,  price:50,   atype:"ローブ", jobs:["mage","priest"]},
  {name:"まりょくのローブ",def:8,  price:260,  atype:"ローブ", jobs:["mage","priest"]},
  {name:"せいれいのローブ",def:14, price:820,  atype:"ローブ", jobs:["mage","priest"]}
];
const SHOP_ITEM=[
  {name:"やくそう",   key:"herb",   price:8},
  {name:"エリクサー", key:"elixir", price:500}
];
const INN_COST=10;
const REVIVE_COST=50;   // 教会での蘇生

// ====== 状態 ======
let mode="field";          // "intro" | "field" | "talk" | "battle" | "status" | "service"
let darkArea=false;
let mapTheme="";           // ""=通常 / "castle"=魔王城（石床・煉瓦壁・赤絨毯・玉座）
let currentMap="field";
let buildings=[];
const TILE_WALL={ "T":1, "W":1, "R":1, "H":1, "d":1, "f":1, "l":1, "s":1, "o":1 };  // 通れないタイル（f噴水 l街灯 o花壇 s像）
let map=[], npcs=[];
let player={x:0,y:0,size:52,dir:"down",walk:0};
let cameraX=0, cameraY=0;
let trail=[];                 // 勇者の足跡 {x,y,dir}。仲間が後ろを追従する
const FOLLOW_GAP=32;          // 仲間どうしの間隔（足跡の何個ぶん後ろか＝32×3=96px。縦移動で重なりすぎないよう絵の高さ寄り）
let respawn={ map:"town", tx:9, ty:11 };   // 教会で更新するリスポーン地点

// 会話状態
let dialog={active:false, lines:[], idx:0, name:"", shown:0, t:0};

// パーティ（party[0]=勇者）。stats は勇者の別名（既存コード互換）。
// gold/herb/elixir/weapon/armor はパーティ共通として勇者に持たせる。
// 呪文/特技フォーマット: {name, mp, kind, power, mult, elem}
//   kind: "mag"(攻撃魔法) / "phys"(物理特技) / "heal" / "buffAtk" / "buffDef"
//   elem: "火" / "氷" / "雷" / "無"
let party=[{ name:"勇者", role:"hero", job:"勇者", img:"hero", color:"#2e57c8", lv:1, hp:22, maxhp:22, mp:10, maxmp:10,
             atk:12, def:7, exp:0, down:false, weapon:null, armor:null,
             skills:[{name:"なぎはらい",mp:0,kind:"phys",mult:1.3,elem:"無"}],
             spells:[{name:"かえん",mp:3,kind:"mag",power:12,elem:"火"},{name:"いやし",mp:4,kind:"heal",power:20}],
             gold:0, herb:3, elixir:0, buffAtkT:0, buffDefT:0 }];
let stats=party[0];
const HERO_KIT={ skills: party[0].skills.slice(), spells: party[0].spells.slice() };   // 勇者の初期の特技/呪文スナップショット（セーブ復元の土台）
function atkOf(m){ let a=m.atk+(m.weapon?m.weapon.atk:0); if(m.buffAtkT>0)a=Math.floor(a*1.4); return a; }
function defOf(m){ let d=m.def+(m.armor?m.armor.def:0); if(m.buffDefT>0)d=Math.floor(d*1.5); return d; }
function aliveMembers(){ return party.filter(m=>!m.down); }
function recruit(m){ party.push(m); }
// 属性倍率（弱点1.5倍 / 耐性0.5倍）
function elemMult(elem,e){
  if(!elem||elem==="無") return 1;
  if(e.weak && e.weak.indexOf(elem)>=0) return 1.5;
  if(e.resist && e.resist.indexOf(elem)>=0) return 0.5;
  return 1;
}

// 仲間テンプレート
function makeMage(){   // 魔法使い：攻撃魔法（火・氷）
  return { name:"マリン", role:"mage", job:"魔法使い", img:"mage", color:"#8a4fc0", lv:3, hp:18, maxhp:18, mp:24, maxmp:24,
           atk:8, def:5, exp:0, down:false, buffAtkT:0, buffDefT:0, weapon:null, armor:null, skills:[],
           spells:[{name:"かえん",mp:4,kind:"mag",power:18,elem:"火"},
                   {name:"こおり",mp:5,kind:"mag",power:20,elem:"氷"},
                   {name:"いやし",mp:5,kind:"heal",power:18}] };
}
function makeWarrior(){ // 戦士：物理特技
  return { name:"ガルド", role:"warrior", job:"戦士", img:"warrior", color:"#a05a2a", lv:3, hp:30, maxhp:30, mp:4, maxmp:4,
           atk:16, def:9, exp:0, down:false, buffAtkT:0, buffDefT:0, weapon:null, armor:null, spells:[],
           skills:[{name:"おおぎり",mp:0,kind:"phys",mult:1.8,elem:"無"},
                   {name:"みだれうち",mp:0,kind:"phys",mult:1.2,elem:"無"}] };
}
function makePriest(){  // 修道女（シスター）：回復・補助
  return { name:"セラ", role:"priest", job:"修道女", img:"sister", color:"#c8b86a", lv:3, hp:22, maxhp:22, mp:22, maxmp:22,
           atk:9, def:7, exp:0, down:false, buffAtkT:0, buffDefT:0, weapon:null, armor:null, skills:[],
           spells:[{name:"いやし",mp:4,kind:"heal",power:24},
                   {name:"まもりのうた",mp:5,kind:"buffDef"},
                   {name:"ちからのうた",mp:5,kind:"buffAtk"}] };
}
// レベルアップで覚える特技/呪文（5レベルごと）。kind="phys"は特技に、それ以外は呪文に追加。
const LEARN = {
  hero:    { 5:{name:"ためる",mp:0,kind:"buffAtk"},
             10:{name:"いなずま",mp:7,kind:"mag",power:24,elem:"雷"},
             15:{name:"オーラぎり",mp:0,kind:"phys",mult:1.7,elem:"無"},
             20:{name:"いやしのひかり",mp:9,kind:"healAll",power:18} },
  mage:    { 5:{name:"ほのおのうず",mp:8,kind:"magAll",power:14,elem:"火"},
             10:{name:"いなずま",mp:9,kind:"mag",power:30,elem:"雷"},
             15:{name:"ふぶき",mp:13,kind:"magAll",power:22,elem:"氷"},
             20:{name:"メテオ",mp:20,kind:"magAll",power:34,elem:"火"} },
  warrior: { 5:{name:"かぶとわり",mp:0,kind:"phys",mult:1.5,elem:"無"},
             10:{name:"したつき",mp:0,kind:"phys",mult:1.9,elem:"無"},
             15:{name:"ぜんりょくぎり",mp:0,kind:"phys",mult:2.4,elem:"無"} },
  priest:  { 5:{name:"いやしのひかり",mp:9,kind:"healAll",power:18},
             10:{name:"スクルト",mp:5,kind:"buffDef"},
             15:{name:"ベホイム",mp:8,kind:"heal",power:40},
             20:{name:"せいなるひかり",mp:14,kind:"magAll",power:24,elem:"雷"} }
};
function baseKit(role){   // 役割ごとの初期キット（固定分）
  return role==="mage"?makeMage() : role==="warrior"?makeWarrior() : role==="priest"?makePriest()
       : { skills: HERO_KIT.skills, spells: HERO_KIT.spells };
}
function rebuildKit(m){    // 特技/呪文を「役割の初期キット＋現Lv以下のLEARN習得」で決定的に再構成（セーブ依存をやめ習得消失を防ぐ／旧セーブも自己修復）
  const base=baseKit(m.role);
  m.skills=base.skills.slice(); m.spells=base.spells.slice();
  const tbl=LEARN[m.role]||{};
  for(const L in tbl){ if(Number(L)<=m.lv){ const a=tbl[L]; const into=a.kind==="phys"?m.skills:m.spells;
    if(!into.some(x=>x.name===a.name)) into.push({...a}); } }
}

// 戦闘状態
let battle=null;
let encCooldown=0;
const BATTLE_CMDS=["たたかう","とくぎ","じゅもん","どうぐ","にげる"];

// 施設（宿屋/武器屋/道具屋/教会）
let service=null;

// ストーリーフラグ（イベント1回判定など）
let flags={ mageRescued:false, seraJoined:false, warriorJoined:false, maohDefeated:false, gehenaDefeated:false };

let keys={};

// ====== 初期化 ======
function loadMap(name, sx, sy){
  if(!MAPS[name]){ name="town"; sx=9; sy=11; }   // 無いマップ名(旧/壊れセーブ)なら町へ＝起動時クラッシュの保険
  currentMap=name;
  trail=[];                 // マップ移動時は足跡をリセット（仲間が勇者に寄る）
  const m=MAPS[name];
  map=m.map;
  buildings=(m.buildings||[]);
  darkArea=!!m.dark;
  mapTheme=m.theme||"";
  npcs=(m.npcs||[]).map(n=>({...n})).filter(n=> !(n.event==="warrior" && flags.warriorJoined));   // 加入済みの戦士はもう出さない
  npcs.forEach(n=>{
    n.imgKey = n.kind==="elder" ? "elder" : (n.kind==="woman" ? "woman" : "villager");   // 村人の本格立ち絵
    n.spr = n.kind==="elder" ? makeSprite(ELDER, ELDER_PAL, CHAR)
                             : makeSprite(VILLAGER, npcPal(n.color), CHAR);                 // 画像未ロード時の保険
  });
  if(sx!==undefined){
    player.x=sx*TILE+(TILE-player.size)/2; player.y=sy*TILE+(TILE-player.size)/2;
  }else{
    for(let y=0;y<map.length;y++)for(let x=0;x<map[y].length;x++)
      if(map[y][x]==="P"){ player.x=x*TILE+(TILE-player.size)/2; player.y=y*TILE+(TILE-player.size)/2; }
  }
  nudgeToFloor();   // 壁の中に置かれたら近くの床へ（壊れた復活地点の保険）
  clearMoveInput(); // 遷移時に押下キー/タップ移動目標を解除（旧座標への自動歩行・出口バウンス防止）
  mode="field";
}

// 建物の足元当たり判定（ドア以外は壁）。ドア=(x+1, y+1)
// 建物のドア位置（装飾建物 deco はドア無し＝全部壁）。サイズは w×h（既定3×2）
function buildingDoor(b){
  if(b.deco) return null;
  const w=b.w||3, h=b.h||2;
  return { dx: b.x + (w>>1), dy: b.y + (h-1) };
}
// 扉タイル判定。偶数幅の建物（大聖堂=幅6）は見た目の中央が2タイルにまたがるので、中央2タイルを扉にして入りやすくする
function isDoorTile(b, tx, ty){
  if(b.deco) return false;
  const w=b.w||3, h=b.h||2;
  if(ty !== b.y+(h-1)) return false;
  if(w%2===0){ const c=b.x+w/2; return tx===c-1 || tx===c; }   // 偶数幅＝中央2タイル
  return tx === b.x+(w>>1);                                    // 奇数幅＝中央1タイル
}
function buildingWall(tx,ty){
  for(const b of buildings){
    const w=b.w||3, h=b.h||2;
    if(tx>=b.x && tx<b.x+w && ty>=b.y && ty<b.y+h){
      if(isDoorTile(b,tx,ty)) return false;  // ドアは通れる（偶数幅は中央2タイル）
      return true;
    }
  }
  return false;
}
function doorAt(tx,ty){
  for(const b of buildings){ if(isDoorTile(b,tx,ty)) return b; }
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

// 置いた場所が壁なら近くの床タイルへ逃がす（壊れた復活地点などの保険＝起動スタック防止）
function nudgeToFloor(){
  if(!isWall(player.x, player.y)) return;
  const cx=Math.floor((player.x+player.size/2)/TILE), cy=Math.floor((player.y+player.size/2)/TILE);
  for(let r=1;r<=10;r++) for(let dy=-r;dy<=r;dy++) for(let dx=-r;dx<=r;dx++){
    if(Math.max(Math.abs(dx),Math.abs(dy))!==r) continue;   // 外側のリングだけ走査
    const tx=cx+dx, ty=cy+dy;
    if(ty<0||tx<0||!map[ty]||tx>=map[ty].length) continue;
    const px=tx*TILE+(TILE-player.size)/2, py=ty*TILE+(TILE-player.size)/2;
    if(!isWall(px,py)){ player.x=px; player.y=py; return; }
  }
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
  player.walk = moved ? 1 : 0;
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
  if(n.event==="warrior" && !flags.warriorJoined){ eventRecruitWarrior(); return; }   // 旅の戦士＝加入イベント
  dialog.active=true; dialog.lines=n.lines; dialog.idx=0;
  dialog.name=n.name; dialog.shown=0; dialog.t=0; dialog.after=null;
  mode="talk";
}
// イベント用：NPCなしのセリフ＋終了後コールバック
function showScene(lines, after, name){
  dialog.active=true; dialog.lines=lines; dialog.idx=0;
  dialog.name=name||""; dialog.shown=0; dialog.t=0; dialog.after=after||null;
  mode="talk";
}
function advanceDialog(){
  const full=dialog.lines[dialog.idx];
  if(dialog.shown<full.length){ dialog.shown=full.length; return; } // 一気に表示
  dialog.idx++;
  if(dialog.idx>=dialog.lines.length){
    dialog.active=false; mode="field"; clearMoveInput();   // 押下キー/タップ目標を解除（戻った瞬間の暴走防止）
    const cb=dialog.after; dialog.after=null; if(cb) cb();
  } else { dialog.shown=0; dialog.t=0; }
}

// ====== オープニング（あらすじ）======
// RPGに入った直後、本編の前に物語の導入を全画面で表示（タップ / Enter / ジョイボタンで送り）。
let intro={active:false, page:0, shown:0, t:0};
const INTRO_PAGES=[
  ["むかし——","魔王が めざめ、","せかいは やみに つつまれた。"],
  ["人々は ひかりを もとめ、","ひとりの 勇者に","のぞみを たくした。"],
  ["なかま とともに たびに でて、","魔王を たおし、","へいわを とりもどすのだ！"]
];
function introLen(){ return INTRO_PAGES[intro.page].join("\n").length; }
function startIntro(){ intro.active=true; intro.page=0; intro.shown=0; intro.t=0; mode="intro"; }
function advanceIntro(){
  if(intro.shown < introLen()){ intro.shown = introLen(); return; }   // タイプ途中なら一気に全表示
  intro.page++;
  if(intro.page >= INTRO_PAGES.length){ intro.active=false; mode="field"; clearMoveInput(); }   // 冒険スタート
  else { intro.shown=0; intro.t=0; }
}
function drawIntro(){
  const g=ctx.createLinearGradient(0,0,0,VIEW);                        // 夜空グラデ背景
  g.addColorStop(0,"#070b18"); g.addColorStop(0.55,"#0b1330"); g.addColorStop(1,"#191145");
  ctx.fillStyle=g; ctx.fillRect(0,0,VIEW,VIEW);
  ctx.fillStyle="rgba(255,255,255,0.65)";                             // 星（固定配置）
  for(let i=1;i<=70;i++){ const sx=(i*131)%VIEW, sy=(i*73)%Math.floor(VIEW*0.5), s=(i%4?1:2); ctx.fillRect(sx,sy,s,s); }
  ctx.textAlign="center";
  ctx.fillStyle="#ffd45e"; ctx.font="bold 60px 'Segoe UI',system-ui,sans-serif";
  ctx.fillText("RPG QUEST", VIEW/2, 158);
  ctx.fillStyle="#9fb0d6"; ctx.font="22px 'Hiragino Kaku Gothic ProN','Segoe UI',sans-serif";
  ctx.fillText("― 魔王討伐の ものがたり ―", VIEW/2, 200);
  ctx.strokeStyle="rgba(255,212,94,0.55)"; ctx.lineWidth=2;
  ctx.beginPath(); ctx.moveTo(VIEW*0.22,232); ctx.lineTo(VIEW*0.78,232); ctx.stroke();
  const joined=INTRO_PAGES[intro.page].join("\n");                    // あらすじ本文（1文字ずつ）
  const shown=joined.slice(0,intro.shown).split("\n");
  ctx.fillStyle="#eef2ff"; ctx.font="34px 'Hiragino Kaku Gothic ProN','Segoe UI',sans-serif";
  const baseY=372, lh=60;
  shown.forEach((ln,i)=>ctx.fillText(ln, VIEW/2, baseY+i*lh));
  ctx.fillStyle="#5f6f9c"; ctx.font="18px 'Segoe UI',sans-serif";
  ctx.fillText((intro.page+1)+" / "+INTRO_PAGES.length, VIEW/2, VIEW-148);
  if(intro.shown>=joined.length && Math.floor(Date.now()/450)%2===0){ // 全文表示後に点滅プロンプト
    const last=intro.page>=INTRO_PAGES.length-1;
    ctx.fillStyle=last?"#ffd45e":"#cfe0ff"; ctx.font="bold 26px 'Segoe UI',system-ui,sans-serif";
    ctx.fillText(last?"▶ ぼうけんを はじめる":"▶ タップ / Enter で すすむ", VIEW/2, VIEW-86);
  }
  ctx.textAlign="left";
}

// ====== 戦闘 ======
function calcDmg(atk,def){ const base=atk-def/2; return Math.max(1, Math.floor(base*(0.85+Math.random()*0.4))); }
function needExp(lv){ return lv*lv*4 + lv*6; }
// マップ別エンカウント表（重み付き）。表が無いマップは序盤の弱敵にフォールバック。
const ENCOUNTERS={
  field:[[GAME_DATA.enemies[0],55],[GAME_DATA.enemies[1],32],[GAME_DATA.enemies[2],13],[GOLD_SLIME,2]],
  cave: [[GAME_DATA.enemies[1],40],[GAME_DATA.enemies[2],38],[E_AREA[0],22],[GOLD_SLIME,2]],
  area2:[[E_AREA[0],38],[E_AREA[1],34],[E_AREA[2],28],[GOLD_SLIME,2]],
  road: [[E_ROAD[0],40],[E_ROAD[1],34],[E_ROAD[2],26],[GOLD_SLIME,2]]
};
function pickEnemy(){
  const tbl=ENCOUNTERS[currentMap]||ENCOUNTERS.field;
  let tot=0; for(const e of tbl) tot+=e[1];
  let r=Math.random()*tot;
  for(const e of tbl){ if((r-=e[1])<0) return e[0]; }
  return tbl[0][0];
}

// ===== 複数敵の戦闘ヘルパー =====
function foes(){ return battle.enemies.filter(e=>e.hp>0); }       // 生存中の敵
function frontFoe(){ return foes()[0]; }                          // 単体対象は前列
function randAlive(){ const a=aliveMembers(); return a.length?a[Math.floor(Math.random()*a.length)]:null; }
function pickGroup(){                                              // 同種を grp 体まで（小さめに偏らせる）
  const e=pickEnemy(); const maxg=e.grp||1;
  let n=1; for(let i=1;i<maxg && i<3;i++){ if(Math.random()<0.55) n++; }
  const arr=[]; for(let i=0;i<n;i++) arr.push(e); return arr;
}
function startBattle(forced, onWin){
  const list = forced ? [forced] : pickGroup();
  const multi = list.length>1;
  const enemies = list.map((e,i)=>({ ...e, hp:e.maxhp, _ph:i*1.3,
                     name: multi ? e.name+" "+String.fromCharCode(65+i) : e.name }));   // 複数は A/B/C
  battle={ enemies, state:"msg", msg:[], msgIdx:0, shown:0,
           after:null, cmd:0, spell:0, shake:0, shakeP:0, flash:0, rects:null, onWin:onWin||null, noFlee:!!onWin,
           order:[], actorIdx:0, actor:null, _foeQ:[] };
  mode="battle"; fx("enc");
  const head = multi ? (list[0].name+" の むれ") : list[0].name;
  queueMsg([head+"が あらわれた！"], startRound);
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
// 1ラウンド：生存メンバー全員のコマンドを順に選ぶ → 敵のターン
function startRound(){
  for(const m of party){ if(m.buffAtkT>0)m.buffAtkT--; if(m.buffDefT>0)m.buffDefT--; }
  battle.order=aliveMembers(); battle.actorIdx=0; nextActorCmd();
}
function nextActorCmd(){
  if(foes().length===0){ winBattle(); return; }
  if(battle.actorIdx>=battle.order.length){ enemyTurn(); return; }
  battle.actor=battle.order[battle.actorIdx];
  if(battle.actor.down){ battle.actorIdx++; nextActorCmd(); return; }
  enterCommand();
}
function enterCommand(){ battle.state="command"; battle.cmd=0; }
function advanceActor(){ battle.actorIdx++; nextActorCmd(); }
function afterActorAction(){ if(foes().length===0){ winBattle(); return; } advanceActor(); }
function confirmCommand(){
  const b=battle, m=b.actor;
  if(b.cmd===0) actorAttack();
  else if(b.cmd===1){                                    // とくぎ
    if((m.skills||[]).length===0){ queueMsg(["とくぎを つかえない！"], enterCommand); return; }
    b.state="skill"; b.spell=0;
  }
  else if(b.cmd===2){                                    // じゅもん
    if((m.spells||[]).length===0){ queueMsg(["じゅもんを つかえない！"], enterCommand); return; }
    b.state="spell"; b.spell=0;
  }
  else if(b.cmd===3) actorItem();
  else tryFlee();
}
function actorAttack(){
  const m=battle.actor, e=frontFoe();
  if(!e){ afterActorAction(); return; }
  if(e.evasion && Math.random()<e.evasion){ queueMsg([m.name+"の こうげき！", e.name+"は ひらりと かわした！"], afterActorAction); return; }
  const d=calcDmg(atkOf(m), e.def); e.hp-=d; battle.shake=12; fx("hit");
  const fell=e.hp<=0?[e.name+"を たおした！"]:[];
  queueMsg([m.name+"の こうげき！", e.name+"に "+d+"の ダメージ！", ...fell], afterActorAction);
}
// 呪文・特技の共通実行
function castAction(m,a){
  const b=battle;
  if(a.mp>0) m.mp-=a.mp;
  if(a.kind==="phys" || a.kind==="mag"){          // 単体（物理特技 / 攻撃魔法）→ 前列の敵
    const e=frontFoe(); if(!e){ afterActorAction(); return; }
    const verb = a.kind==="phys" ? "の "+a.name+"！" : "は "+a.name+"を となえた！";
    if(a.kind==="phys" && e.evasion && Math.random()<e.evasion){ queueMsg([m.name+verb, e.name+"は かわした！"], afterActorAction); return; }
    const wk=elemMult(a.elem,e);
    const d = a.kind==="phys"
      ? Math.max(1, Math.floor(calcDmg(Math.floor(atkOf(m)*a.mult), e.def)*wk))
      : Math.max(1, Math.floor((a.power+m.lv*2)*wk*(0.85+Math.random()*0.3)));
    e.hp-=d; if(a.kind==="mag")b.flash=10; b.shake=12; fx("hit");
    const tag = wk>1?" じゃくてん！" : (wk<1?" きいていない…":"");
    const fell = e.hp<=0?[e.name+"を たおした！"]:[];
    queueMsg([m.name+verb, e.name+"に "+d+"の ダメージ！"+tag, ...fell], afterActorAction);
  }else if(a.kind==="magAll"){                    // 全体攻撃魔法
    b.flash=10; b.shake=8; fx("hit");
    const lines=[m.name+"は "+a.name+"を となえた！"], dead=[];
    for(const e of foes()){ const wk=elemMult(a.elem,e);
      const d=Math.max(1, Math.floor((a.power+m.lv*2)*wk*(0.85+Math.random()*0.3))); e.hp-=d;
      lines.push(e.name+"に "+d+"！"); if(e.hp<=0) dead.push(e.name+"を たおした！"); }
    queueMsg([...lines, ...dead], afterActorAction);
  }else if(a.kind==="heal"){                       // 単体回復（最も傷ついた味方）
    const h=a.power+Math.floor(Math.random()*8);
    const tgt=aliveMembers().slice().sort((x,y)=>(x.hp/x.maxhp)-(y.hp/y.maxhp))[0]||m;
    tgt.hp=Math.min(tgt.maxhp,tgt.hp+h); fx("heal");
    queueMsg([m.name+"は "+a.name+"を となえた！", tgt.name+"の HPが "+h+" かいふく！"], afterActorAction);
  }else if(a.kind==="healAll"){                    // 全体回復
    fx("heal"); const lines=[m.name+"は "+a.name+"を となえた！"];
    for(const t of aliveMembers()){ const h=a.power+Math.floor(Math.random()*6); t.hp=Math.min(t.maxhp,t.hp+h); }
    lines.push("みかた ぜんいんの HPが かいふくした！");
    queueMsg(lines, afterActorAction);
  }else if(a.kind==="buffAtk"){
    for(const mm of aliveMembers()) mm.buffAtkT=3; fx("heal");
    queueMsg([m.name+"は "+a.name+"を となえた！","みかた ぜんいんの こうげきりょくが あがった！"], afterActorAction);
  }else if(a.kind==="buffDef"){
    for(const mm of aliveMembers()) mm.buffDefT=3; fx("heal");
    queueMsg([m.name+"は "+a.name+"を となえた！","みかた ぜんいんの しゅびりょくが あがった！"], afterActorAction);
  }
}
function confirmSpell(){
  const b=battle, m=b.actor, list=m.spells||[];
  if(b.spell>=list.length){ enterCommand(); return; }   // もどる
  const s=list[b.spell];
  if(m.mp<s.mp){ queueMsg(["MPが たりない！"], enterCommand); return; }
  castAction(m,s);
}
function confirmSkill(){
  const b=battle, m=b.actor, list=m.skills||[];
  if(b.spell>=list.length){ enterCommand(); return; }   // もどる
  const s=list[b.spell];
  if(m.mp<s.mp){ queueMsg(["MPが たりない！"], enterCommand); return; }
  castAction(m,s);
}
function actorItem(){
  if(stats.herb<=0){ queueMsg(["やくそうが ない！"], enterCommand); return; }
  const m=battle.actor;
  stats.herb--; const h=20+Math.floor(Math.random()*8);
  const tgt=aliveMembers().slice().sort((x,y)=>(x.hp/x.maxhp)-(y.hp/y.maxhp))[0]||m;
  tgt.hp=Math.min(tgt.maxhp, tgt.hp+h); fx("heal");
  queueMsg([m.name+"は やくそうを つかった！", tgt.name+"の HPが "+h+" かいふく！"], afterActorAction);
}
function tryFlee(){
  if(battle.noFlee){ queueMsg(["にげられない！"], enterCommand); return; }   // ボス/強制戦闘は逃走不可
  if(Math.random()<0.6) queueMsg(["パーティは にげだした！"], endBattle);
  else queueMsg([battle.actor.name+"は にげられなかった！"], afterActorAction);   // 失敗はそのメンバーのターン消費のみ（他は行動できる）
}
function enemyTurn(){ battle._foeQ = foes().slice(); doNextFoe(); }
function doNextFoe(){
  if(aliveMembers().length===0){ loseBattle(); return; }
  const e = battle._foeQ.shift();
  if(!e){ startRound(); return; }            // 全敵が行動済み → 味方ターンへ
  if(e.hp<=0){ doNextFoe(); return; }
  if(e.fleeChance && Math.random()<e.fleeChance){   // レアモンスターの逃走
    e.hp=0; e.fled=true;
    if(foes().length===0) queueMsg([e.name+"は にげさった…"], endBattle);   // 単体で逃走→終了(報酬なし)
    else queueMsg([e.name+"は にげさった！"], doNextFoe);
    return;
  }
  const mv = (e.moves && e.moves.length && Math.random()<(e.moveChance||0.4)) ? e.moves[Math.floor(Math.random()*e.moves.length)] : null;
  if(mv) enemyMove(e, mv); else enemyBasic(e);
}
function enemyBasic(e){
  const t=randAlive(); if(!t){ doNextFoe(); return; }
  const d=calcDmg(e.atk, defOf(t)); t.hp-=d; battle.shakeP=12; fx("hit");
  const lines=[e.name+"の こうげき！", t.name+"は "+d+"の ダメージ！"];
  if(t.hp<=0){ t.hp=0; t.down=true; lines.push(t.name+"は たおれた！"); }
  queueMsg(lines, doNextFoe);
}
function enemyMove(e, mv){
  if(mv.kind==="all"){                       // 全体攻撃
    battle.shakeP=12; fx("hit"); const lines=[e.name+"の "+mv.name+"！"];
    for(const t of aliveMembers()){ const d=Math.max(1, Math.floor(calcDmg(e.atk, defOf(t))*(mv.mult||0.7))); t.hp-=d;
      let s=t.name+"は "+d+"！"; if(t.hp<=0){ t.hp=0; t.down=true; s+=" たおれた！"; } lines.push(s); }
    queueMsg(lines, doNextFoe);
  }else if(mv.kind==="healSelf"){            // 自己回復
    e.hp=Math.min(e.maxhp, e.hp+(mv.power||20)); fx("heal");
    queueMsg([e.name+"の "+mv.name+"！", e.name+"は きずを いやした！"], doNextFoe);
  }else{                                     // heavy（強単体）
    const t=randAlive(); if(!t){ doNextFoe(); return; }
    const d=Math.floor(calcDmg(Math.floor(e.atk*(mv.mult||1.6)), defOf(t))); t.hp-=d; battle.shakeP=14; fx("hit");
    const lines=[e.name+"の "+mv.name+"！", t.name+"に "+d+"の だいダメージ！"];
    if(t.hp<=0){ t.hp=0; t.down=true; lines.push(t.name+"は たおれた！"); }
    queueMsg(lines, doNextFoe);
  }
}
function learnAt(m){      // この lv で覚える特技/呪文があれば習得して名前を返す
  const tbl=LEARN[m.role]; if(!tbl) return null;
  const a=tbl[m.lv]; if(!a) return null;
  const into = a.kind==="phys" ? (m.skills||(m.skills=[])) : (m.spells||(m.spells=[]));
  if(into.some(x=>x.name===a.name)) return null;
  into.push({...a}); return a.name;
}
function gainExp(ex){
  const msgs=[];
  for(const m of party){ if(m.down) continue;
    m.exp=(m.exp||0)+ex;
    while(m.exp>=needExp(m.lv)){
      m.exp-=needExp(m.lv); m.lv++;
      m.maxhp+=6; m.maxmp+=3; m.atk+=2; m.def+=1; m.hp=m.maxhp; m.mp=m.maxmp;
      msgs.push(m.name+"は レベル"+m.lv+"に あがった！");
      const learned=learnAt(m);
      if(learned) msgs.push(m.name+"は "+learned+"を おぼえた！");
    }
  }
  if(msgs.length) fx("level");
  return msgs;
}
function winBattle(){
  fx("win");
  const killed=battle.enemies.filter(e=>!e.fled);
  const totExp=killed.reduce((s,e)=>s+e.exp,0), totGold=killed.reduce((s,e)=>s+e.gold,0);
  stats.gold+=totGold;
  const head = killed.length>1 ? "まものを ぜんめつ させた！" : (killed[0]?killed[0].name+"を たおした！":"しょうり！");
  const lines=[head, "けいけんち "+totExp+"　"+totGold+"ゴールド！"];
  lines.push(...gainExp(totExp));
  const cb=battle.onWin;
  queueMsg(lines, ()=>{ endBattle(); saveGame(); if(cb) cb(); });   // 勝利を保存（強制戦闘なら勝利後イベントへ）
}
function loseBattle(){
  queueMsg(["パーティは ぜんめつした…","きょうかいで めをさました…"], ()=>{
    for(const m of party){ m.down=false; m.hp=m.maxhp; m.mp=m.maxmp; }
    battle=null; encCooldown=120;
    loadMap(respawn.map, respawn.tx, respawn.ty);   // 教会(リスポーン地点)で復活
  });
}
function endBattle(){ mode="field"; battle=null; encCooldown=100; clearMoveInput(); }

// ====== イベント：洞窟で魔法使いを救出して仲間にする ======
function eventRescueMage(){
  showScene(["どうくつの おくで……","まもの が 魔法使いを おそっている！"], ()=>{
    startBattle(GAME_DATA.enemies[2], ()=>{          // ホネナイトと強制戦闘 → 勝利で仲間化
      flags.mageRescued=true;
      if(!party.some(m=>m.role==="mage")) recruit(makeMage());   // 二重加入ガード
      saveGame();
      showScene([
        "魔法使いを たすけだした！",
        "マリン「ありがとう、たすかったわ。",
        "　おれいに いっしょに たたかわせて！」",
        "マリンが なかまに くわわった！"
      ]);
    });
  });
}
// 大聖堂：シスター セラ が仲間に（戦闘なし・会話のみ）
function eventRecruitSera(){
  mode="field";
  showScene([
    "大聖堂は しずかな ひかりに つつまれている……",
    "シスター「あなたが 勇者さま ですね。",
    "　わたくし セラ。女神の おみちびきで まいりました。",
    "　どうか おともを させてください。」",
    "セラが なかまに くわわった！"
  ], ()=>{
    flags.seraJoined=true;
    if(!party.some(m=>m.role==="priest")) recruit(makePriest());   // 二重加入ガード
    saveGame();
  });
}
// 旅の戦士ガルドが仲間に（town2のNPC「旅の戦士」に話しかけると発火）
function eventRecruitWarrior(){
  if(flags.warriorJoined) return;
  mode="field";
  showScene([
    "旅の戦士「おれは ガルド。うでには じしんが ある。",
    "　魔王を たおす たびと きいた。",
    "　ぜひ おれも くわえてくれ！」",
    "ガルドが なかまに くわわった！"
  ], ()=>{
    flags.warriorJoined=true;
    if(!party.some(m=>m.role==="warrior")) recruit(makeWarrior());   // 二重加入ガード
    npcs=npcs.filter(n=>n.event!=="warrior");   // 加入したら その場から消す
    saveGame();
  });
}
// 中ボス：ゲヘナ将（洞窟最奥 N出口でエリアへ抜ける直前に立ちはだかる）
// 二択（施設メニューの入力系を流用。actions が mode 遷移まで行う）
function confirmChoice(question, labels, actions){
  mode="service";
  service={ type:"choice", question:question||[], phase:"menu", labels:labels, actions:actions,
            cursor:0, msgs:[], msgIdx:0, shown:0, after:null, rects:[], navRects:[] };
}
function closeChoice(){ mode="field"; service=null; clearMoveInput(); }
function drawChoice(s){
  ctx.fillStyle="rgba(0,0,8,0.55)"; ctx.fillRect(0,0,VIEW,VIEW);
  const q=s.question||[], qh=44+q.length*36, qy=VIEW*0.42-qh, qx=50, qw=VIEW-100;
  winBox(qx,qy,qw,qh);
  ctx.fillStyle="#fff"; ctx.font="26px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="center";
  q.forEach((ln,i)=>ctx.fillText(ln, VIEW/2, qy+44+i*36));
  s.rects=[];
  const oy=qy+qh+24, oh=44+s.labels.length*58, ox=VIEW/2-160, ow=320;
  winBox(ox,oy,ow,oh);
  ctx.font="28px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="left";
  for(let i=0;i<s.labels.length;i++){
    const by=oy+56+i*58;
    if(i===s.cursor){ ctx.fillStyle="rgba(255,210,63,0.16)"; roundRect(ox+16,by-40,ow-32,52,8); ctx.fill();
      ctx.fillStyle="#ffd23f"; ctx.fillText("▶", ox+28, by); }
    ctx.fillStyle = i===s.cursor?"#fff":"#c8d0e0";
    ctx.fillText(s.labels[i], ox+64, by);
    s.rects.push({x:ox+16,y:by-40,w:ow-32,h:52,i});
  }
}
// 中ボス：ゲヘナ将（とても強いので「たたかう/ひきかえす」を選べる）
function eventGehenaBattle(){
  confirmChoice(
    ["どうくつの さいおくに、むらさきの ほのおが ゆらめく……","ゲヘナ将が ゆくてを はばんでいる！"],
    ["たたかう","ひきかえす"],
    [ ()=>{ closeChoice(); gehenaFight(); },
      ()=>{ closeChoice(); player.x=7*TILE+(TILE-player.size)/2; player.y=3*TILE+(TILE-player.size)/2; } ]   // 引き返す＝洞窟側へ戻る
  );
}
function gehenaFight(){
  showScene([
    "ゲヘナ将「ここから さきへは とおさんぞ。",
    "　まおうさまの ために、ほうむって くれる！」",
    "ゲヘナ将が たちはだかった！"
  ], ()=>{
    startBattle(GEHENA, ()=>{
      flags.gehenaDefeated=true; saveGame();
      showScene([
        "ゲヘナ将を うちやぶった！",
        "おくへの みちが ひらけた。"
      ]);
    });
  });
}
// 魔王城の玉座（Zタイル）：会話イベント → 魔王戦
function eventMaohBattle(){
  showScene([
    "魔王「ようこそ わが しろへ、勇者よ。",
    "　よくぞ ここまで たどり着いた。",
    "　だが この せかいは わが ものだ——",
    "　ここで ほうむって くれよう！」",
    "魔王が たちはだかった！"
  ], ()=>{
    startBattle(MAOH, ()=>{                 // 勝利＝エンディング
      flags.maohDefeated=true;
      const allies=party.slice(1).map(m=>m.name);
      const lines=[
        "魔王を うちたおした——！",
        "まばゆい ひかりが せかいを つつみ、",
        "ながい よるが あけ、大地に へいわが もどった。",
        allies.length ? ("勇者は "+allies.join("、")+" と かたを たたきあい、しょうりを わかちあった。")
                      : "勇者は しずかに そらを みあげ、しょうりを かみしめた。",
        "やがて 旅は おわり、勇者たちは ふるさとへ かえる。",
        "町の ひとびとは えがおで むかえた。",
        "「ありがとう、勇者さま！」　「えいゆう ばんざい！」",
        "まおうの きえた せかいに、おだやかな 日々が もどる。",
        "そして——勇者たちの ものがたりは、まだ つづいていく。",
        "ー  おわり  ー",
        "（ぼうけんを セーブして、はじまりの 町へ もどります）"
      ];
      showScene(lines, ()=>{
        for(const m of party){ m.down=false; m.hp=m.maxhp; m.mp=m.maxmp; }   // 全員 全回復
        saveGame();                          // クリアデータを 自動セーブ
        loadMap("town", 9, 11);              // はじまりの町へ もどる
      });
    });
  });
}

// ====== 施設（宿屋/武器屋/道具屋/教会）＆セーブ ======
function openService(b){
  mode="service";
  service={ type:b.type, name:b.name, b:b, phase:"menu", labels:[], actions:[],
            cursor:0, msgs:[], msgIdx:0, shown:0, after:null, rects:[] };
  if(b.type==="inn")        svcMenu(["とまる ("+INN_COST+"G)","やめる"], [innStay, closeService]);
  else if(b.type==="church"||b.type==="cathedral")svcMenu(["いのる","そせい ("+REVIVE_COST+"G)","やめる"], [churchPray, churchRevive, closeService]);
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
function clearMoveInput(){ keys['w']=keys['s']=keys['a']=keys['d']=false; tapDir=null; moveTarget=null; talkTarget=null; }  // 遷移時に押しっぱなし入力・タップ移動を解除
function closeService(){
  const b=service.b, d=buildingDoor(b);
  // ドアのタイルへ戻し、_onDoor=true のままにする。
  // → 入力を上に入れっぱなしでも建物の壁に当たって止まり、施設が即再オープンしない（スタック防止）。
  //   下へ抜けると _onDoor が解除され、もう一度ドアに乗れば正しく再オープンする。
  const fx=d?d.dx:b.x+1, fy=d?d.dy:b.y+1;
  player.x=fx*TILE+(TILE-player.size)/2; player.y=fy*TILE+(TILE-player.size)/2;
  player._onDoor=true;
  mode="field"; service=null; clearMoveInput();
}
function innStay(){
  if(stats.gold<INN_COST){ svcMsg(["おかねが たりないようだ。"], ()=>openService(service.b)); return; }
  stats.gold-=INN_COST; for(const m of party){ m.down=false; m.hp=m.maxhp; m.mp=m.maxmp; } saveGame();   // 全員を全回復＆蘇生
  svcMsg(["ゆっくり おやすみなさい…","HPとMPが かいふくし、","ぼうけんを セーブした！"], closeService);
}
function churchPray(){
  for(const m of party){ if(!m.down){ m.hp=m.maxhp; m.mp=m.maxmp; } }   // 生存メンバー全員のHP/MP回復（蘇生は「そせい」担当）
  const _d=buildingDoor(service.b);
  respawn={ map:currentMap, tx:(_d?_d.dx:service.b.x+1), ty:(_d?_d.dy+1:service.b.y+2) };   // ドアの真下（大聖堂対応）
  saveGame();
  svcMsg(["女神に いのりを ささげた。","ここが ふっかつの ちと なった。","HP・MPが かいふくした。"], closeService);
}
function churchRevive(){
  const dead=party.filter(m=>m.down);
  if(dead.length===0){ svcMsg(["たおれた なかまは いない ようだ。"], ()=>openService(service.b)); return; }
  if(stats.gold<REVIVE_COST){ svcMsg(["おかねが たりない ようだ。"], ()=>openService(service.b)); return; }
  stats.gold-=REVIVE_COST;
  for(const m of dead){ m.down=false; m.hp=m.maxhp; m.mp=m.maxmp; }
  saveGame();
  svcMsg(["女神に いのりを ささげた…", dead.map(m=>m.name).join("と")+"が よみがえった！"], closeService);
}
function openWeaponShop(){
  // 今のパーティの誰かが装備できる物だけ並べる
  const all=SHOP_WEAPON.concat(SHOP_ARMOR)
    .filter(it=> party.some(m=> it.jobs.indexOf(m.role)>=0))
    .sort((a,b)=>a.price-b.price);
  const labels=all.map(it=>{
    const tag=it.wtype||it.atype, val=(it.atk!==undefined?"攻+"+it.atk:"守+"+it.def);
    return { t:"【"+tag+"】"+it.name+"  "+val, p:it.price+"G" };   // 左:名前+増加値 / 右:値段（離して表示）
  });
  labels.push("やめる");
  const actions=all.map(it=>()=>buyEquip(it)); actions.push(closeService);
  svcMenu(labels, actions);
}
function buyEquip(it){
  if(stats.gold<it.price){ svcMsg(["おかねが たりない！"], openWeaponShop); return; }
  const cand=party.filter(m=> it.jobs.indexOf(m.role)>=0);
  if(cand.length===0){ svcMsg(["そうびできる なかまが いない。"], openWeaponShop); return; }
  if(cand.length===1){ doEquip(it, cand[0]); return; }
  // 装備者を選ぶ
  const labels=cand.map(m=>{
    const cur=(it.atk!==undefined? m.weapon : m.armor);
    return m.name+"（今:"+(cur?cur.name:"なし")+"）";
  });
  labels.push("やめる");
  const actions=cand.map(m=>()=>doEquip(it,m)); actions.push(openWeaponShop);
  svcMsg(["だれに そうびする？"], ()=>svcMenu(labels, actions));
}
function doEquip(it, m){
  if(stats.gold<it.price){ svcMsg(["おかねが たりない！"], openWeaponShop); return; }
  stats.gold-=it.price;
  if(it.atk!==undefined) m.weapon=it; else m.armor=it;
  saveGame();
  svcMsg([m.name+"は "+it.name+"を そうびした！"], openWeaponShop);
}
function openItemShop(){
  const list=SHOP_ITEM.slice().sort((a,b)=>a.price-b.price);
  const labels=list.map(it=> ({ t:it.name, p:it.price+"G" })); labels.push("やめる");
  const actions=list.map(it=>()=>buyItem(it)); actions.push(closeService);
  svcMenu(labels, actions);
}
function buyItem(it){
  if(stats.gold<it.price){ svcMsg(["おかねが たりない！"], openItemShop); return; }
  stats.gold-=it.price; stats[it.key]=(stats[it.key]||0)+1; saveGame();
  svcMsg([it.name+"を かった！"], openItemShop);
}
function saveGame(){ try{ localStorage.setItem("rpgQuestSave", JSON.stringify({party, respawn, flags})); }catch(e){} }
function loadGame(){
  try{ const s=JSON.parse(localStorage.getItem("rpgQuestSave"));
    if(s&&s.party&&s.party.length){ party=s.party; stats=party[0];
      // 旧セーブ互換：欠落フィールドを補完
      for(const m of party){ if(m.buffAtkT===undefined)m.buffAtkT=0; if(m.buffDefT===undefined)m.buffDefT=0;
                             rebuildKit(m);   // 特技/呪文は役割＋現Lvから再構成（習得分を保持・旧セーブも自己修復）
                             if(!m.img)m.img=({hero:"hero",mage:"mage",warrior:"warrior",priest:"sister"})[m.role]||"hero"; }  // 画像キー補完(img導入前のセーブ対策)
      if(stats.gold===undefined)stats.gold=0; if(stats.herb===undefined)stats.herb=0; if(stats.elixir===undefined)stats.elixir=0;
      if(s.respawn) respawn=(MAPS[s.respawn.map]?s.respawn:{map:"town",tx:9,ty:11});  if(s.flags) Object.assign(flags, s.flags);
      // 不整合セーブの自己修復：加入フラグが立っているのに仲間が居なければ入れ直す（過去版の名残対策）
      if(flags.mageRescued && !party.some(m=>m.role==="mage"))      recruit(makeMage());
      if(flags.seraJoined  && !party.some(m=>m.role==="priest"))    recruit(makePriest());
      if(flags.warriorJoined && !party.some(m=>m.role==="warrior")) recruit(makeWarrior());
      return true; }
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
  if(e.repeat) return;   // キーリピートでのメニュー暴走・会話無限ループ防止（移動は上のkeys[]で継続）
  if(mode==="intro"){ advanceIntro(); return; }
  if(mode==="battle"){
    const b=battle; if(!b) return;
    if(b.state==="msg"){ if(k===" "||e.key==="Enter") advanceBattleMsg(); }
    else if(b.state==="command"){
      const nc=BATTLE_CMDS.length;
      if(k==="w"||e.key==="ArrowUp") b.cmd=(b.cmd+nc-1)%nc;
      if(k==="s"||e.key==="ArrowDown") b.cmd=(b.cmd+1)%nc;
      if(k===" "||e.key==="Enter") confirmCommand();
    }else if(b.state==="spell"||b.state==="skill"){
      const list=(b.state==="skill"?b.actor.skills:b.actor.spells)||[];
      const n=list.length+1;
      if(k==="w"||e.key==="ArrowUp") b.spell=(b.spell+n-1)%n;
      if(k==="s"||e.key==="ArrowDown") b.spell=(b.spell+1)%n;
      if(k===" "||e.key==="Enter"){ b.state==="skill"?confirmSkill():confirmSpell(); }
    }
    return;
  }
  if(mode==="service"){
    const s=service; if(!s) return;
    if(k==="escape"){ if(s.type==="choice"){ s.actions[s.labels.length-1](); } else if(s.phase==="menu"){ closeService(); } else { advanceSvc(); } return; }   // Esc=離脱/送り（二択は引き返す）
    if(s.phase==="menu"){
      const n=s.labels.length;
      if(k==="w"||e.key==="ArrowUp")    s.cursor=Math.max(0, s.cursor-1);
      if(k==="s"||e.key==="ArrowDown")  s.cursor=Math.min(n-1, s.cursor+1);
      if(k==="a"||e.key==="ArrowLeft")  s.cursor=Math.max(0, s.cursor-1);
      if(k==="d"||e.key==="ArrowRight") s.cursor=Math.min(n-1, s.cursor+1);
      if(k===" "||e.key==="Enter") confirmSvc();
    }else{ if(k===" "||e.key==="Enter") advanceSvc(); }
    return;
  }
  if(mode==="status"){
    if(k==="m"||k==="escape"||k==="i"||k===" "||e.key==="Enter"){ mode="field"; clearMoveInput(); }
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

// ===== スマホ用 仮想ゲームパッド（十字キー＋A/B）。既存のキー/ボタン処理にそのまま流す =====
// 十字＝矢印キーの押下/離す（移動は押しっぱなし／メニューは押した瞬間に1回）。A＝決定・話す・進む、B＝メニュー・もどる。
function virtualB(){
  if(mode==="field"){ clearMoveInput(); mode="status"; }                 // メニュー(ステータス)を開く
  else if(mode==="status"){ mode="field"; }                              // 閉じる
  else if(mode==="talk"){ advanceDialog(); }                             // 会話送り
  else if(mode==="intro"){ advanceIntro(); }
  else if(mode==="service"){ const s=service; if(s){ if(s.type==="choice"){ s.actions[s.labels.length-1](); } else if(s.phase==="menu") closeService(); else advanceSvc(); } }  // 店を出る/送り（二択は引き返す）
  else if(mode==="battle"){ const b=battle; if(b && (b.state==="skill"||b.state==="spell")) enterCommand(); }     // 特技/呪文→戻る
}
(function setupPad(){
  const pad=document.getElementById('pad'); if(!pad) return;
  const fire=(type,key)=>document.dispatchEvent(new KeyboardEvent(type,{key}));
  pad.querySelectorAll('.db').forEach(b=>{
    const key=b.dataset.k;
    const dn=e=>{ e.preventDefault(); fire('keydown',key); };
    const up=e=>{ e.preventDefault(); fire('keyup',key); };
    b.addEventListener('pointerdown',dn);
    b.addEventListener('pointerup',up);
    b.addEventListener('pointercancel',up);
    b.addEventListener('pointerleave',up);
    b.addEventListener('contextmenu',e=>e.preventDefault());
  });
  document.getElementById('btnA').addEventListener('pointerdown',e=>{ e.preventDefault(); joyButton(); });
  document.getElementById('btnB').addEventListener('pointerdown',e=>{ e.preventDefault(); virtualB(); });
})();

function canvasPos(e){
  const r=canvas.getBoundingClientRect();
  return { x:(e.clientX-r.left)*(canvas.width/r.width), y:(e.clientY-r.top)*(canvas.height/r.height) };
}
let tapDir=null, moveTarget=null, talkTarget=null;
canvas.addEventListener("pointerdown",(e)=>{
  if(mode==="intro"){ advanceIntro(); return; }
  if(mode==="talk"){ advanceDialog(); return; }
  if(mode==="battle"){
    const b=battle; if(!b) return; const p=canvasPos(e);
    if(b.state==="msg"){ advanceBattleMsg(); return; }
    if(b.rects){ for(const r of b.rects){
      if(p.x>=r.x&&p.x<=r.x+r.w&&p.y>=r.y&&p.y<=r.y+r.h){
        if(b.state==="command"){ b.cmd=r.i; confirmCommand(); }
        else if(b.state==="skill"){ b.spell=r.i; confirmSkill(); }
        else if(b.state==="spell"){ b.spell=r.i; confirmSpell(); }
        return;
      } } }
    return;
  }
  if(mode==="service"){
    const s=service; if(!s) return; const p=canvasPos(e);
    if(s.phase==="msg"){ advanceSvc(); return; }
    if(s.navRects){ for(const r of s.navRects){ if(p.x>=r.x&&p.x<=r.x+r.w&&p.y>=r.y&&p.y<=r.y+r.h){   // ページ送り(タップ)
      s.cursor=Math.max(0,Math.min(s.labels.length-1, s.cursor+r.dir*7)); return; } } }
    if(s.rects){ for(const r of s.rects){ if(p.x>=r.x&&p.x<=r.x+r.w&&p.y>=r.y&&p.y<=r.y+r.h){ s.cursor=r.i; confirmSvc(); return; } } }
    return;
  }
  if(mode==="status"){ mode="field"; clearMoveInput(); return; }
  const p=canvasPos(e);
  if(p.x>VIEW-72 && p.y<72){ mode="status"; return; }   // 右上メニューボタン
  // タップした地点へ歩いて行く（ポイント移動）。ワールド座標 = 画面座標 + カメラ
  const wx=p.x+cameraX, wy=p.y+cameraY;
  let npc=null;
  for(const n of npcs){ if(wx>=n.x*TILE && wx<(n.x+1)*TILE && wy>=n.y*TILE && wy<(n.y+1)*TILE){ npc=n; break; } }
  if(npc){ moveTarget={x:npc.x*TILE+TILE/2, y:npc.y*TILE+TILE/2}; talkTarget=npc; }  // NPCなら近づいて会話
  else { moveTarget={x:wx, y:wy}; talkTarget=null; }
});
canvas.addEventListener("pointerup",()=>{ tapDir=null; });
canvas.addEventListener("pointerleave",()=>{ tapDir=null; });

// ====== ハードウェア効果音（Arduinoのブザー＋WS2812 LED） ======
// 短時間に連続して鳴らすとArduino側が詰まるので最小間隔をあける（間引き）
let lastFx=0;
function fx(s){
  const now=Date.now();
  if(now-lastFx < 120) return;   // 120ms以内の連続は無視
  lastFx=now;
  try{ fetch("/fx?s="+s,{cache:"no-store"}); }catch(e){}
}

// ====== ジョイスティック（/state をポーリング） ======
let joyState={x:512,y:512,b:0}, joyNav=true, joyBtn=true, joyBusy=false;
async function pollJoy(){
  if(joyBusy) return;                 // 前回の通信が終わるまで重ねない
  joyBusy=true;
  try{
    const c=new AbortController(); const to=setTimeout(()=>c.abort(),500);  // 500msで打ち切り
    const r=await fetch("/state",{cache:"no-store",signal:c.signal}); clearTimeout(to);
    if(r.ok) applyJoy(await r.json());
  }catch(e){}
  joyBusy=false;
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
    if(b.state==="command"){ const nc=BATTLE_CMDS.length; if(up)b.cmd=(b.cmd+nc-1)%nc; if(down)b.cmd=(b.cmd+1)%nc; }
    else if(b.state==="spell"||b.state==="skill"){ const list=(b.state==="skill"?b.actor.skills:b.actor.spells)||[]; const n=list.length+1; if(up)b.spell=(b.spell+n-1)%n; if(down)b.spell=(b.spell+1)%n; }
  }else if(mode==="service"){ const s=service; if(!s||s.phase!=="menu")return; const n=s.labels.length;
    if(up)s.cursor=Math.max(0,s.cursor-1); if(down)s.cursor=Math.min(n-1,s.cursor+1);
    if(left)s.cursor=Math.max(0,s.cursor-1); if(right)s.cursor=Math.min(n-1,s.cursor+1);
  }
}
function joyButton(){
  if(mode==="intro"){ advanceIntro(); return; }
  if(mode==="field"){ const n=npcInFront(); if(n) talkTo(n); }
  else if(mode==="talk"){ advanceDialog(); }
  else if(mode==="status"){ mode="field"; }
  else if(mode==="battle"){ const b=battle; if(!b)return;
    if(b.state==="msg") advanceBattleMsg(); else if(b.state==="command") confirmCommand(); else if(b.state==="skill") confirmSkill(); else if(b.state==="spell") confirmSpell(); }
  else if(mode==="service"){ const s=service; if(!s)return;
    if(s.phase==="msg") advanceSvc(); else confirmSvc(); }
}
// ※ pollJoy（/state ポーリング）は「起動完了後」に開始する（下の start 内）。
//   実機(WiFiS3)は同時接続数が少なく、起動直後に /state を連打すると /maps.json 取得を邪魔して真っ白になるため。

// ====== 内蔵12x8 LEDマトリクス用ミニマップ ======
// 現在マップを12x8に縮小：出入口/建物=点灯、自分=点滅(Arduino側)。変化した時だけ送信。
let _miniKey="", _miniBusy=false;
function updateMinimap(){
  if(!map.length) return;
  const mw=map[0].length, mh=map.length, exits=(MAPS[currentMap].exits)||{};
  const f=[0,0,0];
  const set=(col,row)=>{ col=col<0?0:col>11?11:col; row=row<0?0:row>7?7:row; const i=row*12+col; f[i>>5]|=(1<<(31-(i&31))); };
  for(let y=0;y<mh;y++){ const r=map[y]; for(let x=0;x<mw;x++){ if(exits[r[x]]) set(Math.floor(x*12/mw), Math.floor(y*8/mh)); } }
  for(const b of buildings){ set(Math.floor((b.x+1)*12/mw), Math.floor((b.y+1)*8/mh)); }   // 建物の入口
  const ptx=Math.floor((player.x+player.size/2)/TILE), pty=Math.floor((player.y+player.size/2)/TILE);
  let pcol=Math.floor(ptx*12/mw), prow=Math.floor(pty*8/mh);
  pcol=pcol<0?0:pcol>11?11:pcol; prow=prow<0?0:prow>7?7:prow;
  const pIdx=prow*12+pcol, a=f[0]>>>0, b2=f[1]>>>0, c=f[2]>>>0;
  const key=currentMap+":"+pIdx+":"+a+","+b2+","+c;
  if(key===_miniKey || _miniBusy) return;       // 変化なし/送信中はスキップ
  _miniKey=key; _miniBusy=true;
  fetch("/led?a="+a+"&b="+b2+"&c="+c+"&p="+pIdx,{cache:"no-store"}).catch(()=>{}).finally(()=>{ _miniBusy=false; });
}

// ====== 更新 ======
function update(){
  if(mode==="intro"){
    intro.t++;
    if(intro.t%2===0 && intro.shown<introLen()) intro.shown++;   // あらすじを1文字ずつ表示
    return;                                                       // 本編はポーズ
  }
  if(encCooldown>0) encCooldown--;
  if(mode==="field"){
    player.walk=0;
    const sp=5; let dx=0,dy=0, arrived=false;   // 移動速度（スマホで縮小表示されても遅すぎないよう）
    if(keys["w"])dy-=sp; if(keys["s"])dy+=sp;
    if(keys["a"])dx-=sp; if(keys["d"])dx+=sp;
    const j=joyState;                          // ジョイスティック
    if(!(j.x===0&&j.y===0)){
      if(j.x<350)dx-=sp; if(j.x>700)dx+=sp;
      if(j.y<350)dy-=sp; if(j.y>700)dy+=sp;
    }
    if(dx||dy){ moveTarget=null; talkTarget=null; }    // キー/ジョイ操作は手動優先（タップ移動を解除）
    else if(moveTarget){                               // タップ地点へ歩く（縦横spずつ・壁は片軸ずつ滑る）
      const pcx=player.x+player.size/2, pcy=player.y+player.size/2;
      const tx=moveTarget.x-pcx, ty=moveTarget.y-pcy;
      if(Math.hypot(tx,ty) < sp) arrived=true;
      else { dx=Math.abs(tx)<sp?tx:Math.sign(tx)*sp; dy=Math.abs(ty)<sp?ty:Math.sign(ty)*sp; }
    }
    const _px=player.x, _py=player.y;
    if(dx)move(dx,0); if(dy)move(0,dy);
    if(moveTarget && (arrived || (player.x===_px && player.y===_py))){   // 到着 or 壁で停止 → タップ移動終了
      if(talkTarget && npcInFront()===talkTarget) talkTo(talkTarget);    // NPCタップなら話しかける
      moveTarget=null; talkTarget=null;
    }
    if(player.walk){ trail.unshift({x:player.x,y:player.y,dir:player.dir});   // 足跡を記録
                     if(trail.length>FOLLOW_GAP*4+8) trail.length=FOLLOW_GAP*4+8; }
    updateCamera();
    // 立っているタイル
    const ptx=Math.floor((player.x+player.size/2)/TILE), pty=Math.floor((player.y+player.size/2)/TILE);
    const tile = map[pty] ? map[pty][ptx] : "";
    // 出口タイル（G/C など）で別マップへ
    if((dx||dy)){
      const exs=MAPS[currentMap].exits;
      if(exs && exs[tile]){ const ex=exs[tile];
        if(currentMap==="cave" && tile==="N" && !flags.gehenaDefeated){ eventGehenaBattle(); return; }  // 中ボスがエリアへの道を塞ぐ
        loadMap(ex.to, ex.tx, ex.ty); return; }
    }
    // 洞窟の救出イベント（一度だけ）
    if((dx||dy) && tile==="E" && !flags.mageRescued){ eventRescueMage(); return; }
    if((dx||dy) && tile==="Z" && !flags.maohDefeated){ eventMaohBattle(); return; }
    // 施設のドア：乗った瞬間だけ反応（同じドア上で立ち止まっても再発火しない＝ドアから出られなくなる不具合の修正）
    if((dx||dy)){
      const b=doorAt(ptx,pty);
      if(b){
        if(!player._onDoor){
          player._onDoor=true;
          if(b.type==="cathedral" && !flags.seraJoined){ eventRecruitSera(); return; }   // 大聖堂で初回はセラ加入
          openService(b); return;
        }
      }else{ player._onDoor=false; }
    }
    // 草地を歩くとランダムエンカウント（安全マップ=街では出ない）
    const safe=MAPS[currentMap].safe;
    if(!safe && (dx||dy) && encCooldown<=0 && onGrass() && Math.random()<0.004){ startBattle(); }
    updateMinimap();   // 内蔵LEDマトリクスへミニマップ送信（変化時のみ）
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

// ===== 魔王城テーマの描画 =====
function drawCastleFloor(gx,gy,x,y){
  ctx.fillStyle=((x+y)&1)?"#272336":"#211e2e";        // 市松の暗い石床
  ctx.fillRect(gx,gy,TILE,TILE);
  ctx.fillStyle="#17141f";                            // 目地
  ctx.fillRect(gx,gy+TILE-2,TILE,2); ctx.fillRect(gx+TILE-2,gy,2,TILE);
  ctx.fillStyle="rgba(150,140,180,0.05)"; ctx.fillRect(gx,gy,TILE,3);
}
function drawCastleWall(gx,gy){
  ctx.fillStyle="#3c3850"; ctx.fillRect(gx,gy,TILE,TILE);        // 石壁ベース
  ctx.fillStyle="#2a2740";
  for(let r=0;r<TILE;r+=20) ctx.fillRect(gx,gy+r,TILE,2);        // 横目地
  for(let r=0;r<TILE;r+=20){ const off=((r/20)&1)?TILE/2:0;      // 縦目地（段ごとにずらす）
    ctx.fillRect(gx+off,gy+r,2,20); ctx.fillRect(gx+((off+TILE/2)%TILE),gy+r,2,20); }
  ctx.fillStyle="#4c4866"; ctx.fillRect(gx,gy,TILE,5);           // 上面ハイライト
  ctx.fillStyle="#1d1a2c"; ctx.fillRect(gx,gy+TILE-4,TILE,4);    // 下の影
}
// ===== 奥の町（レンガ舗装＋装飾）の描画 =====
function drawTownPave(gx,gy,x,y){
  ctx.fillStyle=((x+y)&1)?"#c2a47a":"#b69566";        // 2色レンガ（市松）
  ctx.fillRect(gx,gy,TILE,TILE);
  ctx.fillStyle="#9c8156";                            // 目地
  for(let r=0;r<=TILE;r+=TILE/2) ctx.fillRect(gx,gy+r-1,TILE,2);
  for(let r=0;r<=TILE;r+=TILE/2) ctx.fillRect(gx+r-1,gy,2,TILE);
  ctx.fillStyle="rgba(255,245,220,0.06)"; ctx.fillRect(gx,gy,TILE,3);   // ほのかな艶
}
function drawFountain(gx,gy){
  const cx=gx+TILE/2, cy=gy+TILE/2;
  ctx.fillStyle="rgba(0,0,0,0.18)"; ctx.beginPath(); ctx.ellipse(cx,gy+TILE-8,TILE*0.4,7,0,0,Math.PI*2); ctx.fill();
  ctx.fillStyle="#9a9aa6"; ctx.beginPath(); ctx.arc(cx,cy,TILE*0.42,0,Math.PI*2); ctx.fill();   // 石の基壇
  ctx.fillStyle="#7c7c8a"; ctx.beginPath(); ctx.arc(cx,cy,TILE*0.42,0,Math.PI*2); ctx.lineWidth=4; ctx.stroke();
  ctx.fillStyle="#3f8bcf"; ctx.beginPath(); ctx.arc(cx,cy,TILE*0.30,0,Math.PI*2); ctx.fill();   // 水
  ctx.fillStyle="#6fb6ef"; ctx.beginPath(); ctx.arc(cx-6,cy-4,TILE*0.12,0,Math.PI*2); ctx.fill();// 水面の光
  ctx.fillStyle="#9a9aa6"; ctx.fillRect(cx-4,cy-16,8,18);                                        // 中央柱
  const t=Math.sin(Date.now()*0.006)*2;
  ctx.fillStyle="#bfe3ff"; ctx.fillRect(cx-2,cy-22+t,4,8);                                        // 噴き上がる水
}
function drawLamp(gx,gy){
  const cx=gx+TILE/2;
  ctx.fillStyle="rgba(0,0,0,0.16)"; ctx.beginPath(); ctx.ellipse(cx,gy+TILE-6,12,5,0,0,Math.PI*2); ctx.fill();
  ctx.fillStyle="#2c2c36"; ctx.fillRect(cx-9,gy+TILE-10,18,6);     // 台座
  ctx.fillStyle="#3a3a46"; ctx.fillRect(cx-3,gy+18,6,TILE-26);     // 支柱
  ctx.fillStyle="#4a4a58"; ctx.fillRect(cx-8,gy+8,16,14);          // ランプ枠
  ctx.fillStyle="#ffe08a"; ctx.fillRect(cx-5,gy+11,10,9);          // 灯り
  ctx.fillStyle="rgba(255,220,140,0.22)"; ctx.beginPath(); ctx.arc(cx,gy+15,17,0,Math.PI*2); ctx.fill();  // 光
}
// 大噴水（2×2タイル）。町2マップで f を2×2に並べると、その右下タイルから一括描画される
function drawBigFountain(ax,ay){
  const S=TILE*2, cx=ax+S/2, cy=ay+S/2, t=Math.sin(Date.now()*0.006)*4;
  ctx.fillStyle="rgba(0,0,0,0.20)"; ctx.beginPath(); ctx.ellipse(cx,ay+S-16,S*0.40,13,0,0,Math.PI*2); ctx.fill();   // 影
  ctx.fillStyle="#b3b5c0"; ctx.beginPath(); ctx.arc(cx,cy,S*0.43,0,Math.PI*2); ctx.fill();                          // 石の外縁
  ctx.strokeStyle="#8a8c98"; ctx.lineWidth=7; ctx.beginPath(); ctx.arc(cx,cy,S*0.43,0,Math.PI*2); ctx.stroke();
  ctx.fillStyle="#3f8bcf"; ctx.beginPath(); ctx.arc(cx,cy,S*0.34,0,Math.PI*2); ctx.fill();                          // 外周プールの水
  ctx.strokeStyle="#5aa6e6"; ctx.lineWidth=4; ctx.beginPath(); ctx.arc(cx,cy,S*0.34,0,Math.PI*2); ctx.stroke();
  ctx.fillStyle="rgba(200,235,255,0.45)"; ctx.beginPath(); ctx.ellipse(cx-S*0.10,cy-S*0.07,S*0.11,S*0.045,0,0,Math.PI*2); ctx.fill(); // 水面の反射
  ctx.fillStyle="#9a9ca8"; ctx.fillRect(cx-9,cy-S*0.20,18,S*0.26);                                                  // 中央の石柱
  ctx.fillStyle="#b3b5c0"; ctx.beginPath(); ctx.ellipse(cx,cy-S*0.07,S*0.17,S*0.055,0,0,Math.PI*2); ctx.fill();     // 上段の皿
  ctx.fillStyle="#3f8bcf"; ctx.beginPath(); ctx.ellipse(cx,cy-S*0.08,S*0.12,S*0.035,0,0,Math.PI*2); ctx.fill();
  ctx.fillStyle="#cfeeff"; ctx.fillRect(cx-3,cy-S*0.34+t,6,S*0.20);                                                 // 噴き上がる水
  ctx.fillStyle="rgba(200,235,255,0.85)";
  for(let i=0;i<7;i++){ const a=Date.now()*0.004+i*0.9; ctx.beginPath(); ctx.arc(cx+Math.cos(a)*S*0.13, cy-S*0.15+Math.sin(a*1.4)*5, 3, 0, Math.PI*2); ctx.fill(); }   // 飛沫
}
function drawFlowers(gx,gy,x,y){
  const m=7, bw=TILE-2*m, by=gy+TILE-34;
  ctx.fillStyle="rgba(0,0,0,0.16)"; ctx.fillRect(gx+m,by+30,bw,6);            // 影
  ctx.fillStyle="#c2bba8"; ctx.fillRect(gx+m-2,by-2,bw+4,34);                 // 明るい石枠（はっきり）
  ctx.fillStyle="#5a3a22"; ctx.fillRect(gx+m,by,bw,30);                       // 土
  ctx.fillStyle="#6e4a2a"; ctx.fillRect(gx+m,by,bw,5);
  const cols=["#ff5a6e","#ffd23f","#f06ad0","#5ad0f0","#ff8a3a"];
  for(let i=0;i<5;i++){
    const fx=gx+m+9+i*((bw-18)/4), fy=by+18;
    ctx.fillStyle="#2f8a3a"; ctx.fillRect(fx-1,fy-2,3,11);                    // 茎
    ctx.fillStyle="#3aa84a"; ctx.fillRect(fx-5,fy+3,4,3); ctx.fillRect(fx+2,fy+1,4,3);  // 葉
    ctx.fillStyle=cols[(x*3+y+i)%cols.length];
    ctx.beginPath(); ctx.arc(fx, fy-5, 6, 0, Math.PI*2); ctx.fill();          // 花（大きめ）
    ctx.fillStyle="#fff6c0"; ctx.beginPath(); ctx.arc(fx, fy-5, 2, 0, Math.PI*2); ctx.fill();  // 花芯
  }
}
function drawCarpet(gx,gy){
  ctx.fillStyle="#6e1626"; ctx.fillRect(gx+TILE*0.16,gy,TILE*0.68,TILE);        // 赤絨毯
  ctx.fillStyle="#8c2236"; ctx.fillRect(gx+TILE*0.16,gy,4,TILE); ctx.fillRect(gx+TILE*0.84-4,gy,4,TILE);
  ctx.fillStyle="#caa24a"; ctx.fillRect(gx+TILE*0.16+6,gy,2,TILE); ctx.fillRect(gx+TILE*0.84-8,gy,2,TILE); // 金の縁取り
}
function drawThrone(gx,gy){
  ctx.fillStyle="#2a1730";                                       // 背もたれ
  ctx.fillRect(gx+TILE*0.26,gy+TILE*0.04,TILE*0.48,TILE*0.62);
  ctx.beginPath(); ctx.moveTo(gx+TILE*0.26,gy+TILE*0.14); ctx.lineTo(gx+TILE*0.18,gy-TILE*0.06); ctx.lineTo(gx+TILE*0.36,gy+TILE*0.08); ctx.closePath(); ctx.fill(); // 左の尖塔
  ctx.beginPath(); ctx.moveTo(gx+TILE*0.74,gy+TILE*0.14); ctx.lineTo(gx+TILE*0.82,gy-TILE*0.06); ctx.lineTo(gx+TILE*0.64,gy+TILE*0.08); ctx.closePath(); ctx.fill(); // 右の尖塔
  ctx.fillStyle="#caa24a";                                       // 金の縁
  ctx.fillRect(gx+TILE*0.26,gy+TILE*0.04,TILE*0.48,4);
  ctx.fillRect(gx+TILE*0.26,gy+TILE*0.04,4,TILE*0.62); ctx.fillRect(gx+TILE*0.74-4,gy+TILE*0.04,4,TILE*0.62);
  ctx.fillStyle="#7a1020"; ctx.fillRect(gx+TILE*0.32,gy+TILE*0.16,TILE*0.36,TILE*0.34);   // 赤クッション
  ctx.fillStyle="#3a2238"; ctx.fillRect(gx+TILE*0.22,gy+TILE*0.58,TILE*0.56,TILE*0.18);   // 座面
  ctx.fillStyle="#caa24a"; ctx.fillRect(gx+TILE*0.22,gy+TILE*0.58,TILE*0.56,3);
}

function drawTile(c,x,y){
  const gx=x*TILE, gy=y*TILE;
  if(c==="W"){ ctx.drawImage(SPR.water[Math.floor(Date.now()/350)%2], gx, gy); return; }
  const castle = mapTheme==="castle";
  const town=mapTheme==="town2";
  // 床ベース（城=石床 / 奥の町=レンガ舗装 / 暗いエリア=石畳 / 通常=草地）
  if(castle) drawCastleFloor(gx,gy,x,y);
  else if(town) drawTownPave(gx,gy,x,y);
  else ctx.drawImage((darkArea?SPR.stone:SPR.grass)[(x+y)&1], gx, gy);
  if(castle && c==="c"){ drawCarpet(gx,gy); return; }
  if(castle && c==="Z"){ drawCarpet(gx,gy); drawThrone(gx,gy); return; }
  if(town){
    // f を2×2に並べた大噴水。右下タイル（描画順で最後）で一括描画し、他3タイルは舗装のみ
    if(c==="f"){ if(map[y-1]&&map[y-1][x]==="f"&&map[y][x-1]==="f"){ drawBigFountain(gx-TILE,gy-TILE); } return; }
    if(c==="l"){ drawLamp(gx,gy); return; }
    if(c==="o"){ drawFlowers(gx,gy,x,y); return; }
  }
  if(c==="R"){
    if(castle){ drawCastleWall(gx,gy); }
    else if(darkArea){ // 洞窟の岩壁（岩肌・ひび・陰影）
      ctx.fillStyle="#2b2833"; ctx.fillRect(gx,gy,TILE,TILE);
      ctx.fillStyle="#3a3648"; ctx.fillRect(gx,gy,TILE,7);            // 上の明るい縁
      ctx.fillStyle="#161420"; ctx.fillRect(gx,gy+TILE-9,TILE,9);     // 下の影
      const h1=hash(x,y), h2=hash(x+9,y+3);
      ctx.fillStyle="#211e2a";                                        // ひび
      ctx.fillRect(gx+8+h1*40, gy+16, 4, 22);
      ctx.fillRect(gx+TILE-20, gy+10+h2*20, 4, 16);
      ctx.fillStyle="#46415a";                                        // 岩の出っぱり
      ctx.fillRect(gx+20+h2*24, gy+12, 10, 6);
    }else ctx.drawImage(SPR.rock, gx, gy);
  }
  else if(c==="T") ctx.drawImage(SPR.tree, gx, gy);
  else if(c==="H") ctx.drawImage(SPR.house, gx, gy);
  else if(c==="d") ctx.drawImage(SPR.dead, gx, gy);
  else if(c==="C"){ // 東の洞窟の入口（岩山＋黒い洞穴）
    // 岩山
    ctx.fillStyle="#6f6f7a"; ctx.beginPath();
    ctx.moveTo(gx+2,gy+TILE); ctx.lineTo(gx+8,gy+20); ctx.lineTo(gx+TILE/2,gy+2);
    ctx.lineTo(gx+TILE-8,gy+20); ctx.lineTo(gx+TILE-2,gy+TILE); ctx.closePath(); ctx.fill();
    ctx.fillStyle="#565662"; ctx.beginPath();   // 右側の陰
    ctx.moveTo(gx+TILE/2,gy+2); ctx.lineTo(gx+TILE-8,gy+20); ctx.lineTo(gx+TILE-2,gy+TILE);
    ctx.lineTo(gx+TILE/2,gy+TILE); ctx.closePath(); ctx.fill();
    ctx.fillStyle="#83838e";                     // 岩のハイライト
    ctx.fillRect(gx+TILE*0.3,gy+24,8,4); ctx.fillRect(gx+TILE*0.5,gy+34,6,4);
    // 黒い洞穴
    ctx.fillStyle="#07070d"; ctx.beginPath();
    ctx.moveTo(gx+TILE*0.30,gy+TILE); ctx.lineTo(gx+TILE*0.30,gy+TILE*0.56);
    ctx.arc(gx+TILE/2,gy+TILE*0.56,TILE*0.20,Math.PI,0);
    ctx.lineTo(gx+TILE*0.70,gy+TILE); ctx.closePath(); ctx.fill();
    // 入口の小石
    ctx.fillStyle="#4a4a54"; ctx.fillRect(gx+10,gy+TILE-9,12,7); ctx.fillRect(gx+TILE-22,gy+TILE-7,12,5);
  }
  else if(c==="G"){
    if(darkArea){ // 洞窟の出口（明るい穴）
      ctx.fillStyle="#cabf8a"; ctx.beginPath();
      ctx.moveTo(gx+18,gy+TILE); ctx.lineTo(gx+18,gy+30); ctx.arc(gx+TILE/2,gy+30,TILE/2-18,Math.PI,0); ctx.lineTo(gx+TILE-18,gy+TILE); ctx.closePath(); ctx.fill();
    }else{ // 村の入口（土の道）
      ctx.fillStyle="#b8945a"; ctx.fillRect(gx+TILE*0.28,gy,TILE*0.44,TILE);
      ctx.fillStyle="#a8824a"; ctx.fillRect(gx+TILE*0.28,gy,4,TILE); ctx.fillRect(gx+TILE*0.72-4,gy,4,TILE);
    }
  }
  else if(c==="E" && !flags.mageRescued){ // 救出シーン（マリン＋襲う骸骨）
    const mw=72;
    ctx.drawImage(SPR.bone, gx-TILE*0.55, gy+TILE-mw*1.1, mw, mw);   // 骸骨（左）
    const mi=CHARIMG.mage && CHARIMG.mage.d;                          // マリンの立ち絵
    if(mi && mi.complete && mi.naturalWidth) drawCharPortrait("mage", gx+TILE/2, gy+TILE-78, 84);
    else ctx.drawImage(SPR.mageField, gx+TILE/2-mw/2, gy+TILE-mw+8, mw, mw);   // 未ロード時の保険
    const bob=Math.sin(Date.now()*0.006)*3;
    ctx.fillStyle="#ff5a5a"; ctx.font="bold 26px sans-serif"; ctx.textAlign="center";
    ctx.fillText("!", gx+TILE/2, gy+TILE-mw-6+bob); ctx.textAlign="left";
  }
}

// 街の建物（大きめ・3タイル幅×2.5タイル高）。種類で看板を変える
function drawBuildings(){
  for(const b of buildings){
    if(b.type==="cathedral"){ drawCathedral(b); continue; }   // 大聖堂は専用描画
    const w=(b.w||3)*TILE, x=b.x*TILE, y=(b.y-1)*TILE, h=3*TILE;
    ctx.fillStyle="rgba(0,0,0,0.18)"; ctx.fillRect(x+8,y+h-6,w-16,8);          // 影
    ctx.fillStyle=b.color; ctx.fillRect(x+6,y+h*0.42,w-12,h*0.58);             // 壁
    ctx.fillStyle="rgba(0,0,0,0.12)"; ctx.fillRect(x+6,y+h*0.42,12,h*0.58);
    ctx.fillStyle=b.roof||"#7a2f2f"; ctx.beginPath();                         // 屋根
    ctx.moveTo(x,y+h*0.46); ctx.lineTo(x+w/2,y+8); ctx.lineTo(x+w,y+h*0.46); ctx.closePath(); ctx.fill();
    ctx.fillStyle="rgba(0,0,0,0.22)"; ctx.fillRect(x,y+h*0.43,w,8);
    const dw=44, dx=x+w/2-dw/2, dy=y+h-78;                                     // ドア
    ctx.fillStyle="#3a2410"; ctx.fillRect(dx,dy,dw,78);
    if(!b.deco){ ctx.fillStyle="#ffd23f"; ctx.beginPath(); ctx.arc(dx+dw-10,dy+40,3,0,Math.PI*2); ctx.fill(); }
    if(!b.deco){                                                              // 看板（機能建物のみ）
      const sx=x+w/2-30, sy=y+h*0.49, sw=60, sh=44;
      ctx.fillStyle="#efe6cf"; roundRect(sx,sy,sw,sh,5); ctx.fill();
      ctx.strokeStyle="#5e2222"; ctx.lineWidth=2.5; roundRect(sx,sy,sw,sh,5); ctx.stroke();
      drawSignIcon(b.type, x+w/2, sy+sh/2);
    }
    if(b.type==="church"){                                   // 屋根の十字（縦横バランス調整＋縁取りで見やすく）
      const cxp=x+w/2; ctx.lineCap="round";
      ctx.strokeStyle="rgba(0,0,0,0.35)"; ctx.lineWidth=7;   // 縁取り（影）
      ctx.beginPath(); ctx.moveTo(cxp,y+14); ctx.lineTo(cxp,y+70); ctx.moveTo(cxp-18,y+32); ctx.lineTo(cxp+18,y+32); ctx.stroke();
      ctx.strokeStyle="#ffe9a8"; ctx.lineWidth=5;            // 本体（金）。縦56px・横棒36px・横棒は上から1/3
      ctx.beginPath(); ctx.moveTo(cxp,y+14); ctx.lineTo(cxp,y+70); ctx.moveTo(cxp-18,y+32); ctx.lineTo(cxp+18,y+32); ctx.stroke();
    }
    ctx.textAlign="left";
  }
}
// 大聖堂（大きい教会・5×3）。明るい石・尖塔・十字・バラ窓・アーチ大扉
function drawCathedral(b){
  const x=b.x*TILE, w=(b.w||5)*TILE, footTop=b.y*TILE, footBot=(b.y+3)*TILE, fh=footBot-footTop, yTop=(b.y-2)*TILE;
  ctx.fillStyle="rgba(0,0,0,0.22)"; ctx.fillRect(x+10,footBot-6,w-20,10);             // 影
  ctx.fillStyle="#d8d2c0"; ctx.fillRect(x+8,footTop+fh*0.30,w-16,fh*0.70);            // 本体
  ctx.fillStyle="rgba(0,0,0,0.10)"; ctx.fillRect(x+8,footTop+fh*0.30,14,fh*0.70);
  ctx.fillStyle="#6e8a9a"; ctx.beginPath();                                           // 側廊の屋根
  ctx.moveTo(x+2,footTop+fh*0.40); ctx.lineTo(x+w*0.5,footTop-fh*0.05); ctx.lineTo(x+w-2,footTop+fh*0.40); ctx.closePath(); ctx.fill();
  const tw=TILE*1.5, tx=x+w/2-tw/2;                                                   // 中央の塔
  ctx.fillStyle="#cfc8b4"; ctx.fillRect(tx, yTop+TILE*0.9, tw, (footTop+fh*0.45)-(yTop+TILE*0.9));
  ctx.fillStyle="rgba(0,0,0,0.10)"; ctx.fillRect(tx, yTop+TILE*0.9, 12, (footTop+fh*0.45)-(yTop+TILE*0.9));
  ctx.fillStyle="#5e7a8a"; ctx.beginPath();                                           // 塔の屋根
  ctx.moveTo(tx-8, yTop+TILE*0.95); ctx.lineTo(x+w/2, yTop); ctx.lineTo(tx+tw+8, yTop+TILE*0.95); ctx.closePath(); ctx.fill();
  ctx.strokeStyle="#ffe9a8"; ctx.lineWidth=5; ctx.lineCap="round";                    // 十字
  ctx.beginPath(); ctx.moveTo(x+w/2, yTop-TILE*0.55); ctx.lineTo(x+w/2, yTop-2);
  ctx.moveTo(x+w/2-13, yTop-TILE*0.38); ctx.lineTo(x+w/2+13, yTop-TILE*0.38); ctx.stroke();
  ctx.fillStyle="#3a5a8a"; ctx.beginPath(); ctx.arc(x+w/2, footTop+fh*0.18, 17, 0, Math.PI*2); ctx.fill();  // バラ窓
  ctx.strokeStyle="#caa24a"; ctx.lineWidth=3; ctx.stroke();
  ctx.fillStyle="#1d2e48";                                                            // 側面の窓
  for(const wx of [x+w*0.18, x+w*0.82-16]) ctx.fillRect(wx, footTop+fh*0.5, 16, 30);
  const dw=60, dxp=x+w/2-dw/2, dyt=footBot-96;                                         // アーチ大扉
  ctx.fillStyle="#4a3018"; ctx.beginPath();
  ctx.moveTo(dxp, footBot-6); ctx.lineTo(dxp, dyt+28); ctx.arc(x+w/2, dyt+28, dw/2, Math.PI, 0); ctx.lineTo(dxp+dw, footBot-6); ctx.closePath(); ctx.fill();
  ctx.strokeStyle="#caa24a"; ctx.lineWidth=3; ctx.stroke();
  ctx.fillStyle="#caa24a"; ctx.fillRect(x+w/2-2, dyt+10, 4, footBot-6-(dyt+10));       // 扉の縦桟
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
  }else if(type==="church"){   // 十字（縦横とも中央そろえ）
    ctx.fillStyle="#d9b24a"; ctx.fillRect(cx-3,cy-15,6,30); ctx.fillRect(cx-15,cy-7,30,6);
  }
}

// 方向→画像キー（d=正面 u=背面 r=右 l=左）
function dirKey(d){ return d==="up"?"u" : d==="left"?"l" : d==="right"?"r" : "d"; }
// キャラ画像をワールド座標(wx,wy)へ4方向で描く。描けたら true
// スプライトの「足元の横中心」を測ってキャッシュ。左右非対称な絵(斧など)でも本体を中央そろえできる。
const _footFracCache=new WeakMap();
function footFrac(img){
  if(_footFracCache.has(img)) return _footFracCache.get(img);
  let f=0.5;
  try{
    const w=img.naturalWidth, h=img.naturalHeight;
    const oc=document.createElement('canvas'); oc.width=w; oc.height=h;
    const o=oc.getContext('2d',{willReadFrequently:true}); o.drawImage(img,0,0);
    const d=o.getImageData(0,0,w,h).data;
    const op=(x,y)=>d[(y*w+x)*4+3]>=128;
    let by0=-1,by1=-1;
    for(let y=0;y<h;y++){ for(let x=0;x<w;x++){ if(op(x,y)){ if(by0<0)by0=y; by1=y; break; } } }
    if(by1>=0){
      const band=Math.max(1,Math.round((by1-by0+1)*0.15)), ylo=by1-band+1;
      let sum=0,cnt=0;
      for(let y=ylo;y<=by1;y++) for(let x=0;x<w;x++) if(op(x,y)){ sum+=x; cnt++; }
      if(cnt) f=(sum/cnt)/w;
    }
  }catch(e){}
  _footFracCache.set(img,f); return f;
}
function drawCharImg(imgKey, dir, wx, wy, walk){
  const set=CHARIMG[imgKey];
  const img=set && (set[dirKey(dir)] || set.d);
  if(img && img.complete && img.naturalWidth){
    const dh=104, dw=Math.round(img.naturalWidth*dh/img.naturalHeight);
    const bob=(walk && Math.floor(Date.now()/200)%2)? 5:0;
    const dx=Math.round(wx+player.size/2-footFrac(img)*dw);   // 足元の中心で揃える（斧などの非対称対策）
    const dy=Math.round(wy+player.size-dh)+16-bob;
    ctx.imageSmoothingEnabled=false;
    ctx.drawImage(img, dx, dy, dw, dh);
    return true;
  }
  return false;
}
// ステータス画面用の立ち絵（正面固定）
function drawCharPortrait(imgKey, cx, topY, ph){
  const set=CHARIMG[imgKey]; const img=set && set.d;
  if(img && img.complete && img.naturalWidth){
    const pw=Math.round(img.naturalWidth*ph/img.naturalHeight);
    ctx.imageSmoothingEnabled=false;
    ctx.drawImage(img, Math.round(cx-footFrac(img)*pw), Math.round(topY), pw, ph);   // 足元の中心で揃える
  }
}
// 画像未ロード時の勇者フォールバック（旧ドット絵）
function drawHeroFallback(){
  const dir=player.dir;
  const set = dir==="up"?SPR.heroUp : dir==="left"?SPR.heroLeft
            : dir==="right"?SPR.heroRight : SPR.heroDown;
  let frame=0;
  if(player.walk){ frame = (Math.floor(Date.now()/200)%2) ? 2 : 1; }
  const spr=set[frame];
  const bob=(player.walk && Math.floor(Date.now()/200)%2)? CHAR:0;
  const dx=Math.round(player.x+player.size/2-spr.width/2);
  const dy=Math.round(player.y+player.size-spr.height)+12-bob;
  ctx.drawImage(spr, dx, dy);
}
// 勇者＋仲間を足元のyで奥→手前に並べて描画（仲間は勇者の足跡を追従）
function drawParty(){
  const list=[{key:"hero", dir:player.dir, x:player.x, y:player.y, walk:player.walk, hero:true}];
  const fol=party.slice(1);
  for(let i=0;i<fol.length;i++){
    const need=(i+1)*FOLLOW_GAP;
    if(trail.length<=need) continue;   // 足跡がまだ足りない（建物を出た直後・マップ移動直後）→ 表示しない。歩くと順に現れる
    const t=trail[need];
    const key=fol[i].img || ({hero:"hero",mage:"mage",warrior:"warrior",priest:"sister"})[fol[i].role] || "hero";  // imgが無くても職業から補う
    list.push({key, dir:t.dir, x:t.x, y:t.y, walk:player.walk, hero:false});
  }
  list.sort((a,b)=>a.y-b.y);   // 上(奥)から手前へ
  for(const c of list){ if(!drawCharImg(c.key,c.dir,c.x,c.y,c.walk) && c.hero) drawHeroFallback(); }
}

function drawNpc(n){
  let topY;
  const cset = n.char && CHARIMG[n.char];                         // ガルド等＝パーティと同じ立ち絵
  const im = n.imgKey && NPCIMG[n.imgKey];                        // 村人の立ち絵
  if(cset && cset.d && cset.d.complete && cset.d.naturalWidth){
    const wx=n.x*TILE+(TILE-player.size)/2, wy=n.y*TILE+(TILE-player.size)/2;
    drawCharImg(n.char, "down", wx, wy, false);                   // 足元中心で描画（武器の非対称も補正）
    topY=Math.round(wy+player.size-104)+16;
  }else if(im && im.complete && im.naturalWidth){                 // 本格立ち絵（画像）
    const dh=110, dw=Math.round(im.naturalWidth*dh/im.naturalHeight);   // 勇者(104px)とそろう存在感
    const dx=Math.round(n.x*TILE+TILE/2-dw/2);
    topY=Math.round(n.y*TILE+TILE-dh+8);                          // 足をタイル下端付近に
    ctx.imageSmoothingEnabled=false; ctx.drawImage(im, dx, topY, dw, dh);
  }else{                                                          // 未ロード時は従来のドット絵
    const dx=n.x*TILE+TILE/2-n.spr.width/2;
    topY=n.y*TILE+TILE-n.spr.height+8;
    ctx.drawImage(n.spr, dx, topY);
  }
  // 「！」（ふわふわ上下）
  const bob=Math.sin(Date.now()*0.005)*3;
  ctx.fillStyle="#ffd23f"; ctx.font="bold 24px sans-serif"; ctx.textAlign="center";
  ctx.fillText("!", n.x*TILE+TILE/2, topY-8+bob); ctx.textAlign="left";
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
  // 名前タグ（名前がある時だけ。地の文シーンでは空タグを出さない）
  if(dialog.name){
    ctx.fillStyle="#d9b24a"; roundRect(m+18,y-2,140,34,8); ctx.fill();
    ctx.fillStyle="#101830"; ctx.font="bold 18px sans-serif"; ctx.textAlign="center";
    ctx.fillText(dialog.name, m+18+70, y+21); ctx.textAlign="left";
  }
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
  const x=70, y0=VIEW-160, lh=28, sel=b.state==="command"?b.cmd:b.spell;   // 5項目(にげる)まで枠内に収める
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
  // 上：パーティ枠（横長バナー・全員を列で。DQ風に敵と分離して重ならない）
  const _n=party.length, _bw=VIEW-40, _bh=96, _col=(_bw-24)/_n;
  winBox(20,20,_bw,_bh);
  ctx.textAlign="left";
  for(let i=0;i<_n;i++){
    const m=party[i], cx=32+i*_col;
    ctx.font="20px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.fillStyle=m.down?"#777":"#fff";
    let nm=m.name; if(m.buffAtkT>0)nm+=" 攻↑"; if(m.buffDefT>0)nm+=" 守↑";
    ctx.fillText(nm, cx, 48);
    ctx.font="21px monospace";
    ctx.fillStyle=m.down?"#777":(m.hp<=m.maxhp*0.25?"#ff6a6a":"#cfe0ff"); ctx.fillText("HP "+m.hp+"/"+m.maxhp, cx, 76);
    ctx.fillStyle=m.down?"#777":"#9fd0ff"; ctx.fillText("MP "+m.mp+"/"+m.maxmp, cx, 98);
  }
  // 敵（複数体を横に並べる。前列＝プレイヤーの攻撃対象）
  const list=foes();
  const _top=20+_bh, _bot=VIEW-240;
  const big=list.some(e=>e.big);
  const eh = big?260 : (list.length>=3?160 : (list.length===2?190:210));   // 多いほど小さく
  let total=0; const dims=list.map(e=>{ const im=SPR[e.spr]; const iw=(im&&im.width)||80, ih=(im&&im.height)||80; const w=Math.round(eh*iw/ih); total+=w; return {im,w}; });
  total += Math.max(0,list.length-1)*28;
  let cx0 = VIEW/2 - total/2;
  const front=frontFoe();
  for(let i=0;i<list.length;i++){
    const e=list[i], im=dims[i].im, ew=dims[i].w;
    const float=Math.sin(Date.now()*0.003 + (e._ph||0))*7;
    const sx = (b.shake>0 && e===front) ? (Math.random()-0.5)*b.shake : 0;
    const ex=cx0+sx, ey=_top+((_bot-_top)-eh)/2+float;
    ctx.fillStyle="rgba(0,0,0,0.25)"; ctx.fillRect(Math.round(ex+ew*0.12), Math.round(ey+eh+4), Math.round(ew*0.76), 3);  // 足元の影
    if(im && im.width && !(b.flash>0 && Math.floor(Date.now()/60)%2)){
      if(e.tint){   // 色相を金色などに置き換え（陰影・透明は保持＝color合成→alpha再適用）
        const oc=document.createElement('canvas'); oc.width=im.width; oc.height=im.height;
        const o=oc.getContext('2d'); o.drawImage(im,0,0);
        o.globalCompositeOperation="color"; o.fillStyle=e.tint; o.fillRect(0,0,oc.width,oc.height);
        o.globalCompositeOperation="destination-in"; o.drawImage(im,0,0);
        ctx.drawImage(oc, ex, ey, ew, eh);
      } else ctx.drawImage(im, ex, ey, ew, eh);
    }
    cx0 += ew + 28;
  }
  ctx.fillStyle="#fff"; ctx.font="22px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="center";
  const label = list.length<=1 ? (list[0]?list[0].name:"") : (list[0].name.replace(/ [A-Z]$/,"")+" ×"+list.length);
  ctx.fillText(label, VIEW/2, _bot-10); ctx.textAlign="left";
  // プレイヤー被弾フラッシュ
  if(b.shakeP>0 && Math.floor(Date.now()/50)%2){ ctx.fillStyle="rgba(200,30,30,0.18)"; ctx.fillRect(0,0,VIEW,VIEW); }
  // 下部ウィンドウ
  const my=VIEW-240;
  winBox(20,my,VIEW-40,220);
  if(b.state==="command"||b.state==="spell"||b.state==="skill"){
    // 誰のターンか
    ctx.fillStyle="#ffd23f"; ctx.font="22px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="left";
    ctx.fillText(b.actor.name+"の ばん", 50, my+40);
  }
  if(b.state==="command") drawMenu(BATTLE_CMDS);
  else if(b.state==="spell"){
    const list=(b.actor.spells||[]).map(s=> s.name+(s.mp>0?"("+s.mp+")":""));
    list.push("もどる"); drawMenu(list);
  }
  else if(b.state==="skill"){
    const list=(b.actor.skills||[]).map(s=> s.name+(s.mp>0?"("+s.mp+")":""));
    list.push("もどる"); drawMenu(list);
  }
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
  ctx.fillStyle="#ffd23f"; ctx.font="26px 'Hiragino Kaku Gothic ProN',sans-serif";
  ctx.fillText("パーティの じょうたい", x+34, y+44);
  // 各メンバー
  const lx=x+36; let ly=y+86;
  for(const m of party){
    drawCharPortrait(m.img, x+w-70, ly-26, 88);   // 右側に立ち絵（正面ポーズ流用）
    ctx.fillStyle = m===stats ? "#ffe9a8" : "#cfe0ff"; ctx.font="24px 'Hiragino Kaku Gothic ProN',sans-serif";
    ctx.fillText(m.name+(m.job&&m.job!==m.name?"  "+m.job:"")+"  Lv"+m.lv, lx, ly); ly+=32;
    ctx.font="22px monospace"; ctx.fillStyle="#fff";
    ctx.fillText("HP "+m.hp+"/"+m.maxhp+"   MP "+m.mp+"/"+m.maxmp, lx+16, ly); ly+=28;
    ctx.fillStyle="#9fb0d0";
    ctx.fillText("ちから "+atkOf(m)+"   まもり "+defOf(m), lx+16, ly); ly+=28;
    ctx.fillStyle="#bda0e0";
    ctx.fillText("ぶき:"+(m.weapon?m.weapon.name:"なし")+"  よろい:"+(m.armor?m.armor.name:"なし"), lx+16, ly); ly+=32;
  }
  // 所持金・もちもの
  ctx.fillStyle="#ffd23f"; ctx.font="22px 'Hiragino Kaku Gothic ProN',sans-serif";
  ctx.fillText("しょじきん・もちもの", lx, ly); ly+=32;
  const items=[stats.gold+" G"];
  if(stats.herb>0)   items.push("やくそう×"+stats.herb);
  if(stats.elixir>0) items.push("エリクサー×"+stats.elixir);
  ctx.fillStyle="#fff"; ctx.font="20px monospace";
  ctx.fillText(items.join("   "), lx+16, ly);
  // 閉じる案内
  ctx.fillStyle="#9aa0b0"; ctx.font="18px sans-serif"; ctx.textAlign="center";
  ctx.fillText("M キー / タップ でとじる", VIEW/2, y+h-18);
  ctx.textAlign="left";
}

function draw(){
  if(mode==="intro"){ drawIntro(); return; }
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
  drawParty();
  if(moveTarget){   // タップ移動の目標マーカー（脈動する金色のリング）
    const r=10+Math.sin(Date.now()*0.006)*3;
    ctx.strokeStyle="rgba(255,210,80,0.9)"; ctx.lineWidth=3;
    ctx.beginPath(); ctx.arc(moveTarget.x, moveTarget.y, r, 0, Math.PI*2); ctx.stroke();
    ctx.fillStyle="rgba(255,210,80,0.9)";
    ctx.beginPath(); ctx.arc(moveTarget.x, moveTarget.y, 2.5, 0, Math.PI*2); ctx.fill();
  }
  ctx.restore();
  // 周辺減光は暗いエリアのみ（城は松明照明風に軽め＆紫、洞窟は濃い闇）
  if(darkArea){
    const vg=ctx.createRadialGradient(VIEW/2,VIEW/2,VIEW*0.25,VIEW/2,VIEW/2,VIEW*0.7);
    vg.addColorStop(0,"rgba(0,0,0,0)");
    vg.addColorStop(1, mapTheme==="castle" ? "rgba(22,6,32,0.5)" : "rgba(8,4,18,0.72)");
    ctx.fillStyle=vg; ctx.fillRect(0,0,VIEW,VIEW);
  }
  if(mode==="talk" && dialog.active) drawDialog();
  if(mode==="field"||mode==="status") drawMenuButton();
  if(mode==="status") drawStatus();
  if(mode==="service") drawService();
}

function drawService(){
  const s=service;
  if(s.type==="choice"){ drawChoice(s); return; }   // 二択（戦う/引き返す等）は専用描画
  ctx.fillStyle="rgba(0,0,8,0.62)"; ctx.fillRect(0,0,VIEW,VIEW);   // 背景を暗くして文字を見やすく
  // 店名
  winBox(20,28,380,60);
  ctx.fillStyle="#fff"; ctx.font="28px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="left";
  ctx.fillText(s.name, 46, 68);
  // 所持金（右寄せ）
  winBox(VIEW-250,28,230,60);
  ctx.fillStyle="#ffd23f"; ctx.font="26px monospace"; ctx.textAlign="right";
  ctx.fillText(stats.gold+" G", VIEW-44, 68); ctx.textAlign="left";
  // メイン窓：メニューは縦長で1カラム（長い商品名でも重ならない）、メッセージは下段の小窓
  const isMenu = s.phase==="menu";
  const wh = isMenu ? 470 : 230, wx=20, ww=VIEW-40, wy=VIEW-wh-20;
  winBox(wx,wy,ww,wh);
  if(isMenu){
    s.rects=[];
    const PER=7, page=Math.floor(s.cursor/PER), start=page*PER, end=Math.min(start+PER,s.labels.length);
    const tx=wx+64, y0=wy+62, lh=58;
    ctx.font="26px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="left";
    for(let i=start;i<end;i++){
      const by=y0+(i-start)*lh;
      if(i===s.cursor){                                          // 選択行を帯でハイライト
        ctx.fillStyle="rgba(255,210,63,0.16)"; roundRect(wx+18,by-38,ww-36,50,8); ctx.fill();
        ctx.fillStyle="#ffd23f"; ctx.fillText("▶", wx+28, by);
      }
      const lab=s.labels[i], baseCol=(i===s.cursor?"#fff":"#c8d0e0");
      if(typeof lab==="string"){ ctx.fillStyle=baseCol; ctx.fillText(lab, tx, by); }
      else{                                                      // 値段は右端に金色で（増加値と離して見やすく）
        ctx.fillStyle=baseCol; ctx.fillText(lab.t, tx, by);
        ctx.fillStyle="#ffd23f"; ctx.textAlign="right"; ctx.fillText(lab.p, wx+ww-40, by); ctx.textAlign="left";
      }
      s.rects.push({x:wx+18,y:by-38,w:ww-36,h:50,i});            // 行全体をタップ対象に
    }
    const pages=Math.ceil(s.labels.length/PER);
    s.navRects=[];
    if(pages>1){
      ctx.fillStyle="#9aa0b0"; ctx.font="20px sans-serif"; ctx.textAlign="center";
      ctx.fillText("◀  "+(page+1)+" / "+pages+"  ▶", VIEW/2, wy+wh-22); ctx.textAlign="left";
      s.navRects.push({dir:-1, x:wx, y:wy+wh-46, w:VIEW/2-wx, h:44});        // ◀ タップ＝前ページ
      s.navRects.push({dir:+1, x:VIEW/2, y:wy+wh-46, w:wx+ww-VIEW/2, h:44}); // ▶ タップ＝次ページ
    }
  }else{
    ctx.fillStyle="#fff"; ctx.font="28px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="left";
    ctx.fillText((s.msgs[s.msgIdx]||"").slice(0,s.shown), wx+40, wy+78);
    const full=s.msgs[s.msgIdx]||"";
    if(s.shown>=full.length && Math.floor(Date.now()/400)%2){
      ctx.fillStyle="#ffd23f"; ctx.beginPath();
      ctx.moveTo(VIEW-62,wy+wh-46); ctx.lineTo(VIEW-44,wy+wh-46); ctx.lineTo(VIEW-53,wy+wh-32); ctx.fill();
    }
  }
}

// 施設メッセージの1文字送り
function updateService(){
  const s=service; if(!s||s.phase!=="msg") return;
  const full=s.msgs[s.msgIdx]||""; if(s.shown<full.length) s.shown++;
}

function loop(){ update(); if(mode==="service") updateService(); draw(); requestAnimationFrame(loop); }
const _ld=document.getElementById("loading");
function hideLoading(){ if(_ld) _ld.style.display="none"; }
// マップを /maps.json から取得（PCプレビューは静的ファイル、実機はArduinoが配信）。
// 実機(WiFiS3)は同時接続数が少ないので、確実に読めるよう最大4回リトライする。
async function loadAllMaps(){
  let last;
  for(let attempt=1; attempt<=4; attempt++){
    try{
      const c=new AbortController(); const to=setTimeout(()=>c.abort(),6000);
      const r=await fetch("/maps.json",{cache:"no-store",signal:c.signal}); clearTimeout(to);
      if(!r.ok) throw new Error("status "+r.status);
      return await r.json();
    }catch(e){
      last=e;
      const s=_ld&&_ld.querySelector(".s"); if(s) s.textContent="マップ読み込み中…("+attempt+"/4)";
      await new Promise(res=>setTimeout(res, 500*attempt));   // 少し待って再試行
    }
  }
  throw last;
}
(async function start(){
  try{
    MAPS = await loadAllMaps();                       // マップ読み込み（これが無いと動けない）
    buildSprites(); loadGame(); loadMap(respawn.map, respawn.tx, respawn.ty); startIntro(); loop();
    hideLoading();   // 読み込み完了
    setInterval(pollJoy,150);                         // ★マップ取得後にジョイ通信を開始（起動時の取得を邪魔しない）
  }catch(err){
    console.error("init error:", err);
    if(_ld){ _ld.style.display="flex"; _ld.innerHTML='<div class="t" style="color:#ff8a6a">読み込みエラー</div><div class="s">ページを 再読み込み してください</div>'; }
  }
})();
</script>
</body>
</html>
)rawliteral";
