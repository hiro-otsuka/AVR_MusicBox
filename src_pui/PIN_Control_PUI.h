/*
 * PIN_Control_PUI.h
 *
 * 概要：
 *  PIN_Control_PUI モジュールのための各種定義
 *
 * 使用方法等：
 *  PIN_Control_PUI を使用するモジュールから include
 *
 *  定義済みマイコンとピンアサイン
 *
 *   ATTiny861
 *       SERIAL0/MOSI --*------*-- BTN0/LED0
 *       ~isPLAY/MISO --|      |-- BTN1/LED1
 *       SERIAL1/USCK --|      |-- BTN2/LED2
 *        BTN11/LED10 --|      |-- BTN3/LED3
 *                VCC --|      |-- (AGND)
 *                GND --|      |-- (AVCC)
 *  BTN10/LED10/XTAL1 --|      |-- BTN4/LED4
 *    BTN9/LED9/XTAL2 --|      |-- BTN5/LED5
 *          BTN8/LED8 --|      |-- BTN6/LED6
 *             ~RESET --*------*-- BTN7/LED7
 *
 *   ATTiny2313
 *             ~RESET --*------*-- VCC
 *            SERIAL0 --|      |-- USCK/BTN7/LED7
 *            SERIAL1 --|      |-- MISO/BTN6/LED6
 *              XTAL2 --|      |-- MOSI/BTN5/LED5
 *              XTAL1 --|      |-- BTN4/LED4
 *                    --|      |-- BTN3/LED3
 *                    --|      |-- BTN2/LED2
 *            ~isPLAY --|      |-- BTN1/LED1
 *                    --|      |-- BTN0/LED0
 *                GND --*------*-- 
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

#ifndef PIN_Control_PUI_H_
#define PIN_Control_PUI_H_

//********** 読み込み **********//
#include "config.h"	//プロジェクトごとのピン使用定義が必要
#include <util/delay.h>
#include <avr/io.h>

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
#if defined(_PIN8_LED) && defined(_PIN8_BTN)
 #error "PIN8 LED and BTN"
#endif
#if defined(_PIN9_LED) && defined(_PIN9_BTN)
 #error "PIN9 LED and BTN"
#endif
#if defined(_PIN10_LED) && defined(_PIN10_BTN)
 #error "PIN10 LED and BTN"
#endif
#if defined(_PIN11_LED) && defined(_PIN11_BTN)
 #error "PIN11 LED and BTN"
#endif

//マイコン別定義(対応マイコンを増やす場合は編集が必要)

// ATTiny861系 -----------------
#if defined( __AVR_ATtiny261__ ) | \
	defined( __AVR_ATtiny461__ ) | \
	defined( __AVR_ATtiny861__ ) | \
	defined( __AVR_ATtiny261A__ ) | \
	defined( __AVR_ATtiny461A__ ) | \
	defined( __AVR_ATtiny861A__ )

//for PCINT0
#define PIN_CHANGE_INT_ENBL	GIMSK |= (1 << PCIE1) | (1 << PCIE0);	//PCINT 0-7,12-15 | PCINT 8-11
#define PIN_CHANGE_INT_INIT	PCMSK0=0;PCMSK1=0;
#define PIN_CHANGE_INT_VECT	PCINT_vect

//for DETECT Playing
#define PIN_ISPLAYING_DDR		DDRB &= ~(1<<PINB1)
#define PIN_ISPLAYING			(PINB & (1 << PINB1))

//for Serial
#define PIN_SERIAL0_DDR		DDRB |= (1 << PINB0)
#define PIN_SERIAL0_OFF		PORTB |= (1 << PINB0)
#define PIN_SERIAL0_ON		PORTB &= ~(1 << PINB0)
#define PIN_SERIAL1_DDR		DDRB |= (1 << PINB2)
#define PIN_SERIAL1_OFF		PORTB |= (1 << PINB2)
#define PIN_SERIAL1_ON		PORTB &= ~(1 << PINB2)

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
#if defined(_PIN8_BTN)
 #define PIN_BTN8_DDR		DDRB &= ~(1 << PINB6)
 #define PIN_BTN8_PORT		PORTB &= ~(1 << PINB6)
 #define PIN_BTN8_PIN		(PINB & (1 << PINB6))
 #define PIN_BTN8_INT		PCMSK1 |= (1 << PCINT14)
