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
 *  2017/05/13 機能変更(Hiro OTSUKA) ピンの設定を実行時に指定できるよう変更
 *  2017/05/19 機能追加(Hiro OTSUKA) ピンの設定に、余りピン処理のための無効化フラグを追加
 *  2017/05/19 構成変更(Hiro OTSUKA) コードサイズ削減を実施
 *
 */

#include <avr/interrupt.h>
#include "PIN_Control.h"

//********** 変数定義 **********//
//BTNの押下状態を参照可能（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
volatile uint8_t PIN_Control_Key;
volatile uint8_t PIN_Control_Wait;
volatile uint8_t PIN_Control_Int;
volatile uint8_t PIN_Control_Enbl;
volatile uint8_t PIN_Control_SetBit[_PIN_NUM_MAX];

//********** 外部関数 **********//
//=============================
//ピンアサインと割り込み処理の初期化
void PIN_Control_Init()
{
	PIN_Control_Key = 0;
	PIN_Control_Int = 0;
	PIN_Control_Enbl = 0;
	PIN_Control_Wait = 100;	//待ち時間は仮で 100ms に設定
	
	//ピンアサイン初期化（デフォルトは入力/プルアップ/割り込みなし にしておく）
	for (uint8_t i = 0; i < _PIN_NUM_MAX; i ++) {
		PIN_Control_SetIO(i, PIN_SETBIT_DEF | (1 << PIN_SETBIT_BTN) | (0 << PIN_SETBIT_NOPU) | (0 << PIN_SETBIT_INT));
	}
#ifdef _PLAYING_ENABLE
	PIN_PLAYING_DDR; PIN_PLAYING_ON;
#endif

	//割り込み処理の初期化
	PIN_CHANGE_INT_INIT;
	PIN_CHANGE_INT_ENBL;
}

//=============================
//各ピンのI/O設定
//	引数：ピン番号、設定（ビット設定はヘッダ参照）
void PIN_Control_SetIO(uint8_t pinNum, uint8_t pinSet)
{
	switch(pinNum) {
#if defined(PIN_PIN0_BTN)
		case 0:
			PIN_PIN0_BTN; PIN_PIN0_OFF;
			if ((pinSet & (1 << PIN_SETBIT_BTN)) == 0) PIN_PIN0_LED;
			if ((pinSet & (1 << PIN_SETBIT_NOPU)) == 0) PIN_PIN0_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 0);
				PIN_PIN0_INT;
			}
			if (pinSet & (1 << PIN_SETBIT_ENBL)) PIN_Control_Enbl |= (1 << 0);
			break;
#endif
#if defined(PIN_PIN1_BTN)
		case 1:
			PIN_PIN1_BTN; PIN_PIN1_OFF;
			if ((pinSet & (1 << PIN_SETBIT_BTN)) == 0) PIN_PIN1_LED;
			if ((pinSet & (1 << PIN_SETBIT_NOPU)) == 0) PIN_PIN1_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 1);
				PIN_PIN1_INT;
			}
			if (pinSet & (1 << PIN_SETBIT_ENBL)) PIN_Control_Enbl |= (1 << 1);
			break;
#endif
#if defined(PIN_PIN2_BTN)
		case 2:
			PIN_PIN2_BTN; PIN_PIN2_OFF;
			if ((pinSet & (1 << PIN_SETBIT_BTN)) == 0) PIN_PIN2_LED;
			if ((pinSet & (1 << PIN_SETBIT_NOPU)) == 0) PIN_PIN2_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 2);
				PIN_PIN2_INT;
			}
			if (pinSet & (1 << PIN_SETBIT_ENBL)) PIN_Control_Enbl |= (1 << 2);
			break;
#endif
#if defined(PIN_PIN3_BTN)
		case 3:
			PIN_PIN3_BTN; PIN_PIN3_OFF;
			if ((pinSet & (1 << PIN_SETBIT_BTN)) == 0) PIN_PIN3_LED;
			if ((pinSet & (1 << PIN_SETBIT_NOPU)) == 0) PIN_PIN3_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 3);
				PIN_PIN3_INT;
			}
			if (pinSet & (1 << PIN_SETBIT_ENBL)) PIN_Control_Enbl |= (1 << 3);
			break;
