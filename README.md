# AVR_MusicBox
This is an open source program that implemants a music box and PCM Player on AVR microcontroller.
This program can play 3 parted score, or mono/8bit/8KHz sampled PCM.
The score or the PCM data must be stored in EEPROM that connected by I2C.  
Details are written in Japanese.

# タイトル
AVR マイコンによるオルゴール演奏・PCM再生プログラムおよびその関連ツール

# 作者
(c) 2017 大塚ヒロ <otsuk atmark purple.plala.or.jp>  
ライセンスについては [LICENSE.md](/LICENSE.md) を参照。

# 概要
AVRマイコンに本プログラムを書き込み、適切な回路を組み立てることで、最大3パートまでのオルゴール楽譜、またはモノラルPCM音源を再生する事が可能。オルゴール楽譜とPCM音源の再生は排他実行となる（同時再生は不可）。  
オルゴール楽譜については、MML風のテキストで記述し、本プログラムに添付の変換ツールでバイナリ化する。なお、プログラム本体への組み込みも可。  
PCM音源再生については、波形編集用のソフトウェア等により、モノラル8bit/8KHzのWAVファイルとして作成する。  
楽譜データおよび WAVファイルは、HEXファイルに変換し、添付の書き込みツール等で I2C 接続の EEPROM に書き込む必要がある。