#endif
#if defined(_PIN9_BTN)
 #define PIN_BTN9_DDR		DDRB &= ~(1 << PINB5)
 #define PIN_BTN9_PORT		PORTB &= ~(1 << PINB5)
 #define PIN_BTN9_PIN		(PINB & (1 << PINB5))
 #define PIN_BTN9_INT		PCMSK1 |= (1 << PCINT13)
#endif
#if defined(_PIN10_BTN)
 #define PIN_BTN10_DDR		DDRB &= ~(1 << PINB4)
 #define PIN_BTN10_PORT		PORTB &= ~(1 << PINB4)
 #define PIN_BTN10_PIN		(PINB & (1 << PINB4))
 #define PIN_BTN10_INT		PCMSK1 |= (1 << PCINT12)
#endif
#if defined(_PIN11_BTN)
 #define PIN_BTN11_DDR		DDRB &= ~(1 << PINB3)
 #define PIN_BTN11_PORT		PORTB &= ~(1 << PINB3)
 #define PIN_BTN11_PIN		(PINB & (1 << PINB3))
 #define PIN_BTN11_INT		PCMSK1 |= (1 << PCINT11)
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
#if defined(_PIN8_LED)
 #define PIN_LED8_DDR		DDRB |= (1<<PINB6)
 #define PIN_LED8_ON		PORTB |= (1 << PINB6)
 #define PIN_LED8_OFF		PORTB &= ~(1 << PINB6)
#endif
#if defined(_PIN9_LED)
 #define PIN_LED9_DDR		DDRB |= (1<<PINB5)
 #define PIN_LED9_ON		PORTB |= (1 << PINB5)
 #define PIN_LED9_OFF		PORTB &= ~(1 << PINB5)
#endif
#if defined(_PIN10_LED)
 #define PIN_LED10_DDR		DDRB |= (1<<PINB4)
 #define PIN_LED10_ON		PORTB |= (1 << PINB4)
 #define PIN_LED10_OFF		PORTB &= ~(1 << PINB4)
#endif
#if defined(_PIN11_LED)
 #define PIN_LED11_DDR		DDRB |= (1<<PINB3)
 #define PIN_LED11_ON		PORTB |= (1 << PINB3)
 #define PIN_LED11_OFF		PORTB &= ~(1 << PINB3)
#endif

#endif

// ATTiny2313系 -----------------
#if defined(__AVR_AT90Tiny2313__) | \
	defined(__AVR_ATtiny2313__)

//マイコン固有チェック
#if defined(_PIN8_BTN) || defined(_PIN8_LED)
 #error "This chip has no PIN8"
#endif
#if defined(_PIN9_BTN) || defined(_PIN9_LED)
 #error "This chip has no PIN9"
#endif
#if defined(_PIN10_BTN) || defined(_PIN10_LED)
 #error "This chip has no PIN10"
#endif
#if defined(_PIN11_BTN) || defined(_PIN11_LED)
 #error "This chip has no PIN11"
#endif

//for PCINT0
#define PIN_CHANGE_INT_ENBL	GIMSK |= (1 << PCIE);	//PCINT 0-7
#define PIN_CHANGE_INT_INIT	PCMSK=0;
#define PIN_CHANGE_INT_VECT	PCINT_vect

//for DETECT Playing
#define PIN_ISPLAYING_DDR		DDRD &= ~(1<<PIND4)
#define PIN_ISPLAYING			(PIND & (1 << PIND4))

//for Serial
#define PIN_SERIAL0_DDR		DDRD |= (1 << PIND0)
#define PIN_SERIAL0_OFF		PORTD |= (1 << PIND0)
#define PIN_SERIAL0_ON		PORTD &= ~(1 << PIND0)
#define PIN_SERIAL1_DDR		DDRD |= (1 << PIND1)
#define PIN_SERIAL1_OFF		PORTD |= (1 << PIND1)
#define PIN_SERIAL1_ON		PORTD &= ~(1 << PIND1)

