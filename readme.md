# waveshare e-paper5.83inch(b) の描画ファームウェア

PCからUSBシリアルを使用してjsonを送信し、その内容に従ってe-paperに図形や文字を描画する｡

描画制御に関しては、[waveshareの公開しているコード](https://github.com/waveshare/Pico_ePaper_Code)を取り込んで使用しています。

## フォント

フォントは事前にコンバートして、Picoのフラッシュメモリに書き込みます｡
[自作のコンバートツール](https://github.com/zsuzuki/bdfconv2)を使ってコンバートしたものです。
任意のフォントを用意してください。

## 送信

BOOTボタンを押しながら電源投入、もしくはリセットボタンを押してPicoをファイル転送モードにした後、
writedata.shを実行します｡

```shell
sh ./writedata.sh
sh ./writedata.sh -t # フォントファイルも転送
```

転送後は自動で再起動して実行されます｡

## ライセンス

このプログラムは GPL3 ライセンスを適用します｡
