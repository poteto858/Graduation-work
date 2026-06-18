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
(function(){const D={"hero":{"d":"iVBORw0KGgoAAAANSUhEUgAAADoAAABACAMAAAB83JS9AAAAflBMVEUBAQEfGxkNLGNZXV3o6ehVLwiZoqcLUKqXZRppTB8ENovnng5jY2L5+fclJif3yJE0TGJziJbeomRZdYxYWl4QZce3xM6jj14gISKmt8TbuZGanJqOdVT5z2P9xRpMSkkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADpuryGAAAAIHRSTlMA9/79/v7+/v79/v4RBqL+/v7+/lr+/v5r/v4e/v/+lp9iHkQAAAYOSURBVHjahVeLmrMqDCQEEbQqXqrVttv3f8szCXZ72fY/fN39rDAkhMkkNeZ3HMyB6GC+j5bo/GWqIc+n+zbGTC3GbZKJHbpdWvNx76Zll6G6FC7oaB5WN1dMzUer1Lm7VXwhdvM69wDLckG03sHsB5vmTE6hBzmVLS273ntPVWkJswe16tvGfLCLTQFtxFVbHsuKHZAzlSWwrRimDQumT6fNUEUesR7Q1XsnUAHLGWSB+QbdCmyuyGO2ukMri3Cx+wpdZIrgbblD13XdoWWFcG3foRdMuUjJ7tCZmf0OtZZ0+l/Qjn2GlsS+87w7DLbobPwYpiluuq+3NtlKLqerry5VlcXYoctnq6blzjnPvUTSM7trtMXVwbhzeBc713XxM4MR22XrOMlSsd7ROlfxKtbmuU+Ju43b5kt+HOiyUOoRV+fWOFhjjnaLM6Cr50QL0yuXmqesOxOlcVxXMJBjDQ5WNETvRz/7MSkfm+8pCeTIOWVCIKwO+pz6MY32nfuH83PepTFhZYEBbB0A37/QOKb21WbDdHrKu5SwNOAjtmINTMwvYPnd6omfktScxEgIYkeX49tlyNgiJ+5zfss9PCLeYokui4tAijjES7E7/X5UQMHx+3YTKZTEYISrCLCY3t+eX5kEqP91+SQLdOXoqRgKgmm2o7ou203vVvu5P2Wzk6wBIkZnibshDJ0ncgGmB/HjlfsgrChQ8xDbLVAcHOjfdd3Q9bZEvsAFoj8sgPD1jnJCHEQJJT6dM6UHdLv60rgOZx7o/ajgLTx2487rRtR3uBRD1ykSH791W4ji7/RHuD1Ec757fDjfcCZAB/E2/3UaZvqbNOCE7+Vqd48bBoeDWESUEKetuyq17vXkaZzVY9/enWiV/cB02zCo4ZwBbTt9ElEkWbsXGssjsh1LL+prN1zwzGnse6b2LVDNSWMMVkh8GWmO/ISkCp0w8ARhGkU5oHGCPbyzQsyeiWfRFlxWtDYq/QMeOGua71OO5uEhScKKGXG42V6XjM4WgS2DK/gfCkDFKgSKcOBHxYUH2HOceTpDDGDV+7QKIhjuOYaQsEfC/cHf/QzTUx8AH1d/k0QfPdzyq9SdrKlJahV2G0cqTP1TY4S9aAIrrPD9SgfxfGQeUZBLqCG24VJKF95DsML1p0YpSbGuIz2xAoHiqVW5hvjBqDkey8R2L5HUU32tudJhITx3adR6PvaiQ3kUVXmUgf8KFY271uoBBFax04MVl8C0XCXRA/KVK1mHWiPl3QaYvdZJkGxYihCU8nz3mBcfwk+NUxUR+suBS70cVFs8Q4+vUR1Yfi7iMxvSXEGDRMEcGaFjN26iJNbCB8TXdyEy6ER1nY+9Q20oMpuuAWotTki4QtgPjGInH31G1cp19vIjscLOQYNMuC7oPFSE5SaDym8M0bsZHkcV5Z+UYx1+gtVhsrCo1iM4Ayg3pix/RYizR8HMyh9+WJyt98GVQjFu7YQiERhaiCvHSaV84C6Fw7IPIirQuv4LFadjVv0k1cr2vY9IHlAC4Sq4QuUSh1OyEpRfh3dSxGHIom9xH+uIvQyuFNdQUA1O3JkUalbkHqZcazROmtjlWC5RoEY4ECmoWSuRBTQqNNDpUc8LuRXpVXCBY0029soBjuDDHSpWQ/Xi797+FmQrSfKlLqANPnOgsKh6lnBGYEAjdRhmDs89eAHtg9Uo5pMtJKglyy5an+/xkRENvbbveVZugyPEONTLsoCciTWh4Gqy1Y4sXnsDi9xCJJCzIJGuVkGEF1CAJCmVDcOr4rV0SbuNXSmJgvXLFkkbPAqLyKRyA2DRl+JXYO4CVYo3YlUW2vFCElOkbrLS6VnC9N4YvJcBksZVlJtl9MjRbBVZK4pEOUBoKKaXPk3Mtrgs00CfRJVVrul2Q9mDuHklM7gO/PSnamGb7IdlyPgq0qZbo53PL6Q/NM3NnM2XBvNM2oj4FR6fckuDbPe5YfvaHzYaqyIMA8gMfdAfKALFI0qmQWAPh3/94AsCDVlUs1W92bAE3HH7j9+KJ0ABzM0TOp2D9GBCDX37P9Dfcb+82+MV3V5W/wcF0lIrpPihLgAAAABJRU5ErkJggg==","u":"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","r":"iVBORw0KGgoAAAANSUhEUgAAAC8AAABACAMAAAC9W94RAAAAflBMVEUBAQEeGxkPUalRKgfq7OxYX2MPLFiYo6toZ2c0TGNYdIv7/P5tjKIjIyP5zJNmSyKYZBncpGhXV1YGOYvlu40lJSadn6GdtMjalhOdeFG2xdCrkmoOXcDFeg5KTFD+2FkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADbqnDIAAAAIHRSTlMA+P7+/v39/hD+/gT+ov78/v5c/v5lGv3+//7+/v6P/vsEAI4AAAQ0SURBVHjahVaLsqsgDCRqQPCBWq32cdr//8u7AW2t2l5mDtPxbEISNhuUwipUQUSdylV+lj9s/1nVLeOhWH3If5kUFWfAR6BSXXf+7b2gLIv+SSMwa2F7ljC/rNwLviKtE6xyHJ1m3xXqW0xkgDeUvFc5OkvnbwaV4DPWb3iZ6JHpWxoRn9n1AWyDQX5YnojP9MpAWTdy9bX62e6ERJdjNRwUR+WU9gcG2rLndBuQcEFTW0d8HUK6x42aZ+337jtKNPXBoA9wHRfgk6d9wqSTk07bus4uOrnfE83GmL7mRk2UMAw2JnTCEh6QjlHjglEwbiY6qWxfI5LDT7IBTVya0oxZdg34RLk93gbnWIxQytIwzmCmdILBifbxu8Ra51xZjgabpddqGiJ13vtH2C4YlIyj0rCoSWEw7dznRa5tXM7yggb8go2m9KALuncEL3ja+D/siOfAoFpgr3UR0xb4rjhg6BsfD6C25au+tETVh/N8VoEXnrimsLetobanx4dkvBh3fhXl1noE0dZtPepbn264kA/dXNIF37YN8D3wJsHOamWQV7jE8GNVlhZBk6/rtu/rvr6s+mVgkGrBw4IWg9SDq1jwv+r4AZJWmnh95y4aSK50acVzwK/C6SBprszm655PmFnT1n0vKVyWakrLwnuZZdWc90NFbKRPjGcFL6BoxplFY+W8Dn0AxrGwO/WylmiKIDgB/rq+HHcATstnzToN0+AtViLf8r9qLdHwLrJ1deSoAVPfctvNcHwqVl3gpA2hu5YN+WBQvHIN8NVt56Rt9G/QYOh1U/JjCdSgklcp5YocJE0LC1M6qzPDmTH8zhVw7jZNzOIesm9L5htBJeJAQ66flVxagDkEdDUO1UkSlHWIuY6hkp/wM82awmxlLN3vWlrxlWu1actBJ8IIHzhxCvIM9SeZVKCB2Q6OotKQtgbkf+FNDZ7G0piNhuZBQ7UQTspMIpBcN/T39N65MaNhK0EKEn0CLoxJoVzTTzjr73nzvCtNkGhIOlnTAw5lRiIs8pZOSeP1fojlMnpP5C83nkexBCVi5xVGRrdVrCoIP/oWbDZikLYTcmEJbHq2Ow2tTgEf3g5yzazqBkMm6z2O+Guqrf8wtaQq4bHhmKY2tThm5BRttL2rolrwUiNYwKcnlhHDfjgav0lIV4OhaOMmdL69ggiI/2C6D1rmrU6by03JjAiyW45ZiQJ8kn4ufpzmqyHAgXg4JD14QMTok5gtcEbQclva+vAGPHQf8ByvC/V5Igvv0yP4/IjRD5miVvAlySw6Dh54m8h1EVpMxi4YyiGw4vi9B63VcgbwokAGbelELHShvj3fiq5C8MBbIUEWGhjxHKPzWT67BySCHLgZx15zwMuNQgjXmDz0XIGYaM6f+Mc8sH3T3HohRfoLL7o34z0sPFXDMPzyn388C+ib738DRzV/NpFvrgAAAABJRU5ErkJggg==","l":"iVBORw0KGgoAAAANSUhEUgAAAC8AAABACAMAAAC9W94RAAAAflBMVEUBAQEiHRoOT6YNLWZRKQXp6upWXWDkoBJpamr4+PgzT2qYo6oFNoucZRdSdI37yI9mSRwqKilwjqNdXV3gpmYsKyudm5vnuoqftsm2xtCgdkrIeAZLTVCoi1f9xiS0iSQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADdw1KeAAAAIHRSTlMA+P7+/v7+/g0G/f7+/v7+/KX+XP5fGf7+/f//kv7//laSLv0AAAQ8SURBVHjajZaLlrMqDIUJaFDwgrfa+7z/W54d0NZa+6/DWtOZjl9iAskOSv1ahSpOWAX+LIloUIX6vYrNw6IsKPekfhusD4b1e5bD4KT+uSguY9RQKsrznIvjF5TwOzGTP/e91toYQ/fID2VxjHfMqtdOv5Zh8DmV8vDboDO+N02jN3z+my878o3b0tr4f/AD+97pA/yQL6eOz+zUAT4f8KUKnLkP927BWz7iqVWWjPr23s5kuu9kKdRPGKSl1dugDab6OuFTl2lT25r8PHtmZ5bNb9sZeLV7QaEmypUma6nP87P3DRMslIl1UR3xsjEvvm/6HhaVXsIztOcVVZpqmxE5NvgRCwOTVHqOuoOirK08y7KENE3j+x6fzrnGHfB/wLOa6jrLoo1YNInX33wJEHh9xQtUsnCCYhnnvjsGjZpRGOFdQsqWdyzr/oUXpXTVeMXHNVCis5dduXT2trXVAP6Bkru20WDLn179uO2zgcaZmnZsR9mkMNOOH6bp8wzCOBuVj3Pk65kXg1WAiLkbPvB21r4d51Z8jmNGi/sFiq3fvUsitHM7z23bYk/pKp9Ci8E9JkwXFAtP7/Z9tEK37RiAM7239F6W0X/foDcXMcIvCahNFteMXvS6KZ1PBqfVYGAJHiZjTZv1ypFz3/S5f9fG/Zq8x7ghdLK6e1esPSUGfpMCXIQQhGafX3yP0r5P71YvJSKowJqCqABO2VDW5xfwaDND5eaETgQDvnj8s1gVlJzL+NzHVofcbSsZbim/NM3ldQqQROY6W/Aceuc+x8sUc77QksJgzHkMjKwuEQfvPsUKKXjuz2nTCkaejwyC0nuPrscWOd5kIBnGFM4xhWJix6pKu46Ny73nXW+VKQWHFOBmgnJAEv6shXKxF+FC/B/DJebsJecJUxR+6+dztDZapDH2JbU4tp77ngaUJf2pvyuksNIPa4Pg+l0P72O7NNwQQa5GyzriunLW4qCN3r8A3zARjQvQq9qyKDM/tIimKJLR1X5eF6rErunrLSMVcW2fXIkBcoCI7vmyvMueQHmCTQMm8QYCD1nV2xpa4rEqJjem4Ctrk39jgwwC2mtVF5U5C880t5ApQwSNqZCBzI3PjKVOaejqgHAq/XzaZXFl6pvwFe1nYyzkLIZv7Rev91cg+VIIj41H1EbiiauO8ejv+VhE/bRpWzZ8TfHw6OhGg3pAAibx8TfCMfr4BXFlC7fyUnOR32f8fkEEWcKpTBxoqZrMMU+3NfKYLMpNAqp0zODohoWKq5NFuKGe3L1b+B8ZDDLz7O12i4MMvT6ZGBAa7/gGpxBC1Dh0eoNmFB4t2hXHd8pp0U32uBU4OO1gr4Yfl9VyCo8/FCr8w6CBdtEwqNP+2rupu8eIUB6jNANUix396+aMGgrI8jHXWCGNi+LI85rtaZqmjgQOaQLjXlIo9X9uz+vgej/4D1hsOwn9iakuAAAAAElFTkSuQmCC"},"mage":{"d":"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","u":"iVBORw0KGgoAAAANSUhEUgAAAFQAAABACAMAAACz8qB+AAAAflBMVEUAAABwK5LkYgQmFyJQH2upUAlZKA3plReEOZ81FVAnKSpkaWb5+vnv7/FWW1eROACaVFyLQ6TdqFkiKy1uTSOUId1dVFOZoZqvWtm5hx2tiVjuz6EsMUnfc/z9xCfHY/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABGMXH8AAAAIHRSTlMA/v71/v79/v7+ohYE/l7+/v3+Zfv7/hwU/v79YhH9X4ZJigcAAASmSURBVHjarZiLVusqEIaBgSGQmJBLU3tx9/3f8vxD0hrd7mOozlJb61qfP8Nco1SZNWokWGjVL1ok0jBLsf4tZEu26sGs+l6F+jewtTD7SoQKVdOvMGvSK1Prvu/tr/j1oXOVGn+us4n2gczUyo4/psZQCdTqhwNsbH7GPFLIKGs3Umn8AbWJbFdkkDfV6lVLP3DneHIhI20IG6f2PbXPS43eEAFp5eeWauOHDC7JdpyeEVDZPtw/EuD4kfW2G3s9n6AIWXls8+mrx03126gqUVq3cRi4zYke7UYp0qv6dFW7hV4nr5SfrktObamI24o29/mqbvjeWZncH+8px6pc1p24QO9X9ZpprwJ92xWkznsnTo2EfPrgAJgU1mYBLh643XYF1MTkCEFA1n5Gas3cytGzwOsidY+Nsa1jq47xbyh8MDnxzE1Yh14XpwDTJyaQdvaOUFhut1Yf1Is66KLIitS5sM2nXLImb9gisFplwQQU1LfDzqxqKQQ2ZpofWLhinjyYQSRbpQ9gHTJ0n1vbCI2BnDHeTZPNNs+nP95kplADvsFUWqvb3h6aKxR1Bljvh4sa8GKMg5tX4VM65NfDvibTSMIv6ojZCfePaDTqHQnogPODqWa3o8kcx7Ai7f2VJ57s9tJwdB5mrZLSYRBorerv+rL+O+Q/GXqAGyZ1tufB8/U7aAPot0gUq/mFnEeJ8I7jEcf7rpcgQv8fW2maEaLklemkmu8sqBT+LRJIYb7M7D2XjG3HFsOjXbNoC0R8IiIIzBf2F+8Ku2tNnHtfRmWTfk0SZJ0IFZ+iPaq6oEmrKyPUxSSWbE4FJu6cwRcLVLnBcNl01SBHXWecW8jZnBg+Mp0SKJ0m9NaSmTWimiwE925G0kt+iFTFDqNFCbPG4RdEhjxo+QVS4dXgSq8piq7OPMy9v0Xxg1T1wqguZTPQyIwb8V9Bje+6DlByXDqujgxB3nxlyEyJVPTG4hmYO/bLaTcHX39lnuWeyqd1zGZ+w8nQ/AE8jb/NSIMnVgDi1ZXe33XmNwj+oJNL88zlczWgzq04f5cpZ+86LAPD+XI5F4ZUM5L0ve7THcl/QWppndSkLufC848oqUEC4K52sQ6JS1JtE8r+xY2FsZ9kx0UJRlXZQLkznGtt8tMwUF048w9J57rkBLTKdB113bIFWIOEKjw9OXVOuXwib+hxdoQRWZJSmBKKfuHdXzH3OiW7CUooLQ6AM4P4M0Cqc7Y08XNCYYpwMqjk2px/CfCmnBxkl8iV72lYJYDK7Q+VH7Ma02OYsNJl+AmlkYcldtbBImzb6+LqVhU+qGjEff/s/pZmltMXQfGYgzXqm/p6kkjO0JR3gtLC33u04JSqT2NVqvzZGwxSnoqPTyd/opD8MKT02CKBxCf4T6gKw/BENUXVJxU4JZl5IRiWvIy/Cb5GbvjTE9Brruut3BbkYZS+oPQpRHwIEdsbudMTcYptTx4XYE4PeUFf9vQguYDVrOWJ6NnnSXVezwBqYMuiii362EgSP/2MalxTKq5PqZbQB25iNT77jG6UeRJCcz2qadlZsi/H47OPfGpsp9mO676WLS7z/Qeh/wG8+zVCXUM5mQAAAABJRU5ErkJggg==","r":"iVBORw0KGgoAAAANSUhEUgAAADUAAABACAMAAACN188wAAAAflBMVEUAAABzLZUmFiNQH2jdYwVaKhGkUw/49/g0ElCDOKBnaWaROALplRX4+fkoKytjWlidWltqSyDeoGH70JqPR55XXlqhpKMiKSiUIePjuYuwkWD1y1Szih+eoZ7mcvf/AP8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABP7O4AAAAIHRSTlMA/vX9/v7+/v7+FP7+BZ7+/vz+/v5U/2T+///+/hkPAR5m1h8AAAS7SURBVHjalZfpduIwDIVtORLObickEJbp+7/lXNkJUJZOxz9CDycf2q5k1ZiH01Yd4XST+Y8zGXIWx5Ex198hlQFTltaW+/3eeWp/B01k96VNB6D7DdSaK9mV+Q/MdO4OKVZSpV7/fA5k7Tdq7xDaP4K7UjLl3CPW/QOqyCf/vLub25f+8HNM5G0u1KOXe0tP5f76uv85ueyYc7nIt6wgtOqH7NkVom+QLUvXPRh6opJ7kANMuUxt1YaP149KIu/BQFHZ1bLMz1Ijzen/+mP+tF/fLAp1XYeaUoLKBADBb/ieDqZqq3fhxTXD3jq7WXGel6EpGhOfXv6zQU2Kue1SWMk/x8u5Gc9NI4WQWqraBwCfhy6ceVIXcjLgWC94vylEnB5K59DC0VXoCJKb89nIIVEaFxgDgtmtuYHxPQoOeVljbc6fOtgUaN4W+nXeBYFbkRUo14OvvUPlrNnt8MhJlUZyYOR8LJqiZtUXXs5EdtBTn6Dj0U4aXheZhBLFRVHUqyRzUoi4rhmW/LxX5444TrNRddO1m1pzAJPNZETTwCyCXyLq5x2+Vchm6qYreWBQLa7xKzLUSvW73eyOylmlUkZS5VvaXEOXqRG8XcAUPmtAu/5kj8odbby3z3Rn1DG1UEg9DEOoC05U0ycfjRtuQ6/aOivFL7VaqgW5A5VM7VB5kpM3NDQbtUJoFiAQUa0FkEimX0KyhENFcT7L6dxwonR+JiXdcsZUQxwLz34e+xXa9dxAMwVvkFpKxdTgNfqTPmU87Xw/xt2daoTpkNtUu9JqXzpUOWduEYESl3Fn5nFZbfXSDOjDaYsJtsirVL1qQ6nB1IhrGMdlHMeVcjKMDW/aTWCXZ0CdqZoj0iFh1LPZmtV9/n434LrrutWUYqouUUtjnNe4GCnHfKweB46eO1WTFBzMsCw3qGfhrnq+iCApFCkdeIJ0OvSCqnajRBvjZep0skIolVf1uqGnG7RjVPcZarsIScgGhRBQif4U3IbN3NDrVQmXalkTz5gAMgR0GMtmDQ6+uQOBYXxoZEggyucpysnFfs/ZFBXcvUztyiiUlaHjPonfkfRH189zqha9WUK2BGpDr9eS3ZsYUXvPsYeuuzdDG7USgILZQtsFWJY+RqwTA59OsY+vFyet6QO1NrWnNHdBhYbR0+PpleqkqJN/m4PoAsccl0sfBlQd6gpvbOH1BGH4rQ46F0Iv3LumQNuPI79SE9KdITW1V1vs2fFFohNG74zy7opGr9SCOUsMXQwhuHTB6Lwp9BHlbQ5bxQimjCmaMzBWmxcABgVkvYk+7F8qCiqCk/l0wcB9oJ7a8dlNHTpIXY9WUVsnNYNxJ/R510sXMyHycRyipjJcFJhVTp/3N732UFhBTZltory7kHbxT7ueW+9h5lTpoBZVzfJxMWqrFdLBneVhhmgtlCifw4IUxTj3sOmF0JCVIIzRPn0Ki1zAEJQbF8Jy8cHUGqd0H5d56NWhW5qATQF2hmVItzjhtv9MZVlBUQCX8XQJUauH1RZyop+XUiwd20ET68Jm6ILmbz9ul7eFZ1qXHMz+7lrBQ/7Fjp6otPZ6q/syvZu57yifGxqfB706fvUfy3W6Bddd1cyrpb9QwD74ckun9wAAAABJRU5ErkJggg==","l":"iVBORw0KGgoAAAANSUhEUgAAADEAAABACAMAAACEPG9KAAAAflBMVEUBAQFyLJUlFiNSIGreYQSjVA9bKxH4+PgzFE2DOJ9namYrLS3nmRj8+vqbYF340Z3eoGaROgRpTB+RSqFnXVtRV1iVIeMhKyXftIrHb1OWn56pjGGepKiviyH5ylXbd+4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABjED6GAAAAIHRSTlMA/vf9/v79/v3+FqH+BP7+/v79/f5X/mT//xv+//7+Dvb8xfgAAASASURBVHjajZaLdqsgEEVhRkYMiEo0z6b9/7+8Z0DT3iR90K52tXF75s0Y89Ppl0A4wfTmjyd43u+7Pfvwx+dZn7fWKrOY3S+P78ziu/3e1rO3fvcLgs+pKwIbQv3uV5u+AGrY8VenvwJALF1+tMkQnrFdVyhmRTj87IRVp7uuMOyLzI8xJi5RWgnri4r13wfsuDqxWmU9FQJi/5fEFwm7X9211RHybBXyrxUuxJ3dAlVVPHkwIOsTH735qArVyECfHhQRZjwaAFF46caO8sw88+pGV8JLO3M8BikCz7lzEef9Sr5KVaJkXHLx9+NOFcuO0jjXxPcYhysxrzmsuXC3rVRQZCiBS/AwlRAXHAesiddcMqGO4HXhOm5J1P8uqD01Ai1RDfECqegclzfAEYrDexR1ZGeNOVgLoijYe5hYFHJOwKClTMYLNL4XNm2ryOwZVgko3vKAREhzbqCjhItNIRZrAByM2SsiTeMKxGsSmWRqGtHAZqFMmjR7AAAN285MTcTHcLsorTpemQyR5WKWShTgwKltCRGa3Ak/m0lg/90258JnDtjofw8mzfAHb3Nn5ybVciJr0VoNyq43tdrd1arCgWNu2xb5k9M4AtEj/zHLqiLvs7ezo+ig0b6JkwFelAw20wTjyNs1i2sdknsf3Ti4htq2IDJcjZdREaehA1OFuKr0C9xC5TUytxWhYfBmHkfRhxXRX1T7Y9cXxMCIGMlUop2Hkzen06kEITkQRUaFtpYyiow3eavAdRjM2zCMmrgGSk1JqtYDrYT6I2aIG3EaBnwPZ9UYZ1eCNpGWI5it7Y4lze2moUDJomgtFhHSVqfqR+0kGqNsfowABjVHaHKVaCSEsPT95+DpEbBVA6VyRQ7PWSCxAXFaHrt7Ibf6oSoIjLCW47QRjTwPTvcp0rJPSKJaLt8j4YtZmXUAMafEtBJaAOHhApW7WTlnwR2TULuSNxEkMTyZNdU6mfcpyzUlbW/4I80aLXqQ6CEC4u1t9gfrkGIqs+ruSYzyCBgdCjMGB7FkvQTXet0iHBt6dD3PaZA4dp6cXgfbzSQ1wtrOD47QaUCF51OylPW68CtB04a4R2JEZaQ8Jp7fHBJY24h1+hS75JlIwwCbI9Mojs8drx1eRFyZSY/hlRGEuETulnXWUb1vLHxXw0Ses17nWoOidqVFkfNr4oQ/kZSJ6PgY3j5ofCFD/rbXOX0+wzU0OzzBV1heLQo6cg0KEETHmHQpZQN78CYJz1vZrt8pgUpnf1WCvKDAoIj2fhaoyCIut5Sdv6USpHHUXkSL+6N5vfktxaycvDvXGycNo9OZYOm7XSnACHLIekxdvaSoDJHtsn1BZB25SMGY7H1PshV5YRYuXAy6LDElGs/pcxurdy+9IkzG0BbkgVGQaV3iODEuwKT3c/+CwJAtI5bcMEaFuoQFImkzLv3L1U0BrCCklcu30wn3seGyVpvvFqWb03sFyzR52k4os/DV2tPfCeKtOY739H4nEsJFdzhftt3v0/C8WFJV+Bux02rZjrk8mv8PmZw7eGN5JeYAAAAASUVORK5CYII="},"warrior":{"d":"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","u":"iVBORw0KGgoAAAANSUhEUgAAAEsAAABACAMAAABlV3obAAAAflBMVEUBAQEhHhxVMRlXWl2Zm586PUVra2twRin5+fnf4OIqKys8QUiYVypVVlagoqLejlEmKSpISUnIcDR2eoO5u8KMcVy+wcZ8gH7UfkE4REmohGgsOUSKiogzPEN7gYi7wL8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABnBqjeAAAAIHRSTlMA/P79/v4S/gb9of7+YCD+Zpb+/v7///7/If8tSnf//+Vsh1EAAASlSURBVHjatZjrmqMgDIYNQWAQrLad0x7v/y43B/DQajvus8sP23HwNYTkS2jTfGU4x5em+QEQXfP3g5/NkOnaj0AsvheG+JdM1xAkE1BG6DMz6cZhGi/P8bM9rMYYjhs1gKsWtUsWocJxFvSDPI1ruyKIEw+ydkd27l+x+mNWBfX7nl3hiMvG2MPDcQAWAf4DawqItgWc/jjge5dhgYC7MDu2jY8XmY9s4sKwzXHAsPzM9Qdi7Dlr3Beq21U+geW+//++dwOPtXa5J8Gat5wfxogyYpxxLrjxISrmrd0PYF9kWIsrKY/G7ICMav/9+kK0L2/2aploIYYimuETTmmPlbY135G2XNWw6/UqsDIesM4w7AQ5WHu5XOz1hYybYd8hnXdYKZl+sxaF0EvSth0SyqJsN/N+mpR2lmhy2BVQbGl0KDRs5Z2DOnmTZURZNyxjh12YRWEBAiO/9hEQu92I6FAS8h5Ga7x0bFaBIdIdfKcvHW6BaCb/k6XCLeMhcxUAWaOwBAYuopV4o7+NLKoMrZZIcxqGrZY5oKWEB7rftRr8JMPEg4tsQ/eOIIQJxRcrMLquu4GBbwJSjEJhob5Z5rb8EmM8M7xnlFxRTJO5S1eJiymHEMr/ySwKWihUtpoxqeynAnlTZEK31kPZ+V8WpMDw85KYUMyz8kGGKSx5L59ymyoKbU2Y2xTXBEWQRch1RtJcI0NJxfWp+EqDC9Vl4v2wIcaoTtcMl9klLPkLOc0LpdNABSXdKuIseYR6UdhSXXS9DJPvq04MfiyCwjUUp3NAov3t1eF1RWRJJ0ij+9k9UmqKCmy7MoU9R57xJ1abWQnVpilWS36fz8msJDG3jOJsbEuRNx5eT6fTeX7OeIWYBc2caQ69seYkxW1mD3Q0KIXaaa0nmWeqp4yZvVZhKX2cTh/nSexAEq4tg1ku51hZ9FJKifo44kJuwIKnJdIip84JZPu7guo6TQkqP4lRNLHswt2QKElJlHKy6+36hrCwq8QbsT5SMnNYUF2xqrmw6qvNxMqU09fJ9WRXy9vCNVb2qEYGRamkgSTC5LC6EYXVA2sUwYTG7ocpRc1KkHGhIbeNZqmAEakwskyB7CSHWZ67kqcNqzZO1TCsfuhYeYmnB7sQYySZ69UqCVNfxNCs68lY7SK5rjB74QV0yB95nakM8JNG37QFc/kgWBRdV9wv/cQhTAJCjyaVQMHx96VdNyfJEWaaokqOhaibxc/TlXi+fDer1HbNwjYJHyn/VlqASb35KeSsTnWNXtLVzBnweVMe+8w0BVF+BX1DVqvQQ2F5tRFKokPZie/utkWlQzSPfta1XARQdR5ZdlBYE2yR6k+OM4Sd25qyRp/uGgtDQvy+bqndfZOxOKWx172v1UgH/zYgQUOJuK6TG4NXQ2ljJjmsfkLZDy7ZYRw1v+DxbwmkksZ/+5bUO9Jn/1Tqq7BA1sL5xknz+JgG0mFcsBNYNZV8hq+v0k3FKjZPD1putU+qxGGVi1Wde+cen0CCRKpPZYl9FalSKHXde8efu3bYLHuu6e6ie6qdNJ1Q3BdY3mtjM4un6qzChq+eRde1ImzdXbL+AJcfLYluJmSRAAAAAElFTkSuQmCC","r":"iVBORw0KGgoAAAANSUhEUgAAADsAAABACAMAAACTHv+DAAAAflBMVEUBAQEhHRtUWV1UMRmfoqfdo3FlZWY8QkooKSlpRy3X2NqTWC86PUT09fdZWVkqKyuecU1JSUmfoqLrtYW1hl65vMJ4hIlxe4Lkw6a9wMXLdTiHiIc+V1o7PkHJfEOysskAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABykqNIAAAAIHRSTlMA+/3+/v4Q/qD9/f7+CFpf/pcY/v7+//7+/f1JHE//DDGxRD0AAASTSURBVHjalZeJeqowEIXJBgkhEJBql7u8/1vecyZRwd4Wm6+t+uGf2c5M0qYpqy0vo5rT7cPPVjsp5UeB259Q04QXsErNy88sejB+bkclC0bH9inTKbVXiFsordTYKJWa9APDm/XhwT9jNqXxAZU1NfMRO6ov1pRg+7ugEeu8R7QuMSN3BywepgeY4FDeHbDNxw4ddvABm3bkMOhhs8/37H8y1d3f7tiWa8uq75Y/KNHiv2Z3ukxt86tpd1r/lGWT1xz2LvPvNGsste2w5RFV8eSwcoWrAa+17bH0lv2Px6uTFfm+uDh5ki/W2p3d9jMcTnk9VVi+OJG09s1a/ZCCxyoF+pudO+XKTpo2XyzD9VKre8SfWP7Np1P2wSO5RIVE5fWwq/gGDaGGS2+Ny35huEStHrTuIDpmr5199fzuchYoIFJt+TGKg0FQdBfZTqtpVFZPNVk31kXjDJxFqD3MOgWHG28F7TqBO+7RW/Vb7Lb3TLs/Jucb++qU1JDsoIVlyCiT3sq8wtHFvNJld3rV2CLQZ20ZLCwKil84IUOwlSE+3XKVc9WFE215qY+9wOOOhhXFsdflVCaNtR2ze3LXlcVuzxyjr7sBqMwjOXeadqzqoEeXy/kMeC10disFOdFHbIzKKuyuOxkJ88eEOGkeXUirfX/RF4YN+nRyq5GeY0PMeE5UkqWGMgx7JT2CR2f7hjqWhxQJZJLuUSUJqpCs8qBfemHF44u1L709V1YS4LfdBvAs84iRd8PlxQo7KUQDwySZTGNkOmsRT6pob+vslpQrGNKFHVSpPAuIZAQjFrQvXidMFi0ZRqSDZJzZwZFDWclMwFMoDskyFqyhDcCcSLOJipOX6iKqO9u/cYIwXJo9I9xOzhNjpC7F6TmiPWIZX5CXaERpRKvHZdNHLJ8Si1bzy8aMaQ7sLE7bcSzBsh2YV8nGsj8/DaqjDUI273Q9Sm8qhu7ZwuxhS7N1dGsT12iurDGqum5ezzJGQoxyF+mkC9kcKG6tHxrIubAxLblCJ6NebC70NdhlGjg8YBQoPC5T0Lw7Z0KupunvdaOgQnQr9v0tI+8iJNHr3PbmPRoMR7M/vRksrP/JmCUyfJTlhH5hNaeb3pAQk6NMpIL2lL6J4ojUp5WT4Y0m8czfbzFsX/iJopE12ijWysRwdcQvTZm0IiA/3Uc7hyyyM6B/iybk2A+3xKuR0kyTNNI8jfv7ZjmwOYosdok8Doq/4yg3pa/P/rQsN2ExySvOkori4d/U7K93j/vM1+sNUdbamM35+e0dadyoIvP8c+XT3ycuhXPpgHqsrE5EDF1M6dBse9UhJ+zqRF/UlnniJjvW7pGj4T2KUTQhdmvTwRU6SQuaIkEViw/ayMvRBf5XVUO8qyFUSc2HAVfdB/N4/ofnrmZGJG/CdoBAYEe3dn7vcjP6sb00+cMsd7Y0X73G+HsXw6tvWUzF/lwPUR7oS6l2wNTVj1r+xGp7vl96003cxel0pIyujKlMRS13n2E86HT8bwraNubXE+CrVEDGnI0/rC++iJ91RbqrXSMrPMFuy+Kfvrn/A8zuMSe+WP7AAAAAAElFTkSuQmCC","l":"iVBORw0KGgoAAAANSUhEUgAAADsAAABACAMAAACTHv+DAAAAflBMVEUCAgIhHh1UWFxRLxmfoqbdo3E3PERmZ2dqRy7Y2dtXWVksLCyUWjE8REydb0v6+vopKiqio6S1h2BKSkl5hIzps4VoeYe2usLrw6FxcQDKfUfHcja+wcWhts4+QD6JiYcAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABJ2xkLAAAAIHRSTlMB+/3+/v7+Ffz9WKP+/v4GWxj+mv7+/v7/Av7+/RD8V4AFzlkAAAR2SURBVHjanVbbgqM6DMNJIJQEApS2s3Pb///LlRwoLZ0pPccvO0ujyBfZcVG8ZL4ohqM0xX807/WfRkQaX/wPAymxRZH6vn+ZtfBg86I2FEfeEV6mFKG/htiQr+gLn9Ir0CAB0EpWe402NaSVeyjATUq7xA3c42Ejj9bsYhVmfoAHv1OvXuQX6HE3V/0S7KPTR/8SNlu7zZjfqM/fR9HIM3uerRTuT1dS3XLfatZTgel34jufw72sxcCK/kbLMzaOY1dvk3XvojnAnDH9tQ14yJa0U7cNNwx3vA4GuFF/UjEQ2Sm07H5I1or844M5Z7QjtVctj28EjqXsVLiXakGbHjkbxHyPp7JEqA/gcF8hL6atkC93IVhL5EynUKkfiIe1SXsWxFQtUz2B2VE3PQktDuKGkYC6ewD3xpljQ9qKWDFgNv0AoYWOp12NoCNqNV8xT4+lsgUQGUmwu1zU6+ILFRI5zJku61g+ZBmcMh2MzNjKwGv+mLzcYOvOvi9YMyR/DVgmZ9oFLAC74+CHGpGajnUqx7p8j0udw8309tISYLLXbSZmhVGiUznb2K3K7DcNg/K4mRkRHJhJKW/Mahu3y5yldkLzt9DJQLbMjN+NI9Yjw6rKk0IRid4sf3OKD/wb+SGx01jx/waOhKTlK0+wcoysFdOgZ/Q3YA+fqA6NzDKZGXXtkBpGdwmF1h1zmuOVA0opUEXVaiQOssQUvAq9n+ecZSYoODeBt9LyHs3h4ohtlRlfoalmfnNRPEZgjVgrTsGEgmshZoYUrMxMtjTpC12JAogmQiy81jSekci2WiadOXweJqmqJWb4ZqOdX1t0EjoAWEef6fb5wmxlbOrNwV2WiCuNzsJDpKiZXWa9+GUy7dl9TvpJ0jKm1GsavusTz8TGuklH/mHr0TFZgsGwvBBaij8pzOBMe+R0jNrwdDx+gLY2+bbtZE8MKletIhbZ/UKzdSPPupNErQ7DyGDbjR3T0S+jVMFn9gDv8xK7MpY8jbwpvV1YIwcY58Ayrxr1WuWGb37QmcymmRE3Llvwsi2KtIoHarso+oiOHDihMCJsHa3cPvy2Ozlrx/Ld3s7IQDTJ9W3EW2JjlBmsvTn7UJ/G2nYf9m7E9iilwtF6fn3EbKwJNig3anSeTB0jU1AUt6M9wfNvHDvm9zBcwTirkkEN3eVcad7C7qISeIOC1dx0ns5Z35u9MD0srvwU5qBRU3TSBPn8hP15m0y5d7tTyQGgnZvn5AsrbZ/zy5FVg7ldJel3l1h4zMrEqOB1bwj7y2hvIUroy16H7FLo/ZU7sBeYWhB3lLGOEKvYvaWQisiKRKk+bnpis278vHirEOull7JI7d3j/Stvt2l5axel7GGD6eK6YmQ+bRIGnJ6XqLFrgM3SINbmaed3ic333PaqsYzikN+tUmowDbra5tYZQmvmQcllJjxHbvblda0109Q+Ifapsd3MqVhcVeODhsDXp3naBhynBM9Cwrh6e9Mvu6s3sNbalRe0cSSv3ddGf68K77dL4T/QFi6x/YsWKQAAAABJRU5ErkJggg=="},"sister":{"d":"iVBORw0KGgoAAAANSUhEUgAAADQAAABACAMAAABiFaQOAAAAflBMVEUAAAD18enz1aHfoSGXZhdoTRzZqV8rIBVTNA75zWKtjmDNtZetopEpKSqUdFK0hyppYFJiZGn9/v0fJCyYnqH+xjIyOklSVl0YXSnEehQYIFdkco9GSU47PUJvi6U7QlIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABiqo6nAAAAIHRSTlMA/v7+/vz+8/z+/v7+o/7++w4EYh3+ZFf//h8glfkaXG0qu8EAAAV/SURBVHjajVeJdusqDGSREGAMxNnT3vb///KNcLo4t+m79LRJHAaJ0WipMZv1Ys79jy7Zm535p7WcTOfkrLUucDcn8w/A09x9ck5B1qWUeT78r5lZKNmBCAPmPMl5McsvmJ2w93askAbKOt/6/AvCHIQ8DN1Bd7SnQPMvtmby0wC5YEMIq5u+lsyvz9hY3hmHD5BPAbcJXj302eYWnzIobbK26cbgcw40QMBal+nptWLG/jyuMuF1dS9d8TqRvDy5UdboZL8lItHk3JT59ARURki9W0E+38kDppS+f2ZpmpySoDF1LgxduBU01Segfc8KIh/GfucSSPSU7TTl3M9PSB+mcsNW4nU1730rsJSfsgdQydhFWNjtqTWKeJfLbyAu2AJAapRSOlJLST/CRX4KGtKLmhikIMqE3Dj6SMSy24h0IwkCBlnUQmiJggvt6IBiivuv/Q/KXWTYObbIFRy6iZmChYv8CVohLxtQ5CNOpimEiRpChYTHKekLdEBGzjwvW//06oBocCk75Af5o2/yrq4t+7NEkRrmb/e6cWNlT0SCBrdiB1iAac9va15nRDuhAHyTBOICkPQiXUGTGOmRPeGwm37PKisHOcb5E7anCnapl8sKciyXEpkoMN+WzuKhLDDjjtcYP2rUnnP1HKZyuZih2cvlUnApAo3mwJqVBGM+eR8/uZgbQFSdNWYYcnhmHflaG9LpJATvKrLY+yvLsobtTMkLeHCbBUuRjglMwE8Qaqfqr/Qhqx1riEKTLYg4gdKU5xe8+IgYuISbz2+rc0hwgLzUYUCXvomkIJ/oFFX4IEepaSTrhZI+9NDMHTUizLGFAfIjZjlEZE6Md//mcBwpFCJHBH24lhk7mx/J5dl2aBEJhhVlrTMzEMolfqiL4GuOOFE/8nCBbcFTPCIce/iIrB6IACIlvMtRF26FC0UFMZfCwatpiGv3VciROcUyjoS8OWvE4CFxmRTVbY+giuQVWje3FfOuWu3WFjgwIXX5wipEZn3G/kjFirq7KbUzUoC1TtZqiTMXc8G9J8taMVUF0UJISICD2X16d2J1bu1jievFXgxK2HEttw68QPtwznzvVDfzLuUOQcIQm8uFlYK1ixZ42eHbQ3fbSb5jkAgaEqwyIUp3VO/sNtkHtd5Y1agVPGg1sZVJvUUlGr0+tKvkFOdt+dpDWSlo7ZY+Doeh0aVZUKcdeBAYOjyOHAh3y1CBaDtDy3WgbvKhioqHhXuK81/dWnLtXatKxrii5QvtAtgqKOUUc4Uk3x47x75rJ2tgrK6Z4Rr6mgs5ChRSAyJe5+WhOvMYNLjHClBFj3ZXXNCFkqF5YtV9kK2d8yAcudAlK4m+5FQ1CwGKKPGV1by8PYCKjhyxxu7W1ouSPII2TRWJEjXBaOseyjg2JtAkxZUxt4HmOwj+IcczKDlsiFiMYAeyS7ryPLr6mDxA4mgeEaxG2W0sLcod6mSQOlk3QOqdgvAH/mUP/8tDO5wnNUQhZvc5SQxoA7BwTBTxUB56oI4maGU82VHj75YUhCEnQnaooXY7+M0gHNWK1DvcabAX1hmuaF40L10dRfP4MhbxpDGUUnQ6Ck4ZTzq62IDjQHrsjKduMyEJzs0cYi86vOkgGsBeUCIAQnw5a40P/fvM/Se7hGorpO4RXFPOx3CVCJaypjIj1zbuvaCr86tgcshg0Sl/6h1MojlNSM8oM1o2bafSXWy87OEF5B1VZk6LJsTnY2s1a0GJnl8f518WczihxkOAOvOuIQZMyyELJvr415wIq6/4PcyzVl0orWgHyyosETO/LGanvfmHOXsZfkYt/CKzmRWtPeRsnv2/sXw06+sV7XFMG0Z4vJ9/mqO+Iw/qzyi+cPb14/1P4/9/e91AGk0U7H4AAAAASUVORK5CYII=","u":"iVBORw0KGgoAAAANSUhEUgAAADgAAABACAMAAAB4KUSAAAAAflBMVEUBAQH7+ffkoCHq0qQpIBadZhRlSiFOMQ9mYFnMtprdrVn5yWKvopKqjmVqamv5+vwkJCSTdVNOUlceICOWmJ+5hjDEeh5KSklpdY0UHFNHQz38wzc5PUQzOkVxAAA7RU4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB0pK4/AAAAIHRSTlMA/v7+8P79/Pn+/v7+/hAGpP5ZZRv//pgXFbL/6mwCTwzwgKMAAAQpSURBVHjajZeLeqowDIBpk6Z3SgHd9Wzv/5YnKeicAjPfpvscv7k0t3bdVfquK+PosIkbx7l99JRMg3NqFedepifJfnBJ3UhywxNc3/Xo1J0kNz5BYlKPQq/H1pZu3uRUwte/4rLJKWX//eGg2+aU/0PlnkIF+HbEzcMu6F5Kv2/p956lYut84OJ0BB7FpjsAqRwc4zvtcZBpOABRmx0wG3MMapO37Kz1EOwY1EGH32eSqgms8E8wRmA215pzrvx2CsCfPgHqEJgFiEFeWYK+A1silPIAChshRg06avlZwIeo3pDFkW6PNm3B68YvQngB516aSyn4Pn9dSOcscXQ+IktgjUHemkJr7QL2XwP9m4jQZur6NX1H8t47jrzYCqIryqsxjlzK2C/eDcEaiCYEvDaxIQG0+JPjmBqRWomkd/kMdVq+HTU/xEE743BpRQxeT6+6RSTd5MEMuYEzclqCPrH3xr5sgLd548WMBpaXTwsqshXE/ttVY9kBlXzqIU2F7TRBAaclaGKVuHbcvfr3Ta1onAf2MBOc6MMGOVy7tGKvdjUqn7AvUgcx0Aczth3S2GojqQPxyg2le+fnoyF9RsPc2k6cOgQhDRwcJC6Dk2VOwPEYhCU6np8r32giLNnLuSQjsEwW9pSJ7+DBcVw5PCZIalhDl9LwexpTcz5DwsLnTyeDyGZy+rZWW/A2hr815kUvuLeOzQoWkXCWWS3g6G4cugdhsTUPhQ3lgx/Hn4l5Of1N0Ks1PGVgE02bXWuxXGN6BAJ1GDTiMN92DdgF1Qr6rNGAwV+D3cFKbp1Jbm54owEhWi7gm5mHOeqwo5BBKUnmPokVTr8a3CtmTojgN5Mg5eSljxCmaB/mHUqV6XPeQDlRGPMWm8L7bjxy44LAVRZ+o9yRTzWAZ3XOaouP4y4px23v1Ho3P7iItDoFkmAOFJAUxN1IH6iNes4mL93oIi0qaNFxfCBSrvcaJ4xRFiHKxL/5JqAGKfGH3DCDsYHwwUNxkKQLeFG7dkfHAUnSHZGkw5zB3sdmoEoQ1gUwt57fRJt2+HwSxMUEFec7HzFAtbTWZNI/coJ1g3RkbMXxft1BzmBu/eoGXCZVXuvbIh/F9Lh/ItvZ0Vpaqc0ogFuwUsSttfW9hkyXrrOCbV6tIIA1uLWUSeO77sYCtlEDP6ByOO4tVuhvNIYAkoAXUAX/2W+tyZwPvMldQhji1dT1y3JQadjeqmXJ9W3h4IqNl+BEU9vqIcuPm7aXXLdGAXwNy94ir5wXIP5yFtatDfKL7xv2GpwM52Vp4KG2+phoNzrc+sivJSKJKyJJmpd7AGK/u64yinKpslxGNYnwWtKuWPyPuXRve7cO2bR4+BgpBBymibtvmy9YnrqSWXM+G9MCgUb+XsA/Llh9Py8XQOnWr+Pyd7d/A/gPNDozPOypsaEAAAAASUVORK5CYII=","r":"iVBORw0KGgoAAAANSUhEUgAAADUAAABACAMAAACN188wAAAAflBMVEUAAQH59/PmpB3z1p+YZxUoHhNnTB1RNA7Yql1gXFSrj2VmaGv5zGElJieoophQVVvQt5f9/P7+xDSVdVO2hSKZn6LEexMxMTNJS0+dq9QwOUldbpAbWip5i58SG2Q3PUUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACpLOotAAAAIHRSTlMA/v7+/vL6/P75/g/+o/5X/gT+/v4c/mCVEWUk/R0anj5JAHIAAATjSURBVHjajVeJdtswDJNFijos2/KRo2m7/v9fDpSdxNvitepeu74YBgWSIGvMP2e8/0diNK35+bng3Hxommjefgg8dV3s+z4GapqGu59AxtYBgscbIaCSTdfx+9BcbNZTWKmSpTj8H9Ka4Y7B8zXAhkNw31xsB2piVlQRzvz1Y1ATrV4rFM7BD4c6tn+CircgS8sizH44vlO7A1EKDBJqejI+z914xDV2O6JC2UqwwZaGxJK+80Bz82QKZHFEv6Xes43vB6hT1z9Fr6Bp8fpjYS8HarTm1y5TGaEpzArjRwjy6whl4jO9k80WCvZzdA7x2eBO5lVZteP1gVqUB5hYmaC7t3KkxgNUanBJOSZ8LZNlj3Yx/0elKp8oBpRQZMqLeWv/jyJeQVPOgGUOS84k799EqOoFVNNElAOoAodMcXevP2RxG0rDilZROTOnEhJP04Prb0ZHa4AK0iCnCTLMIVAR5O3tyfP+B6rQJnuqlaSaz965marw5rPrzNW9ny6yY+wSbQHmWBPGEgocJCZya3wsnTDeEnb205W7Fskju/iaiaSJjsjVB26zhf3YRLLr7O6uYBBPge3ETBQa8wFU7Umn5gPXyr7r2vZe8XcFiwhpRy7gosYYouSG9uqkocyS0HcsT65+rSYIKIWSrKj1pM7PYM7AiGSybsvZOKzCB0vOLviUbeAHSoZBTYQTImT0Hvuupu60ZctOvebKi0MtPakQCk/kyfq8sFbzKtApbimmuKWKJ64hhhLcG97KNldp0d7sbm97T7O2lgU+MF6sn4syoSPN4HwNgdE/Z65ePJox3rXwK8iLeIQpqA3V/ZcZ/NoKXLv0AhjUj1sTp5XKcQAJXgzQVWP57JzT+Gprfw172UkDRE9NHEspqd5qZoHKo8fDov9AxL6t9riZbrD9WrdSf52VkJcPvQEKgxYQQ/POde2utegcKpXl2ASdQihI4Q8Tr5pOv2R2gyZqy/Bmn1q3K1Uf1KPA1vcsH32nGtbQzOezR+JmTWEFWa81nNW350bJ8GJ0GZTpdp3s7UoV7rny1ACkWrg+CY42unWu/dwZQCS7JnhrYcFMLVZBIo5mkcvYzdn/NWZdU21dHRCTYLHepxCWQAwpxRd22Ary2f21QFxio2W8ia5p1KjEz5oM/BIHl/6hQm8775F5PG8XFBG7UvQ+EBFCBkGaIMW/s6g1t81wNf9SNS2z9HAANNectBxfDRUNj2+yyBlZWdPXS61GtQ8qs3sxz0enTmYuKDfmeF9tWHXxtTFfbQGjubDFFL15PltJ25RYOGm/rB7gXs7YK6jaKuNjtlhOKXqtWwr+YHG7Dm0tqPiY6YoCDKJCxtG8XtxGFd+m50zXpWF29gzloeCr8YUXDXXcx/JABaAcVi/Hx4vloJeanvFV844wMxcPboWgu6pEiLTbimxyAaWErc2bVxGO16pEjmm3tJEWCzs0AMvLna2rnZXnnph2q57Q2WHAhBRvL1EFsyfP0C9tw68JyCxlTRfsTl5vDhHzrFZtgtGEkPNSyCVQpR5UKRxsDrJK3uvFtIRK08M+vLpVk5gPEvY5YAY3vbvLQRQ9pEjJO22w4WgXVc/D59LXowXh12/Xt+NNHu8av076xwbcFQ/rETVB+IaOh2/PUNuM+YyjZO/fIjT8we3OMLzcyn8Dbko5eMD76lsAAAAASUVORK5CYII=","l":"iVBORw0KGgoAAAANSUhEUgAAADUAAABACAMAAACN188wAAAAflBMVEUBAQH59vHkoRwoHxSdaRPz16JqTRvYqmBRNA1kXlSuj2Kqopb5zWJnamz4+vsnKCvPtpSPdlhRVVkiJCq4hRr+xjJMTEmbnp8pM0h5h5bEegk8Q01fdYo2SWEOXCUrN1QAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABeBM6AAAAAIHRSTlMA/v7y/v78/v37/v7+EQab/vxdY//+mh5aLP5nJzX9HzZM5oIAAATNSURBVHjajVeJVuMwDEwsWT5zNr2Ahf//yx05RwPbsPWjvL7SieTRaCSq6vkJQda3TfXiGUNdWxlPONXL5zbUOM4nnK5rXovXBOeAsgWbUqDXckwmKiqmej6he+FSZFm/a92woOoULlXV/pJd1VLI3BdUqLeTLlV7CMMfhK1xDtGy2aHq9FuWbTWw82y4ttlIX3+L9ksscCE2WpfzJC7aPaxtjq5VBctS984Z4022bgejpj1CfYkRiQoSNoDFl67WVJ4Vw7icnhz3wQ5v1sVsFGTOBWVc3AU7Iv80ZECIkKa1Bcbp/yl2wMiEL08swc2wFdWHrwMdj16QG5f02McC23iMpwMuRsnnzKJ3A3qmZFpR4ajdCOV1g5uyghYY2/+hOqDEDeRszvgxJeoBqmnWTkAswy724hRlzhwUVjD231jtcq/LdGaoEKrnuWISN5R9oNrqSl17x+vUXduqvUySHBG5yLM6fG9Kb9d9L7tYJ45EE5PHh1+AUbQhDGQhYy1cyXFGpfBA/cFzceHoFgvsUh2oHnrrxDC0LG7jY0O1I3mDnPEZ010/0Fif+MQ6Pk+QMqWzcT9QDbGxNuKJFrBuvFKwtqoWFLPrSezCx4Zq7x2jQlInwKxEf/N2PVEy+MQD04pabRzqRg+JfkmyNb77oH5FVfHMABkmt6Qop7lCHVBn6A6NIfFsCJSGFcXZoGO0Zv2W4vsc7AP3gkUUn8BTx7btBrhNuRZkXGrGwbwtLbZ2Cig0k2f2KI73pCQOwPTOwzoWgZBdUhyWdh5xMzxQUFEgLy1U1UAahKoziZeiD4EfzyUL49yYbSVFO8xMYL4tl0UChNHSLx32CJZOW8EECeLlvYj60A2mkWtno0sp+jXY2558yBz5IBskBdqoWCkPWq1iM3OKZiN/0z2UTn4kiMfTqWo8UMJ+0AHh6lhSPEvYyH8YzvXjTsgQzKNe1SeHBBC60rjkl2BpCWb3rnhHouKU+c9PaGiALK1DWyeaUVNwb3Zhfz8XQDhnBBPhYEkLXaMde1lutuVo/DcXuJJXOtBRNiAuBorBL1HNlKYeCszk8N0MO9BIXRX6QKlm7pGgQ6nhw6Wgqj2l0n83HFCZI10YSWlBoQwRUATNmGlSLxbtxR8rQdMq/SgczEX5CqJ7iipAeFLZsZDX4vz0N9Q7aLeBv+SlcBYgGOMnIbUeusGQfrp8U6grTYr7zCMoouKIJjflkrsns+GPd6Wro4Esl1FuIW7cjNpGlJH2yUShghrQwszrkAwzge/VDT43PpkN1069BoQEOO+6bfTkCuyKlno+LcH9ALIlxrdt/tihTAjpjjeAUTvGaCxTPyZrmZnyy4r5jlCZCbLYUDaWYEx/jmJdaYjQFRr/gVqDMX0cbocR7eIs8W4XwjBx5rdoI6XJkea4i2XjPDIBuz1dVEbpGX2CRpMHKg5YR2eY3P5dOdAsErFWotEybcuQ09KloObn0un55qBjBIRgLDl22WHps/1QGochnDA+2RIbaMkixuCz9WmeLmoV/cKlf6aotoV5eO9DQqm9bEu2oLeRI1X3w13v4528Gqu+UjmCtwJraI81hbvxGxX/9oqkruuu1djdl23mIBYUrNNJx4UeGl/6h6NtxnfanZM6SrMv01//czb2og8b0QAAAABJRU5ErkJggg=="}};for(const k in D){CHARIMG[k]={};for(const dd in D[k]){const im=new Image();im.src="data:image/png;base64,"+D[k][dd];CHARIMG[k][dd]=im;}}})();
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
}

