#pragma once
// ゲームのHTML/JS（C++11 生文字列）。
// ※Arduinoのプロトタイプ生成器がJSの function を誤認し、巨大なbase64行で解析が壊れるため、
//   生文字列はこの別ヘッダに分離して #include する（gccは正しく処理する）。
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
#loading{position:fixed;inset:0;z-index:99;display:flex;flex-direction:column;
  align-items:center;justify-content:center;background:radial-gradient(circle at 50% 40%,#243a5e,#0c1424 75%);}
#loading .t{color:#ffe9a8;font-size:30px;letter-spacing:8px;font-weight:700;text-shadow:0 0 12px #b58a2a;}
#loading .s{color:#9fb0d0;font-size:16px;margin-top:18px;animation:blink 1s infinite;}
#loading .bar{width:200px;height:8px;margin-top:18px;border-radius:4px;background:#1a2540;overflow:hidden;}
#loading .bar>i{display:block;height:100%;width:40%;border-radius:4px;background:#ffd23f;animation:slide 1.1s infinite;}
@keyframes blink{0%,100%{opacity:1}50%{opacity:.3}}
@keyframes slide{0%{margin-left:-40%}100%{margin-left:100%}}
</style>
</head>
<body>
<div id="loading"><div class="t">⚔ Q U E S T ⚔</div><div class="bar"><i></i></div><div class="s">よみこみちゅう…</div></div>
<h2>― Q U E S T ―</h2>
<div class="frame"><canvas id="game" width="800" height="800"></canvas></div>
<div id="hint">WASD / 矢印で移動　Space / Enter で話す　スマホはタップ</div>

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
(function(){const E={"rotcorpse":"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","soulflame":"iVBORw0KGgoAAAANSUhEUgAAAC4AAABQCAMAAABRT7e0AAAAY1BMVEX+//78/f36/PPT+P3L8fu18/yO6/uE3vxq2f2Zqt2If89g0fxUt/w9jPVrYslUVtFFUNhEOLoybPQzWfQ1PtgzKLcravkqX/YoTOQoM9onJ707FLYMCjgAAMYAAQAAAAAAAABUW/+0AAAAIXRSTlP//////////////////////////////////////////wCfwdAhAAAER0lEQVR42u1X23LiOhCU8U3EFl5JlnUJZfj/r9yeke3EwWTh4bwdUpUi0DQ9PT0jRdyPH04fviyO0Tq9BXdjGl6Aj3/WZzG9Arebmhfg1i5wl2J0/2ZfyT+jDUfVHpfqUvAvwnVG+8NaH+BDdC5aM4VXtAMdUwwT0K8YeXcx7NHhN/gnqO1kbIyfq7WH8NwVnaKdph7w5Fa4OYC7GBcDDeCKxAzf2LfvWOCfIZDLmsmlBJ6+7vLQ7ZU9mJAIHvxkOtkZDyP1Wq6xNuy162AXY7xXwCsDPWlc2YP9AY9/1mj5qWtkP0123Aq+259GXgYeOKbv2laiXghK7mmbXEKoHMhV19VSdpP3lrulj+Au4cvJdyVlWzed7I2HoXo35l/wSBFHuMDd1nXddbATfrrdnIu1Cu5nCMbA9prhHdRHYsd7awliaQDxmslmdFXX8ouc3lxLFkvDMG2qnxbusmK4X6OQuIRNzKABDwrFIQFNXRdl3aBTk82wZLfR4lK1pnChmb3CL8ALyGm7rAaOYbZSbhbDEzqKFyFEwpC6KgTw6Kyi6ESqiuHQLXLKHbUTQhpYXlVFATl1Q60FHt5atibDB1SSLmP0CrrJ8hJwkfV0ColTkK43MfAQf8eJZFf0w2gBfgiSSvX91+QSHBmP3B9QV0UJLH8AgioU0MvekxaX4Y4mn0NO3SyEYHRBT0rm73pDfrIegkPdpHLvi28P5od+ZJ/8ZEFCpzD1HZR0TVWXC7dY+alewBHkOFIQBBaLUWQ3bIFsfGJjrytRZnuQtcD1CpcTLim1paiu52LFl/qD/IF6mkS/wtmUDlrKk/iYrxv9eb5W4sRp6+EF50ag+zxurKXQt/lSZHeq620+U7WtpMHNa1NgB0QDY1hLfb3dQCky+TwPrF5KVs+lUmTC5JEXFHoG4/VyAlxU+KL5euZiSXzubI5YIHhVio8rYRh+vs14znBJyRk5ZTnvo1Vd25TMvsKv83yDT1VF5D7kFb1sguj7rkWpFwhmKyEGH2yprxgya6PWu4VNxZZF5WZNlSKRcOZyylIswb8NH+xBxhrksdENGUlduugy14kNMS6rQKwbFTPTS3hZrl0VZZlHkE7BbXEsCxVZ8KZvYE9Bo5r566bJ87etsW3p0fFoejS3oqSBuwI10mVWU/bwgVY1TpqWRuqUR5vBu6UtltOEdhNlmSpAydhjvAewJHcrnuHK58WH8y9Q3rDIEPOs2h0cB1/XmBS9ITwdN5QqRlv/9AqRD4S65cbrH+fkI5wObtO1tH83+6w6YjcLPixH68GlQ+xuYutNiZYOtj3XaX6Fc/hbWA67RtCr3R1CPEi3aNX3U/VXOC4H+QoR4sMZfAjHeZP749y/L1c6oU715CZ23CalzOE170mbRqOmJ/SP187hgk9MU9Sv399dNPad637y4/AOPL7zz0QK+v4OfLy/A3/6+B/+38I/nsD/Ah0GX828jtDoAAAAAElFTkSuQmCC","gravespider":"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","greatswordsoldier":"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","infernolion":"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","darkknight":"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","hellgeneral":"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"};for(const k in E){const im=new Image();im.src="data:image/png;base64,"+E[k];ENEMYIMG[k]=im;}})();
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
  SPR.stone=[makeStone(0),makeStone(1)];
  SPR.water=[makeWater(0),makeWater(1)];
  SPR.mageField=makeSprite(VILLAGER, npcPal("#8a4fc0"), CHAR);  // 救出シーン用の魔法使い
  SPR.slime=makeSprite(SLIME,SLIME_PAL,CHAR);
  SPR.bat=makeSprite(BAT,BAT_PAL,CHAR);
  SPR.bone=makeSprite(BONE,BONE_PAL,CHAR);
  SPR.rotcorpse=ENEMYIMG.rotcorpse;
  SPR.soulflame=ENEMYIMG.soulflame;
  SPR.gravespider=ENEMYIMG.gravespider;
  SPR.greatswordsoldier=ENEMYIMG.greatswordsoldier;
  SPR.infernolion=ENEMYIMG.infernolion;
  SPR.darkknight=ENEMYIMG.darkknight;
  SPR.hellgeneral=ENEMYIMG.hellgeneral;
}

