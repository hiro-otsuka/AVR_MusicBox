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
 *
 */

#include "PWM_EEPROM_Play.h"

//********** 関数定義 **********//
uint8_t EEPROM_Search(uint8_t, uint8_t*, uint8_t* );

//********** 定数定義 **********//
#define	EEPROM_I2C_MAX	8

//********** 変数定義 **********//
// EEPROM に保存されているファイルの数（種類別）
volatile uint8_t EEPROM_Files[PWM_PCMPLAY_TYPES];

uint8_t	EEPROM_File[PWM_PCMPLAY_TYPES][EEPROM_I2C_MAX];

//********** 外部関数 **********//
//=============================
//EEPROMからのファイル再生を開始（I2Cアドレス指定）
//	引数：I2Cスレーブアドレス, 再生するファイルの種類, 何番目の情報を再生するか
void EEPROM_PlayAt(uint8_t slaveaddr, uint8_t type, uint8_t count)
{
	uint8_t PlayMode = 0;
	uint16_t StartAddr = 0;
	uint32_t inSize = 0;
	int TimeOut = 0;
	
	//指定された count 番目のファイルを見つけるまでスキップしていく
	do {
		if (inSize != 0) {
			StartAddr += inSize + 8;
			inSize = 0;
		}
		TimeOut = 0;
		do {
			USI_TWI_Master_tranBegin(slaveaddr);
			USI_TWI_Master_tranSend((uint8_t)(StartAddr >> 8));
			USI_TWI_Master_tranSend((uint8_t)(StartAddr & 0xFF));
			TimeOut ++;
		} while(USI_TWI_Master_tranCommit() && TimeOut < PWM_PCMPLAY_TIMEOUT);
	
		//WAVフォーマットのヘッダファイル読み込み
		USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'RIFF' or 'MMLD'
		PlayMode = USI_TWI_Master_receive();		// 'R' or 'M'
		USI_TWI_Master_receiveFrom(slaveaddr, 4);	//Size(below)
		//ファイルのサイズを得る
		for (uint8_t i = 0; i < 4; i++) {
			inSize >>= 8;
			inSize += ((uint32_t)USI_TWI_Master_receive() << 24);
		}
		//対象ファイルじゃない場合はループを抜けない
		if (type == PWM_PCMPLAY_MML && PlayMode != 'M') count ++;
		if (type == PWM_PCMPLAY_PCM && PlayMode != 'R') count ++;
	} while(count --);
	
	//フォーマットを確認
	switch(PlayMode) {
		case 'R':	//RIFF = WAV
			PWM_PCM_WAV_Play(slaveaddr, StartAddr + 4+4);
			break;
		case 'M':	//MMLD = MML
			PWM_PCM_MB_EEPROM_Play(slaveaddr, StartAddr + 4+4);
			break;
	}
}

//=============================
//EEPROMのファイル数を検索
void EEPROM_Init()
{
	uint8_t mmlnum, pcmnum;
	
	for (uint8_t i = 0; i < PWM_PCMPLAY_TYPES; i++ ) EEPROM_Files[i] = 0;
	for (uint8_t i = 0; i < EEPROM_I2C_MAX; i++) {
		uint8_t num = EEPROM_Search(0x50 + i, &mmlnum, &pcmnum);
		EEPROM_File[PWM_PCMPLAY_MML][i] = mmlnum;
		EEPROM_Files[PWM_PCMPLAY_MML] += mmlnum;
		EEPROM_File[PWM_PCMPLAY_PCM][i] = pcmnum;
		EEPROM_Files[PWM_PCMPLAY_PCM] += pcmnum;
		EEPROM_File[PWM_PCMPLAY_ANY][i] = num;
		EEPROM_Files[PWM_PCMPLAY_ANY] += num;
	}
}

//=============================
//EEPROMからのファイル再生を開始
//	引数：再生するファイルの種類, 何番目の情報を再生するか
void EEPROM_Play(uint8_t type, uint8_t count)
{
	uint8_t index = 0;
	
	while(count >= EEPROM_File[type][index] && index < EEPROM_I2C_MAX) {
		count -= EEPROM_File[type][index];
		index ++;
	}
	if (index >= EEPROM_I2C_MAX || EEPROM_File[index] == 0) return;
	EEPROM_PlayAt(0x50 + index, type, count);
}

//********** 内部関数 **********//
//-----------------------------
//EEPROMからのWAV再生を開始
//	引数：I2Cスレーブアドレス, MML数の返戻アドレス, PCM数の返戻アドレス
//  戻値：ファイル数
uint8_t EEPROM_Search(uint8_t slaveaddr, uint8_t* mmlnum, uint8_t* pcmnum)
{
	uint8_t PlayMode = 0;
	uint16_t StartAddr = 0;
	uint32_t inSize = 0;
	int Files = 0;
	int TimeOut = 0;
	
	*mmlnum = 0;
	*pcmnum = 0;
	//ファイルが存在する限りスキップしていく（I2Cアドレスが無い場合は 0 ）
	while(1) {
		if (inSize != 0) {
			StartAddr += inSize + 8;
			inSize = 0;
		}
		TimeOut = 0;
		do {
			USI_TWI_Master_tranBegin(slaveaddr);
			USI_TWI_Master_tranSend((uint8_t)(StartAddr >> 8));
			USI_TWI_Master_tranSend((uint8_t)(StartAddr & 0xFF));
			TimeOut ++;
		} while(USI_TWI_Master_tranCommit() && TimeOut < PWM_PCMPLAY_TIMEOUT);
		
		// I2Cアドレスが無ければ 0 とする
		if (TimeOut >= PWM_PCMPLAY_TIMEOUT) return 0;
		
		//WAVフォーマットのヘッダファイル読み込み
		USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'RIFF' or 'MMLD'
		PlayMode = USI_TWI_Master_receive();		// 'R' or 'M'
		if (PlayMode == 'R') (*pcmnum) ++;
		else if (PlayMode == 'M') (*mmlnum) ++;
		else return Files;
		USI_TWI_Master_receiveFrom(slaveaddr, 4);	//Size(below)
		
		//ファイルのサイズを得る
		for (uint8_t i = 0; i < 4; i++) {
			inSize >>= 8;
			inSize += ((uint32_t)USI_TWI_Master_receive() << 24);
		}
		Files ++;
	}
}
