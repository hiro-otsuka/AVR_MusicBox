/*
 * PIN_Control.h
 *
 * 概要：
 *  PIN_Control モジュールのための各種定義
 *
 * 使用方法等：
 *  PWM_MusicBox, PWM_PCMPlay_EEPROM 等を使用するモジュールから include
 *
 *  定義済みマイコンとピンアサイン
 *    ※定義すればほかのマイコンでも動作する可能性があるが、PLL等で16MHz以上の速度が必要
 *
 *   ATTiny85
 *        ※ ~PLAYを使う場合、Fuse設定で Reset を無効化する必要がある
 *        ※ _PWM_DEBUG を有効化する場合はBTN1が使えない
 *        ~PLAY/RESET --*------*-- VCC
 *    BTN1/LED1/DEBUG --|      |-- I2C-SCL
 *              AUDIO --|      |-- BTN0/LED0
 *                GND --*------*-- I2C-SDA
 *
 *   ATTiny861
 *            I2C-SDA --*------*-- BTN0/LED0
 *              DEBUG --|      |-- BTN1/LED1
 *            I2C-SCL --|      |-- BTN2/LED2
 *              AUDIO --|      |-- BTN3/LED3
 *                VCC --|      |-- (AGND)
 *                GND --|      |-- (AVCC)
 *            (XTAL1) --|      |-- BTN4/LED4
 *            (XTAL2) --|      |-- BTN5/LED5
 *              ~PLAY --|      |-- BTN6/LED6
 *              RESET --*------*-- BTN7/LED7
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

#ifndef PIN_Control_H_
#define PIN_Control_H_

//********** 読み込み **********//
#include "config.h"	//プロジェクトごとのピン使用定義が必要
#include <util/delay.h>
#include <avr/io.h>
#include "PWM_PCMPlay.h"

//********** 定数定義 **********//
//共通定義
// ピン設定用のビット番号
#define PIN_SETBIT_BTN	0	// 1=BTN / 0=LED
#define PIN_SETBIT_NOPU	1	// 1="NO"PullUp / 0=PullUP
#define PIN_SETBIT_INT	2	// 1=Interrupt / 0="NO"Interrupt
#define PIN_SETBIT_ENBL	3	// 1=Enable / 0=Disable
#define PIN_SETBIT_DEF	0b11110000	//デフォルトはすべて 1

#define PIN_WAIT_OFF	0
#define PIN_WAIT_ON		1

//マイコン別定義(対応マイコンを増やす場合は編集が必要)
// ATTiny85系 -----------------
#if defined(__AVR_ATtiny25__) | \
	defined(__AVR_ATtiny45__) | \
	defined(__AVR_ATtiny85__) | \
	defined(__AVR_AT90Tiny26__) | \
	defined(__AVR_ATtiny26__)

#define _PIN_NUM_MAX	2

//for PCINT0
#define PIN_CHANGE_INT_ENBL	GIMSK |= (1 << PCIE)
#define PIN_CHANGE_INT_INIT	PCMSK=0;
#define PIN_CHANGE_INT_VECT	PCINT0_vect

//for CTRL-Amp
#define PIN_PLAYING_DDR			DDRB |= (1<<PINB5)
#define PIN_PLAYING_OFF			PORTB |= (1<<PINB5)
#define PIN_PLAYING_ON			PORTB &= ~(1<<PINB5)

//for TIM0(PWM_MusicBox)
#define TIM0_COUNT	TCNT0
#define TIM0_TCCR0A (1 << WGM01) | (1 << WGM00)

//for PWM(PWM_PCMPlay)
#define PWM_PIN_OUT		PB4
#define PWM_DEBUG_OUT	PB3
#define PWM_SPEED_32000	TCCR1 = (1 << CTC1) | (0 << CS13) | (0 << CS12) | (1 << CS11)	| (0 << CS10);	//=CK/2,FREQ=32000
#define PWM_SPEED_16000	TCCR1 = (1 << CTC1) | (0 << CS13) | (0 << CS12) | (1 << CS11)	| (1 << CS10);	//=CK/4,FREQ=16000
#define PWM_SPEED_8000	TCCR1 = (1 << CTC1) | (0 << CS13) | (1 << CS12) | (0 << CS11)	| (0 << CS10);	//=CK/8,FREQ= 8000
#define PWM_TIM1B_ON	GTCCR = (1 << PWM1B) | (1 << COM1B1) | (0 << COM1B0);