// ====== ゲームデータ（後でAPI配信へ差し替えやすいよう分離） ======
// マップは maps.json に分離し、起動時に /maps.json から読み込む（下の MAPS）。
// 記号: . 草地 / T 木 / W 水 / R 岩 / H 家 / P 開始 / G,C,N,B,D,K=出入口 / E=救出 / Z=玉座 / c=赤絨毯 / f,l,o,s=装飾
let MAPS={};   // {field,cave,area2,town,town2,road,castle} を /maps.json から取得
const GAME_DATA={
  // 敵（戦闘）。spr はスプライトキー
  enemies:[
    { name:"ブルン",     spr:"slime", maxhp:8,  atk:7,  def:3, exp:3,  gold:2,  weak:["火"],        resist:[] },
    { name:"ヤミバット", spr:"bat",   maxhp:13, atk:10, def:4, exp:6,  gold:5,  weak:["氷"],        resist:["火"] },
    { name:"ホネナイト", spr:"bone",  maxhp:20, atk:15, def:7, exp:13, gold:11, weak:["火"],        resist:["氷"] }
  ]
};
// 魔王（ラスボス）。※スプライトは仮(bone)。魔王の絵が出来たら spr を差し替え
const MAOH={ name:"まおう", spr:"bone", maxhp:120, atk:28, def:12, exp:200, gold:300, weak:[], resist:[], big:true };
// 中ボス：ゲヘナ将（東の洞窟最奥でエリアへの道を塞ぐ関門）
const GEHENA={ name:"ゲヘナ将", spr:"hellgeneral", maxhp:64, atk:23, def:17, exp:85, gold:100, weak:["氷"], resist:["火","雷"], big:true };
// エリアの強敵（中くらい）
const E_AREA=[
  { name:"クサレビト",   spr:"rotcorpse",         maxhp:32, atk:16, def:9,  exp:19, gold:16, weak:["火"],       resist:["氷"] },
  { name:"ヒトダマギ",   spr:"soulflame",         maxhp:27, atk:18, def:8,  exp:24, gold:22, weak:["火"],       resist:["氷"] },
  { name:"ハカモリグモ", spr:"gravespider",       maxhp:38, atk:19, def:10, exp:33, gold:38, weak:["火","氷"], resist:["雷"] }
];
// 道の強敵（魔王城へ近づくほど強い）
const E_ROAD=[
  { name:"ザンギル斬兵", spr:"greatswordsoldier", maxhp:46, atk:22, def:11, exp:48, gold:50,  weak:["氷"], resist:["火"] },
  { name:"ガオウレオン", spr:"infernolion",       maxhp:58, atk:24, def:13, exp:70, gold:80,  weak:["氷"], resist:["火"] },
  { name:"ノクスナイト", spr:"darkknight",        maxhp:70, atk:26, def:16, exp:92, gold:115, weak:["火"], resist:["氷","雷"] }
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
let mode="field";          // "field" | "talk" | "battle" | "status" | "service"
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
const HERO_KIT={ skills: party[0].skills, spells: party[0].spells };   // 勇者の固定の特技/呪文（セーブ復元用）
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

// 戦闘状態
let battle=null;
let encCooldown=0;
const BATTLE_CMDS=["たたかう","とくぎ","じゅもん","どうぐ","にげる"];

// 施設（宿屋/武器屋/道具屋/教会）
let service=null;

// ストーリーフラグ（イベント1回判定など）
let flags={ mageRescued:false, seraJoined:false, maohDefeated:false, gehenaDefeated:false };

let keys={};

// ====== 初期化 ======
function loadMap(name, sx, sy){
  currentMap=name;
  trail=[];                 // マップ移動時は足跡をリセット（仲間が勇者に寄る）
  const m=MAPS[name];
  map=m.map;
  buildings=(m.buildings||[]);
  darkArea=!!m.dark;
  mapTheme=m.theme||"";
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

// 建物の足元当たり判定（ドア以外は壁）。ドア=(x+1, y+1)
// 建物のドア位置（装飾建物 deco はドア無し＝全部壁）。サイズは w×h（既定3×2）
function buildingDoor(b){
  if(b.deco) return null;
  const w=b.w||3, h=b.h||2;
  return { dx: b.x + (w>>1), dy: b.y + (h-1) };
}
function buildingWall(tx,ty){
  for(const b of buildings){
    const w=b.w||3, h=b.h||2;
    if(tx>=b.x && tx<b.x+w && ty>=b.y && ty<b.y+h){
      const d=buildingDoor(b);
      if(d && tx===d.dx && ty===d.dy) return false;  // ドアは通れる
      return true;
    }
  }
  return false;
}
function doorAt(tx,ty){
  for(const b of buildings){ const d=buildingDoor(b); if(d && tx===d.dx && ty===d.dy) return b; }
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
    dialog.active=false; mode="field";
    const cb=dialog.after; dialog.after=null; if(cb) cb();
  } else { dialog.shown=0; dialog.t=0; }
}

// ====== 戦闘 ======
function calcDmg(atk,def){ const base=atk-def/2; return Math.max(1, Math.floor(base*(0.85+Math.random()*0.4))); }
function needExp(lv){ return lv*lv*4 + lv*6; }
// マップ別エンカウント表（重み付き）。表が無いマップは序盤の弱敵にフォールバック。
const ENCOUNTERS={
  field:[[GAME_DATA.enemies[0],55],[GAME_DATA.enemies[1],32],[GAME_DATA.enemies[2],13]],
  cave: [[GAME_DATA.enemies[1],40],[GAME_DATA.enemies[2],38],[E_AREA[0],22]],
  area2:[[E_AREA[0],38],[E_AREA[1],34],[E_AREA[2],28]],
  road: [[E_ROAD[0],40],[E_ROAD[1],34],[E_ROAD[2],26]]
};
function pickEnemy(){
  const tbl=ENCOUNTERS[currentMap]||ENCOUNTERS.field;
  let tot=0; for(const e of tbl) tot+=e[1];
  let r=Math.random()*tot;
  for(const e of tbl){ if((r-=e[1])<0) return e[0]; }
  return tbl[0][0];
}

function startBattle(forced, onWin){
  const e=forced || pickEnemy();
  battle={ enemy:{...e, hp:e.maxhp}, state:"msg", msg:[], msgIdx:0, shown:0,
           after:null, cmd:0, spell:0, shake:0, shakeP:0, flash:0, rects:null, onWin:onWin||null,
           order:[], actorIdx:0, actor:null };
  mode="battle"; fx("enc");
  queueMsg([e.name+"が あらわれた！"], startRound);
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
  if(battle.enemy.hp<=0){ winBattle(); return; }
  if(battle.actorIdx>=battle.order.length){ enemyTurn(); return; }
  battle.actor=battle.order[battle.actorIdx];
  if(battle.actor.down){ battle.actorIdx++; nextActorCmd(); return; }
  enterCommand();
}
function enterCommand(){ battle.state="command"; battle.cmd=0; }
function advanceActor(){ battle.actorIdx++; nextActorCmd(); }
function afterActorAction(){ if(battle.enemy.hp<=0){ winBattle(); return; } advanceActor(); }
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
  const m=battle.actor, d=calcDmg(atkOf(m), battle.enemy.def);
  battle.enemy.hp-=d; battle.shake=12; fx("hit");
  queueMsg([m.name+"の こうげき！", battle.enemy.name+"に "+d+"の ダメージ！"], afterActorAction);
}
// 呪文・特技の共通実行
function castAction(m,a){
  const b=battle, e=b.enemy;
  if(a.mp>0) m.mp-=a.mp;
  if(a.kind==="phys"){           // 物理特技：攻撃力ベース
    const d=Math.max(1, Math.floor(calcDmg(Math.floor(atkOf(m)*a.mult), e.def)*elemMult(a.elem,e)));
    e.hp-=d; b.shake=12; fx("hit");
    queueMsg([m.name+"の "+a.name+"！", e.name+"に "+d+"の ダメージ！"], afterActorAction);
  }else if(a.kind==="mag"){      // 攻撃魔法：威力＋レベル、属性倍率
    const wk=elemMult(a.elem,e);
    const d=Math.max(1, Math.floor((a.power+m.lv*2)*wk*(0.85+Math.random()*0.3)));
    e.hp-=d; b.flash=10; fx("hit");
    const tag = wk>1?" じゃくてん！" : (wk<1?" きいていない…":"");
    queueMsg([m.name+"は "+a.name+"を となえた！", e.name+"に "+d+"の ダメージ！"+tag], afterActorAction);
  }else if(a.kind==="heal"){     // 回復（自分）
    const h=a.power+Math.floor(Math.random()*8); m.hp=Math.min(m.maxhp,m.hp+h); fx("heal");
    queueMsg([m.name+"は "+a.name+"を となえた！", m.name+"の HPが "+h+" かいふく！"], afterActorAction);
  }else if(a.kind==="buffAtk"){  // 味方全体の攻撃力アップ
    for(const mm of aliveMembers()) mm.buffAtkT=3; fx("heal");
    queueMsg([m.name+"は "+a.name+"を となえた！","みかた ぜんいんの こうげきりょくが あがった！"], afterActorAction);
  }else if(a.kind==="buffDef"){  // 味方全体の守備力アップ
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
  stats.herb--; const h=20+Math.floor(Math.random()*8); m.hp=Math.min(m.maxhp, m.hp+h); fx("heal");
  queueMsg([m.name+"は やくそうを つかった！", m.name+"の HPが "+h+" かいふく！"], afterActorAction);
}
function tryFlee(){
  if(Math.random()<0.6) queueMsg(["パーティは にげだした！"], endBattle);
  else queueMsg(["まわりこまれて しまった！"], enemyTurn);
}
function enemyTurn(){
  const e=battle.enemy, alive=aliveMembers();
  if(alive.length===0){ loseBattle(); return; }
  const t=alive[Math.floor(Math.random()*alive.length)];
  const d=calcDmg(e.atk, defOf(t)); t.hp-=d; battle.shakeP=12; fx("hit");
  const lines=[e.name+"の こうげき！", t.name+"は "+d+"の ダメージ！"];
  if(t.hp<=0){ t.hp=0; t.down=true; lines.push(t.name+"は たおれた！"); }
  queueMsg(lines, ()=>{ if(aliveMembers().length===0) loseBattle(); else startRound(); });
}
function gainExp(ex){
  const msgs=[];
  for(const m of party){ if(m.down) continue;
    m.exp=(m.exp||0)+ex;
    while(m.exp>=needExp(m.lv)){
      m.exp-=needExp(m.lv); m.lv++;
      m.maxhp+=6; m.maxmp+=3; m.atk+=2; m.def+=1; m.hp=m.maxhp; m.mp=m.maxmp;
      msgs.push(m.name+"は レベル"+m.lv+"に あがった！");
    }
  }
  if(msgs.length) fx("level");
  return msgs;
}
function winBattle(){
  const e=battle.enemy; fx("win");
  stats.gold+=e.gold;
  const lines=[e.name+"を たおした！","けいけんち "+e.exp+"　"+e.gold+"ゴールド！"];
  lines.push(...gainExp(e.exp));
  const cb=battle.onWin;
  queueMsg(lines, ()=>{ endBattle(); if(cb) cb(); });   // 強制戦闘なら勝利後イベントへ
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
// 中ボス：ゲヘナ将（洞窟最奥 N出口でエリアへ抜ける直前に立ちはだかる）
function eventGehenaBattle(){
  showScene([
    "どうくつの さいおくに、むらさきの ほのおが ゆらめく……",
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
    startBattle(MAOH, ()=>{                 // 勝利
      flags.maohDefeated=true; saveGame();
      showScene([
        "魔王を うちたおした！",
        "せかいに ひかりが もどった。",
        "勇者たちの ぼうけんは これからも つづく——",
        "ー  THE END  ー"
      ]);
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
function clearMoveInput(){ keys['w']=keys['s']=keys['a']=keys['d']=false; tapDir=null; }  // 遷移時に押しっぱなし入力を解除
function closeService(){
  const b=service.b;
  player.x=(b.x+1)*TILE+(TILE-player.size)/2; player.y=(b.y+2)*TILE+(TILE-player.size)/2; // ドアの前へ
  mode="field"; service=null; clearMoveInput();
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
    return "【"+tag+"】"+it.name+" "+val+" "+it.price+"G";
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
  const labels=list.map(it=> it.name+" "+it.price+"G"); labels.push("やめる");
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
                             // 特技/呪文は役割ごとに固定 → セーブの値に頼らず最新テンプレから常に復元（古いセーブで空でも直る）
                             const kit = m.role==="mage"?makeMage() : m.role==="warrior"?makeWarrior() : m.role==="priest"?makePriest() : HERO_KIT;
                             m.skills=kit.skills; m.spells=kit.spells;
                             if(!m.img)m.img=({hero:"hero",mage:"mage",warrior:"warrior",priest:"sister"})[m.role]||"hero"; }  // 画像キー補完(img導入前のセーブ対策)
      if(stats.gold===undefined)stats.gold=0; if(stats.herb===undefined)stats.herb=0; if(stats.elixir===undefined)stats.elixir=0;
      if(s.respawn) respawn=s.respawn; if(s.flags) Object.assign(flags, s.flags);
      // 不整合セーブの自己修復：加入フラグが立っているのに仲間が居なければ入れ直す（過去版の名残対策）
      if(flags.mageRescued && !party.some(m=>m.role==="mage"))   recruit(makeMage());
      if(flags.seraJoined  && !party.some(m=>m.role==="priest")) recruit(makePriest());
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
        else if(b.state==="skill"){ b.spell=r.i; confirmSkill(); }
        else if(b.state==="spell"){ b.spell=r.i; confirmSpell(); }
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
    if(up)s.cursor=Math.max(0,s.cursor-2); if(down)s.cursor=Math.min(n-1,s.cursor+2);
    if(left)s.cursor=Math.max(0,s.cursor-1); if(right)s.cursor=Math.min(n-1,s.cursor+1);
  }
}
function joyButton(){
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
  else if(c==="E" && !flags.mageRescued){ // 救出シーン（魔法使い＋襲う骸骨）
    const mw=72;
    ctx.drawImage(SPR.bone, gx-TILE*0.55, gy+TILE-mw*1.1, mw, mw);   // 骸骨（左）
    ctx.drawImage(SPR.mageField, gx+TILE/2-mw/2, gy+TILE-mw+8, mw, mw); // 魔法使い
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
    if(b.type==="church"){ ctx.strokeStyle="#ffe9a8"; ctx.lineWidth=4; ctx.lineCap="round";
      ctx.beginPath(); ctx.moveTo(x+w/2,y+12); ctx.lineTo(x+w/2,y+h*0.4); ctx.moveTo(x+w/2-12,y+h*0.18); ctx.lineTo(x+w/2+12,y+h*0.18); ctx.stroke(); }
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
  }else if(type==="church"){   // 十字
    ctx.fillStyle="#d9b24a"; ctx.fillRect(cx-3,cy-15,6,30); ctx.fillRect(cx-12,cy-7,30,6);
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
    ctx.font="19px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.fillStyle=m.down?"#777":"#fff";
    let nm=m.name; if(m.buffAtkT>0)nm+=" 攻↑"; if(m.buffDefT>0)nm+=" 守↑";
    ctx.fillText(nm, cx, 48);
    ctx.font="15px monospace";
    ctx.fillStyle=m.down?"#777":(m.hp<=m.maxhp*0.25?"#ff6a6a":"#cfe0ff"); ctx.fillText("HP "+m.hp+"/"+m.maxhp, cx, 74);
    ctx.fillStyle=m.down?"#777":"#9fd0ff"; ctx.fillText("MP "+m.mp+"/"+m.maxmp, cx, 98);
  }
  // 敵（バナーの下〜下部ウィンドウの間の中央。枠と重ならない）
  const eimg=SPR[b.enemy.spr], float=Math.sin(Date.now()*0.003)*7;
  const shakeX=b.shake>0?(Math.random()-0.5)*b.shake:0;
  const iw=eimg.width||80, ih=eimg.height||80;
  const eh=b.enemy.big?260:200;                 // 表示高さ（中ボス/魔王は大きめ）
  const ew=Math.round(eh*iw/ih);                // アスペクト比を保つ（画像は非正方）
  const _top=20+_bh, _bot=VIEW-240;
  const ex=VIEW/2-ew/2+shakeX, ey=_top+((_bot-_top)-eh)/2+float;
  ctx.fillStyle="#1c1838"; ctx.fillRect(0, Math.round(ey+eh+6), VIEW, 3);   // 敵の足元の地面ライン
  if(eimg.width && !(b.flash>0 && Math.floor(Date.now()/60)%2)) ctx.drawImage(eimg, ex, ey, ew, eh);
  ctx.fillStyle="#fff"; ctx.font="22px 'Hiragino Kaku Gothic ProN',sans-serif"; ctx.textAlign="center";
  ctx.fillText(b.enemy.name, VIEW/2, ey+eh+34); ctx.textAlign="left";
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
    ctx.font="20px monospace"; ctx.fillStyle="#fff";
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
    buildSprites(); loadGame(); loadMap(respawn.map, respawn.tx, respawn.ty); loop();
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
