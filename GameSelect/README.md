# GameSelect（ゲーム選択ランチャー・統合）

Arduino 1台で **迷路ゲーム（MazeGame）** と **RPG_Quest** の2作を配信する統合スケッチ。
起動してアクセスすると、まず **ゲーム選択画面** が出て、遊ぶ作品を選べます。

<p align="center">
  <img src="報告画像/launcher.png" width="460" alt="ゲーム選択画面">
</p>

### 選べる2作（プレイ画面）
<p>
  <img src="../MazeGame/報告画像/maze_play.png" width="45%" alt="迷路ゲーム">
  <img src="../RPG_Quest/報告画像/fig3_battle.png" width="45%" alt="RPG_Quest">
</p>

## プレイ動画

ゲーム選択 → 迷路プレイ → RPGフィールド探索・バトル、の一連の流れです。

#### ① ゲーム選択

https://github.com/user-attachments/assets/f1a50624-f9f0-4f63-a5b9-811000a9c9f4

#### ② 迷路プレイ

https://github.com/user-attachments/assets/8dd8d202-f109-4340-bb27-46e730ab1825

#### ③ RPG フィールド探索・バトル

https://github.com/user-attachments/assets/cd854839-ded3-4380-9915-2d6cd00a5f8b

## しくみ（URLパスで出し分け）
Arduino が「どのURLが来たか」で配信を変えます（2つの大きな JS を混ぜないので名前衝突がない）。

| パス | 配信 |
|------|------|
| `/`（起動画面） | ゲーム選択（`launcher.h`） |
| `/maze` | 迷路（`maze_gz.h` … **MazeGame由来**） |
| `/rpg` | RPG（`webpage_gz.h` … **RPG_Quest由来**） |
| `/state` `/fx` `/led` `/maps.json` | 共通（迷路は `/state` のみ使用） |

各ゲームは選択画面から開く独立した画面として動作します。どちらの画面にも「← ゲーム選択」ボタンがあり、選択画面へ戻れます（迷路は操作パネル内、RPGは画面左上に常時表示）。

## 開発の流れ（重要）
- **各ゲームの中身は `MazeGame/` と `RPG_Quest/` で開発**します。GameSelect は“集めて配信するだけ”。
- 各ゲームを更新したら `_sync.py`（開発用ローカルスクリプト・リポジトリ未同梱。単純コピーなので手動コピーでも可）で最新の生成物を取り込みます：
  - `MazeGame/maze_gz.h` → `GameSelect/maze_gz.h`
  - `RPG_Quest/webpage_gz.h` → `GameSelect/webpage_gz.h`
  - `RPG_Quest/maps.h` → `GameSelect/maps.h`
- そのあと GameSelect を書き込み直します（取り込みを忘れると古い版が焼かれます）。

## 操作・配線
RPG_Quest と同じ（ジョイスティック VRx→A0 / VRy→A1 / SW→D2、パッシブブザー→D9、WS2812→D6、**QR表示用OLED**）。

- **OLED（SSD1306 128×64）**：接続用QRを表示（外で遊ぶ用）。`SDA→SDA / SCL→SCL`（I2C・アドレス0x3C）、`VCC→3.3V`、`GND→GND`。要ライブラリ：**Adafruit SSD1306 / Adafruit GFX / QRCode**。
  - AP時：Wi-Fi参加QR（`GameSelect`）を表示／STA時：`http://<IP>/?go` のQRを表示（キャッシュ回避付き）。
- **キャプティブポータル**：APにつなぐと**自動でゲーム選択画面が開くことが多い**（簡易DNSで全ドメインを 192.168.4.1 に誘導）。端末依存で開かない場合は `192.168.4.1/?go` を開けば確実。

> ハードが無くてもキーボード（WASD/矢印）・スマホ（**仮想十字パッド**／タップ）で全機能が動作します。

## ビルド
1. （任意）家のWi-Fiで使う場合のみ `arduino_secrets.h` をこのフォルダに置き（2.4GHz帯）、あわせて `GameSelect.ino` の `#define PORTABLE_DEMO 1` を **`0`** に変更。**無くてもビルドできます**＝初期状態のデモモード（`PORTABLE_DEMO 1`）はAP起動なのでWi-Fi情報が不要。
2. ライブラリ **Adafruit NeoPixel** / **Adafruit SSD1306**（依存の Adafruit GFX も）/ **QRCode**（Richard Moore 作）を導入（実機ハード未接続でもコンパイルに必要）。
3. `GameSelect.ino` ＋ `launcher.h` / `maze_gz.h` / `webpage_gz.h` / `maps.h` を同じフォルダに置いて書き込み。
4. シリアルモニタ（9600bps）の IP に `http://<IP>/` でアクセス → 選択画面。

> **外で遊ぶ（モバイルバッテリー＋アクセスポイント）と接続用QR** はリポジトリ直下の README を参照。
> このスケッチのAP名は `GameSelect`（**パスワード不要のオープンAP**＝R4でも確実に接続でき、キャプティブで自動的にゲームが開く）。
