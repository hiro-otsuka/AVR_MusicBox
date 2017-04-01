/*
 * PWM_PCM_WAV.h
 *
 * 概要：
 *  PWM_PCM_WAV モジュールのための各種定義
 *
 * 使用方法等：
 *  PWM_PCM_WAV を使用するモジュールから include
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

#ifndef PWM_PCM_WAV_H_
#define PWM_PCM_WAV_H_

//********** 読み込み **********//
#include "PWM_PCMPlay.h"

//********** 定数定義 **********//
#define PWM_PCMPLAY_READSIZE	32		//EEPROM から一度に読み込むサイズ（ <= EEPROM_ARRAY_BUFF_MAX であること）

//********** 関数定義 **********//
//EEPROMからのWAV再生を開始
//	引数：再生開始アドレス
void PWM_PCM_WAV_Play( uint32_t );

#endif /* PWM_PCM_WAV_H_ */
