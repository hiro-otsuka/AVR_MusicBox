/*
 * PWM_PCM_MB.h
 *
 * 概要：
 *  PWM_PCM_MB モジュールのための各種定義
 *
 * 使用方法等：
 *  PWM_PCM_MB を使用するモジュールから include
 *  別途楽譜定義を行う必要がある（同梱のサンプル参照）
 *  音符マクロに与える長さの基本値（一分音符に相当する長さ）の求め方
 *     ((60 / 四分音符のリズム数) * 4) / ((1,024 * 256) / CPUの周波数) 
 *      ⇒ここから3*16の倍数の一番近い数字に丸める（3連符を使わない場合は16の倍数でもよい）
 *        CPUの周波数が20MHzの時 ＝ 一分音符の長さ(秒) / 0.0131072
 *        CPUの周波数が16MHzの時 ＝ 一分音符の長さ(秒) / 0.016384
 *     例）四分音符=120の楽譜で、CPU=16MHz の場合
 *         ((60/120)*4)/0.016384=122 ⇒ 3連符を使う場合は 96 or 144、使わない場合は 128
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *  2017/02/17 構成変更(Hiro OTSUKA) EEPROMからのMML再生およびWAVとの自動判別に対応
 *                                  音色の選択機能を追加
 *
 */

#ifndef PWM_PCM_MB_H_
#define PWM_PCM_MB_H_

//********** 読み込み **********//
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "PWM_PCMPlay.h"

//********** 定数定義 **********//
// 各種サイズ
#define PWM_MUSICBOX_BUFF	32	// MML格納用バッファのサイズ（2の倍数であること）

// 音符情報
#define N_C		0+1
#define N_Cs	1+1
#define N_Db	1+1
#define N_D		2+1
#define N_Ds	3+1
#define N_Eb	3+1
#define N_E		4+1
#define N_F		5+1
#define N_Fs	6+1
#define N_Gb	6+1
#define N_G		7+1
#define N_Gs	8+1
#define N_Ab	8+1
#define N_A		9+1
#define N_As	10+1
#define N_Bb	10+1
#define N_B		11+1

// 演奏制御
#define C_CMD	0x80
#define C_VOL	1	//ボリューム設定（付加情報：音量0-16）
#define C_DIVST	2	//減衰ウェイト（付加情報：ウェイト数）
#define C_DIVWT	3	//減衰間隔（付加情報：間隔数）
#define C_OFF	4	//音符オフセット（付加情報：キー加算値）
#define C_END	5	//再生の終了
#define C_L_ON	6	//LEDx にHIGHを出力（付加情報：LED番号）
#define C_L_OFF	7	//LEDx にLOW を出力（付加情報：LED番号）
#define C_TYPE	8	//音色番号の変更

//*********** マクロ ***********//
//通常の音符（引数：音符情報,オクターブ,長さ）
#define M_NOTE(n,o,l) (uint16_t)(n+o*12)*256+l
//スタッカート（引数：音符情報,オクターブ,長さ）
#define M_NOTE_S(n,o,l) (uint16_t)(n+o*12)*256+N_LEN/16,(uint16_t)l-N_LEN/16
//休符（引数：長さ）
#define M_REST(l) l
//コントロールコマンド（引数：演奏情報, 付加情報）
#define M_CTRL(x,y) (uint16_t)(C_CMD+x)*256+y

//********** 関数定義 **********//
//オルゴールモジュールの初期化（最初に一度だけ呼び出しでOK）
void PWM_PCM_MB_Init();

//楽譜情報設定
//	引数：楽譜番号, 初期化用コールバック関数
void PWM_PCM_MB_MEM_SetScore(uint8_t , void (*)());

//チャンネル情報設定
//	引数：チャンネル番号, 楽譜アドレス, 楽譜数(!=バイト数)
void PWM_PCM_MB_MEM_SetScoreChannel(uint8_t , const uint16_t* , uint16_t );

//オルゴールモジュールの再生開始（C_ENDまで再生すると終了） メモリ版
//	引数：楽譜番号
void PWM_PCM_MB_MEM_Play(uint8_t );

//オルゴールモジュールの再生開始（C_ENDまで再生すると終了） EEOROM版
//	引数：I2Cスレーブアドレス, 開始アドレス
void PWM_PCM_MB_EEPROM_Play(uint8_t , uint16_t );

#endif //PWM_PCM_MB_H_