# 使用条件
## 必要ソフトウェア（開発用）
* AVRマイコン用コンパイラ  
例）Atmel Studio 7(http://www.atmel.com/ja/jp/tools/atmelstudio.aspx)
* WAVファイル(PCMモノラル/8bit/8KHz)を作成するためのソフト  
例）spwave(http://www-ie.meijo-u.ac.jp/~banno/spLibs/spwave/index-j.html)
* 楽譜データやWAVファイルをHEXファイルに変換するためのソフト  
例）BIN2HEXGUI(https://synapse.kyoto/tips/LPC1114/page001.html)
* EEPROM に HEXファイルを書き込むためのソフト  
例）TeraTerm(https://ttssh2.osdn.jp/)
* MML変換ツールコンパイル用 C++ 開発環境  
例）Eclipse + CDT(http://mergedoc.osdn.jp/)

## 必要ハードウェア（開発用）
* 前述のソフトウェアが動作するパソコン  
例）Windows 7,8,10 の PCなど
* AVRマイコンにプログラムを書き込むためのハード  
例）AVRISP mk2等
* EEPROM にHEXファイルを書き込むためのハード  
例）Arduino および自作回路等

## 必要部品等（実装用）
* AVRマイコン（※確認済みのマイコンは変更履歴に記載）
* オーディオアンプ（自作回路、単体IC等）
* I2C接続のEEPROM（1MHz の高速アクセスが可能なもの）
* スピーカー（アンプの性能に合わせた負荷抵抗のもの）
* 抵抗器、コンデンサ、電池等

# 実装例
[CIRCUITS.txt](/CIRCUITS.txt) を参照

# 簡易な実行方法
## EEPROM の準備
[sample_pcm/](/sample_pcm/) および [sample_mml/](/sample_mml/) ディレクトリに格納されている hex ファイルを、すべて EEPROM に書き込む。書き込み順が再生の順番になる。添付のツール([eepromwriter.ino](/tools/eepromwriter.ino))を使った EEPROM への書き込み方法は、[README_EEPROM_RW.txt](/README_EEPROM_RW.txt) を参照。  
参考：http://studiohiro.cocolog-nifty.com/blog/2017/01/i2c-eeprom-fdf9.html

## ATTiny85の準備
[sample_elf/](/sample_elf/) ディレクトリに格納されている ATT_85.hex を ATTiny85 に書き込む。  
また、PLL 16MHz で動作するように Fuse を設定する。

## 回路の実装
実装例を参考に回路を組み、電源ON。  
BTN1 を押すと、EEPROM に保存したサンプルPCMおよびサンプル楽譜が順番に再生され、BTN0 を押すと逆順に再生される。BTN0 と BTN1 を同時に押すと、内蔵の楽譜が順に再生される。  
参考：https://youtu.be/nrbSN_aUKCQ

# 詳細な使用方法
## PCM音源の作成
### 音源の録音
Windows のサウンドレコーダー等を用いて録音する。

### 録音形式の変換
前述の spwave 等を用いて、PCMモノラル/8bit/8KHz サンプリングの WAVファイルに変換する。なお、当方の環境では、spwave で保存時に形式 "Microsoft PCM(libsndfile)" ではエラーとなった。"Microsoft PCM" を選択すれば正常に保存できた。

### HEXファイルへの変換
前述の BIN2HEXGUI 等を用いて、WAVファイルを HEXファイルに変換する。なお、BIN2HEXGUI では、変換元ファイルの拡張子が wav のままでは選択できない。いったん bin に変更してから処理する必要がある。

### EEPROM への書き込み
Arduino 等から I2C 接続を行い、EEPROM に HEX ファイルを書き込む。添付のツールを使った EEPROM への書き込み方法は、[README_EEPROM_RW.txt](/README_EEPROM_RW.txt) を参照。

## 楽譜データの作成
### MMLファイルの記述
テキストエディタ等で、MMLファイルを作成する。MMLの仕様は、[README_MML.txt](/README_MML.txt) を参照。

### MML2BIN ツールのコンパイル
Eclipse 等を用いて、[MML2BIN.cpp](/tools/MML2BIN.cpp) をコンパイルする。アプリケーションの種類としてはコンソール用アプリケーションとなる。リンクオプションには -static をつけた方がよい。

### MMLファイルの変換
コマンドプロンプトを起動し、以下のコマンドを実行する。  
`MML2BIN.EXE MMLファイル名`  
EXEファイルにMMLファイルをドラッグ＆ドロップしても変換可能だが、エラーや警告が出力された場合に確認できないので注意が必要。

### HEXファイルへの変換
前述の BIN2HEXGUI 等を用いて、HEXファイルに変換する。

### EEPROM への書き込み
Arduino 等から I2C 接続を行い、EEPROM に HEX ファイルを書き込む。添付のツールを使った EEPROM への書き込み方法は、[README_EEPROM_RW.txt](/README_EEPROM_RW.txt) を参照。

## 制御プログラムのカスタマイズ
### プロジェクトの準備
Atmel Studio で新規のプロジェクトを作成し、以下の設定を行う
* [src/](/src/) と [src_mml/](/src_mml/) ディレクトリに格納されている全ファイルを追加する。  
⇒[Project]->[Add Existing Item...]
* ターゲットマイコンに ATTiny85 を選択する。  
⇒[Project]->[Properties]->[Device]->[Change Device...]
* コンパイラの設定で、-D オプションに F_CPU=16000000UL を追加する。  
⇒[Project]->[Properties]->[Toolchain]->[AVR/GNU C Compiler]->[Symbols]

### 設定のカスタマイズ
設定用のヘッダファイル([config.h](/src/config.h))を編集してカスタマイズを行う。基本はそのままで問題ない。詳細はヘッダ内のコメントを参照。ATTiny85 ではなく、多ピンの ATTiny861 を使う場合は変更が必要。

### 動作のカスタマイズ
メイン処理を編集し、動作をカスタマイズする。
* BTN0 や BTN1 に対する動作内容
* オルゴール楽譜や PCM 音源の再生順等

その他、たとえば、 BTN0 と BTN1 を使った低速シリアル通信などを実装すれば、2線で複数の楽譜・音源の再生を制御できる・・・はず。

### 楽譜データの内蔵
EEPROM がなくても動作させたい場合、プログラムに楽譜情報を内蔵することが可能。[src_mml/](/src_mml/) ディレクトリに格納されたファイルを参考に、楽譜情報をプログラムする。
* [MML_HOMusic_C3.h](/src_mml/MML_HOMusic_C3.h)  
一応、3パート再生のサンプル音楽。
* [MML_Test.h](/src_mml/MML_Test.h)  
全音階の再生、音色変化などのサンプル。音色は劇的には変わらないので、参考程度。

### マイコンへの書き込み
AVRISP 等を用いて、AVRマイコンにプログラムを書き込む。

# 謝辞
本ドキュメントで紹介している各フリーソフト作者の皆様に感謝致します。また、本プログラムの試作段階で参考にさせていただいた、AVRマイコンや電子回路に関する情報を公開されている先輩方に感謝致します。

# 変更履歴

最新確認済みマイコン：ATTiny85, ATTiny861

* 2017/02/18  以下の更新を行った  
 1. MMLの仕様に、サブパート定義とループの機能を追加
 2. 楽譜の波形タイプに鋸波を追加

* 2017/02/17  以下の更新を行った  
 1. 楽譜情報も EEPROMから読み込めるように構成変更
 2. EEPROM の書き込みツールとMML変換ツールを同梱

* 2017/02/01  新規公開  