//for Physical User Interface
#define PIN_PIN0_BTN		DDRB &= ~(1 << PINB1)
#define PIN_PIN0_LED		DDRB |= (1<<PINB1)
#define PIN_PIN0_ON			PORTB |= (1 << PINB1)	//Pull Up
#define PIN_PIN0_OFF		PORTB &= ~(1 << PINB1)	//not Pull Up
#define PIN_PIN0_DATA		(PINB & (1 << PINB1))
#define PIN_PIN0_INT		PCMSK |= (1 << PCINT1)

#define PIN_PIN1_BTN		DDRB &= ~(1 << PINB3)
#define PIN_PIN1_LED		DDRB |= (1<<PINB3)
#define PIN_PIN1_ON			PORTB |= (1 << PINB3)	//Pull Up
#define PIN_PIN1_OFF		PORTB &= ~(1 << PINB3)	//not Pull Up
#define PIN_PIN1_DATA		(PINB & (1 << PINB3))
#define PIN_PIN1_INT		PCMSK |= (1 << PCINT3)

//デバッグピン使用の場合はPIN1が無効化される
#ifdef _PWM_DEBUG
#warning "PIN1 disabled!"
#undef PIN_PIN1_BTN
#undef PIN_PIN1_LED
#undef PIN_PIN1_ON
#undef PIN_PIN1_OFF
#undef PIN_PIN1_DATA
#undef PIN_PIN1_INT
#endif

#endif

// ATTiny861系 -----------------
#if defined( __AVR_ATtiny261__ ) | \
	defined( __AVR_ATtiny461__ ) | \
	defined( __AVR_ATtiny861__ ) | \
	defined( __AVR_ATtiny261A__ ) | \
	defined( __AVR_ATtiny461A__ ) | \
	defined( __AVR_ATtiny861A__ )

#define _PIN_NUM_MAX	8

//for PCINT0
#define PIN_CHANGE_INT_ENBL	GIMSK |= (1 << PCIE1)	//PCINT0-7 and PCINT12-15
#define PIN_CHANGE_INT_INIT	PCMSK0=0;PCMSK1=0;
#define PIN_CHANGE_INT_VECT	PCINT_vect

//for CTRL-Amp
#define PIN_PLAYING_DDR			DDRB |= (1<<PINB6)
#define PIN_PLAYING_OFF			PORTB |= (1<<PINB6)
#define PIN_PLAYING_ON			PORTB &= ~(1<<PINB6)

//for TIM0(PWM_MusicBox)
#define TIM0_COUNT	TCNT0L
#define TIM0_TCCR0A (0 << ICEN0) | (0 << TCW0) | (1 << WGM00)

//for PWM(PWM_PCMPlay)
#define PWM_PIN_OUT	PB3
#define PWM_DEBUG_OUT	PB1
#define PWM_SPEED_32000	TCCR1B = (0 << CS13) | (0 << CS12) | (1 << CS11)	| (0 << CS10);	//=CK/2,FREQ=32000
#define PWM_SPEED_16000	TCCR1B = (0 << CS13) | (0 << CS12) | (1 << CS11)	| (1 << CS10);	//=CK/4,FREQ=16000
#define PWM_SPEED_8000	TCCR1B = (0 << CS13) | (1 << CS12) | (0 << CS11)	| (0 << CS10);	//=CK/8,FREQ= 8000
#define PWM_TIM1B_ON	TCCR1A = (1 << PWM1B) | (1 << COM1B1) | (0 << COM1B0);

//for Physical User Interface
#define PIN_PIN0_BTN		DDRA &= ~(1 << PINA0)
#define PIN_PIN0_LED		DDRA |= (1<<PINA0)
#define PIN_PIN0_ON			PORTA |= (1 << PINA0)	//Pull Up
#define PIN_PIN0_OFF		PORTA &= ~(1 << PINA0)	//not Pull Up
#define PIN_PIN0_DATA		(PINA & (1 << PINA0))
#define PIN_PIN0_INT		PCMSK0 |= (1 << PCINT0)

#define PIN_PIN1_BTN		DDRA &= ~(1 << PINA1)
#define PIN_PIN1_LED		DDRA |= (1<<PINA1)
#define PIN_PIN1_ON			PORTA |= (1 << PINA1)	//Pull Up
#define PIN_PIN1_OFF		PORTA &= ~(1 << PINA1)	//not Pull Up
#define PIN_PIN1_DATA		(PINA & (1 << PINA1))
#define PIN_PIN1_INT		PCMSK0 |= (1 << PCINT1)