#endif
#if defined(PIN_PIN4_BTN)
		case 4:
			PIN_PIN4_BTN; PIN_PIN4_OFF;
			if ((pinSet & (1 << PIN_SETBIT_BTN)) == 0) PIN_PIN4_LED;
			if ((pinSet & (1 << PIN_SETBIT_NOPU)) == 0) PIN_PIN4_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 4);
				PIN_PIN4_INT;
			}
			if (pinSet & (1 << PIN_SETBIT_ENBL)) PIN_Control_Enbl |= (1 << 4);
			break;
#endif
#if defined(PIN_PIN5_BTN)
		case 5:
			PIN_PIN5_BTN; PIN_PIN5_OFF;
			if ((pinSet & (1 << PIN_SETBIT_BTN)) == 0) PIN_PIN5_LED;
			if ((pinSet & (1 << PIN_SETBIT_NOPU)) == 0) PIN_PIN5_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 5);
				PIN_PIN5_INT;
			}
			if (pinSet & (1 << PIN_SETBIT_ENBL)) PIN_Control_Enbl |= (1 << 5);
			break;
#endif
#if defined(PIN_PIN6_BTN)
		case 6:
			PIN_PIN6_BTN; PIN_PIN6_OFF;
			if ((pinSet & (1 << PIN_SETBIT_BTN)) == 0) PIN_PIN6_LED;
			if ((pinSet & (1 << PIN_SETBIT_NOPU)) == 0) PIN_PIN6_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 6);
				PIN_PIN6_INT;
			}
			if (pinSet & (1 << PIN_SETBIT_ENBL)) PIN_Control_Enbl |= (1 << 6);
			break;
#endif
#if defined(PIN_PIN7_BTN)
		case 7:
			PIN_PIN7_BTN; PIN_PIN7_OFF;
			if ((pinSet & (1 << PIN_SETBIT_BTN)) == 0) PIN_PIN7_LED;
			if ((pinSet & (1 << PIN_SETBIT_NOPU)) == 0) PIN_PIN7_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 7);
				PIN_PIN7_INT;
			}
			if (pinSet & (1 << PIN_SETBIT_ENBL)) PIN_Control_Enbl |= (1 << 7);
			break;
#endif
	}
	PIN_Control_SetBit[pinNum] = pinSet;
}

//=============================
//ボタン待ち時間設定
//	引数：ボタン安定までの待ち時間（ms）
void PIN_Control_SetWait(uint8_t wait)
{
	PIN_Control_Wait = wait;	//指定された待ち時間を設定
}

