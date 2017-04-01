/*
 * PWM_EEPROM_Play.c
 *
 * 概要：
 *  I2C接続のEEPROMから、WAVまたはMMLを自動判別して再生するモジュール
 *
 * 使用方法等：
 *  PWM_EEPROM_Play を使用するプロジェクトにインポートして使用
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

#include "PWM_EEPROM_Play.h"

//********** 関数定義 **********//
uint8_t EEPROM_Search(uint8_t, uint8_t*, uint8_t* );

//********** 定数定義 **********//

//********** 変数定義 **********//
// EEPROM に保存されているファイルの数（種類別）
volatile uint8_t EEPROM_Files[PWM_PCMPLAY_TYPES];
// EEPROM に保存されているパラメータを保存
volatile uint8_t EEPROM_Params[PWM_PCMPLAY_PARAMS];

//********** 外部関数 **********//
//=============================
//EEPROMのファイル数を検索
void EEPROM_Init()
{
	for (uint8_t i = 0; i < PWM_PCMPLAY_PARAMS; i++ ) EEPROM_Params[i] = 0;
	for (uint8_t i = 0; i < PWM_PCMPLAY_TYPES; i++ ) EEPROM_Files[i] = 0;
	
	uint8_t PlayMode = 0;
	uint32_t StartAddr = 0;
	uint32_t inSize = 0;
	
	//ファイルが存在する限りスキップしていく（I2Cアドレスが無い場合は 0 ）
	while(1) {
		if (inSize != 0) {
			StartAddr += inSize + 8;
			inSize = 0;
		}
		if (EEPROM_Array_SetAddr(StartAddr)) return;

		//WAVフォーマットのヘッダファイル読み込み
		EEPROM_Array_Read(4);	//'RIFF' or 'MMLD' or 'PARM'
		PlayMode = EEPROM_Array_data();		// 'R' or 'M' or 'P'
		//どれでもない場合は終了
		if (PlayMode != 'R' && PlayMode != 'M' && PlayMode != 'P') break;
		//ファイルのサイズを得る
		EEPROM_Array_Read(4);	//Size(below)
		inSize = EEPROM_Array_data4();
		
		//種類ごとに対象のファイルをカウントアップする
		if (PlayMode == 'R') EEPROM_Files[PWM_PCMPLAY_PCM] ++;
		else if (PlayMode == 'M') EEPROM_Files[PWM_PCMPLAY_MML] ++;
		else if (PlayMode == 'P') {
			//パラメータを見つけた場合はカウントせず、その場で読み込む
			EEPROM_Array_Read(inSize);
			for (uint8_t i = 0; i < inSize; i++) {
				EEPROM_Params[i] = EEPROM_Array_data();
			}
		}
	}
}

//=============================
//EEPROMからのファイル再生を開始
//	引数：再生するファイルの種類, 何番目の情報を再生するか
void EEPROM_Play(uint8_t type, uint8_t count)
{

	uint8_t PlayMode = 0;
	uint32_t StartAddr = 0;
	uint32_t inSize = 0;
	
	//指定された count 番目のファイルを見つけるまでスキップしていく
	do {
		if (inSize != 0) {
			StartAddr += inSize + 8;
			inSize = 0;
		}
		if (EEPROM_Array_SetAddr(StartAddr)) return;

		//WAVフォーマットのヘッダファイル読み込み
		EEPROM_Array_Read(4);	//'RIFF' or 'MMLD' or 'PARM'
		PlayMode = EEPROM_Array_data();		// 'R' or 'M' or 'P'
		//どれでもない場合は終了
		if (PlayMode != 'R' && PlayMode != 'M' && PlayMode != 'P') break;

		//ファイルのサイズを得る
		EEPROM_Array_Read(4);	//Size(below)
		inSize = EEPROM_Array_data4();

		//対象ファイルじゃない場合はループを抜けない（'P'は常になかったことにする）
		if (PlayMode == 'P') count++;
		else if (type == PWM_PCMPLAY_MML && PlayMode != 'M') count ++;
		else if (type == PWM_PCMPLAY_PCM && PlayMode != 'R') count ++;
	} while(count --);
	
	//フォーマットを確認
	switch(PlayMode) {
		case 'R':	//RIFF = WAV
		PWM_PCM_WAV_Play(StartAddr + 4+4);
		break;
		case 'M':	//MMLD = MML
		PWM_PCM_MB_EEPROM_Play(StartAddr + 4+4);
		break;
	}
}

