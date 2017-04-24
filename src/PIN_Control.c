/*
 * PIN_Control.c
 *
 * 概要：
 *  プロジェクトごとに定義されたピンアサイン情報に従って、
 *  割り込み処理やキー入力の管理を行うモジュール
 *
 * 使用方法等：
 *  PIN_Control を使用するプロジェクトにインポートして使用
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *  2017/02/17 構成変更(Hiro OTSUKA) EEPROMからのMML再生およびWAVとの自動判別に対応
 *  2017/02/25 機能追加(Hiro OTSUKA) キー状態を確認する機能を追加
 *  2017/04/07 構成変更(Hiro OTSUKA) ピン設定の互換性とエラーチェックを向上
 *  2017/04/09 構成変更(Hiro OTSUKA) 初期化処理を整理し、待ち時間設定を可変に
 *  2017/04/22 構成変更(Hiro OTSUKA) ピンのPU要否をパラメータで指定できるよう変更
 *
 */

#include <avr/interrupt.h>
#include "PIN_Control.h"

//********** 変数定義 **********//
//BTNの押下状態を参照可能（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
volatile uint8_t PIN_Control_Key;
volatile uint8_t PIN_Control_Wait;

//********** 外部関数 **********//
//=============================
//ピンアサインと割り込み処理の初期化
void PIN_Control_Init(uint8_t isNotPU)
{
	PIN_Control_Key = 0;
	PIN_Control_Wait = 100;	//待ち時間は仮で 100ms に設定
	
	//ピンアサイン初期化	
#if defined(PIN_BTN0_DDR)
	PIN_BTN0_DDR;
	if (isNotPU) PIN_BTN0_PORT_NO;
	else PIN_BTN0_PORT_PU;
#elif defined(PIN_LED0_DDR)
	PIN_LED0_DDR; PIN_LED0_OFF;
#endif

#if defined(PIN_BTN1_DDR)
	PIN_BTN1_DDR;
	if (isNotPU) PIN_BTN1_PORT_NO;
	else PIN_BTN1_PORT_PU;
#elif defined(PIN_LED1_DDR)
	PIN_LED1_DDR; PIN_LED1_OFF;
#endif

#if defined(PIN_BTN2_DDR)
	PIN_BTN2_DDR;
	if (isNotPU) PIN_BTN2_PORT_NO;
	else PIN_BTN2_PORT_PU;
#elif defined(PIN_LED2_DDR)
	PIN_LED2_DDR; PIN_LED2_OFF;
#endif

#if defined(PIN_BTN3_DDR)
	PIN_BTN3_DDR;
	if (isNotPU) PIN_BTN3_PORT_NO;
	else PIN_BTN3_PORT_PU;
#elif defined(PIN_LED3_DDR)
	PIN_LED3_DDR; PIN_LED3_OFF;
#endif

#if defined(PIN_BTN4_DDR)
	PIN_BTN4_DDR;
	if (isNotPU) PIN_BTN4_PORT_NO;
	else PIN_BTN4_PORT_PU;
#elif defined(PIN_LED4_DDR)
	PIN_LED4_DDR; PIN_LED4_OFF;
#endif

#if defined(PIN_BTN5_DDR)
	PIN_BTN5_DDR;
	if (isNotPU) PIN_BTN5_PORT_NO;
	else PIN_BTN5_PORT_PU;
#elif defined(PIN_LED5_DDR)
	PIN_LED5_DDR; PIN_LED5_OFF;
#endif

#if defined(PIN_BTN6_DDR)
	PIN_BTN6_DDR;
	if (isNotPU) PIN_BTN6_PORT_NO;
	else PIN_BTN6_PORT_PU;
#elif defined(PIN_LED6_DDR)
	PIN_LED6_DDR; PIN_LED6_OFF;
#endif

#if defined(PIN_BTN7_DDR)
	PIN_BTN7_DDR;
	if (isNotPU) PIN_BTN7_PORT_NO;
	else PIN_BTN7_PORT_PU;
#elif defined(PIN_LED7_DDR)
	PIN_LED7_DDR; PIN_LED7_OFF;
#endif

#ifdef _PLAYING_ENABLE
	PIN_PLAYING_DDR; PIN_PLAYING_ON;
#endif

	//割り込み処理の初期化
	PIN_CHANGE_INT_INIT;
	PIN_CHANGE_INT_ENBL;

#if defined(PIN_BTN0_DDR)
	PIN_BTN0_INT;
#endif
#if defined(PIN_BTN1_DDR)
	PIN_BTN1_INT;
#endif
#if defined(PIN_BTN2_DDR)
	PIN_BTN2_INT;
#endif
#if defined(PIN_BTN3_DDR)
	PIN_BTN3_INT;
#endif
#if defined(PIN_BTN4_DDR)
	PIN_BTN4_INT;
#endif
#if defined(PIN_BTN5_DDR)
	PIN_BTN5_INT;
#endif
#if defined(PIN_BTN6_DDR)
	PIN_BTN6_INT;
#endif
#if defined(PIN_BTN7_DDR)
	PIN_BTN7_INT;
#endif
}