//for Physical User Interface
#if defined(_PIN0_BTN)
 #define PIN_BTN0_DDR		DDRB &= ~(1 << PINB0)
 #define PIN_BTN0_PORT		PORTB &= ~(1 << PINB0)
 #define PIN_BTN0_PIN		(PINB & (1 << PINB0))
 #define PIN_BTN0_INT		PCMSK |= (1 << PCINT0)
#endif
#if defined(_PIN1_BTN)
 #define PIN_BTN1_DDR		DDRB &= ~(1 << PINB1)
 #define PIN_BTN1_PORT		PORTB &= ~(1 << PINB1)
 #define PIN_BTN1_PIN		(PINB & (1 << PINB1))
 #define PIN_BTN1_INT		PCMSK |= (1 << PCINT1)
#endif
#if defined(_PIN2_BTN)
 #define PIN_BTN2_DDR		DDRB &= ~(1 << PINB2)
 #define PIN_BTN2_PORT		PORTB &= ~(1 << PINB2)
 #define PIN_BTN2_PIN		(PINB & (1 << PINB2))
 #define PIN_BTN2_INT		PCMSK |= (1 << PCINT2)
#endif
#if defined(_PIN3_BTN)
 #define PIN_BTN3_DDR		DDRB &= ~(1 << PINB3)
 #define PIN_BTN3_PORT		PORTB &= ~(1 << PINB3)
 #define PIN_BTN3_PIN		(PINB & (1 << PINB3))
 #define PIN_BTN3_INT		PCMSK |= (1 << PCINT3)
#endif
#if defined(_PIN4_BTN)
 #define PIN_BTN4_DDR		DDRB &= ~(1 << PINB4)
 #define PIN_BTN4_PORT		PORTB &= ~(1 << PINB4)
 #define PIN_BTN4_PIN		(PINB & (1 << PINB4))
 #define PIN_BTN4_INT		PCMSK |= (1 << PCINT4)
#endif
#if defined(_PIN5_BTN)
 #define PIN_BTN5_DDR		DDRB &= ~(1 << PINB5)
 #define PIN_BTN5_PORT		PORTB &= ~(1 << PINB5)
 #define PIN_BTN5_PIN		(PINB & (1 << PINB5))
 #define PIN_BTN5_INT		PCMSK |= (1 << PCINT5)
#endif
#if defined(_PIN6_BTN)
 #define PIN_BTN6_DDR		DDRB &= ~(1 << PINB6)
 #define PIN_BTN6_PORT		PORTB &= ~(1 << PINB6)
 #define PIN_BTN6_PIN		(PINB & (1 << PINB6))
 #define PIN_BTN6_INT		PCMSK |= (1 << PCINT6)
#endif
#if defined(_PIN7_BTN)
 #define PIN_BTN7_DDR		DDRB &= ~(1 << PINB7)
 #define PIN_BTN7_PORT		PORTB &= ~(1 << PINB7)
 #define PIN_BTN7_PIN		(PINB & (1 << PINB7))
 #define PIN_BTN7_INT		PCMSK |= (1 << PCINT7)
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
extern volatile uint16_t PIN_Control_PUI_Key;

//********** 関数定義 **********//
//ピンアサインと割り込み処理の初期化
void PIN_Control_PUI_Init();

//ボタン待ち時間設定
//	引数：ボタン安定までの待ち時間（ms）、再生モジュールの起動時間（ms）、シリアル通信の待ち時間（ms）
void PIN_Control_PUI_SetWait(uint8_t, uint8_t, uint8_t);

//BTNがすべて離されるのを待つ
//	引数：対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_PUI_WaitKeyOff(uint16_t );

//BTNがすべて押されるのを待つ
//	引数：対象マスク 0=Wait（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
void PIN_Control_PUI_WaitKeyOn(uint16_t );

//BTNのその瞬間の押下状態を調べる
//	戻値：BTNの押下状態（BTN0 が 1ビット目, BTN1 が 2ビット目 ...）
volatile uint16_t PIN_Control_PUI_GetKey();

//外部の再生モジュールにシリアル通信
//	引数：送信する数値
void PIN_Control_PUI_Serial(uint8_t);

//外部の再生モジュールのON/OFFを取得
//	戻値：0=OFF/1=ON
uint8_t PIN_Control_PUI_isPlaying();

#endif /* PIN_Control_H_ */