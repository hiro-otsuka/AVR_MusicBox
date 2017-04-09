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
 *        ※ BTN2/LED2/~PLAYを使う場合、Fuse設定で Reset を無効化する必要がある
 *        ※ _PWM_DEBUG を有効化する場合はBTN1が使えない
 *    BTN2/LED2/~PLAY --*------*-- VCC
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
//エラーチェック
#if defined(_PIN0_LED) && defined(_PIN0_BTN)
 #error "PIN0 LED and BTN"
#endif
#if defined(_PIN1_LED) && defined(_PIN1_BTN)
 #error "PIN1 LED and BTN"
#endif
#if defined(_PIN2_LED) && defined(_PIN2_BTN)
 #error "PIN2 LED and BTN"
#endif
#if defined(_PIN3_LED) && defined(_PIN3_BTN)
 #error "PIN3 LED and BTN"
#endif
#if defined(_PIN4_LED) && defined(_PIN4_BTN)
 #error "PIN4 LED and BTN"
#endif
#if defined(_PIN5_LED) && defined(_PIN5_BTN)
 #error "PIN5 LED and BTN"
#endif
#if defined(_PIN6_LED) && defined(_PIN6_BTN)
 #error "PIN6 LED and BTN"
#endif
#if defined(_PIN7_LED) && defined(_PIN7_BTN)
 #error "PIN7 LED and BTN"
#endif

//マイコン別定義(対応マイコンを増やす場合は編集が必要)
// ATTiny85系 -----------------
#if defined(__AVR_ATtiny25__) | \
	defined(__AVR_ATtiny45__) | \
	defined(__AVR_ATtiny85__) | \
	defined(__AVR_AT90Tiny26__) | \
	defined(__AVR_ATtiny26__)

//マイコン固有チェック
#if defined(_PIN3_BTN) || defined(_PIN3_LED)
 #error "This chip has no PIN3"
#endif
#if defined(_PIN4_BTN) || defined(_PIN4_LED)
 #error "This chip has no PIN4"
#endif
#if defined(_PIN5_BTN) || defined(_PIN5_LED)
 #error "This chip has no PIN5"
#endif
#if defined(_PIN6_BTN) || defined(_PIN6_LED)
 #error "This chip has no PIN6"
#endif
#if defined(_PIN7_BTN) || defined(_PIN7_LED)
 #error "This chip has no PIN7"
#endif

#if defined(_PWM_DEBUG) && ( defined(_PIN1_BTN) || defined(_PIN1_LED) )
 #error "PIN1 and PWM_DEBUG"
#endif

#if defined(_PLAYING_ENABLE) && ( defined(_PIN2_BTN) || defined(_PIN2_LED) )
 #error "PIN2 and PLAYING_ENABLE"
#endif

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
#if defined(_PIN0_BTN)
 #define PIN_BTN0_DDR		DDRB &= ~(1 << PINB1)
 #define PIN_BTN0_PORT		PORTB &= ~(1 << PINB1)
 #define PIN_BTN0_PIN		(PINB & (1 << PINB1))
 #define PIN_BTN0_INT		PCMSK |= (1 << PCINT1)
#endif
#if defined(_PIN1_BTN)
 #define PIN_BTN1_DDR		DDRB &= ~(1 << PINB3)
 #define PIN_BTN1_PORT		PORTB &= ~(1 << PINB3)
 #define PIN_BTN1_PIN		(PINB & (1 << PINB3))
 #define PIN_BTN1_INT		PCMSK |= (1 << PCINT3)
#endif
#if defined(_PIN2_BTN)
#define PIN_BTN2_DDR		DDRB &= ~(1 << PINB5)
#define PIN_BTN2_PORT		PORTB &= ~(1 << PINB5)
#define PIN_BTN2_PIN		(PINB & (1 << PINB5))
#define PIN_BTN2_INT		PCMSK |= (1 << PCINT5)
#endif

#if defined(_PIN0_LED)
 #define PIN_LED0_DDR		DDRB |= (1<<PINB1)
 #define PIN_LED0_ON		PORTB |= (1 << PINB1)
 #define PIN_LED0_OFF		PORTB &= ~(1 << PINB1)
#endif
#if defined(_PIN1_LED)
 #define PIN_LED1_DDR		DDRB |= (1<<PINB3)
 #define PIN_LED1_ON		PORTB |= (1 << PINB3)
 #define PIN_LED1_OFF		PORTB &= ~(1 << PINB3)
#endif
#if defined(_PIN2_LED)
#define PIN_LED1_DDR		DDRB |= (1<<PINB3)
#define PIN_LED1_ON			PORTB |= (1 << PINB3)
#define PIN_LED1_OFF		PORTB &= ~(1 << PINB3)
#endif

#endif

// ATTiny861系 -----------------
#if defined( __AVR_ATtiny261__ ) | \
	defined( __AVR_ATtiny461__ ) | \
	defined( __AVR_ATtiny861__ ) | \
	defined( __AVR_ATtiny261A__ ) | \
	defined( __AVR_ATtiny461A__ ) | \
	defined( __AVR_ATtiny861A__ )

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
#if defined(_PIN0_BTN)
 #define PIN_BTN0_DDR		DDRA &= ~(1 << PINA0)
 #define PIN_BTN0_PORT		PORTA &= ~(1 << PINA0)
 #define PIN_BTN0_PIN		(PINA & (1 << PINA0))
 #define PIN_BTN0_INT		PCMSK0 |= (1 << PCINT0)
#endif
#if defined(_PIN1_BTN)
 #define PIN_BTN1_DDR		DDRA &= ~(1 << PINA1)
 #define PIN_BTN1_PORT		PORTA &= ~(1 << PINA1)
 #define PIN_BTN1_PIN		(PINA & (1 << PINA1))
 #define PIN_BTN1_INT		PCMSK0 |= (1 << PCINT1)
