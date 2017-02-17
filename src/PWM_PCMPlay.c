/*
 * PWM_PCMPlay.c
 *
 * 概要：
 *  PWMによるPCM音声の再生を行うモジュール
 *
 * 使用方法等：
 *  PWM_PCMPlay を使用するプロジェクトにインポートして使用
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM_PCMPlay.h"

//********** 変数定義 **********//
//PCM再生用のバッファ
volatile uint8_t PwmSoundBuff[PWM_SOUND_MAX_BUFF];
//PCM再生用バッファのインデックス（先頭＝書き込み先）
volatile uint8_t PwmSoundHead = 0;
//PCM再生用バッファのインデックス（最後＝読み込み元）
volatile uint8_t PwmSoundTail = 0;
//PWM再生中のフラグ
volatile uint8_t PWM_PCM_Playing;

static volatile uint8_t PwmSoundWait = 0;
static volatile uint8_t PwmSoundNow  = 0;
static volatile uint8_t PwmSoundVal  = 0;

//********** 外部関数 **********//
//=============================
//PCM再生モジュールの初期化（最初に一度だけ呼び出しでOK）
void PwmSound_Init()
{
	DDRB |= (1 << PWM_PIN_OUT);
	PORTB |= (1 << PWM_PIN_OUT);
#ifdef _PWM_DEBUG
	DDRB |= (1 << PWM_DEBUG_OUT);
	PORTB &= ~(1 << PWM_DEBUG_OUT);
#endif

#if PWM_SOUND_FREQ == 32000
	PWM_SPEED_32000;
#elif PWM_SOUND_FREQ == 16000
	PWM_SPEED_16000;
#else
	PWM_SPEED_8000;
#endif
	PWM_TIM1B_ON;

	OCR1C = 0xFF;
	OCR1B = 0xFF;
	TCNT1 = 0;
}

//=============================
//PCM再生モジュールの再生開始（割り込みONの場合に再生開始）
//	引数：音声の解像度(サンプリングレート)
void PwmSound_Start(uint16_t MusicFreq)
{
	PwmSoundHead = 0;
	PwmSoundTail = 0;
	PwmSoundWait = (uint8_t)((uint16_t)PWM_SOUND_FREQ / MusicFreq);
	PwmSoundNow = 0;
	PwmSoundVal = 128;

	TIMSK |= (1 << TOIE1);
}

//=============================
//PCMバッファへの情報追記（バッファに空きが無い場合は待つ＝割り込みOFFの場合処理が進まないのでフリーズする）
//	引数：音声情報
void PwmSound_AddBuff(uint8_t val) {
	uint8_t PwmSoundTmp = (PwmSoundHead + 1) & (PWM_SOUND_MAX_BUFF-1);
	while(PwmSoundTmp == PwmSoundTail) {
#ifdef _PWM_DEBUG
		PORTB &= ~(1 << PWM_DEBUG_OUT);
#endif
	}
	PwmSoundBuff[PwmSoundTmp] = val;
	PwmSoundHead = PwmSoundTmp;
}

//=============================
//PCM再生モジュールの再生停止
void PwmSound_Stop()
{
	TIMSK &= ~(1 << TOIE1);
}

//********** 割込関数 **********//
//+++++++++++++++++++++++++++++
//タイマ1割り込みによるPCM再生（PwmSound の Buff を Tail から Head まで読んでいく）
ISR(TIMER1_OVF_vect)
{
	PwmSoundNow ++;
	if (PwmSoundNow >= PwmSoundWait) {
		PwmSoundNow = 0;
		if(PwmSoundHead != PwmSoundTail) {
			PwmSoundVal = PwmSoundBuff[PwmSoundTail];
			OCR1B = PwmSoundVal;
			PwmSoundTail = (PwmSoundTail + 1) & (PWM_SOUND_MAX_BUFF-1);
#ifdef _PWM_DEBUG
		} else {
			PORTB |= (1 << PWM_DEBUG_OUT);
#endif
		}
#ifdef _HIGH_RESO
	} else {
		//If next data is available
		if (PwmSoundHead != PwmSoundTail) {
			uint16_t MulTmp = 0;
			for (uint8_t i = 0; i < PwmSoundNow; i++) {
				MulTmp += (uint16_t)PwmSoundBuff[PwmSoundTail];
			}
			for (uint8_t i = PwmSoundNow; i < PwmSoundWait; i++) {
				MulTmp += (uint16_t)PwmSoundVal;
			}
			MulTmp >>= 2;
			OCR1B = MulTmp;
		}
#endif
	}
}