// ====== ゲームデータ（後でAPI配信へ差し替えやすいよう分離） ======
const GAME_DATA={
  // 記号: . 草地 / T 木 / W 水 / R 岩 / H 家 / P 開始位置
  // フィールド（魔物が出る）。G=街の入口 / C=東の洞窟の入口
  field:{
    safe:false,
    exits:{ "G":{to:"town", tx:9, ty:11}, "C":{to:"cave", tx:5, ty:11} },
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
      "T...........TT..C..T",
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
  // 東の洞窟（暗い・魔物が出る）。E=魔法使い救出イベント / G=フィールドへ戻る / N=奥の町(town2)へ
  // ※奥(上)のNへ向かう道に、デザインが出来たら中ボスを配置する
  cave:{
    safe:false, dark:true,
    exits:{ "G":{to:"field", tx:16, ty:10}, "N":{to:"area2", tx:9, ty:13} },
    map:[
      "RRRRRRRNRRRRRRRR",
      "R..............R",
      "R..RR....RR....R",
      "R..............R",
      "R....RR....RR..R",
      "R..............R",
      "R..RR....E.....R",
      "R..............R",
      "R....RR....RR..R",
      "R..............R",
      "R..RR....RR....R",
      "R..............R",
      "RRRRRGRRRRRRRRRR",
      "RRRRRRRRRRRRRRRR"],
    npcs:[]
  },
  // 洞窟を抜けた先のエリア（魔物が出る）。B=洞窟へ戻る / G=奥の町(家あり) / D=魔王城への道
  area2:{
    safe:false,
    exits:{ "B":{to:"cave", tx:7, ty:1}, "G":{to:"town2", tx:7, ty:20}, "D":{to:"road", tx:8, ty:13} },
    map:[
      "TTTTTTTTTTDTTTTTTTTT",
      "T..................T",
      "T..................T",
      "T...HHHH...........T",
      "T...HHHH...........T",
      "T....G.............T",
      "T..................T",
      "T..................T",
      "T............TT....T",
      "T............TT....T",
      "T..................T",
      "T..................T",
      "T..................T",
      "T..................T",
      "TTTTTTTTTBTTTTTTTTTT",
      "TTTTTTTTTTTTTTTTTTTT"],
    npcs:[]
  },
  // 街（魔物が出ない・安全）。G=フィールドへの出口。建物は buildings で配置
  town:{
    safe:true,
    exits:{ "G":{to:"field", tx:10, ty:6} },
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
  // 奥の町（縦長・レンガ舗装・中央通り・左右対称）。G=エリアへ戻る。最奥中央に大聖堂(セラ加入)、その手前に大噴水
  // 装飾: f=大噴水(2x2) l=街灯 o=花壇。入口(下)→中央通り→噴水を回り込んで大聖堂へ
  town2:{
    safe:true, theme:"town2",
    exits:{ "G":{to:"area2", tx:5, ty:6} },
    map:[
      "TTTTTTTTTTTTTT",
      "T............T",
      "T............T",
      "T............T",
      "T............T",
      "T............T",
      "T...l....l...T",
      "T.....ff.....T",
      "T.....ff.....T",
      "T...o....o...T",
      "T............T",
      "T............T",
      "T............T",
      "T...l....l...T",
      "T............T",
      "T............T",
      "T...o....o...T",
      "T............T",
      "T............T",
      "T............T",
      "T...l....l...T",
      "TTTTTTGGTTTTTT"],
    buildings:[
      { type:"cathedral", x:4,  y:3,  w:6, h:3, name:"だいせいどう", color:"#d8d2c0" },  // 大聖堂（最奥中央・セラ加入）。w:6で町の中央軸(噴水・通り)と一直線

      { type:"inn",    x:1,  y:10, name:"やどや",   color:"#c89a5a", roof:"#7a4a2a" },
      { type:"weapon", x:10, y:10, name:"ぶきや",   color:"#9aa0aa", roof:"#4a5560" },
      { type:"item",   x:1,  y:14, name:"どうぐや", color:"#6aa06a", roof:"#3a6a3a" },
      { type:"house",  x:10, y:14, deco:true, color:"#b08a5a", roof:"#6a4a2a" },
      { type:"house",  x:1,  y:18, deco:true, color:"#9a7a8a", roof:"#5a3a4a" },
      { type:"house",  x:10, y:18, deco:true, color:"#8a9a7a", roof:"#4a5a3a" }
    ],
    npcs:[
      { x:5, y:11, name:"町の人", color:"#6a6ab0",
        lines:[
          "この 大聖堂には シスターさまが おられる。",
          "女神の おしえを まもる やさしい かただ。"
        ] },
      { x:8, y:11, name:"旅の戦士", color:"#9a6a3a",
        lines:[
          "城へ 行く前に そうびを 整えな。",
          "宿屋で 休めば HPも MPも 回復するぞ。"
        ] },
      { x:8, y:16, name:"旅人", color:"#5a8a6a",
        lines:[
          "ここは 魔王城に いちばん 近い 町さ。",
          "じゅうぶん 準備して いくと いい。"
        ] },
      { x:5, y:16, name:"見はりの兵", color:"#8a6a3a",
        lines:[
          "北の 道は 魔王城へ つづいている。",
          "道には 魔物が うようよ いる。きをつけろ！"
        ] }
    ]
  },
  // 魔王城への道（魔物が出る）。G=エリアへ戻る / K=魔王城へ
  road:{
    safe:false,
    exits:{ "G":{to:"area2", tx:10, ty:1}, "K":{to:"castle", tx:8, ty:13} },
    map:[
      "RRRRRRRRKRRRRRRRRR",
      "R................R",
      "R...RR......RR...R",
      "R................R",
      "R........RR......R",
      "R................R",
      "R..RR.......RR...R",
      "R................R",
      "R.....RR.........R",
      "R................R",
      "R...RR......RR...R",
      "R................R",
      "R.........RR.....R",
      "R................R",
      "RRRRRRRRGRRRRRRRRR",
      "RRRRRRRRRRRRRRRRRR"],
    npcs:[]
  },
  // 魔王城（最奥・魔物は出ない）。G=道へ戻る / Z=玉座(魔王イベント) / c=赤絨毯
  // theme:"castle" で石床・煉瓦壁・赤絨毯・玉座を描画
  castle:{
    safe:true, dark:true, theme:"castle",
    exits:{ "G":{to:"road", tx:8, ty:1} },
    map:[
      "RRRRRRRRRRRRRRRRRR",
      "R................R",
      "R...R...Z....R...R",
      "R.......c........R",
      "R.......c........R",
      "R....R..c...R....R",
      "R.......c........R",
      "R.......c........R",
      "R...R...c....R...R",
      "R.......c........R",
      "R.......c........R",
      "R....R..c...R....R",
      "R.......c........R",
      "R.......c........R",
      "RRRRRRRRGGRRRRRRRR",
      "RRRRRRRRRRRRRRRRRR"],
    npcs:[]
  },
  // 敵（戦闘）。spr はスプライトキー
  enemies:[
    { name:"ブルン",     spr:"slime", maxhp:8,  atk:7,  def:3, exp:3,  gold:2,  weak:["火"],        resist:[] },
    { name:"ヤミバット", spr:"bat",   maxhp:13, atk:10, def:4, exp:6,  gold:5,  weak:["氷"],        resist:["火"] },
    { name:"ホネナイト", spr:"bone",  maxhp:20, atk:15, def:7, exp:13, gold:11, weak:["火"],        resist:["氷"] }
  ]
};
// 魔王（ラスボス）。※スプライトは仮(bone)。魔王の絵が出来たら spr を差し替え
const MAOH={ name:"まおう", spr:"bone", maxhp:120, atk:28, def:12, exp:200, gold:300, weak:[], resist:[] };

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
const FOLLOW_GAP=26;          // 仲間どうしの間隔（足跡の何個ぶん後ろか）
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
const SPELLS=[{name:"かえん",mp:3},{name:"いやし",mp:4},{name:"もどる",mp:0}];

// 施設（宿屋/武器屋/道具屋/教会）
let service=null;

// ストーリーフラグ（イベント1回判定など）
let flags={ mageRescued:false, seraJoined:false, maohDefeated:false };

let keys={};

// ====== 初期化 ======
function loadMap(name, sx, sy){
  currentMap=name;
  trail=[];                 // マップ移動時は足跡をリセット（仲間が勇者に寄る）
  const m=GAME_DATA[name];
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
function loadField(){ loadMap("field"); }

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
function pickEnemy(){ const r=Math.random(); const i=r<0.6?0:r<0.9?1:2; return GAME_DATA.enemies[i]; }

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
                             if(m.skills===undefined)m.skills=[]; if(m.spells===undefined)m.spells=[]; }
      if(stats.gold===undefined)stats.gold=0; if(stats.herb===undefined)stats.herb=0; if(stats.elixir===undefined)stats.elixir=0;
      if(s.respawn) respawn=s.respawn; if(s.flags) Object.assign(flags, s.flags); return true; }
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
setInterval(pollJoy,150);

// ====== 内蔵12x8 LEDマトリクス用ミニマップ ======
// 現在マップを12x8に縮小：出入口/建物=点灯、自分=点滅(Arduino側)。変化した時だけ送信。
let _miniKey="", _miniBusy=false;
function updateMinimap(){
  if(!map.length) return;
  const mw=map[0].length, mh=map.length, exits=(GAME_DATA[currentMap].exits)||{};
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
      const exs=GAME_DATA[currentMap].exits;
      if(exs && exs[tile]){ const ex=exs[tile]; loadMap(ex.to, ex.tx, ex.ty); return; }
    }
    // 洞窟の救出イベント（一度だけ）
    if((dx||dy) && tile==="E" && !flags.mageRescued){ eventRescueMage(); return; }
    if((dx||dy) && tile==="Z" && !flags.maohDefeated){ eventMaohBattle(); return; }
    // 施設のドアに入ると施設メニュー
    if((dx||dy)){ const b=doorAt(ptx,pty); if(b){
      if(b.type==="cathedral" && !flags.seraJoined){ eventRecruitSera(); return; }   // 大聖堂で初回はセラ加入
      openService(b); return; } }
    // 草地を歩くとランダムエンカウント（安全マップ=街では出ない）
    const safe=GAME_DATA[currentMap].safe;
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
function drawCharImg(imgKey, dir, wx, wy, walk){
  const set=CHARIMG[imgKey];
  const img=set && (set[dirKey(dir)] || set.d);
  if(img && img.complete && img.naturalWidth){
    const dh=104, dw=Math.round(img.naturalWidth*dh/img.naturalHeight);
    const bob=(walk && Math.floor(Date.now()/200)%2)? 5:0;
    const dx=Math.round(wx+player.size/2-dw/2);
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
    ctx.drawImage(img, Math.round(cx-pw/2), Math.round(topY), pw, ph);
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
    const t = trail.length ? trail[Math.min(trail.length-1,(i+1)*FOLLOW_GAP)]
                           : {x:player.x,y:player.y,dir:player.dir};
    list.push({key:fol[i].img, dir:t.dir, x:t.x, y:t.y, walk:player.walk, hero:false});
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
  ctx.fillStyle="#1c1838"; ctx.fillRect(0,VIEW*0.50,VIEW,3);
  // 敵
  const ew=192, float=Math.sin(Date.now()*0.003)*7;   // 96px原画を2倍で綺麗に
  const shakeX=b.shake>0?(Math.random()-0.5)*b.shake:0;
  const ex=VIEW/2-ew/2+shakeX, ey=VIEW*0.16+float;
  if(!(b.flash>0 && Math.floor(Date.now()/60)%2)) ctx.drawImage(SPR[b.enemy.spr], ex, ey, ew, ew);
  ctx.fillStyle="#fff"; ctx.font="22px sans-serif"; ctx.textAlign="center";
  ctx.fillText(b.enemy.name, VIEW/2, ey+ew+30); ctx.textAlign="left";   // 名前は敵の下（左上のパーティ枠と重ならない）
  // パーティ状態（左上・全員）
  winBox(20,20,380,24+party.length*40);
  ctx.textAlign="left"; let yy=50;
  for(const m of party){
    ctx.font="20px sans-serif"; ctx.fillStyle=m.down?"#777":"#fff"; ctx.fillText(m.name,38,yy);
    ctx.font="16px monospace";
    ctx.fillStyle=m.down?"#777":(m.hp<=m.maxhp*0.25?"#ff6a6a":"#cfe0ff"); ctx.fillText("HP"+m.hp+"/"+m.maxhp,150,yy);
    ctx.fillStyle=m.down?"#777":"#9fd0ff"; ctx.fillText("MP"+m.mp,265,yy);
    ctx.font="15px sans-serif";
    if(m.buffAtkT>0){ ctx.fillStyle="#ff9a4a"; ctx.fillText("攻↑",318,yy); }
    if(m.buffDefT>0){ ctx.fillStyle="#6ad0ff"; ctx.fillText("守↑",352,yy); }
    yy+=40;
  }
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
try{
  buildSprites(); loadGame(); loadMap(respawn.map, respawn.tx, respawn.ty); loop();
  hideLoading();   // 読み込み完了
}catch(err){
  console.error("init error:", err);
  if(_ld){ _ld.innerHTML='<div class="t" style="color:#ff8a6a">エラー</div><div class="s">リロードしてください</div>'; }
}
setTimeout(hideLoading, 5000);   // 念のため：何があっても5秒で必ずオーバーレイを消す
</script>
</body>
</html>
)rawliteral";
