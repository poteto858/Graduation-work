# Graduation-work — Arduino UNO R4 WiFi Web RPG

組み込みエンジニア研修の卒業制作。Arduino UNO R4 WiFi を Web サーバーにし、ブラウザで遊べる迷路RPGを開発しています。

## 構成

- `arduinoIDE/MiniGame_Phase12_RPG.ino` … ゲーム本体（最新版）
- `arduinoIDE/arduino_secrets.h.example` … WiFi情報テンプレート

## 仕組み

- ゲームのロジックは **ブラウザ側の JavaScript**。Arduino は HTML を配信し、`/state` でジョイスティックの値を JSON で返すだけ。
- ジョイスティックが無くても、キーボード（WASD/矢印）・スマホタップで全機能動作。

## 主な機能

- 迷路3ステージ（カメラスクロール対応、全ステージクリア可能を検証済み）
- コイン収集（全取得でゴール解放）／ 巡回する敵
- タイマー・ベストタイム保存
- ジョイスティック操作対応

## セットアップ

1. `arduinoIDE/arduino_secrets.h.example` を `arduino_secrets.h` にコピー
2. 自分の WiFi（2.4GHz帯）の SSID・パスワードを記入
3. Arduino IDE で書き込み、シリアルモニタ（9600bps）に表示される IP に `http://<IP>/` でアクセス

> `arduino_secrets.h` は `.gitignore` で除外され、GitHubには公開されません。
