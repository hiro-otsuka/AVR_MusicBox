/*
 * PWM_EEPROM_Play.h
 *
 * 概要：
 *  PWM_EEPROM_Play モジュールのための各種定義
 *
 * 使用方法等：
 *  PWM_EEPROM_Play を使用するモジュールから include
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/02/17 新規作成(Hiro OTSUKA) EEPROMからのMML再生およびWAVとの自動判別に対応
 *  2017/02/25 機能改善(Hiro OTSUKA) MMLとWAVを分離して再生できるよう機能改善
 *  2017/02/26 機能追加(Hiro OTSUKA) Init時にEEPROMからパラメータを読み込む機能を追加
 *  2017/04/01 機能変更(Hiro OTSUKA) EEPROM Array の実装に対応
 *
 */

#ifndef PWM_EEPROM_PLAY_H_
#define PWM_EEPROM_PLAY_H_

//********** 読み込み **********//
#include "EEPROM_Array.h"
#include "PWM_PCM_WAV.h"
#include "PWM_PCM_MB.h"

//********** 定数定義 **********//
#define PWM_PCMPLAY_PARAMS		32		//EEPROM から読み込むパラメータの最大サイズ（ <= EEPROM_ARRAY_BUFF_MAX であること）
#define PWM_PCMPLAY_READSIZE	32		//EEPROM から一度に読み込むサイズ（ <= EEPROM_ARRAY_BUFF_MAX であること）
#define PWM_PCMPLAY_TIMEOUT		5		//EEPROM の検出にかける再実行回数
#define PWM_PCMPLAY_ANY			0		//EEPROM で再生するファイルの種類＝両方
#define PWM_PCMPLAY_PCM			1		//EEPROM で再生するファイルの種類＝PCM
#define PWM_PCMPLAY_MML			2		//EEPROM で再生するファイルの種類＝MML
#define PWM_PCMPLAY_TYPES		3		//EEPROM で再生するファイルの種類数=2


//********** 変数定義 **********//
// EEPROM に保存されているファイルの数（種類別）
extern volatile uint8_t EEPROM_Files[PWM_PCMPLAY_TYPES];
extern volatile uint8_t EEPROM_Params[PWM_PCMPLAY_PARAMS];

//********** 関数定義 **********//
//EEPROMのファイル数を検索
void EEPROM_Init();

//EEPROMからのファイル再生を開始
//	引数：再生するファイルの種類, 何番目の情報を再生するか
void EEPROM_Play(uint8_t, uint8_t );

#endif /* PWM_EEPROM_PLAY_H_ */
