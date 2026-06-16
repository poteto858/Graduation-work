# Graduation-work — Arduino UNO R4 WiFi で動くブラウザゲーム

組み込みエンジニア研修の**卒業制作**。
Arduino UNO R4 WiFi を Web サーバーにして、PC・スマホのブラウザから遊べるゲームを開発しました。

このリポジトリには **2つの作品** が入っています。

| フォルダ | 作品 | 内容 |
|----------|------|------|
| [`MiniGame_Phase12_RPG/`](MiniGame_Phase12_RPG/) | **迷路ゲーム** | 基礎を学ぶために最初に作った迷路ゲーム（完成） |
| [`RPG_Quest/`](RPG_Quest/) | **RPG_Quest** | 迷路の仕組みを土台に発展させたドラクエ風RPG（開発中・メイン作品） |

---

## 共通の仕組み（このプロジェクトの肝）

```
┌──────────────┐   HTTPリクエスト    ┌────────────────────────────┐
│  ブラウザ     │ ───────────────▶  │ Arduino UNO R4 WiFi          │
│ (ゲーム本体)  │                    │  ・Wi-Fi接続 / Webサーバー    │
│  JavaScript  │ ◀───────────────  │  ・HTMLを丸ごと配信           │
└──────────────┘   HTML / JSON      │  ・センサー値や効果音を処理    │
                                     └────────────────────────────┘
```

- **ゲームのロジック・描画はすべてブラウザ側の JavaScript**。
- **Arduino は「Webサーバー」**として、HTMLを配り、ジョイスティックの値を返し、効果音やLEDを鳴らす。
- だから重い描画はPC/スマホ側で動き、Arduinoの非力なCPUを使わずに済む。

詳しい仕組み・操作・配線は、各フォルダの README を参照してください。

---

## 実機ハードウェア（RPG_Quest で対応）

| 部品 | 役割 | 接続 |
|------|------|------|
| ジョイスティックモジュール | 移動・メニュー操作 | VRx→A0 / VRy→A1 / SW→D2 |
| パッシブブザー ＋ ボリューム | 効果音と音量調整 | 信号→D9 / ブザー→ボリューム→GND（直列） |
| WS2812 RGBテープ | イベントに応じて発光 | DIN→D6 |

> ハードが無くても、キーボード（WASD/矢印）・スマホタップで全機能が動作します。

---

## 開発環境

- ハードウェア: Arduino UNO R4 WiFi / Wi-Fiルーター（2.4GHz帯）/ PC・スマホ
- ソフトウェア: Arduino IDE / HTML・CSS・JavaScript（Canvas API）
- ライブラリ: `WiFiS3`（標準）/ `Adafruit NeoPixel`（RPG_Questのみ）

## 共通セットアップ

1. `arduino_secrets.h.example` を **`arduino_secrets.h`** にコピーし、自分のWi-Fi（2.4GHz帯）のSSID・パスワードを記入。
2. 遊びたい作品のフォルダ（`MiniGame_Phase12_RPG/` または `RPG_Quest/`）を Arduino IDE で開き、`arduino_secrets.h` を同じフォルダに置く。
3. 書き込み後、シリアルモニタ（9600bps）に表示される IP に `http://<IP>/`（httpsではなくhttp）でアクセス。

> `arduino_secrets.h`（実際のWi-Fi情報）は `.gitignore` で除外され、GitHubには公開されません。
