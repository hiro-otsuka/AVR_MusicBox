/*
 * PIN_Control_PUI.c
 *
 * 概要：
 *  プロジェクトごとに定義されたピンアサイン情報に従って、
 *  割り込み処理やキー入力の管理を行うモジュール
 *
 * 使用方法等：
 *  PIN_Control_PUI を使用するプロジェクトにインポートして使用
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/04/06 新規作成(Hiro OTSUKA)
 *  2017/04/09 構成変更(Hiro OTSUKA) 初期化処理を整理し、待ち時間設定を可変に
 *
 */

#include <avr/interrupt.h>
#include "PIN_Control_PUI.h"

//********** 変数定義 **********//
//BTNの押下状態を参照可能（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
volatile uint16_t PIN_Control_PUI_Key;
volatile uint8_t  PIN_Control_PUI_Wait_Boot;
volatile uint8_t  PIN_Control_PUI_Wait_Btn;
volatile uint8_t  PIN_Control_PUI_Wait_Serial;

//********** 外部関数 **********//
//ピンアサインと割り込み処理の初期化
//=============================
void PIN_Control_PUI_Init()
{
	PIN_Control_PUI_Key = 0;
	PIN_Control_PUI_Wait_Boot = 100;	//待ち時間は仮で 100ms に設定
	PIN_Control_PUI_Wait_Btn = 100;		//待ち時間は仮で 100ms に設定
	PIN_Control_PUI_Wait_Serial = 100;	//待ち時間は仮で 100ms に設定

	//ピンアサインの初期化
#if defined(PIN_BTN0_DDR)
	PIN_BTN0_DDR; PIN_BTN0_PORT;
#elif defined(PIN_LED0_DDR)
	PIN_LED0_DDR; PIN_LED0_OFF;
#endif

#if defined(PIN_BTN1_DDR)
	PIN_BTN1_DDR; PIN_BTN1_PORT;
#elif defined(PIN_LED1_DDR)
	PIN_LED1_DDR; PIN_LED1_OFF;
#endif

#if defined(PIN_BTN2_DDR)
	PIN_BTN2_DDR; PIN_BTN2_PORT;
#elif defined(PIN_LED2_DDR)
	PIN_LED2_DDR; PIN_LED2_OFF;
#endif

#if defined(PIN_BTN3_DDR)
	PIN_BTN3_DDR; PIN_BTN3_PORT;
#elif defined(PIN_LED3_DDR)
	PIN_LED3_DDR; PIN_LED3_OFF;
#endif

#if defined(PIN_BTN4_DDR)
	PIN_BTN4_DDR; PIN_BTN4_PORT;
#elif defined(PIN_LED4_DDR)
	PIN_LED4_DDR; PIN_LED4_OFF;
#endif

#if defined(PIN_BTN5_DDR)
	PIN_BTN5_DDR; PIN_BTN5_PORT;
#elif defined(PIN_LED5_DDR)
	PIN_LED5_DDR; PIN_LED5_OFF;
#endif

#if defined(PIN_BTN6_DDR)
	PIN_BTN6_DDR; PIN_BTN6_PORT;
#elif defined(PIN_LED6_DDR)
	PIN_LED6_DDR; PIN_LED6_OFF;
#endif

#if defined(PIN_BTN7_DDR)
	PIN_BTN7_DDR; PIN_BTN7_PORT;
#elif defined(PIN_LED7_DDR)
	PIN_LED7_DDR; PIN_LED7_OFF;
#endif

#if defined(PIN_BTN8_DDR)
	PIN_BTN8_DDR; PIN_BTN8_PORT;
#elif defined(PIN_LED8_DDR)
	PIN_LED8_DDR; PIN_LED8_OFF;
#endif

	PIN_SERIAL0_DDR; PIN_SERIAL0_OFF;
	PIN_SERIAL1_DDR; PIN_SERIAL1_OFF;
	
	PIN_ISPLAYING_DDR;
	
	//割り込み処理を初期化
	PIN_CHANGE_INT_INIT;
	PIN_CHANGE_INT_ENBL;

#if defined(PIN_BTN0_INT)
	PIN_BTN0_INT;
#endif
#if defined(PIN_BTN1_INT)
	PIN_BTN1_INT;
#endif
#if defined(PIN_BTN2_INT)
	PIN_BTN2_INT;
#endif
#if defined(PIN_BTN3_INT)
	PIN_BTN3_INT;
#endif
#if defined(PIN_BTN4_INT)
	PIN_BTN4_INT;
#endif
#if defined(PIN_BTN5_INT)
	PIN_BTN5_INT;
#endif
#if defined(PIN_BTN6_INT)
	PIN_BTN6_INT;
#endif
#if defined(PIN_BTN7_INT)
	PIN_BTN7_INT;
#endif
#if defined(PIN_BTN8_INT)
	PIN_BTN8_INT;
#endif
}