#define PIN_PIN2_BTN		DDRA &= ~(1 << PINA2)
#define PIN_PIN2_LED		DDRA |= (1<<PINA2)
#define PIN_PIN2_ON			PORTA |= (1 << PINA2)	//Pull Up
#define PIN_PIN2_OFF		PORTA &= ~(1 << PINA2)	//not Pull Up
#define PIN_PIN2_DATA		(PINA & (1 << PINA2))
#define PIN_PIN2_INT		PCMSK0 |= (1 << PCINT2)

#define PIN_PIN3_BTN		DDRA &= ~(1 << PINA3)
#define PIN_PIN3_LED		DDRA |= (1<<PINA3)
#define PIN_PIN3_ON			PORTA |= (1 << PINA3)	//Pull Up
#define PIN_PIN3_OFF		PORTA &= ~(1 << PINA3)	//not Pull Up
#define PIN_PIN3_DATA		(PINA & (1 << PINA3))
#define PIN_PIN3_INT		PCMSK0 |= (1 << PCINT3)

#define PIN_PIN4_BTN		DDRA &= ~(1 << PINA4)
#define PIN_PIN4_LED		DDRA |= (1<<PINA4)
#define PIN_PIN4_ON			PORTA |= (1 << PINA4)	//Pull Up
#define PIN_PIN4_OFF		PORTA &= ~(1 << PINA4)	//not Pull Up
#define PIN_PIN4_DATA		(PINA & (1 << PINA4))
#define PIN_PIN4_INT		PCMSK0 |= (1 << PCINT4)

#define PIN_PIN5_BTN		DDRA &= ~(1 << PINA5)
#define PIN_PIN5_LED		DDRA |= (1<<PINA5)
#define PIN_PIN5_ON			PORTA |= (1 << PINA5)	//Pull Up
#define PIN_PIN5_OFF		PORTA &= ~(1 << PINA5)	//not Pull Up
#define PIN_PIN5_DATA		(PINA & (1 << PINA5))
#define PIN_PIN5_INT		PCMSK0 |= (1 << PCINT5)

#define PIN_PIN6_BTN		DDRA &= ~(1 << PINA6)
#define PIN_PIN6_LED		DDRA |= (1<<PINA6)
#define PIN_PIN6_ON			PORTA |= (1 << PINA6)	//Pull Up
#define PIN_PIN6_OFF		PORTA &= ~(1 << PINA6)	//not Pull Up
#define PIN_PIN6_DATA		(PINA & (1 << PINA6))
#define PIN_PIN6_INT		PCMSK0 |= (1 << PCINT6)

#define PIN_PIN7_BTN		DDRA &= ~(1 << PINA7)
#define PIN_PIN7_LED		DDRA |= (1<<PINA7)
#define PIN_PIN7_ON			PORTA |= (1 << PINA7)	//Pull Up
#define PIN_PIN7_OFF		PORTA &= ~(1 << PINA7)	//not Pull Up
#define PIN_PIN7_DATA		(PINA & (1 << PINA7))
#define PIN_PIN7_INT		PCMSK0 |= (1 << PCINT7)

#endif

//********** 変数定義 **********//
//BTNの押下状態を参照可能（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
extern volatile uint8_t PIN_Control_Key;
extern volatile uint8_t PIN_Control_SetBit[];

//********** 関数定義 **********//
//ピンアサインと割り込み処理の初期化
void PIN_Control_Init();

//各ピンのI/O設定
//	引数：ピン番号、設定（ビット設定は PIN_SETBIT_* 定数を参照）
void PIN_Control_SetIO(uint8_t, uint8_t);

//ボタン待ち時間設定
//	引数：ボタン安定までの待ち時間（ms）
void PIN_Control_SetWait(uint8_t);

//BTNがすべて離される／押されるのを待つ
//	引数：モード（0=OFF待ち／1=ON待ち）、対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_WaitKey(uint8_t, uint8_t );

//BTNのその瞬間の押下状態を調べる
//	戻値：BTNの押下状態（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
volatile uint8_t PIN_Control_GetKey();

//外部のオペアンプをON/OFF制御(ピンが有効な場合)
//	引数：0=OFF/1=ON
void PIN_Control_Playing(uint8_t);

//LEDのON/OFF制御(ピンが出力設定の場合)
//	引数：PIN番号、0=OFF/1=ON
void PIN_Control_SetLED(uint8_t, uint8_t);

//LEDの全クリア（初期設定に戻す）
void PIN_Control_ClearLED();

#endif /* PIN_Control_H_ */