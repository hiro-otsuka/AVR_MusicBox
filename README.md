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
AVRマイコンに本プログラムを書き込み、適切な回路を実装することで、
最大3パートまでのオルゴール楽譜、またはモノラルPCM音源を再生する事が可能。
オルゴール楽譜とPCM音源の再生は排他実行となる（同時再生は不可）。  
動作モード（ボタン入力への応答方式等）やピンのI/O設定は、AVRマイコン内蔵の EEPROM に書き込む。  
また、オルゴール楽譜およびPCM音源のファイルは、 I2C 接続の EEPROM に書き込む。
書き込み用の回路（ハードウェア）は別途用意する必要があるが、ソフトウェアは添付のツールが使用できる。  
オルゴール楽譜については、MML風のテキストで記述し、本プログラムに添付のMML変換ツールでバイナリ化する。  
PCM音源については、波形編集用のソフトウェア等により、モノラル8bit/8KHzのWAVファイルとして作成する。PCMとADPCMが再生可能。  
再生のためのパラメータ（再生範囲等）も、パラメータファイルとして I2C 接続の EEPROM に格納可能。
テキストで記述し、本プログラムに添付のPAR変換ツールでバイナリ化が可能。  

# 成果物
本プロジェクトに含まれる成果物は以下の通り。
* [circuits/](/circuits/) ：参考回路図
* [elf/](/elf/) ：AVRマイコン用コンパイル済みファームウェア
* [sample_mml/](/sample_mml/) ：サンプルオルゴール楽譜ファイル
* [sample_mode/](/sample_mode/) ：AVR内蔵EEPROM用のサンプル動作モード設定ファイル
* [sample_par/](/sample_par/) ：I2C接続EEPROM用のサンプルパラメータファイル
* [sample_pcm/](/sample_pcm/) ：サンプルPCM音源ファイル
* [src/](/src/) ：AVR_MusicBox のソースファイル
* [src_mml/](/src_mml/) ：AVR_MusicBox にオルゴール楽譜を内蔵する際のソースファイル
* [src_pui/](/src_pui/) ：AVR_MusicBox を外部から操作するソースファイル
* [tools/](/tools/) ：AVR_MB_Writer のためのソフトウェアや MML・PAR変換ツール（ソースファイルと実行ファイル）

