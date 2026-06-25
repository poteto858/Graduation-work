# Graduation-work — Arduino UNO R4 WiFi で動くブラウザゲーム

組み込みエンジニア研修の**卒業制作**。
Arduino UNO R4 WiFi を Web サーバーにして、PC・スマホのブラウザから遊べるゲームを開発しました。

このリポジトリには **2つの作品** と、それらを **1台にまとめて起動時に選べる統合版** が入っています。

| フォルダ | 中身 | 説明 |
|----------|------|------|
| [`MazeGame/`](MazeGame/) | **迷路ゲーム** | 基礎を学ぶために最初に作った迷路ゲーム（完成） |
| [`RPG_Quest/`](RPG_Quest/) | **RPG_Quest** | 迷路の仕組みを土台に発展させたドラクエ風RPG（メイン作品） |
| [`GameSelect/`](GameSelect/) | **統合（ランチャー）** | 起動時に迷路 / RPG を選んで遊べる統合スケッチ |

> 各ゲームの**中身は `MazeGame/` と `RPG_Quest/` で開発**し、`GameSelect/` はその生成物を集めて配信します（更新の置き場所を分離）。

---

## 共通の仕組み（このプロジェクトの肝）

```mermaid
flowchart LR
    B["ブラウザ（ゲーム本体）<br/>JavaScript で描画・ロジック"]
    A["Arduino UNO R4 WiFi<br/>・Wi-Fi接続 / Webサーバー<br/>・HTMLを丸ごと配信<br/>・センサー値や効果音を処理"]
    B -->|"HTTPリクエスト"| A
    A -->|"HTML / JSON"| B
```

- **ゲームのロジック・描画はすべてブラウザ側の JavaScript**。
- **Arduino は「Webサーバー」**として、HTMLを配り、ジョイスティックの値を返し、効果音やLEDを鳴らす。
- だから重い描画はPC/スマホ側で動き、Arduinoの非力なCPUを使わずに済む。
- HTMLは **gzip圧縮のまま配信**（ブラウザが自動展開＝マイコンは展開しない）してフラッシュを節約。

詳しい仕組み・操作・配線は、各フォルダの README を参照してください。

---

## 実機ハードウェア

| 部品 | 役割 | 接続 | 使う作品 |
|------|------|------|---------|
| ジョイスティックモジュール | 移動・メニュー操作 | VRx→A0 / VRy→A1 / SW→D2 | 全作品 |
| パッシブブザー ＋ ボリューム | 効果音と音量調整 | 信号→D9 / ブザー→ボリューム→GND（直列） | RPG_Quest / GameSelect |
| WS2812 RGBテープ | イベントに応じて発光 | DIN→D6 | RPG_Quest / GameSelect |
| OLED（SSD1306 128×64） | 接続用QRを表示（外で遊ぶ用） | SDA→SDA / SCL→SCL（I2C・0x3C）, VCC→3.3V, GND→GND | GameSelect |

> 迷路ゲームはジョイスティックのみ（効果音はブラウザのWeb Audioで再生）。
> OLEDは要ライブラリ（Adafruit SSD1306 / Adafruit GFX / QRCode）。
> ハードが無くても、キーボード（WASD/矢印）・スマホタップで全機能が動作します。

---

## 開発環境

- ハードウェア: Arduino UNO R4 WiFi / Wi-Fiルーター（2.4GHz帯）/ PC・スマホ
- ソフトウェア: Arduino IDE / HTML・CSS・JavaScript（Canvas API）
- ライブラリ: `WiFiS3`（標準）/ `Adafruit NeoPixel`（RPG_Quest・GameSelect）

## 共通セットアップ

1. `arduino_secrets.h.example` を **`arduino_secrets.h`** にコピーし、自分のWi-Fi（2.4GHz帯）のSSID・パスワードを記入。
2. 遊びたいフォルダ（`MazeGame/` / `RPG_Quest/` / `GameSelect/`）を Arduino IDE で開き、`arduino_secrets.h` を同じフォルダに置いて書き込み。
3. シリアルモニタ（9600bps）に表示される IP に `http://<IP>/`（httpsではなくhttp）でアクセス。

> `arduino_secrets.h`（実際のWi-Fi情報）は `.gitignore` で除外され、GitHubには公開されません。

---

## 外で遊ぶ（持ち出しプレイ）

家のWi-Fiが無い屋外でも、**Arduino自身がWi-Fiアクセスポイント(AP)になる**ので単体で遊べます。

- **電源**：USBモバイルバッテリーをUSB-Cに挿すだけ（別の電源モジュール不要）。
- **接続**：起動時にルーターが見つからなければ**自動でAPモード**（ボタン押し起動で即AP）。
- **キャプティブポータル**：①のWi-Fiにつなぐと、ホテルWi-Fiのログイン画面のように**自動でゲーム選択画面が開きます**（②のURLを開く操作は不要。開かない時だけ②）。
- **OLED表示**：接続用QRは**本体のOLEDにも表示**されるので、印刷した紙が無くてもその場で読み取れます。
- 各スケッチのAP名はフォルダ名（`MazeGame` / `RPG_Quest` / `GameSelect`）。下のQRは統合版(GameSelect)用。

<p align="center">
  <img src="GameSelect/qr_outdoor.png" width="520" alt="外プレイ用QR：①Wi-Fiに接続すると自動でゲームが開く">
</p>

| 手順 | 内容 |
|------|------|
| ① | スマホで左のQR（またはOLEDのQR）を読み、Wi-Fi **`GameSelect`**（**パスワード不要**）に接続 → **自動でゲーム選択画面が開く** |
| ②（保険） | 自動で開かない場合は右のQRを読み、`http://192.168.4.1/` を開く |

> iPhoneは接続時に「インターネット未接続」警告が出ます。**「このまま接続」**を選べばOK（選ばないと切れます）。
