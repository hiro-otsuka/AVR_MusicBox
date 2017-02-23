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
 *
 */

#ifndef PWM_EEPROM_PLAY_H_
#define PWM_EEPROM_PLAY_H_

//********** 読み込み **********//
#include "USI_TWI_Master.h"
#include "PWM_PCM_WAV.h"
#include "PWM_PCM_MB.h"

//********** 定数定義 **********//
#define PWM_PCMPLAY_READSIZE	32		//EEPROM から一度に読み込むサイズ
#define PWM_PCMPLAY_TIMEOUT		5		//EEPROM の検出にかける再実行回数
#define PWM_PCMPLAY_ANY			0		//EEPROM で再生するファイルの種類＝両方
#define PWM_PCMPLAY_PCM			1		//EEPROM で再生するファイルの種類＝PCM
#define PWM_PCMPLAY_MML			2		//EEPROM で再生するファイルの種類＝MML
#define PWM_PCMPLAY_TYPES		3		//EEPROM で再生するファイルの種類数=2


//********** 変数定義 **********//
// EEPROM に保存されているファイルの数（種類別）
extern volatile uint8_t EEPROM_Files[PWM_PCMPLAY_TYPES];

//********** 関数定義 **********//
//EEPROMからのファイル再生を開始（I2Cアドレス指定）
//	引数：I2Cスレーブアドレス, 再生するファイルの種類, 何番目の情報を再生するか
void EEPROM_PlayAt(uint8_t, uint8_t, uint8_t );

//EEPROMのファイル数を検索
void EEPROM_Init();

//EEPROMからのファイル再生を開始
//	引数：再生するファイルの種類, 何番目の情報を再生するか
void EEPROM_Play(uint8_t, uint8_t );

#endif /* PWM_EEPROM_PLAY_H_ */
