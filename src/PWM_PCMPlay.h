/*
 * PWM_PCMPlay.h
 *
 * 概要：
 *  PWM_PCMPlay モジュールのための各種定義
 *
 * 使用方法等：
 *  PWM_PCMPlay を使用するモジュールから include
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *  2017/02/17 構成変更(Hiro OTSUKA) EEPROMからのMML再生およびWAVとの自動判別に対応
 *
 */

#ifndef PWM_PCMPLAY_H_
#define PWM_PCMPLAY_H_

//********** 読み込み **********//
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "PIN_Control.h"

//********** 定数定義 **********//
#define PWM_SOUND_MAX_BUFF	32		//PCM再生用バッファ（2の倍数であること）

//********** 変数定義 **********//
//PCM再生用のバッファ
extern volatile uint8_t PwmSoundBuff[];
//PCM再生用バッファのインデックス（先頭＝書き込み先）
extern volatile uint8_t PwmSoundHead;
//PCM再生用バッファのインデックス（最後＝読み込み元）
extern volatile uint8_t PwmSoundTail;
//PWM再生中のフラグ
extern volatile uint8_t PWM_PCM_Playing;

//********** 関数定義 **********//
//PCM再生モジュールの初期化（最初に一度だけ呼び出しでOK）
void PwmSound_Init();

//PCM再生モジュールの再生開始（割り込みONの場合に再生開始）
//	引数：音声の解像度(サンプリングレート)
void PwmSound_Start(uint16_t);

//PCMバッファへの情報追記（バッファに空きが無い場合は待つ＝割り込みOFFの場合処理が進まないのでフリーズする）
//	引数：音声情報
void PwmSound_AddBuff(uint8_t);

//PCM再生モジュールの再生停止
void PwmSound_Stop();

#endif // PWM_PCMPLAY_H_