//=============================
//ボタン待ち時間設定
//	引数：ボタン安定までの待ち時間（ms）
void PIN_Control_SetWait(uint8_t wait)
{
	PIN_Control_Wait = wait;	//指定された待ち時間を設定
}

//=============================
//BTNがすべて離されるのを待つ
//	引数：対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_WaitKeyOff(uint8_t waitmask)
{
	uint8_t WaitCount;
	
	WaitCount = 0;
	while(WaitCount < PIN_Control_Wait) {
		if (0

#if defined(PIN_BTN0_DDR)
		|| (PIN_BTN0_PIN == 0 && (waitmask & 1 << 0) == 0)
#endif
#if defined(PIN_BTN1_DDR)
		|| (PIN_BTN1_PIN == 0 && (waitmask & 1 << 1) == 0)
#endif
#if defined(PIN_BTN2_DDR)
		|| (PIN_BTN2_PIN == 0 && (waitmask & 1 << 2) == 0)
#endif
#if defined(PIN_BTN3_DDR)
		|| (PIN_BTN3_PIN == 0 && (waitmask & 1 << 3) == 0)
#endif
#if defined(PIN_BTN4_DDR)
		|| (PIN_BTN4_PIN == 0 && (waitmask & 1 << 4) == 0)
#endif
#if defined(PIN_BTN5_DDR)
		|| (PIN_BTN5_PIN == 0 && (waitmask & 1 << 5) == 0)
#endif
#if defined(PIN_BTN6_DDR)
		|| (PIN_BTN6_PIN == 0 && (waitmask & 1 << 6) == 0)
#endif
#if defined(PIN_BTN7_DDR)
		|| (PIN_BTN7_PIN == 0 && (waitmask & 1 << 7) == 0)
#endif
		) WaitCount = 0;
		else WaitCount ++;
		_delay_ms(1);
	}
}

//=============================
//BTNがすべて押されるのを待つ
//	引数：対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_WaitKeyOn(uint8_t waitmask)
{
	uint8_t WaitCount;
	
	WaitCount = 0;
	while(WaitCount < PIN_Control_Wait) {
		if (0

#if defined(PIN_BTN0_DDR)
		|| (PIN_BTN0_PIN == 1 && (waitmask & 1 << 0) == 0)
#endif
#if defined(PIN_BTN1_DDR)
		|| (PIN_BTN1_PIN == 1 && (waitmask & 1 << 1) == 0)
#endif
#if defined(PIN_BTN2_DDR)
		|| (PIN_BTN2_PIN == 1 && (waitmask & 1 << 2) == 0)
#endif
#if defined(PIN_BTN3_DDR)
		|| (PIN_BTN3_PIN == 1 && (waitmask & 1 << 3) == 0)
#endif
#if defined(PIN_BTN4_DDR)
		|| (PIN_BTN4_PIN == 1 && (waitmask & 1 << 4) == 0)
#endif
#if defined(PIN_BTN5_DDR)
		|| (PIN_BTN5_PIN == 1 && (waitmask & 1 << 5) == 0)
#endif
#if defined(PIN_BTN6_DDR)
		|| (PIN_BTN6_PIN == 1 && (waitmask & 1 << 6) == 0)
#endif
#if defined(PIN_BTN7_DDR)
		|| (PIN_BTN7_PIN == 1 && (waitmask & 1 << 7) == 0)
#endif
		) WaitCount = 0;
		else WaitCount ++;
		_delay_ms(1);
	}
}

//=============================
//BTNのその瞬間の押下状態を調べる
//	戻値：BTNの押下状態（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
volatile uint8_t PIN_Control_GetKey()
{
	uint8_t KeyNow = 0;
	
#if defined(PIN_BTN0_DDR)
	if (PIN_BTN0_PIN == 0) KeyNow |= 1 << 0;
#endif
#if defined(PIN_BTN1_DDR)
	if (PIN_BTN1_PIN == 0) KeyNow |= 1 << 1;
#endif
#if defined(PIN_BTN2_DDR)
	if (PIN_BTN2_PIN == 0) KeyNow |= 1 << 2;
#endif
#if defined(PIN_BTN3_DDR)
	if (PIN_BTN3_PIN == 0) KeyNow |= 1 << 3;
#endif
#if defined(PIN_BTN4_DDR)
	if (PIN_BTN4_PIN == 0) KeyNow |= 1 << 4;
#endif
#if defined(PIN_BTN5_DDR)
	if (PIN_BTN5_PIN == 0) KeyNow |= 1 << 5;
#endif
#if defined(PIN_BTN6_DDR)
	if (PIN_BTN6_PIN == 0) KeyNow |= 1 << 6;
#endif
#if defined(PIN_BTN7_DDR)
	if (PIN_BTN7_PIN == 0) KeyNow |= 1 << 7;
#endif
	return KeyNow;
}