# 使用条件
## 必要ソフトウェア（開発用）
* WAVファイル(PCMモノラル/8bit/8KHz)を作成するためのソフト  
例）spwave(http://www-ie.meijo-u.ac.jp/~banno/spLibs/spwave/index-j.html)
* オルゴール楽譜やPCM音源のファイルをHEXファイルに変換するためのソフト  
例）BIN2HEXGUI(https://synapse.kyoto/tips/LPC1114/page001.html)  
 ※[AVR_MB_Writer](https://github.com/hiro-otsuka/AVR_MB_Writer) を使用する場合は不要
* EEPROM に HEXファイルを書き込むためのソフト  
例）TeraTerm(https://ttssh2.osdn.jp/)  
 ※[AVR_MB_Writer](https://github.com/hiro-otsuka/AVR_MB_Writer) を使用する場合は不要
* AVRマイコン用コンパイラ  
例）Atmel Studio 7(http://www.atmel.com/ja/jp/tools/atmelstudio.aspx)  
 ※カスタマイズせず、[elf/](/elf/) のファームウェアをそのまま使用する場合は不要
* MML・PAR変換ツールコンパイル用 C++ 開発環境  
例）Eclipse + CDT(http://mergedoc.osdn.jp/)  
 ※カスタマイズせず、[tools/](/tools/) の実行ファイルをそのまま使用する場合は不要

## 必要ハードウェア（開発用）
* 前述のソフトウェアが動作するパソコン  
例）Windows 7,8,10 の PCなど
* I2C接続EEPROM に各ファイルを書き込むためのハード  
例）Arduino およびAVR_MB_Writer用ハードウェア（[参考回路](/circuits/CIRCUITS_Writer.jpg)） 等
* AVRマイコンにプログラムやAVR内蔵EEPROMの設定を書き込むためのハード  
例）AVRISP mk2等  
 ※[AVR_MB_Writer](https://github.com/hiro-otsuka/AVR_MB_Writer) およびAVR_MB_Writer用ハードウェア（[参考回路](/circuits/CIRCUITS_Writer.jpg)）を使用する場合は不要

# 実装例
## 回路図
AVR_MusicBox および AVR_MB_Writer用ハードウェア の回路図は[circuits/](/circuits/) を参照

## 必要部品等
* AVRマイコン（※確認済みのマイコンは変更履歴に記載）
* オーディオアンプ（自作回路、単体IC等）
* I2C接続EEPROM（1MHz の高速アクセスが可能なもの）
* スピーカー（アンプの性能に合わせた負荷抵抗のもの）
* 抵抗器、コンデンサ、電池等


# 基本的な使用方法
## AVR_MB_Writer用ハードウェア の作製
Arduino にライタ用ファームウェア（[tools/AVR_MB_Writer.ino](/tools/AVR_MB_Writer.ino)）を書き込む。  
Arduino から ATMega328 を取り外し、AVR_MB_Writer用ハードウェア（[参考回路](/circuits/CIRCUITS_Writer.jpg)）を作成する。  
なお、12V を供給可能な電源（ACアダプタ等）が必要。また、参考回路上の CONN-USB の箇所には、任意の USB-シリアル変換ボード等を接続すること。  


## AVR_MusicBox ハードウェアの作製
AVR_MusicBox回路図（[参考回路](/circuits/CIRCUITS_MB.jpg)）を参考に、再生用の回路を実装する。  
オンボードでファームウェアの書き込みを行う場合、AVRマイコンと EEPROM 等との間にジャンパ（J1）が必須。
ただし、DIP版のAVRマイコンや EEPROM で実装し、取り外して書き込みを行う場合にはジャンパは不要となるので、以降の手順は読み替えること。  

## AVR_MusicBox の設定
AVR_MB_Writer用ハードウェアを USB でパソコンに接続し、[AVR_MB_Writer](https://github.com/hiro-otsuka/AVR_MB_Writer) を起動する。  
AVR_MusicBox のジャンパ設定でAVRマイコンと EEPROM 等との間を切断状態にし、 AVR_MB_Writer用ハードウェア に接続する。
AVR_MB_Writerのマニュアルを参照し、以下の操作を行う。
 * Fuse の初期設定(Fuse Initialize)
 * RSTピンの無効化(RST Disable)
 * [elf/](/elf/)のファームウェアを書き込む
 * [sample_mml/](/sample_mml/)および[sample_pcm/](/sample_pcm/)データを書き込む

## 実行
AVR_MusicBox のジャンパ設定でAVRマイコンと EEPROM 等との間を接続状態にし、 電源をON。  
ボタン0 を押すたびに[sample_pcm/](/sample_pcm/)のファイルが順に再生され、ボタン1 を押すと[sample_mml/](/sample_mml/)のファイルが再生される。
ボタン0とボタン1を同時に押すと、最初に書き込んだファイル（0番目）が再生される。  
参考：https://youtu.be/nrbSN_aUKCQ

# 詳細な使用方法
## 動作のカスタマイズ
AVR内蔵EEPROMで、動作モード（ボタン入力への応答方式等）やピンのI/O設定が変更可能。
EEPROM の1バイト目が動作モード、2バイト目以降がピンのI/O設定となる。  
各動作モードのサンプルバイナリファイルを[sample_mode/](/sample_mode/)に格納しているのでご参考に。  
### 動作モードの指定（1バイト目）
1バイト目で、動作モードの指定を行う。  
* 0x00 ... 分離再生モード(ボタン0 = PCM音源、ボタン1 = オルゴール楽譜)
* 0x01 ... 全再生モード(ボタン0 = 戻る、ボタン1 = 進む)
* 0x02 ... シリアル指定モード(ボタン0 押下中の ボタン1 押下回数で指定)
* 0x03 ... ボタン番号指定モード(ボタン0～ボタンX で直接指定)
* 0x04 ... パラレル指定モード(ボタン0～ボタンX で２進数指定)
* 0xFF ... 分離再生モード（0x00）と同じ ※AVR内蔵EEPROM初期状態  
※0x00,0x01,0x02は小ピンAVR用、0x03,0x04は多ピンAVR用を想定

### ピンのI/O設定の指定（2バイト目以降）
動作モードの指定に続けて、各ピンの設定を1バイト分のビット指定で行う。2バイト目がピン0、3バイト目がピン1となる。各ビットの意味は以下の通り。  
* bit7(MSB) ～ bit3 ... 未定義のため 1 の指定を推奨
* bit2      ... 割り込み設定：  1=変化割り込みON、0=変化割り込みOFF
* bit1      ... プルアップ設定：1=ピン内部プルアップOFF、0=ピン内部プルアップON
* bit0(LSB) ... 入出力設定：    1=入力(ボタン)として使用、0=出力(LED等を接続)として使用  
※AVR内蔵EEPROMが初期値(0xFF)の場合、全ピンが入力(ボタン)・変化割り込みON・ピン内部プルアップOFFの設定となる。  
※入出力設定を出力(LED等を接続)とした場合、割り込み設定はOFFにすること  
※入出力設定を出力(LED等を接続)とした場合、プルアップ設定は初期状態のON/OFF出力設定として動作する  

## パラメータファイルによる再生設定
一部の動作モードでは、I2C接続EEPROM に格納するパラメータファイルによって再生範囲等の設定が可能。  
パラメータファイルは、テキストで記述したものを、PAR変換ツールでバイナリ化することで作成可能だが、最初からバイナリエディタで作成することも可能。
ヘッダ等含めた最大ファイルサイズは32バイト、ヘッダ等を除く実データ領域の最大サイズは24バイトとなる。
I2C接続EEPROM にパラメータファイルが格納されていない場合は 24バイト分の 0x00 が指定されたものとして動作する。  
各動作モードのサンプルパラメータファイルを[sample_par/](/sample_par/)に格納しているのでご参考に。  
### 分離再生モードのパラメータファイル
1バイト目：ボタン0・ボタン1同時押し時の再生対象番号を指定する。

### シリアル指定・ボタン番号指定・パラレル指定モード共通のパラメータファイル
1バイト目から順に、各ボタンの格納曲数-1 を列挙する。  
初期状態（オール 0x00）の場合は、全ボタン１曲ずつとなるので、ボタン番号＝曲番号となる。  
例）  
`0x00,0x01,0x00,0x02`  
ボタン0＝1番目の曲のみを再生、ボタン1＝2番目～3番目の曲を順に再生、ボタン2＝4番目の曲のみを再生、ボタン3＝5番目～7番目の曲を順に再生

## オルゴール楽譜のデータ作成
### MMLファイルの記述
テキストエディタ等で、MMLファイルを作成する。MMLの仕様は、[README_MML.txt](/README_MML.txt) を参照。

### MMLファイルのBIN変換
[AVR_MB_Writer](https://github.com/hiro-otsuka/AVR_MB_Writer) ツールから変換操作を行う。
操作方法は、AVR_MB_Writerのマニュアルを参照すること。  
または、コマンドプロンプトを起動し、以下のコマンドを実行する。  
`MML2BIN.EXE MMLファイル名`  
EXEファイルにMMLファイルをドラッグ＆ドロップしても変換可能だが、エラーや警告が出力された場合に確認できないので注意が必要。

### BINファイルの書き込み
[AVR_MB_Writer](https://github.com/hiro-otsuka/AVR_MB_Writer) ツールで I2C接続EEPROM への書き込みを行う。
操作方法は、AVR_MB_Writerのマニュアルを参照すること。  

## PCM音源の作成
### 音源の録音
Windows のサウンドレコーダー等を用いて録音する。

### 録音形式の変換
前述の spwave 等を用いて、PCMモノラル/8bit/8KHz サンプリングの WAVファイルに変換する。
なお、当方の環境では、spwave で保存時に形式 "Microsoft PCM(libsndfile)" ではエラーとなった。
"Microsoft PCM" を選択すれば正常に保存できた。ADPCMにする場合は、"WAVE IMA ADPCM(libsndfile)"を選択する。  
※ただし、ADPCMのWAVファイルは、ヘッダに格納されるファイルサイズがファイルフォーマットに即していない。  
　本来はファイル全体のサイズ -8 バイトであるはずが、ファイル全体のサイズが記録されている。  
　通常使用では問題は発生しないが、ファイルを連結して一度に書き込みを行う場合は要注意。

### WAVファイルの書き込み
[AVR_MB_Writer](https://github.com/hiro-otsuka/AVR_MB_Writer) ツールで I2C接続EEPROM への書き込みを行う。
操作方法は、AVR_MB_Writerのマニュアルを参照すること。  

# 更に高度な使用方法
## 制御プログラムのカスタマイズ
### プロジェクトの準備
Atmel Studio で新規のプロジェクトを作成し、以下の設定を行う
* [src/](/src/) ディレクトリに格納されている全ファイルを追加する。  
⇒[Project]->[Add Existing Item...]
* ターゲットマイコンに ATTiny85 または ATTiny861A を選択する。  
⇒[Project]->[Properties]->[Device]->[Change Device...]
* コンパイラの設定で、-D オプションに F_CPU=16000000UL を追加する。  
⇒[Project]->[Properties]->[Toolchain]->[AVR/GNU C Compiler]->[Symbols]

### 設定のカスタマイズ
設定用のヘッダファイル([src/config.h](/src/config.h))を編集してカスタマイズを行う。
基本はそのままで問題ない。詳細はヘッダ内のコメントを参照。

### 動作のカスタマイズ
メイン処理を編集し、動作をカスタマイズする。
* 各ボタンに対する動作モードの変更、機能追加、機能削減など
* 外部モジュールとの連携強化など

### 楽譜データの内蔵
EEPROM がなくても動作させたい場合、オルゴール楽譜のデータをプログラムに内蔵することが可能。
ただし、プログラムのサイズが巨大になったため、現状のままオルゴール楽譜を内蔵させようとすると、フラッシュメモリに収まらない。
この用途で使用する場合、一部の機能（PCMの再生等）を削除する必要がある。  
使用する場合、[src_mml/](/src_mml/) ディレクトリに格納されたファイルを参考にオルゴール楽譜をプログラムし、サンプルプログラムのコメントアウト箇所を有効化すること。  
* [MML_HOMusic_C3.h](/src_mml/MML_HOMusic_C3.h)  
一応、3パート再生のサンプル音楽。
* [MML_Test.h](/src_mml/MML_Test.h)  
全音階の再生、音色変化などのサンプル。音色は劇的には変わらないので、参考程度。

## 他マイコンとの連携
本プログラムは、ATTiny85 や ATTiny861A のフラッシュメモリ領域ぎりぎりのサイズとなっているため、複雑な処理を追加する余地が少ない。
また、小ピンマイコンとして動作実績のある ATTiny85 では、省スペースの基板が作成できる一方、入出力に使えるピンが 2ピンしかない。  
このため、他のマイコンで再生タイミングやユーザ入出力処理を担当し、本プログラムを搭載したマイコンに対してシリアル通信やパラレル通信で再生対象を指定する、といった連携が考えられる。  
[src_pui/](/src_pui/) に、ATTiny861A から、シリアル通信で AVR_MusicBox を制御するためのサンプルプログラムを格納しているのでご参考まで。  
※ただし、この程度の処理なら、ATTiny861A 版の AVR_MusicBox だけで実現可能。

# 謝辞
本ドキュメントで紹介している各フリーソフト作者の皆様に感謝致します。また、本プログラムの試作段階で参考にさせていただいた、AVRマイコンや電子回路に関する情報を公開されている先輩方に感謝致します。

# 変更履歴

最新確認済みマイコン：ATTiny85, ATTiny861A

* 2017/05/15  以下の更新を行った  
 1. シリアル通信の直接指定モードと範囲指定モードを、I2C接続EEPROMのパラメータでの指定に統合
 2. 多ピンマイコンのための動作モード（パラレル指定、2進数指定）を追加
 3. 動作モードや各ピンのI/O設定をAVR内蔵EEPROMから読み込むよう変更
 4. 本ドキュメントを現状の実装に即して改訂

* 2017/04/24  以下の更新を行った  
 1. 複数バンクのEEPROMを一連のメモリとして扱うよう機能変更
 2. 動作モードやピンのプルアップ有無をI2C接続EEPROMから読み込むよう変更

* 2017/03/12  以下の更新を行った  
 1. [AVR_MB_Writer](https://github.com/hiro-otsuka/AVR_MB_Writer) との連携のための調整を実施

* 2017/02/18  以下の更新を行った  
 1. MMLの仕様に、サブパート定義とループの機能を追加
 2. 楽譜の波形タイプに鋸波を追加

* 2017/02/17  以下の更新を行った  
 1. 楽譜情報も EEPROMから読み込めるように構成変更
 2. EEPROM の書き込みツールとMML変換ツールを同梱

* 2017/02/01  新規公開  