//=============================
//ボタン待ち時間設定
//	引数：ボタン安定までの待ち時間（ms）、再生モジュールの起動時間（ms）、シリアル通信の待ち時間（ms）
void PIN_Control_PUI_SetWait(uint8_t wait, uint8_t boot, uint8_t serial)
{
	PIN_Control_PUI_Wait_Btn = wait;		//指定された待ち時間を設定
	PIN_Control_PUI_Wait_Boot = boot;		//指定された待ち時間を設定
	PIN_Control_PUI_Wait_Serial = serial;	//指定された待ち時間を設定
}

//=============================
//BTNがすべて離されるのを待つ
//	引数：対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_PUI_WaitKeyOff(uint16_t waitmask)
{
	uint8_t WaitCount;
	
	WaitCount = 0;
	while(WaitCount < PIN_Control_PUI_Wait_Btn) {
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
#if defined(PIN_BTN8_DDR)
		|| (PIN_BTN8_PIN == 0 && (waitmask & 1 << 8) == 0)
#endif
		) WaitCount = 0;
		else WaitCount ++;
		_delay_ms(1);
	}
}

//=============================
//BTNがすべて押されるのを待つ
//	引数：対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_PUI_WaitKeyOn(uint16_t waitmask)
{
	uint8_t WaitCount;
	
	WaitCount = 0;
	while(WaitCount < PIN_Control_PUI_Wait_Btn) {
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
#if defined(PIN_BTN8_DDR)
		|| (PIN_BTN8_PIN == 1 && (waitmask & 1 << 8) == 0)
#endif
		) WaitCount = 0;
		else WaitCount ++;
		_delay_ms(1);
	}
}

//=============================
//BTNのその瞬間の押下状態を調べる
//	戻値：BTNの押下状態（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
volatile uint16_t PIN_Control_PUI_GetKey()
{
	uint16_t KeyNow = 0;
	
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
#if defined(PIN_BTN8_DDR)
	if (PIN_BTN8_PIN == 0) KeyNow |= 1 << 8;
#endif
	return KeyNow;
}

//=============================
//外部の再生モジュールにシリアル通信
//	引数：送信する数値
void PIN_Control_PUI_Serial(uint8_t number)
{
	//通信開始
	PIN_SERIAL1_OFF;
	PIN_SERIAL0_ON;
	for(uint8_t i = 0; i < PIN_Control_PUI_Wait_Boot; i++) _delay_ms(1);
	for(uint8_t i = 0; i < (PIN_Control_PUI_Wait_Serial << 1); i++) _delay_ms(1);
	//数値を送信
	for (uint8_t i = 0; i < number; i++) {
		PIN_SERIAL1_ON;
		for(uint8_t i = 0; i < (PIN_Control_PUI_Wait_Serial << 1); i++) _delay_ms(1);
		PIN_SERIAL1_OFF;
		for(uint8_t i = 0; i < (PIN_Control_PUI_Wait_Serial << 1); i++) _delay_ms(1);
	}
	//通信終了
	PIN_SERIAL0_OFF;
	for(uint8_t i = 0; i < (PIN_Control_PUI_Wait_Serial << 1); i++) _delay_ms(1);
	for(uint8_t i = 0; i < PIN_Control_PUI_Wait_Boot; i++) _delay_ms(1);
}

//=============================
//外部の再生モジュールのON/OFFを取得
//	戻値：0=OFF/1=ON
uint8_t PIN_Control_PUI_isPlaying()
{
	if (PIN_ISPLAYING) return 0;
	return 1;
}

//********** 割込関数 **********//
//+++++++++++++++++++++++++++++
//ピン変化割り込みによるキー入力受付
ISR(PIN_CHANGE_INT_VECT)
{
#if defined(PIN_BTN0_DDR)
	if (PIN_BTN0_PIN == 0)  {
		PIN_Control_PUI_Key |= (1 << 0);
	}
#endif
#if defined(PIN_BTN1_DDR)
	if (PIN_BTN1_PIN == 0)  {
		PIN_Control_PUI_Key |= (1 << 1);
	}
#endif
#if defined(PIN_BTN2_DDR)
	if (PIN_BTN2_PIN == 0)  {
		PIN_Control_PUI_Key |= (1 << 2);
	}
#endif
#if defined(PIN_BTN3_DDR)
	if (PIN_BTN3_PIN == 0)  {
		PIN_Control_PUI_Key |= (1 << 3);
	}
#endif
#if defined(PIN_BTN4_DDR)
	if (PIN_BTN4_PIN == 0)  {
		PIN_Control_PUI_Key |= (1 << 4);
	}
#endif
#if defined(PIN_BTN5_DDR)
	if (PIN_BTN5_PIN == 0)  {
		PIN_Control_PUI_Key |= (1 << 5);
	}
#endif
#if defined(PIN_BTN6_DDR)
	if (PIN_BTN6_PIN == 0)  {
		PIN_Control_PUI_Key |= (1 << 6);
	}
#endif
#if defined(PIN_BTN7_DDR)
	if (PIN_BTN7_PIN == 0)  {
		PIN_Control_PUI_Key |= (1 << 7);
	}
#endif
#if defined(PIN_BTN8_DDR)
	if (PIN_BTN8_PIN == 0)  {
		PIN_Control_PUI_Key |= (1 << 8);
	}
#endif
}
