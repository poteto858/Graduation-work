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
(function(){const D={"hero":{"d":"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","u":"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","r":"iVBORw0KGgoAAAANSUhEUgAAAEIAAABACAMAAACZQlHRAAAAflBMVEUAAAAfGxoPUqnq7OxRKQZXYGUOLFmYo6poZ2czS2Fsip8gIB9lSiFXdIv39vf6zJKYYxcFOYvip2dZWlmbm5wrKyrkuoyujWGetcnemQ+3xM+SdlESYsNIRkY4QUu2jDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADda2e5AAAAIHRSTlMA+f79/v3+/g38/qH8/gb+/v7+Wxpn/v79/v7//pxh/64cje8AAARmSURBVHjalZfpdqs6DIUtjGwMZg5Dxr7/W54tm0M5964kRj9oVlt/kTVulHprFdGIH8WlUIU8ztvE2lbN4RfFOUrRgAAEPl4Ujo7j5bwTNOvoBRkico4nwNQZRwrSQBAZkz2yrFzXmpjH5gyjsgHRZ7uVa8l0OcEICF2aX0KZGXuKERGaj26wW08wmmoJCO0ODOXqlav0i9wjQnN2MFPeaEr04kKS1P8xkFvPvmiSnDC0tBsjhPQRGdSp1qfUBJzIDM2R4eRob4KBcO2oSHMCh+BHqxfKHo/MOGvt3HKurpQxuq9IQPQ9GDnlUpzGMMoTWebuSr3S95SsVChsE5xHFohLW9pV61tAZI86CWGktyj2ly1Ly8YxM9FVGJSEcMhfXdc4bPFwtFvXEalLGgIWGGUNQB6MOg/GNU+rLLdZ7eD9ThjwICCalMKQwfnX8o2Qd17hScmd9vfkbnehDUC8miRCMe2I6AYNg7+Z+0D0z0jGTH7LG3f/fRufw2BpmenZ/Hvjt+NCWnVDLIOH90s7tKtZWv+f4i6m8UOB7ogOiAEIm83twEdGUaHe3sWi2i+CKODh23aY53Zul+m4rdA4bxEhm7Qz8mFuYfCCLsd9p0tL72LRjM/AIGH4Qb4/IO6vPZIjVk1dvvcChRH82DpjaGdcY2iXvyktwrIqS60/9NzzGY+bUKCLONEutBMastrWVvP0sbywT9FoTA7TJ/cw/q3tivUqhOpjwzQg4L/W2rCTkIyHYgJB42+2+rKO4EPci1STRwX8buVxI+BXzYduLchtCIvRRd7xdiCGEsmwGObF520kiBsIGFsYvbbkZ7OHsi5vks/PTUuMwQmIxeAyevbarhxqRna2BYHHryKFGdfVa+lcybhJDYExNYdkTN/1mjDiTWrURp8ht1UM5S2kM0EfbIvAMQYwVtKjD3kNoYzpbL6Mz4l6qUsfijyqDMUVKYSylGR8n6DNZLDIOgyLHWFb7wcqQbjZ77tZ2gxrVfoMPstFjHFth83Ovq5XTVPKGqCsl5vYoB3RI918/cEmU0vHOk0uiT4AYplBQPgxNFiWWX7N7t4kES6VXJ/8fWHcPnpCxBLf7uppTNFKIo+MfDdBnoDhf7BD0PjYB1c10PdQXKqICPob+gIt0i4sLB/We/U9mAUFcWZM1PB1Tdchd1pkSv56SVSbb68S4R6CgF5CYh3lyhMLwflJpaz2S3CiB8LxD1RBHMNWhk+eJ74RGdGZJu/uyw8u78M6CVMKn9MUOEW5/KsxJJ0kYsv5Kkm5xkhALPbxuOVNZRnjaUwhbE5kkg++SwhQnZ0Cwvs8RbYW6ilOoDiNcobZxfejXMlmpFfim51xQgCiIpGbsrZCXJKyGYJ5GWUPAgGJIOMXlBLY2p17RRxFAKNdXajqMDyJUkJ5fC0eYVQz1VINQbUhJ68m3YvocGgxdCfEwQ+GztKROmPymr1pV991y3wPvpx7b2+iXBKEB8RTNU2TUif9GOlob+LwB8OUNwHmOdv8AAAAAElFTkSuQmCC","l":"iVBORw0KGgoAAAANSUhEUgAAAEEAAABACAMAAAByderSAAAAflBMVEUAAAAiHBkPT6UQL2RRKAXq7OxUXGHloBNra2sGN4oyTGaXo6uaYxH9/f1Sc435yJBqSBlvjKItLCxYWFjgpWWfoKMlJSVMTU2lt8bluYuqjWCzxtWZeFgMXcTHegqxiCwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACWmnKpAAAAIHRSTlMA9/7+/v39/gn+/v7+Bf7++/6dV/4WX5b9/v79///+/uFqNEQAAARqSURBVHjanVeLdqowEMwSCJCEkPAS0er//+WdDaD2tEp691jb02PG2Ud2BiHeRSWqcRwr/qMnohG//xJV9Qo1qUJ14j+javA2LUWh+uZvJCYB5pKkPIleTNeiOFP6twrRd0rJeQ4hyzIpqetUARJjMkCD84IP71GTAYJJrETDALOTzj0RJFNIRYgMjFPfKMjiDwhCnMBAZ6+xAiQjTGSCzr5+MCiKa9ck1GD0yuf6GwW1AZypO55KfIBaUVL99SOFol1IVocQlRjJW2ExRzFiMqrdAGqZUIiqU5m0pSVzbg2GSkYW56JlBnVN4pAEbpCoqbRyRulNUJrA40sSD3gNhOm4EUpkjMAzOJvggkMudb0lJYFwWEtigDIn0oqkBkJQBCIUQydwAARZy5/O80jdORfABSOutXbHHDDSYEDoB9mcg7/YxWCELKGSoiKk4O3VgoRYMZTTHBKvfkzKIic7WJ9zIjuPLfqklcY7dfB4u+a0ns93pO79Tn3NrTkRtVc3q2HYCDwRms8X4nm76TzQPAwYQxxbFjrk0E/TN6zet4u8tEM7oCT50NJejrf3ipTq+hdp8Of2nJkNwQ/DA+FtEsRr+EljuraAOOPHA2FoPW315F78ToIu4RVixKlzy4FuoJz0bGj/lkNwRfHMslftBtEOlsdjjU68zaIzDGFelPWKIiCNNs7FI5pPO8G48NjE0IvTshKIM63UmoGq3srm2DGEgbK+rlvv+Tz+PRun6XSqmg/z1HAeei5ofGxLjBXJPDeXuGhA43S0XgEBIki12S85r5hgWC6BgW3Xf7rX6AIVswuXzWQgDc7fKhPinr8gD03N4YZFNee94yfSlyHHqjURIDgQ6g+WC0phlIktxQeVRmG90QF5QIVRSqe7w+1mLkHNIZKdlAxh3a6aaUBI9QECUkcpLs5dFBe9W7dzifDEVkChDsdWrt9KwX/i+F2IMkbc3JDBBB/WKeRhHPWEZVmKu6qh/OrOu5ugmXurP5kflMLxDOOIKBU7B7xqBYicoHjjsVZUZIzU/m5pKaNqLwsQasmJ1FmKcIsRbc8gWV6oaCBEyQhgAQ5QwyMH01TjietvoRdl9B66ZATJJNgFHMoF+5chiq4fVPRPQGDJl9KXMAEpaYxRdPPclnWkgBqy2jEJL9mWVgfdYBJopQUCurhNA0JvCJlMcHK8NZkDzNMDoJQPhC7FyomeC8mDICXXYY0VIUX8eXBPlIM49+CBgHaulizBksYLAhKSrVPsBSPcPEWEJAcSS+HjyZ1DnKgVQXYpCBX0fyPBhlKuU70iJJJo4miq3QGq242flrKdRNpDSpffSrtisMHFrqc6XtVM9iIxKLe2vN1uFtdE6hWBAT4Lx3O8uacyZ93CcV60DfGEMJnUp7WJct410pkZq15ia8ESJ5/Gs3Knljt0GxzUbDSxZPV983h+TZmpYfFsgO6oQoBsyvSH1R2BDemwWIQ/Mg+/L8xpmrgZli0Ae6hDyfv1mfHVvnwG+Acs8zwFAMdg+wAAAABJRU5ErkJggg=="},"mage":{"d":"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","u":"iVBORw0KGgoAAAANSUhEUgAAAGwAAABACAMAAADI1mKyAAAAflBMVEUAAABxLJLkYgQlGCJSIGpZKQ6pUQvrlhM1FFBnaWmEOJ/6/PqROAQoKyyJRKGjX1hmRxpPVlKZnp6SIdzVpFghKixeWVwaHFtNUFDca/b1xV329PaxiB80OEVpbpd5jnUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADWV0x9AAAAIHRSTlMA/v7y/vz++/0Q/gj+pP3+91oU/fRk4x+YEupk/KQnJ5SNpX4AAATBSURBVHjatZmLduI6DEUdy5IdJ8FOGh6lUP7/L+fIARqmcztrLsYUCpSVXcl6HAtj/m1dWtJ1aM2LV4cbkWuwHOH5i1kDuU1smrjZbNwwvda0Q4jKwgItvNiPFDfNsqKaZl7pSmpurGIamfZlYXKhJt5ZN9NetEZyynJftEgvMqwbl5BvgluZRvVBrfkkXhguBOXcTdvWp32aExeUc+Qed62rH5DsewpABX1cwSKp3XW92BGzGUpNdA/xuAnb7gH2/jxumKVAUa5WMKRabFYh0tVwaDtmcxrGYsHaMpjWRBrr+nEQfzx6/sRFh9WWLbDHxH7etoG88UKo8Qdyd1gsd/hxKqaVzdpPT+9bR977vDdmooACsnYkeAFlpH1vV+H0ZP0V4jx2umPuEaW5x7kQ3q8Z+Y7bE8BuO0zbc4viGH6HwZfB89JFW9OYt6Zag+HwyIrKOllyNMKu5g2fAa17Nk5aM41EllewWFoAnbwJiBKUrcZ8fJi3GrZdDAUSK3l35zm349lb0Teig2Gw7O1NH5+JR5SrrYZGYGsRlhmVHy92O/He2n5pB9EF2ARU0zyVb2Ad0MRw/cBilefN7GfknqLCzc4cFYRndHkqzwZFuVL0uS84o0aJu6OahmdX7Hpz8oxYmO6s++/MzC481GTnjWuSaYLx9KObfs7B4Vsef1+bGNjL6cib2efhf8OQy+HvLHQaYgvnes/0k1Ce/hYfqBw/omJU4f9BcvTC9GxxbB/b2CMoIrPB6nZk0RqGCqX4Ak2A+Igx/oHEdIDsYXs8WhWST1umccK8FOJ4W6q1IE56y/hIt2OkBG8raZ5eetal9UNzDkkHkghgh4+2+2QNjkoSgVAvUByvRF2iC+9ayx/oZCQCQVJFfaMKW/1R3nUp25Z3eYeCT8LnGpK1NWe2y6Xt9XH1BKahaGDPailWwiXlCyZ3Iipljz/toJqF2q4OTIPOevu7VQqzfd9neFEqHdfadhDhL1aB3V9yTzvdsqGeUmXxa4J2musdGWhQrXhf7XBBJIvTvngLTCwTZS0k9Q5PW8D6G2NF1aRunEjaZUi6rsq4BbW/v8H8GtYj5JtmPmkUfdYJ/XYLBaIlxF6d6W9xgoqiLToprNLIZ8B/Tz2sKMB7TGqpIpyA0yyg1TnO740kiF/qdd/6r/AHXI+/UFVJ0oySX8OL+3yEloFwRO9X665mCTMMK0IoZZFKXtyTP6bSvkCjKw0o0oajXdUZhP9UxbAOnfPkobOpNDFaHInLBz2wqWmSuFpKa23Mevmm+LJIVfTt5biGXwZ6lesNKfKc6CrrQ1HGq+OTyhOUx2ow9M5ZrorOlanSSm6VrWQztZUK44jNov/Uc6D1XHE84Rr5MyvGlCB0hMdqLE5JyEtK31hpNj6F7PO51nh6j1MDLEvznNJ6s1LCMS05yjiuUa3Qn4beyxmCcZNwdJjTRjWqPvUJiXAesiyDklqNOmfk9hlRgjIIHs6etyzXIauchNt6Q6xxKEO54Mog0Ghj1vB0dEC0coben6oOzCDUitrH2KPDGsJy8r10I6RQrj/XLwntFpWtLW75UsaMjPZSe3yLLYLnwjJjHLSiqGkqUPam/hiclq+zyvZMyxz3n6abvwDdjjOTzp5syAAAAABJRU5ErkJggg==","r":"iVBORw0KGgoAAAANSUhEUgAAADwAAABACAMAAABxwuT6AAAAflBMVEUAAABzLZUmFiJRH2nfYwVbKg+lUw81FFD5+fiDOJ7mmBZkaWUpLS2ROAXcnmCbYlhrTB360JthWVmNRaFTWliUIOL39PYgJyecopr5+Pusj1v2zVPibPWan6DdtI7i5ecAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADi6/AcAAAAIHRSTlMA/vT9/vz+/Qn++hGi/vr9+/zo/Ff9XnAUl/TsEej42s0PNnIAAATaSURBVHjalVeJYqowEEw22QQCJAHxwqr//5dvNuDR59m0VasOs+fsotST0yiSMzbqz6cZyWscQ/6P8EaNZPSq1nW9WgEv73x7KrUjA2g5gBuq/kLdmpW+HLCb9g/okWp9O+D23zsMo+/BQOvv3W5nYnOHNrT7Cl21pAVszD33F27j4m1LZk6w0TfjV/VHwyt8vhAa7+/d1p/dxoczxBg/89fPDZdXP4/gggEtkThd3/B1bdTuk9vkBdmSX+yvZ2QtUWhmw3/UT9U88Vm53LYtXgG8IGtBGjgyMD5o3vmduTyhThbiguSpDyHEvL3S7NSj223M5XlL+hIwQ8dDiIcQnLMkdjfVz2O48PaWe1f8av0MNiYDFqxzRk6RB9rO4Z79vF4FPh1UIKky+ao2QwbQMpf/Ll2iPZImr9WvzI8MFpYLw2XvkxNOQZaozZFDMnSrtNoopc2vzFNwoThNxmdng2WJuuSpAMVmj59Bq/1Grdd6/LkDsyOWcDfEztqO6RJyiRtxJ+8YD961Ana9Nvd53rbFj4YckAu05ApIRrStJTJqr9dasBrgx6hv2TH7BWo8A4h4RxbwsNkfzRqf4O8X8xI+dDRdmrlYC5AFcWE+V/th0usZnp+pwQXqYa3wWdelGFNnO9pvzkMYJFFr5Sd6kAM/11YhdU54JVIUJ+6GTQVmpygrrygG+p/XX6FiaME6R34AOO+rSlVnxPNwcNMh8L2iVwvW+Et0mYDmmAef+kR7fEGo2SpUXhZs9UvuxVMcVwLk0gRwd4rQmD7JxdUOYApSP9v/1YEkp6IHhRectjD3vTr2/XDcAL1RKFvHNFaPw8J76Xyy5SDInQ0dwKe+75OAq/PAAe5KdP+Hb2dinsGOM8DW9eXks4D3R/FJZOXx7CoI0UIsaITWid0gHiRgiDYjRzS+UuKKr+AOBcZOxROwYqUEm3DJ5oXyI+YuLFh8DYU+oEZQ1UqyrFAklt7IIIeb1dIYJiZgRW+R5jOFV8QFOxdWqRGTkkH8p2SmTcnz5siB34z2rnNLpthlzlNCzUEnkKhmp6qhc22jqpfzGdmRX3Q1ZAh5c1PKeVW6d3Mm+zxNS5VJZgtx6esa9WooD0oGJeoLNowvh3V7CZfr5l1Me71asUPpeM4DWoZeTtmm5DiUFF/mPOhNdtivYk7osNvYeRZr6ScHBVimNZVpkbFuRFYx9lP7Btwt4GXawWXDdFLZRExRd+qnd8wilp1Iz6xkJGk1iZMJVnHqe7d9uZZQJ7LdCbHWScBiNoHUOFxY9YrfDGmoF4YbiFVScZU0ychEoaITUeKc3qRZrkqYF1KZFl0fU8ZCWKMJyXQyOaj9tHxykTHj0qCcZaQKzVAkynH7cXWF8HuhQVLxiD0sMnLlvwKDW/RXBCQ6CbqK5NEapTY/bqGlLjzHGDNLslPAeKSBvyBuCtYU7S9Vhrr02mZYTZ9od5D+eQ0xfhnvMRs9YPQ7+kS8ZetuO4wYjXbAA1Q05E8ut7LLOJfMFXuKlBQqL0TXflq6S0HYEBLuThLK7IRElTzhku039yg4Cvh4mkLChPWEaUD5c7SbS40vB5xyZwY5V28U6D94VbZfWQXB3ILZBR6/v8Natt8V7gDMVmwZv74zLMyzCIJa1so/3RdW49Xx+c7qkfcf8kM6l6p2gnIAAAAASUVORK5CYII=","l":"iVBORw0KGgoAAAANSUhEUgAAADkAAABACAMAAACX6y++AAAAflBMVEUAAAByLZUlFSFRIGjfYQSkVRFbLBIzE1D6+fmDOJ9oamvjlxmiYl4nKSiSOQNoSh/dm2X91KJOVViNRaVhWlffr4mUIeHq6e4lLSeXoZ6vlFz3zWR2kKVTV1vs7u61hxYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACcVsLBAAAAIHRSTlMA/vX9/v78/gr+Dvr9oP75+/1a/eb5/WNtG/jxIp2j/cIIO5oAAATBSURBVHjalZeJduIwDEVtKZKzOHESEtYu//+X8+QEWpgGWs85dA7kou1JFs49PaMjO+7k/nbGNrDfVTvPdCz/RDJXu533Hmx4/zVauxP5zNnZVfwHiy2e9/6G0m+50vHNYka5/SV5pOoKLpYrGn8V5EgMsKoWijNPp19lh3NSq4WljDLVv0gwL66uaAicTYcjvvX5ocXN1SSUwEvIof4J/fKkRiX9XXqImC3apwkuLcjKV3coEwWwPrRb1lZfV0+vMDOTaykEoi1f8/sn0ol54hVebcLY8ajpueqkwbmcKdwMM4fWvlXTuBWiHS2kK5rLpbnMithWlOxDbR7VW9a5d9tgoVgumJNIUzTnZHnJgWablwd3S/t0dC1b/Xa77CEcJC2KphHJ34Sstp+Xi5Pjd7F55/bOg2Sz+E2uqlI0haixbTlq4x7dPZRu7/1EDG8VdM5rTms2bCxsvmkjjX5XkXcHt9/D6hQ4qD1HWTNX5WkneK89IbWk6q4k/vi9gXjITUzIS6EihZrllQ1q3+fKEdkfv3XUQnq8psMHidmcJbuoCPrGCv1fQ3b26X4fE+LtiqKLIp3ZNnZtEgRsk/NefCl6s7jnRsvyA9nUvu8hBjvdHfv+YFX7KfjpMzSFlsiVwtlIJJnFSwef+SrBh86Qy1maS1HQCC0Bpbl3gRazcEHAWpUtzQ93ACk07hpFzBg0paN5mNx0npFiMS3kP9kwxsn3jizt2gFL08EZ+DH1Q3DzPIglKyb4LF1HWdA+HO/NvmVUM+ra8zA4NwxWmk76VOR40dcQWLgfJ7kLHNydPjI5gxqGoUf5uz4uSZbcQWCPjwPWYiJzti7fYwZnE7smlUyapgg1HR/VgLTA2w/8ByWd4C5MQoIdmRCXylLbHsvy/2FQ4hla7sjyQHHu+yha6ArCvGzOy5FE38s8UMrDBNeU4T9dnS2abnty4cGTZQv4gUOMULm164oi5s2RpwUdICKLFdNnQtk5Rs6dnR1GA9Qb14q5WwM9pJQUK0KUxFDxNUXQ0QZpOUJ23WFyu6RniTZ+4O4NRUl+BDGgxKT7PtHe29Ba2hpDrVsjlXbr+rThNYniylNhbBe8XmPdNb9fM+heRsc0TUOSvgooEK5BDmtDd7moliJ6cz+ZhV4xC9IcPSV0BUL0X7NvQbfI8zBrTH3kyZ0hhOWitiR1GcXLFinDALV9RuoRp8UdVqNd9rfTDbKstUeYSSPJp2Dcfy0HarMQ3aLHLRUlseFeoBCJ81oRY+x9NKOaZ/fPVUFn5rqgqhQkWj3E9ZdLYcKHgLp2rLd3GrIrBfEFcVic1MkUxUXzE//entzzJ0xZMsGFs419CgoBo2MI/o/P1q/aBEgHSrJ4y9T3NhtsBNGLda/O7qqE1C83bzIQowuLUOnKZ0YdWolw0dBnXC4xuwx5QV9stVYY4YrO0d/2U59ZbZ/ubW1qRJsY6TzHr33aQNRz3CwK3k9NjxpEx3SBCtb1LXJsCujhBSl5S0Cwc984wFXEQia2tbyfNnNkq3QCOGKJZ8uM9A6UszTRyzUcarcbAGu8wctp21P5cg0vM1kaGK7X9ALV9cvfN6ZP6zH7mfP/Bf/Scu5NpCXQX0jM6pFuZ/OHxj8kJzjAsfI7xQAAAABJRU5ErkJggg=="},"warrior":{"d":"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","u":"iVBORw0KGgoAAAANSUhEUgAAAGcAAABACAMAAAAwNplFAAAAflBMVEUAAAAiHhxVMRlXWl6Ym55ra2o6PUVxRin29/Y8QUff4OIrKyuYVytVVlako6PejlElJyhJSkrJcTS5u8J3e4OIbFa+wcY4RU2og2h2k544QkecnJx9gHx6gIjGw7y7wL8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAkAGXWAAAAIHRSTlMA/P79/hH+/gf+/aD+YR7+ZZX+/v7//x7/FGBC/////3lL3rkAAATESURBVHjavZiHdusoEEA9DAgUQLZckld3//8vdwqoJM8xcrSPc+KiWFxNH+ZwaFnOHQ6B/n5GiO7w/yzeN0Om12EEgMjXQo7Ec7uTCJAJJiuEzDy6sCuHVeZ43wFWaww7SwOuStItOYQJe6rsRJIoB9fyRBCj7ci5u7Lbz0SfcH7up7egPnBnjWEvE+U4wKdr2McbIjxYOwk0c7rFp276spMfuAwfQYsw2s+xP1dc3i3pZPgboPzIDcjf/g5nfK7QvA/TR2obdks8+5vHnXit3cA9CNTT1jgNY0RZMZ4WmnTjp5g4bk2XAeyLLGtx1WpEY+5A6Pp2nYVoX17txTLNQgwlPYY3OKZ7nAQ/tvYIpCG4qECXy0VA1Q/uc85weqLSgLXX69VeXkioGfQG6XyHk5L5vrnnCWGQBNn1SBiLwDIy64dJ6Y7aTA5PFU7saPUoJOwGftZTMfifOHx5M4kNdGUOuTYIiPrAIQJif9ere5QKtE11pLdrz+IUEGIIwKJhj3co/M8NIEdZLNOmgJUjIHARrcQTfTeiqLK0m2PMPwwaWkEntJR4ge7pO00KHTAL1CV6/AWy+4ThFysgeoWxtTac+AZAik8oHNQnln16fgBjPO/vPWPkFUUk+W2racTclHcQyr0kDgUsqq1IKhREKn6nMH4A+XHfXuvEe3+TOETgm0ESHRSxrLyRQApK3su7FU/s2OsaLUQBqduTJMi3SkpV71ZKcYNUbKPBg2oi3NIs1rqJ6gCaTWmnX1hCkmUjI3kh9BqkoJT2E5dbtE+EeVHQqgKIDhlUTLOuqq4RQzE6ByPaf9mlS3JR4/e94Iz6Xf9s9SbPxq4vt7OlaDd/5IowVzmVZYrTkkvP57OJzdbpGMPZrSuNrfFwOx6P53lP4xVgFiRzpt/Q07SkHornzI7W06K00036O8oeplrGmNlKFZQSgQ7nplMDSALrymKOy/lUOfSwlCrq1jWjKseq2s6pjSMu3BdMT1uxDqjNSYyhTYpHfFh8nWqgVMEmeV4vrwgLeUo8eZYmmdm1qUexWmthdf42TZxM+fMyuQHJ0/Fk5TsFFCmlYIxEqKQHSRCTgapTNHAG4BpDICGxK8CU8syqSOOcy837Q8NjfwsRqXHjMgPicRxGee7gHx5QWzmHAavee253iAUy/AoxRhriDCqNhKgvhc6sJYoteZRirILslZWiVSevD468eaEYD+9a4bYOgX4VpQ9Q1G99x3ISyApJWt4ExZ9X8rQ3PCPMJMXFaj11KuVwifPyeZnlsp7R2mSS8JCW10rbO1V03hE5g6ZZb5z+zJwZ3ja0bzz7qhBqroPSs0qDHgrHq2xTBBWvODkdCDZWhyhrmKXMpbhpX4BcGlA44OekXZlfOJwOixN31ZtPHzttThY8vQrPHbjdYVidDxI7QjLLKVIY1fFfLH5llsAaonAyU6mrdkHxDRnGjqMGXXsD/EEcqoDGf/uW1Bp6HldN3W7SxYnxOUdxMgnPTppplsgd9xV7AVURyR54u0nXE2vSf+YstOQsfFjbDYpc83HONzj3vHWCRKlPRW1DLSSlkVNd3hvZbDoam+WZZzmJmS7GaXLivsjxXg8IVUWh1FcFxT2Gyn8cua6vtk4Q/gOkNC2qCTGW9gAAAABJRU5ErkJggg==","r":"iVBORw0KGgoAAAANSUhEUgAAAFcAAABACAMAAABYxRt9AAAAflBMVEUAAAAhHRxWWl5UMBifo6fboXBramo8QUlrRyzW2Nr5+fk5PUQnKCmSVy2ic01ZWlonKClJSUmdnp/os4W5u8GzhV5ve4J6hIi+wcaJiYnNeDv1zqumyu9rfIqutMk+PkQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABwHGXVAAAAIHRSTlMA+v3+/v4Q/v79Bf6f/v5bXpwe/v7+/v/+T/39DB4OMH/7h9QAAATFSURBVHjarViJltsgDDQCY4zBZ87dHv//lx0JnLPb0Hj1WrvJdodhkEayq+p1WPlTBTXg6qtvDh+UCvN3Io4I4YsYvHD/lmC8aZoHpTqhbId5sxzeWy9EgciXRqlzpabvI3wTs5IT3EbX+yHcoXZytX4T50k9hwAPZ2XfP0FrHzCJMjAWtFsyY34kC2BK/3wb11bzOTwpQOunDXzP96hE3c3nN3F9NU9fHNozrrcchSoMXyFKjBuO7SF17+JOhtn+/l3ZYleaH3F1v+9drrobjxgnQhQq7p+yV6l+qRExA9tsoESmRVBVgovF/7L7OoUgJ76jYlSDKM0QWz3Lu6/7mjm7FTcI6u5kTKfK9R0edaiJseulxweBBerO7AzLO0rllyXEE66QXhZXCcxI7UlQ4cxUrsONwk5nefnuahesF1goSx1R08E54Mo2hAL0cHtejukuezLM28nCBA0Mob4Zt2kU9mdoLOiYV1zt9iJAvbSguw+eE8wI24aTF7iM35ZlxaXg6qhBmFPss1X02Yd8aObQUSPAkBipxmLY/+htfR3r3jEuzmy/1G5kdXdCNxFW1ByhyeRLssJeFHZ9SlxG5jwD4RF0STUcgOXCwG0uoxtS9zFH3OIKjKAVt4OykBaw0qMy35f92k98GnQ8gBnOKyH3dc8T1UioMfy44d4E3EbcdDqPIdXMvzsG9miYGNOBnUHfpe7Vj+zPOFgSWDpISxXS1KoXheEZ9shulYAB7RxK5Ma5ZhEqNWrO4o527UtcZnM4MF9zILq2+/H2aDOqaljppjvszAtcnk1JdGBUllGnfkwC7LP7tibPFpIayuxaUgXpkLKeE5RVyBCB51X/w/PK2XY4MVgG7E6NL+2M1T0dwYXPRxOKWWshjOkCRTPpKDtoQJZhqcF/587xgi7Wly5zZL9iHcg4QRmrn1PEClGnHXSpPhSdTEtD6YTK6Snqwq9YCq0xW0d0I/4QhknEFesxLO/4ajK593WNDCPNC3zorPV+r1B5luVlYILJtxReGrtHKfR5q8wy50PUrpV77XqX+gZbcMOZg+Qt6HP6I7eIFVr+at3jEFW/j/BP4NqxkXzcMSwVOGXQcAMVV8q89bU6nOphnVhzkK58ENQEW+C+8SOy914ZU14BX7m+Aqx0YWVOQD1xuY9Fro5fj5icKI2oOheBjrKSXGapuRNTbVEvQ+mTECfBcScuiFQg8cvo1g1Ms6jQJmcKY+lAKeUF6yEBTON6vMoycDb6Uep8GoeqKh1e5bfZIU5IAM2VkDU4T2nieWsItn7IMzXYOq3rxamkARP99fis/B9r+LzhRmA5l3Xe/7a4PGJohl36vk/FsfFFhF9rDFfpwzqmb4ZtD/TD6grsBagPPjKe07TaJkPICcy3jxhdrjis9PN9IWxOM+YIbJEgG4TeRpjnSUdryuY2f6mIbTKIx+iHud25sAE2jetOyOZOnFnHDbhpUOroQtb7G+gNuDM/miUf5/FhWItP88df7z8eDxjg2gwLf0hmgWx2GNlow6u/gd22uezc37/umfzbL3cmxRMzKGJYdxpA14NDvr0L7OWVJEPEfv+5RABfUSMMaNySv6wnDN2gyYXqYm3y7du4/v5FzFQ9vjH5Ov4A9jwwzjb3g0UAAAAASUVORK5CYII=","l":"iVBORw0KGgoAAAANSUhEUgAAAFUAAABACAMAAABcMMtAAAAAflBMVEUAAAAhHh1VWFxSMRqeoabdonFnZ2hoSDA5PUT39vY8Q0zZ2duUWTBWV1crLCuecU4nKSmam5q0h2Jjd4bstYVMTEx5gom1ucHDekMvSl2et9y9wcZvfJLDdj6+wb+RkY8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABza0idAAAAIHRSTlMA+/3+/v4S/f4H/v3+XqL+YRz+//6e/v7+HBD+Hf/+SKFRz4cAAASgSURBVHjarZjpdpswEIUZSSAEQmLx0qRp+/5v2TsjQWwnjVXInJMf8bE/X65mk6uqKJqqms40VU1TfU9k0EhE03cxV/SZmArF4zh+B3AY8EcSbhD4N2AdhaoiUqCFxCZId+4YNdA5K11jOKpzkEMiau+wYXJHtA6UqPfQFFPV7H56CXjaqgfosD/Lwj+h56o6SEV0HwwYmqMOrAfWPRhbWpvNfZ0H+iqGnU3BPXBaam/S4VyUXRNbdf/14wP0JorbASnE3btzVcV5jpp2VpjqEQxee6pAbS1hP9haeFxkEH1vVGgE2jAzJmj85LTKUiCoy8rdpNYnRtb1J9DCVjBSt3JV5dhVRawTltbzAzOU+jq0qmtxYoKlxlVTMKpOMuMHsQUlG7i/Q1rbcRow1rC3SCyB6jkba9/PLDw1YFRGoTdDqlBJXU2vBodKIz57Ff1JkiDenlqBoeo3WG2rJDoSD+QELTiqh9aTQo5p/05tCtqzIsPgFAQT+vQxOs2gpoSNXm9ai8oVfeRiNrGKFnM1+GDD6WrnSjLW1t6vxTAU7REDkgoKVcJ2rYiFHA3gqc4R43vdlrQrJz2PTz+bQMgCodY3kQdDCZUT+k+Qd7PCpBbdSRniIWvrWWr2xMVgkXPynfjCr7HU87sExBbwUeH/AeKDpEesT4h69pgFhlPZcJI8a4PU9z9xTK30Y0w9ozJvzTptbU5+QNEjDL/jqQP9tVfUgdrxXGoNJA+3judppenCvvfmclEJ+xX4rPqroQ5l1ebxgbp6yzsrUi75Q1qT6aSYL+xS94QKsXxGYHZJLScC2tzrK3/M5UPXXLigLwtc7Qp2F2jtF2pFq6hVnfY6WefEZvyjjSjm1vMTartnWeBG1Zvr6mxaUrQFyNsmG8BZpuW0mLkw83nLUn3ygAOQiTffhB3c2Ur/y4rz+Zdsxi4Ae+UmIFJls47c/0WfR54qy6el/2cndMAayUJOA8WLzgvVeubmpM1MHnBlmZqwOs78Tc9ygGepYBfuKnxGL+Tr2teMUenRaRMaZ242vmR/HcRaSUWaXDMRNxUbt6dWyWimaxnlv+itoL+OuRbhAzbT6g+eE2JJy7MmxRI+nmZtWetL2VLJXAZLhWMTsL8spxgyV0qKVldPeIQfnl7L1tcRqSlgbqvVdm3RnqsKYtGw22W5dJZf0FR623DjKCWP2crYsGF93uC7DnNnYW8P3I4gNzQrVtZ4syC6PAoKN2L34ZKNl6aE1TxfUVsLCuX/qJ/nsku9dT7NfHBLhsKB5tiFc91hI/ec7cLhKrdfaiOdFd0AuT9bfbu8HhAbtGKulgk+a9qKNxyAToy0PH+xtvyofSpaXbS8fSFVpqvUq/WoKMlVffQ+PyUTrV37Fkp47TTugFb/sGJLZ6BjFoQY37EhqV1rbb8Fw81gmapzlqtlfQp7fyCR66LNafrGLqdOu3RojWEvFL+uoI9HK4JHN/EGkhr4FR1zZ3W52x+HOO23mz120stOse6sY9xmK0E4hlZMfvBWtK+6RhnSVmcwlNu5P0WfDNkJZarWN1qxyfrZa7uW7E6t9/nv7n+C+3LR+gtoZy7QCoILFQAAAABJRU5ErkJggg=="},"sister":{"d":"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","u":"iVBORw0KGgoAAAANSUhEUgAAAEgAAABACAMAAACOYMEYAAAAflBMVEUAAAD7+ffloCHp0qQoIBadZhVmSiLMtphOMQ+qkGndrVpoYVn6ymJqa2v3+PkjJCSuopNQU1eTd1YeICOYnKC5hzLEeh5LSkgvUG9keItyiZWerd8AAP+w4Pg0PUcmJ2oAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADTUdgHAAAAIHRSTlMA/v7+8P79/vv+/vr+Dgak/lf/ZR7//pgaIBESAgppEpU7IJAAAARlSURBVHjarZiJduo4DEBtS5a3xM7C0tLp6///5UhOKExfDKRndLrQQG61S45STemVyuOYsEoaxwP/rX4nc0lJr5LSea7w/dKXNOg7GVL5FWdEpX/IkPr9Oo046L+FDvsoWb1vcvSAl71+3uRo7f/scrPq0zZHO3z/XxTSgG97OIfSBKUx969bNrcsE9vyDhc1LRPQHl/Pqg3yOwouE7U4EKi8DvrwxjZAwdo9IDTGhi27pmkX6B8GmWii+295TDayQrtBXQfMCtMUQpj41zECX/0FyMTILIAuwiLR/ADVxMyPgvhZQcLqoOsMmM7I1wL6K2oPSDmR2LbqFJ2pvEUIr6BDL804Z1TtJpWSJ/Z2J2JYIwbGRSHvfVntKvSnEKEPpPpG+Y3knEscabENRJdOflqbKA0B135borfQ2Rjxs9WAywBQ402JY2ZFpolIZoELkOa1Fxv+EAfhhGVsWFYc6NsYWkTKQ24MECrogFxGYI7sPevPLZCGzXqtalbQx1l5rTvWkth/vqFRLgDbFVtpYc5sl40auIzgSKwSNiZeabWQCppmaaHsoUCMUT5KcvnNzo+gmxppN2CfVUbTRVLM8JIUuGVcxnZ/rGoRZ6Hi+ztL5oRWOB/b6agfgmAo6vOMxPl69MxpKaS+miBYvO34vqzQdrBUH+f6RpH0eW72WSfeBscpmcXdNkqqekuNLMKmVUN1XoABM+cjGYvIZnH5bYw6icedHT80CotekL6UBx09IuFBdrmtNDqnByBYbAsls2GciOM4NnvRNfgbueSC06u7c2GLbN1NNpev/vM7+JsarSAghdEglvdGA+nVmzfQBOkV5IJBCxbHRxtxgpW0VSahmu2sAQR29aOV74KBe3VDIQZJS2KOIlZozo/2h3cMnLDRbdbtEAYnfZfQdR6fbCJM4s5nTmEDxYnMGOexKvR8MaYg09DGH5UWzXGK4AgHHle+PF9vuFHzmDnW2co3LiKjhLcsLogEGrwU/JMVsEjNOj7LkJPufhXxMg8xTOxv6HywzzSafddxeNg84u9wFzCLNPBFHlDR+kj41EPsIP6YW9W6jiNfNXT8H6RDn8DPj48OHyVZgojTokad0VWMrcnI5hHWtD488pEEn8eEp7X9B3OT43opJbJ+wsfrttQax5dHtb4D1elvwrq/eTQe5/HZYvzGRYSK1lYS4h1oTdCJOhyfH68uaooT+fWm4QbqVo0AvMX+hV09n9lD154kGtXVAW4gnZ7noqyPAkN3p1HkHTLGb5COTr10IOV8LUlfQxTjzbTrJR1e2mtzPfS5utByBxMGSKF0NtTVVpbvNL+0HPMZqw4LAVVnL+vjRAJilRj02gmpR5++GxmcliXUwLT6KFB6Hv7rolXrqtZaZGNEQMp4OWcjR//rteOaoFAeinDbSdMgIh1EHpHwG4es3l499cvJIM/eSqFjmeeCvu4L+KunLAw6nSzJsfqCVl4voJ0PNfr+sDzwKfzs4XJeXqt2yf8LhIQ0CNoifQAAAAAASUVORK5CYII=","r":"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","l":"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"}};for(const k in D){CHARIMG[k]={};for(const dd in D[k]){const im=new Image();im.src="data:image/png;base64,"+D[k][dd];CHARIMG[k][dd]=im;}}})();
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
                             if(m.skills===undefined)m.skills=[]; if(m.spells===undefined)m.spells=[];
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
// 仲間i(0始まり)の立ち位置。足跡が足りない時は勇者の後方へ等間隔に補完（必ず後ろに並ぶ）
function followerSpot(i){
  const need=(i+1)*FOLLOW_GAP;
  if(trail.length>need) return trail[need];
  const base = trail.length ? trail[trail.length-1] : {x:player.x,y:player.y,dir:player.dir};
  const back={right:[-1,0],left:[1,0],up:[0,1],down:[0,-1]}[base.dir] || [0,1];
  const miss=(need-(trail.length-1))*3;   // 足りない足跡ぶんだけ進行方向の逆へ(1足跡=3px)
  return {x:base.x+back[0]*miss, y:base.y+back[1]*miss, dir:base.dir};
}
// 勇者＋仲間を足元のyで奥→手前に並べて描画（仲間は勇者の足跡を追従）
function drawParty(){
  const list=[{key:"hero", dir:player.dir, x:player.x, y:player.y, walk:player.walk, hero:true}];
  const fol=party.slice(1);
  for(let i=0;i<fol.length;i++){
    const t=followerSpot(i);
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
