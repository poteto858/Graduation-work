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
(function(){const D={hero:"iVBORw0KGgoAAAANSUhEUgAAADAAAABACAMAAABr/gR0AAAAk1BMVEX////18tXs1Z/ut13HzdC9r4ubtLmWoaK0kkSMkpareReNeEV4g453dV1jcHc0cbmHXBNhXVFxRwxdRRdOWmNTU1BKSkhLPSAWWK4BUssrRWQCRacLO38AOpQ9PDgzOUFSLAY6KhIxKR8lKjEjGg8FMG8LK1YHH0YWHCQMFB8LDBAEAwMAK3MADzEAADIAAAAAAAAbHpkvAAAAMXRSTlP///////////////////////////////////////////////////////////////8AH5pOIQAABhVJREFUeNp1lot2gygQhr2ioCGoqPF+AUGN7ub9n27HtE1tN/WcnCrlY4Zh+GeMx+lJH388afZ6NU7Do/saHztOU/H6T4jfApn5CbTdzg3HmfUXgq3sDTC7Zvt8EarYMACiatTHSGhGb4D1E1BNUTTYdMwO/n4QfwEGBh9EVTwB5ABQVKoDX52/LICrYisOIEQUlQfQ6FFczL8Aw0qzqQITTYZDqo/5W9fC/LebfkSWaWC8l7wuSuRyxLc6o5pj0zTd9v05WNaFcsfwQuxY19DpQscJeeRa7uXtOaSZ62cZMg3DQZdrUSPqOAjRLHUv41vgIVrZceRhTEOLT67TURqikC9j+3gPPB5dSDsJz4hMy8mONxih4vE3wGXPGOulDMP23vcslrLF7Q8gik6fq+xj1veDnwx9nwQREH1/n89Aa3rr92fb9z0seg+iOM598KjvZdzzM5AZTvf9OfRyGIY8m/NB+u2cBsMBDb+A86baIb7HGIkgD+yocy6J9Nc1+wWE54HBl8zEGs7QKifDHph//xEWo4XDOQOrH8QWuh4Avlp2bsfLT+ARGt75LosxGWwbph8/yx+S+fELoAbC55NJ/119mG4flJ3c18dvIDOQ9+3TvHbZLgLb8gGwk11m8zz/PGnhOYi+ksnDFOt9SXywwfJ973DpobBrz6mBDY9+3n5IbRN7ebzvcoQT+EcMmCJIXpydLxA1vE+fBDJMJwyzmLW7VvucsAHDGhBHOp+A1HA+gLHEyMEhnqdL4LuEkIBrHlGEPMqz9huYHdN7ZksO95hzB81Vcb26Pr1VlfKsoKTY9Zn8BlYMcToCMaYYYR5yfQUniuqpHhynIcKRWMXpPrxOYr23kVhk7cBzfQK3ZFzSbP1nWcaxewGZ+bWJZdq2pa/AwvXDQpUM87yMgW0TEg9fgPAM72nivt2KSoupusL85pAnlY3zyGD6JUoZSU4AbCIHBVCwT8HKplFlqba6jLL9Hlh2VhVVVWck/7zTsAnK14BECKtJr1k0qsh1AyUSLtbctA/NxJeqSVj3AcwIwyW2SY69YFn2fdPUpzzwJ63X1baC4lpcbVJWNes/ANHiKydEIpMG/b73Q9dmHi4TIeVwd+3bFcLlkrpqoicQtQvDBSeyg2C18b72hCShc6MjA5VJLB8MFAXym6Yu4xGAwPZt4pOhzy+cz8suOhAK6t3oEA/zElhRUYDiwuVLy40NACQ2hDiRMRlk2u169lAgR8W5WNaeK2LTIrSO20QI655A28t7TA6R68d92qmBL/mlqLemjOcIgLSgPmSiDw6qJ3BIBegbKJJcJ7Ug5M8cw9EVJdOBZgAUUTYRIjo1kU+giwkDwbrrek8dHG3cO4ArUTztSQAZUlWETPWWPTf9zKKYxeusp3rHKMir3DsS6Up0d9GENDC/Jmyrt7T/Uu+2jwe5wRo5DcnwAjbhawYn1jQduNRMLHnJvbizeFGNBg/ycmshNEXhk121bQ9rN3XN4mkrWftdHyS7xLqpdynqWy19G+qIH+t62teYJHXddKpRYOAbABGVW9Psurnd6kkGru8nuq5rSEbI66luYP24OwHdMig4/F1kHZhQ+/GoulZdNwPBkoyRY/43oIpmapptCBBK1LTvk1KLnvTFwcmySyhjbGjPNU5ORQ3APoQeB18WNU3g/pK7IUolgCsIVXYGdFEdforQML2W+bk6vE/8KHUMxIWq9VRN918AbGGboYtBPPXzHSKpW0yP2s7nqZ5UU6sTIGrwCM5nz0AWBST4BICayxm6B2/eIVq6qc7A3BSbVnWjKAbx4+0R0Bo6H5qCsLVzU0MW/AIqLaXQAnkcmgUAGnh0S8EA3CqltdI/gO7DQdBv2AP0k/MRpXmRKag97AECKvYfwKP7AGaMQswzaF5aAbPWlEK5yZ46Oqv0/73GGDjOhWbzV/noBA89E2fjX82JZDgERXq1R2NQYte7nCvpD6CF1qQfQjf5KgYjc/EAY/I90CXko/WRX8VggfeYQdrlbwEJ6sTg6vX9awQ+4hiGY/kWgMVisC+/9zjIIY6fZt8269D5QLH9WflH0FcYfLWh/wEKY3xo1eZeVAAAAABJRU5ErkJggg==",mage:"iVBORw0KGgoAAAANSUhEUgAAACoAAABACAMAAABbchVVAAAAk1BMVEX+//X127HptlDhlZmusoO/lEC7hER/gIm7bX7EZAKaZy2YUUN9ZDF5Tx5cXE9BV1eILK2CJqySQyiGM3h1PyhyLk17JKRxIo1UPDFZOwxVKTpUKAY7Oz4qNkA5JxUlJi18GqdmG49cGX1YEoFLFWxBGixEDWczGiczEEMgFB8tB08iCCwIBAsAATYAAQAAAAAAAADyVqDWAAAAMXRSTlP///////////////////////////////////////////////////////////////8AH5pOIQAABS9JREFUeNqNlot2oygYgKOJNqgkeEMRiwEJcnF2+/5Pt79pOk1m2pml57TGfP3vF3Zv35zRWjs+vdl9Q066LKl5/R/ouJFlqezfUVvCoZTI6W/oqyQbqijRr39GN/XbUZ7ST9++RAO9Ce2oNbSz5z+goyI3oV2ngwb+/D1q39WDrSoYLTs7jd+gkyZ3VKoQrNE+y8avUdt9omadnHMoe/0SnTZLKa07Wkqlb3GdMP5A94+on1WtDlGStEoZbd3mZ7JZ8IneeTfkaZzEMUIoTps2rG9vAsUIT4B+iHz/22OEsn5o23oYtmcs4CVDSNykAjPP3f0fBB6skvRyuVAq5+AwA9WCsXf07aXckwu9V6m+lCW5n1JKfwVjwTcIwQ5k0v0uutzYDSTkmNzPkdDLrD+qANCu3O+IlORtNPJyPGYZ5kJwzlfBsvxYz/OdBQPAMNBEiFVdmmIsOEYV+I8LeGY4y2tl7B3VJZ3L/b4TGDHxL9+o268CHGegIMta7W9x7RMDSdzLIlmd9wIgEAWCeZowkM7X1WGcDyOgIknqNhmKmHnjLciDyDQNq6pi5HzT4H1wDDcnMEAkMaQD4WCMgTgiMR7OL0WyP51PrsKYBw/HMbHFdeRglQuz1gb8qNjh8OMQRdH5HDUsy5gz3mhjx1sNnBeMxCl4YwTOKxwdzIbql13Ks+amzU4fRcigPE7RaXSsEojto42MDoeowBWvXJimzy7YwvKy2+1OCGzlOXB7+In2rkIcsemxtyacCCAPEFmGmMuiKI6jKBUcYlYh9tSGQrhoFzFUYYGqNfTp/nDolxVcgKjd2Ts6YnEA9Rsq0OAFRJYL6xEPvIJE9I8di/nLjiNeCT+LtiviOI3TthZmDmB8xR5RtqGcN01XXmZSH+L4UBMqL7LtG6igJxSPuxeOhzqtCanrZn/sSU1JneZqqYR4REUmdgfBIWVtfUzxiZCepXWdDn6L9dMgWrk7Q/PNHTWQNd/kOVSPCcHMnvPxEZ2Kl+LYp1DzUlpVkoGxK0wXq0up3VMK3qZ9lEbN1kxKB9PJdFlyCSPLdKU0v6EJS7MkVbNZQWu2XPNZ+9XPXafMswFplLIUJ6jVIYirF8si/CBc0JR28/um+XCrjzMHHZopa4PAuChwD1W1biip86Z/QEfH3Q+eZddgrbHLqSn6raZXM9f7JM/ug+hjAnHPtpIPSknZi37puVHBt+kxjrEYH9AJc9tgIL1SnUzArWt21XrJ8zhj69PUnjCDNBgPQjtySa9DZrJMmAWKvoj7510gsPMGvKKSlnVyHZJ6wcs8YJFDzNdf0EVrHRSV5HJMLKB0nqVe16LZY/erVKNM6CjoP2ZGo/pCZCet9a6x4Ql1WHhYaVR2JY2LecZHWnayM0FbdkvYT3QMDGopyBJMpTGe5yKBBwgGFBrL5+kxAoZju+1URUtyQ2NKSwixCYBuNfMTbRUYu9RkCxWJmZzxhnYKyssPqNPrJ5rlDjuc13NZ1nXMqWQxAQu0zLOrx3XjHtBkBTTJW0KSNLFwEUiOKb20bYqEwyl6QDH6h8E7BG19jLGhpUHxMU1hucEgwgg/Bqt/xWwbMxhWCw/aBJjRsOHE1gOfO/Z+igRKbWIwtSE545uDuYzdrQN+uw+wbUsa1ffDbaNMtu9btXx1zXnNEli6VuedfkeNzFs/fXkjYtvIs9swf//eGa3t1/esV0DtTOXH9wEGiPrm8jS9TZRs6HRHIXXUfnPPchLaPrj7h2Cger9DJ2sn9/BxsdPPm95/JrNWg1BjFicAAAAASUVORK5CYII=",warrior:"iVBORw0KGgoAAAANSUhEUgAAAD0AAABACAMAAACeAI/EAAAAk1BMVEX6/Pne4ODWwKWwtrzooFjJklubm5qFio63dTeCfHSgYCSJWypwdHdoYlpcYWZWVVVJWWSBTBZlSSpoPRBaNxRNTk5JRkNLOSZMLQ42SlYzRVE7QUI8OzksO0U4NTE1LiUsLS0iMT1PJQc+JAk0IAsoJyYoIx4qHRAoFQQgIiQcHR4ZFhMUDgkDAwMAAAcAAAAAAAD9igQDAAAAMXRSTlP///////////////////////////////////////////////////////////////8AH5pOIQAABvlJREFUeNqNl4t2q6oWhk2Id9AgCiYSNIKKl+6d93+6MzFd63S02W0ZIxnR+Dnvv+g9fr+qjH864/0WnXkYeOHyM929fz6uDB1g4Z9td/NjaDr1eDR/TzWhYw/B8BOtOr4ushq2Zfjrpgw8dEAZxn/OqPklPchOVmuX87Xj3fznbHgA+HK//glbIvWS7uatkV3F5VDxZZ3X58Xe4YCu18sF5/sxR2h9SUspG8mbSnLedXKYnxd73uEK63bG7wnEL+luAZtAZ5Ir2aino7nnefji8EtYPZNQvYx76AY1dEpVqpu7blidiQYdPHQ9n2/X6z3Dio86DNTLnFeDWkQLOV/WVqzLvFceSpXdLueLw8+5xKX9W41Pcc+DOZ3SdhnT04ktg6sv3lN2dbavt9uSHYJu/Y96z3NxgpX67guSDqZdpe8QMyTtdp3nILtcn1Hj7BPNxViq2PEnX4qxFUAHB3S/wbqA67cFo/N9XiAiFXrhJ1r4U7Eu4PUpWv4ppyh9p3cc/J5vAb7dt/s5w1A3VH2iI5P68VsbFWseJGOSPD2/hEF4BvwOpi+3ZbldL+cbtP4nuosKejysLEpt4EV1VLjmgLibpsHZ5b5eEb6C6QtU/x5+tC2hA+RSp+x0dCnz/WOSpmsLfxwyiRDKm/NtDYLrfXUphCQEH2zPeRAOdWE27TvU3YButi6HAfrec+tQLRd0vs2LS8Hllh3+2u5mxSUKJlZv2hmOop02fc0HrpZwx4M7wncwvdNXoN9zvvAsQFgi3puSJaco8v3o5Nd0YyKrgjDrYmc7X0J8dynbqwdaE+90hnP4jXAem7qM+mSvd1TT1NA2xAcU5ksApoewud2X+x721XWg3GlVYSc7IQ+60tBIl1EUFfAD7jXhKkAhURLoMQjndbjdby5nyN1upzswHQQB5jiz1CZRqrVmSVTWaWlwTDCp1Iq8ltm5k4/uAvTd6Rzf+7zrcugGnOVdhjWrZBolUZJERQ6+2xDxnFdExSFLejcdHbTNHSMEOXuneY6zLM9R0EyiwDbyk+hEc1KUte14FeC8XSueUrOX53x3jYLCZqdnHIMQ5ZmMEZ5aEnLIXOSnTMSkK9MxobquKfuXJnrcu+qShc6yep/QDvzInBbifKJcwaW6EIwyWRWnSEAQvp+UOmK7aeh0WAFu/sx3takMPAll1TLfp5qxPMwFrTWFuokavtNyq7WVT0l0l/L/q0Ob0H6UcjbWuB6LrCIpI7aPThCAeUtPRUpTP5qcTEle8aqZP2iLgMtSSktrR5ZCrvu2KoWyOk1oKsRGE6gcpNEvvj60gC6Bdr2tN8JpVJYpG7tBw93ggBRTX9YG2ASU6gXdgrtw6yiZjCAFZYxCr6c1Y6ymRdGuU1onLiL/mbZPtJNBn1JqEJ9GK6DIZZSmRa2L3K7ThmRvEvDslDiV+0Kn0NV+lJjQQ29Gi9zoGAVBKHrNa73lHvoXps4/+b14Rdcljrlg7cELrN7stoVBHBIc1tZaY4l32LSrBDOl+kovCh/iqpJT4OWgJZQRRGD6CMk0K2s9eRkrtzSprRDDV3rkSkBW25F4xPI+LUgYw7xVea7zkluF6qQEjwBuX1RsmDRJaWk25OXbVBe6ynJJJAyWEXVtV+glu2gh1PB4VW9TCiiOVS5sa3U9ZirPq4F07tDY0k1HB+L4eElPWgQxn5uD0ozW0M8KZ5xn1aaNZtokL8r8gV4FDkXbCmJhrqDubJryLG9t7w4No7r7jl4USBjUE9SIJQCnotTa9rooGO01jNfyHf2YqOs2GMY0hmEpfJhtsNrzlDFwxxb8W9ok8ORgNBHH49Evjn4bs4SxlBxjUid9S4pvaQHiD08ATeIC+COBJNIiJm1wFJRaQeZvaV4nTkKb9m0kQRyv6/GQ+kf+JohOevumHt/SoC7tOKnHrMQmxmncYhfC1HUWGmVs15/o526rJaSC+wzLVBwLscqBJXotxPIrellllXPVDusyEmiveU2ZVcWyzb+gOdTHTMM87xslOAGCpt8sNM30C9okNICZnqx+H4c6Km0dhAWtf0NTguKcJpQ9Ax3hAWwJwg3rf0GPhhOuoLf7/YHzKEEQaJFVg/2N5zBrHERcb9vzZmkKexg7SqP/+Q1tQIdqY8wzbKcoZqpL3evtF/TQxnHJQE32I1AUs+q6LEjxSpG+0KvEIYHR3Ftrg+3L2GymJgFR6880bDWolrzdhVPGpNfNQwpe1TCteviBnnqaJgmbuNqerzESMj9xWcJJVprv6YHDfg3E0Wy9iztHWQaj1deWMY6CjHff0psMwryZ+t7qad/A7e9i2pp+ylD44YH9kl4VxlyOxqz35/vgc6i7Sbd5iLkavo8b3oOGYRi//D0MXaPmIf8P+n+VwvFcrKp5NgAAAABJRU5ErkJggg==",sister:"iVBORw0KGgoAAAANSUhEUgAAACsAAABACAMAAAC0sH5rAAAAk1BMVEX5+/3v8PPs7PDt6eH127by04Xww2nttDbh3d7fzareuGLgryjMxcbNsXfAuLytsa/GoF6tn5XXoBXGmBzJjRK2iimzfhGqcwiZlZOXej6WaQ+OXQ1/gX5/YytUdnVhXVl/UgpsTxRmQQpUU1NPRTJNOhVLKwk8Qkw2Nzo5Lx8rLTEzJA4eICYNCQYAB0kAAAAAAAAbLDmhAAAAMXRSTlP///////////////////////////////////////////////////////////////8AH5pOIQAABXFJREFUeNqFlmtzq7oOhrmZm40xxmAwsDAOmGtm9///uqPunTZNp11Hn8j4iUaWXkty3r7Zsli7vP1ozjdyrsIwqs38f9lL48B3PSeJar39nZ106HouigLX8yM1/Y1dBALUTXHiuK4fyel3dmJR4roJQhih2HMxK81v7FK5DDluJCIuBHguc1JNP7OzChzGfTchmJMkdpFIYlqfP7MF+Zd1nJTHDtxQEEILff+JXQmNCU/c2E+Y5/seEjmhVP/zI9tQ4lYFCgMExUgLVgGaDz/6fRvhrMK8HTWY5KKlOW2On/Nwo00ljZFgpuusVVVD+/vP7Nq+k1HEI25EVHZWy/74xe9ljY4ihBXXTOMoqq3pzt/qthuMsTCDyoLBCA6f5+81FlgozUIEmWC6w7y2n2czm19ZydWyz1mQoGHfFynkhx72q432r+wsDIg2vrUBQs2a753Ucv+XXGRX6xd2ka2eGnLvAxTMV9x2upXvAS8lQxmSZvvKqqEzATkaYMfVR1oOkIehUDJBWajr5QurOyMMCzIIIUBZwDph5fRniKqqHYIJol8+2Z1rxc8hABTgAM2Gm7q+v51dCf9tBw0BPdijFlpzY9sgSFPwPZ2SGyUmyKQyLGWRNPaDVVwpI+T9PrMkQ+1+P2VnOikPyJ1KoeBK4uk/9pKyk+aUWp/nvuz2BA2dRmmpD9MprDtttHqwR2elmcep7IeWVYy1rB76GfRm8hF0YZVU5viIYdfdTp1ChCxDacKyqKIumXW3NEPFtNV6f+b3UKZ3PI/VuGqqgZUqgzeXL7aPT5CpubandjaoJ/Fi12+7vlmHrmOOF/t0121jBdxy/aKz1YCHOE4iPPV/1mkyPIq9mECs/ZSWZnvR5LpROAz5MrXzoabRcOa5Mb165zCfOn6wpgAUc9PGfVtPedxZHiVe1g65ffYI59H2yiwBdGAxmUzj0XoyNU9LoS/97K7OI2UCc2mPqqeU1QUt6vbaQbXT9Y8+v7+hY16W6Tw4i4uUK5KX1dGDTeM+79/8Xj3NC7nuIiOUiYYWvLr6PEgnxBI0v7I36sdsWnfOUoxwxaHQx05Bim2Gwld26z0vkes2R6ysGK/rNq3GvYhMu6RBurywt9iN8TJuIyZZkYmC0bTq56G0ZkJoOF7Y0fPDepxvfZq4XiaIQ9KqGU95nue0vOZhpW6Ap3rvi5S4Scorj0Rl01xGW/OQzSd7xG6orF5pk4J8KgEdO6qa/HbpU9nb+MLeYh9Pw30kwHpuwlwoN2vy/rad6uyL9Qt7UAcNLdob0kS+924xsFC8bYRWWdCvtThIkA4D23JS4Cfb0Hy8jDUZ7Y9PFpIbqGG+bjR+smGZ03yV6akZya9P9mq8sLvGdSQxfbCeh97ZDV6wZTR/+h1JIOa+3woYWk+2ArZf1GUxpf3nuxjdRG5Nf1DifbIusCTPr71feEKah9qdtfHwNDa3EViI1334FQWwt1tz6xDJbw92pnF5gNuCkphhyK+PHB8KzWAUNkczakTI+F8Qzs6YWPp+zalH+XstQlgNIBoImDZr/0dHhI7rB5tKWy8jzd2co8RLI8wDP0trCJiOWzuVKXso2LEstZc5VUHcBsaam2FcJr5I64I0hbbqLBGbPuJVfHnbrFJZ0vAgDZwAu04IE6KgTGq7XyKszUfOZglzDLqnwKpMEhyEHIWcYtlimETm7aqFeephh3k0d7IWHUY0CJlIw4SlvIOWrOH97PP3ubmZEr+vAgicIpTWXGkuzPXLjL3b87QGp7AaidrCj/PcfpnH8wSdR5v7cYGdWgmYsbdf2JMjXKvHGrBDOCEXv+0EMKRg3p2Pi5xWlvVf9od9P597yO3c9/3b/vA/21hK9YRP6b8AAAAASUVORK5CYII="};for(const k in D){const im=new Image();im.src="data:image/png;base64,"+D[k];CHARIMG[k]=im;}})();
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
  // 洞窟を抜けた先のエリア（魔物が出る）。B=洞窟へ戻る / G=奥の町 / D=魔王城への道
  area2:{
    safe:false,
    exits:{ "B":{to:"cave", tx:7, ty:1}, "G":{to:"town2", tx:8, ty:11}, "D":{to:"road", tx:8, ty:13} },
    map:[
      "TTTTTTTTTTDTTTTTTTTT",
      "T..................T",
      "T..................T",
      "T.....TT...........T",
      "T.....TT...........T",
      "T..................T",
      "T...G..............T",
      "T..................T",
      "T............TT....T",
      "T............TT....T",
      "T..................T",
      "T..................T",
      "T..................T",
      "T..................T",
      "TTTTTTTTTBTTTTTTTTTT",
      "TTTTTTTTTTTTTTTTTTTT"],
    npcs:[
      { x:7, y:6, name:"旅人", color:"#5a8a6a",
        lines:[
          "西の とりでが この さきの 街だ。",
          "そうびを ととのえて いくと いい。"
        ] },
      { x:11, y:11, name:"見はりの兵", color:"#8a6a3a",
        lines:[
          "北の 道は 魔王城へ つづいている。",
          "道には 魔物が うようよ いる。きをつけろ！"
        ] }
    ]
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
  // 奥の町（エリアの中・安全）。G=エリアへ戻る
  town2:{
    safe:true,
    exits:{ "G":{to:"area2", tx:4, ty:7} },
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
      { type:"weapon", x:11, y:2, name:"ぶきや",   color:"#8a8f9a" },
      { type:"item",   x:2,  y:6, name:"どうぐや", color:"#5a9a5a" },
      { type:"church", x:11, y:6, name:"だいきょうかい", color:"#e6dcc0" }
    ],
    npcs:[
      { x:7, y:9, name:"町の人", color:"#6a6ab0",
        lines:[
          "北の はずれに 魔王の城が そびえている。",
          "あの 黒い 城こそ 諸悪の 根源さ。"
        ] },
      { x:9, y:5, name:"旅の戦士", color:"#9a6a3a",
        lines:[
          "城へ 行く前に そうびを 整えな。",
          "教会で 蘇生も できるから 覚えておきな。"
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
  // 魔王城（最奥・暗い・魔物は出ない）。G=道へ戻る / Z=玉座(魔王イベント)
  // ※専用の城タイルは今後追加予定。今は岩タイル流用＋暗闇で雰囲気付け
  castle:{
    safe:true, dark:true,
    exits:{ "G":{to:"road", tx:8, ty:1} },
    map:[
      "RRRRRRRRRRRRRRRRRR",
      "R................R",
      "R...R...Z....R...R",
      "R................R",
      "R................R",
      "R....R......R....R",
      "R................R",
      "R................R",
      "R...R........R...R",
      "R................R",
      "R................R",
      "R....R......R....R",
      "R................R",
      "R................R",
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
let currentMap="field";
let buildings=[];
const TILE_WALL={ "T":1, "W":1, "R":1, "H":1, "d":1 };  // 通れないタイル
let map=[], npcs=[];
let player={x:0,y:0,size:52,dir:"down",walk:0};
let cameraX=0, cameraY=0;
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
let flags={ mageRescued:false, maohDefeated:false };

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
    const tag = wk>1?" よわてん！" : (wk<1?" きいていない…":"");
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
  else if(b.type==="church")svcMenu(["いのる","そせい ("+REVIVE_COST+"G)","やめる"], [churchPray, churchRevive, closeService]);
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
    if((dx||dy)){ const b=doorAt(ptx,pty); if(b){ openService(b); return; } }
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

function drawTile(c,x,y){
  const gx=x*TILE, gy=y*TILE;
  if(c==="W"){ ctx.drawImage(SPR.water[Math.floor(Date.now()/350)%2], gx, gy); return; }
  // 床ベース（暗いエリア=石畳 / 通常=草地）
  ctx.drawImage((darkArea?SPR.stone:SPR.grass)[(x+y)&1], gx, gy);
  if(c==="R"){
    if(darkArea){ // 洞窟の岩壁（岩肌・ひび・陰影）
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
  // 生成キャラ画像（正面固定）。歩行中は上下バウンドで動きを出す
  const img=CHARIMG.hero;
  if(img && img.complete && img.naturalWidth){
    const dh=104, dw=Math.round(img.naturalWidth*dh/img.naturalHeight);
    const bob=(player.walk && Math.floor(Date.now()/200)%2)? 5:0;
    const dx=Math.round(player.x+player.size/2-dw/2);
    const dy=Math.round(player.y+player.size-dh)+16-bob;
    ctx.imageSmoothingEnabled=false;
    ctx.drawImage(img, dx, dy, dw, dh);
    return;
  }
  // フォールバック（旧ドット絵）
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