#endif
#if defined(_PIN2_BTN)
 #define PIN_BTN2_DDR		DDRA &= ~(1 << PINA2)
 #define PIN_BTN2_PORT		PORTA &= ~(1 << PINA2)
 #define PIN_BTN2_PIN		(PINA & (1 << PINA2))
 #define PIN_BTN2_INT		PCMSK0 |= (1 << PCINT2)
#endif
#if defined(_PIN3_BTN)
 #define PIN_BTN3_DDR		DDRA &= ~(1 << PINA3)
 #define PIN_BTN3_PORT		PORTA &= ~(1 << PINA3)
 #define PIN_BTN3_PIN		(PINA & (1 << PINA3))
 #define PIN_BTN3_INT		PCMSK0 |= (1 << PCINT3)
#endif
#if defined(_PIN4_BTN)
 #define PIN_BTN4_DDR		DDRA &= ~(1 << PINA4)
 #define PIN_BTN4_PORT		PORTA &= ~(1 << PINA4)
 #define PIN_BTN4_PIN		(PINA & (1 << PINA4))
 #define PIN_BTN4_INT		PCMSK0 |= (1 << PCINT4)
#endif
#if defined(_PIN5_BTN)
 #define PIN_BTN5_DDR		DDRA &= ~(1 << PINA5)
 #define PIN_BTN5_PORT		PORTA &= ~(1 << PINA5)
 #define PIN_BTN5_PIN		(PINA & (1 << PINA5))
 #define PIN_BTN5_INT		PCMSK0 |= (1 << PCINT5)
#endif
#if defined(_PIN6_BTN)
 #define PIN_BTN6_DDR		DDRA &= ~(1 << PINA6)
 #define PIN_BTN6_PORT		PORTA &= ~(1 << PINA6)
 #define PIN_BTN6_PIN		(PINA & (1 << PINA6))
 #define PIN_BTN6_INT		PCMSK0 |= (1 << PCINT6)
#endif
#if defined(_PIN7_BTN)
 #define PIN_BTN7_DDR		DDRA &= ~(1 << PINA7)
 #define PIN_BTN7_PORT		PORTA &= ~(1 << PINA7)
 #define PIN_BTN7_PIN		(PINA & (1 << PINA7))
 #define PIN_BTN7_INT		PCMSK0 |= (1 << PCINT7)
#endif

#if defined(_PIN0_LED)
 #define PIN_LED0_DDR		DDRA |= (1<<PINA0)
 #define PIN_LED0_ON		PORTA |= (1 << PINA0)
 #define PIN_LED0_OFF		PORTA &= ~(1 << PINA0)
#endif
#if defined(_PIN1_LED)
 #define PIN_LED1_DDR		DDRA |= (1<<PINA1)
 #define PIN_LED1_ON		PORTA |= (1 << PINA1)
 #define PIN_LED1_OFF		PORTA &= ~(1 << PINA1)
#endif
#if defined(_PIN2_LED)
 #define PIN_LED2_DDR		DDRA |= (1<<PINA2)
 #define PIN_LED2_ON		PORTA |= (1 << PINA2)
 #define PIN_LED2_OFF		PORTA &= ~(1 << PINA2)
#endif
#if defined(_PIN3_LED)
 #define PIN_LED3_DDR		DDRA |= (1<<PINA3)
 #define PIN_LED3_ON		PORTA |= (1 << PINA3)
 #define PIN_LED3_OFF		PORTA &= ~(1 << PINA3)
#endif
#if defined(_PIN4_LED)
 #define PIN_LED4_DDR		DDRA |= (1<<PINA4)
 #define PIN_LED4_ON		PORTA |= (1 << PINA4)
 #define PIN_LED4_OFF		PORTA &= ~(1 << PINA4)
#endif
#if defined(_PIN5_LED)
 #define PIN_LED5_DDR		DDRA |= (1<<PINA5)
 #define PIN_LED5_ON		PORTA |= (1 << PINA5)
 #define PIN_LED5_OFF		PORTA &= ~(1 << PINA5)
#endif
#if defined(_PIN6_LED)
 #define PIN_LED6_DDR		DDRA |= (1<<PINA6)
 #define PIN_LED6_ON		PORTA |= (1 << PINA6)
 #define PIN_LED6_OFF		PORTA &= ~(1 << PINA6)
#endif
#if defined(_PIN7_LED)
 #define PIN_LED7_DDR		DDRA |= (1<<PINA7)
 #define PIN_LED7_ON		PORTA |= (1 << PINA7)
 #define PIN_LED7_OFF		PORTA &= ~(1 << PINA7)
#endif

#endif

//********** 変数定義 **********//
//BTNの押下状態を参照可能（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
extern volatile uint8_t PIN_Control_Key;

//********** 関数定義 **********//
//ピンアサインと割り込み処理の初期化
void PIN_Control_Init();

//ボタン待ち時間設定
//	引数：ボタン安定までの待ち時間（ms）
void PIN_Control_SetWait(uint8_t);

//BTNがすべて離されるのを待つ
//	引数：対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_WaitKeyOff(uint8_t );

//BTNがすべて押されるのを待つ
//	引数：対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_WaitKeyOn(uint8_t );

//BTNのその瞬間の押下状態を調べる
//	戻値：BTNの押下状態（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
volatile uint8_t PIN_Control_GetKey();

//外部のオペアンプをON/OFF制御(ピンが有効な場合)
//	引数：0=OFF/1=ON
void PIN_Control_Playing(uint8_t);

#endif /* PIN_Control_H_ */