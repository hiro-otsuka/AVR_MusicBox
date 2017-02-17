/*
 * PWM_PCM_WAV.c
 *
 * 概要：
 *  I2C接続のEEPROMからWAV再生を行うモジュール
 *
 * 使用方法等：
 *  PWM_PCM_WAV を使用するプロジェクトにインポートして使用
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

#include "USI_TWI_Master.h"
#include "PWM_PCM_WAV.h"

//********** 外部関数 **********//
//=============================
//EEPROMからのWAV再生を開始
//	引数：I2Cスレーブアドレス, 再生開始アドレス
void PWM_PCM_WAV_Play(uint8_t slaveaddr, uint16_t StartAddr)
{
	// EEPROM からヘッダ情報を読み込み
	do {
		USI_TWI_Master_tranBegin(slaveaddr);
		USI_TWI_Master_tranSend((uint8_t)(StartAddr >> 8));
		USI_TWI_Master_tranSend((uint8_t)(StartAddr & 0xFF));
	} while(USI_TWI_Master_tranCommit());
	
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'WAVE'
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'fmt '
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//Size(below)
	USI_TWI_Master_receiveFrom(slaveaddr, 2);	//format
	USI_TWI_Master_receiveFrom(slaveaddr, 2);	//channels
	
	//サンプリングレートを得る（8Kbps で確認済み）
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//samplerate
	
	uint32_t inRate = 0;
	for (uint8_t i = 0; i < 4; i++) {
		inRate >>= 8;
		inRate += ((uint32_t)USI_TWI_Master_receive() << 24);
	}
	
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//bytepersec
	USI_TWI_Master_receiveFrom(slaveaddr, 2);	//blockalign
	USI_TWI_Master_receiveFrom(slaveaddr, 2);	//bitswidth
	
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'data'
	
	//音声データのサイズを得る
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//Size(below)

	uint32_t inSize = 0;
	for (uint8_t i = 0; i < 4; i++) {
		inSize >>= 8;
		inSize += ((uint32_t)USI_TWI_Master_receive() << 24);
	}
	
	//PWM開始
	PwmSound_Start((uint16_t)inRate);
	
	PWM_PCM_Playing = 1;
	
	//再生が終了するまでEEPROMから読み込み続ける
	while (PWM_PCM_Playing) {
		USI_TWI_Master_receiveFrom(slaveaddr, PWM_PCMPLAY_READSIZE);
		for(uint8_t i = 0; i < PWM_PCMPLAY_READSIZE ; i++) {
			PwmSound_AddBuff(USI_TWI_Master_receive());
		}
		//最後がきれいに終わらないので要改善
		if (inSize < PWM_PCMPLAY_READSIZE) PWM_PCM_Playing = 0;
		else inSize -= PWM_PCMPLAY_READSIZE;
	}

	//PWM終了
	PwmSound_Stop();
}