//=============================
//外部のオペアンプをON/OFF制御(ピンが有効な場合)
//	引数：0=OFF/1=ON
void PIN_Control_Playing(uint8_t isOn)
{
#ifdef _PLAYING_ENABLE
	if (isOn) PIN_PLAYING_ON;
	else PIN_PLAYING_OFF;
#endif
}

//********** 割込関数 **********//
//+++++++++++++++++++++++++++++
//ピン変化割り込みによるキー入力受付
ISR(PIN_CHANGE_INT_VECT)
{
#if defined(PIN_BTN0_DDR)
	if (PIN_BTN0_PIN == 0 && (PIN_Control_Key & (1 << 0)) == 0)  {
#if !defined(PIN_BTN0_INT)
		if (PWM_PCM_Playing == 0) {
#endif
			PIN_Control_Key |= (1 << 0);
			PWM_PCM_Playing = 0;
#if !defined(PIN_BTN0_INT)
		}
#endif
	}
#endif
#if defined(PIN_BTN1_DDR)
	if (PIN_BTN1_PIN == 0 && (PIN_Control_Key & (1 << 1)) == 0)  {
#if !defined(PIN_BTN1_INT)
		if (PWM_PCM_Playing == 0) {
#endif
			PIN_Control_Key |= (1 << 1);
			PWM_PCM_Playing = 0;
#if !defined(PIN_BTN1_INT)
		}
#endif
	}
#endif
#if defined(PIN_BTN2_DDR)
	if (PIN_BTN2_PIN == 0 && (PIN_Control_Key & (1 << 2)) == 0)  {
#if !defined(PIN_BTN2_INT)
		if (PWM_PCM_Playing == 0) {
#endif
			PIN_Control_Key |= (1 << 2);
			PWM_PCM_Playing = 0;
#if !defined(PIN_BTN2_INT)
		}
#endif
	}
#endif
#if defined(PIN_BTN3_DDR)
	if (PIN_BTN3_PIN == 0 && (PIN_Control_Key & (1 << 3)) == 0)  {
#if !defined(PIN_BTN3_INT)
		if (PWM_PCM_Playing == 0) {
#endif
			PIN_Control_Key |= (1 << 3);
			PWM_PCM_Playing = 0;
#if !defined(PIN_BTN3_INT)
		}
#endif
	}
#endif
#if defined(PIN_BTN4_DDR)
	if (PIN_BTN4_PIN == 0 && (PIN_Control_Key & (1 << 4)) == 0)  {
#if !defined(PIN_BTN4_INT)
		if (PWM_PCM_Playing == 0) {
#endif
			PIN_Control_Key |= (1 << 4);
			PWM_PCM_Playing = 0;
#if !defined(PIN_BTN4_INT)
		}
#endif
	}
#endif
#if defined(PIN_BTN5_DDR)
	if (PIN_BTN5_PIN == 0 && (PIN_Control_Key & (1 << 5)) == 0)  {
#if !defined(PIN_BTN5_INT)
		if (PWM_PCM_Playing == 0) {
#endif
			PIN_Control_Key |= (1 << 5);
			PWM_PCM_Playing = 0;
#if !defined(PIN_BTN5_INT)
		}
#endif
	}
#endif
#if defined(PIN_BTN6_DDR)
	if (PIN_BTN6_PIN == 0 && (PIN_Control_Key & (1 << 6)) == 0)  {
#if !defined(PIN_BTN6_INT)
		if (PWM_PCM_Playing == 0) {
#endif
			PIN_Control_Key |= (1 << 6);
			PWM_PCM_Playing = 0;
#if !defined(PIN_BTN6_INT)
		}
#endif
	}
#endif
#if defined(PIN_BTN7_DDR)
	if (PIN_BTN7_PIN == 0 && (PIN_Control_Key & (1 << 7)) == 0)  {
#if !defined(PIN_BTN7_INT)
		if (PWM_PCM_Playing == 0) {
#endif
			PIN_Control_Key |= (1 << 7);
			PWM_PCM_Playing = 0;
#if !defined(PIN_BTN7_INT)
		}
#endif
	}
#endif
}