//=============================
//BTNがすべて離される／押されるのを待つ
//	引数：モード（0=OFF待ち／1=ON待ち）、対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_WaitKey(uint8_t waitmode, uint8_t waitmask)
{
	uint8_t WaitCount = 0;
	uint8_t WaitTarget = 0;
	
	waitmask = ~waitmask;
	if (waitmode == PIN_WAIT_ON) WaitTarget = waitmask;
	while(WaitCount < PIN_Control_Wait) {
		uint8_t nowPin = PIN_Control_GetKey() & waitmask;
		if (nowPin != WaitTarget) WaitCount = 0;
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
	
#if defined(PIN_PIN0_BTN)
	if ((PIN_Control_SetBit[0] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN0_DATA == 0) KeyNow |= 1 << 0;
#endif
#if defined(PIN_PIN1_BTN)
	if ((PIN_Control_SetBit[1] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN1_DATA == 0) KeyNow |= 1 << 1;
#endif
#if defined(PIN_PIN2_BTN)
	if ((PIN_Control_SetBit[2] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN2_DATA == 0) KeyNow |= 1 << 2;
#endif
#if defined(PIN_PIN3_BTN)
	if ((PIN_Control_SetBit[3] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN3_DATA == 0) KeyNow |= 1 << 3;
#endif
#if defined(PIN_PIN4_BTN)
	if ((PIN_Control_SetBit[4] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN4_DATA == 0) KeyNow |= 1 << 4;
#endif
#if defined(PIN_PIN5_BTN)
	if ((PIN_Control_SetBit[5] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN5_DATA == 0) KeyNow |= 1 << 5;
#endif
#if defined(PIN_PIN6_BTN)
	if ((PIN_Control_SetBit[6] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN6_DATA == 0) KeyNow |= 1 << 6;
#endif
#if defined(PIN_PIN7_BTN)
	if ((PIN_Control_SetBit[7] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN7_DATA == 0) KeyNow |= 1 << 7;
#endif
	return KeyNow & PIN_Control_Enbl;
}

//=============================
//LEDのON/OFF制御(ピンが出力設定の場合)
//	引数：PIN番号、0=OFF/1=ON
void PIN_Control_SetLED(uint8_t pinNum, uint8_t pinSet)
{
	switch(pinNum) {
#if defined(PIN_PIN0_BTN)
		case 0:
			if ((PIN_Control_SetBit[0] & (1 << PIN_SETBIT_BTN)) == 0) {
				if (pinSet) PIN_PIN0_ON;
				else PIN_PIN0_OFF;
			}
			break;
#endif
#if defined(PIN_PIN1_BTN)
		case 1:
			if ((PIN_Control_SetBit[1] & (1 << PIN_SETBIT_BTN)) == 0) {
				if (pinSet) PIN_PIN1_ON;
				else PIN_PIN1_OFF;
			}
			break;
#endif
#if defined(PIN_PIN2_BTN)
		case 2:
			if ((PIN_Control_SetBit[2] & (1 << PIN_SETBIT_BTN)) == 0) {
				if (pinSet) PIN_PIN2_ON;
				else PIN_PIN2_OFF;
			}
			break;
#endif
#if defined(PIN_PIN3_BTN)
		case 3:
			if ((PIN_Control_SetBit[3] & (1 << PIN_SETBIT_BTN)) == 0) {
				if (pinSet) PIN_PIN3_ON;
				else PIN_PIN3_OFF;
			}
			break;
#endif
#if defined(PIN_PIN4_BTN)
		case 4:
			if ((PIN_Control_SetBit[4] & (1 << PIN_SETBIT_BTN)) == 0) {
				if (pinSet) PIN_PIN4_ON;
				else PIN_PIN4_OFF;
			}
			break;
#endif
#if defined(PIN_PIN5_BTN)
		case 5:
			if ((PIN_Control_SetBit[5] & (1 << PIN_SETBIT_BTN)) == 0) {
				if (pinSet) PIN_PIN5_ON;
				else PIN_PIN5_OFF;
			}
			break;
#endif
#if defined(PIN_PIN6_BTN)
		case 6:
			if ((PIN_Control_SetBit[6] & (1 << PIN_SETBIT_BTN)) == 0) {
				if (pinSet) PIN_PIN6_ON;
				else PIN_PIN6_OFF;
			}
			break;
#endif
#if defined(PIN_PIN7_BTN)
		case 7:
			if ((PIN_Control_SetBit[7] & (1 << PIN_SETBIT_BTN)) == 0) {
				if (pinSet) PIN_PIN7_ON;
				else PIN_PIN7_OFF;
			}
			break;
#endif
	}
}

//=============================
//LEDの全クリア（初期設定に戻す）
void PIN_Control_ClearLED()
{
	for (uint8_t i = 0; i < _PIN_NUM_MAX; i++) {
		//LEDかどうかは呼び先の関数任せ
		PIN_Control_SetLED(i, 0);
		if ((PIN_Control_SetBit[i] & (1 << PIN_SETBIT_NOPU)) == 0) PIN_Control_SetLED(i, 1);
	}
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
	uint8_t nowBtn = PIN_Control_GetKey() & (~PIN_Control_Key);
	
	if (PWM_PCM_Playing == 0) PIN_Control_Key |= nowBtn;
	else if (nowBtn & PIN_Control_Int) {
		PWM_PCM_Playing = 0;
		PIN_Control_Key |= (nowBtn & PIN_Control_Int);
	}
}
