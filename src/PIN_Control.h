/*
 * PIN_Control.h
 *
 * �T�v�F
 *  PIN_Control ���W���[���̂��߂̊e���`
 *
 * �g�p���@���F
 *  PWM_MusicBox, PWM_PCMPlay_EEPROM �����g�p���郂�W���[������ include
 *
 *  ��`�ς݃}�C�R���ƃs���A�T�C��
 *    ����`����΂ق��̃}�C�R���ł����삷��\�������邪�APLL����16MHz�ȏ�̑��x���K�v
 *
 *   ATTiny85
 *        �� ~PLAY���g���ꍇ�AFuse�ݒ�� Reset �𖳌�������K�v������
 *        �� _PWM_DEBUG ��L��������ꍇ��BTN1���g���Ȃ�
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
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/01/29 �V�K�쐬(Hiro OTSUKA)
 *  2017/02/17 �\���ύX(Hiro OTSUKA) EEPROM�����MML�Đ������WAV�Ƃ̎������ʂɑΉ�
 *  2017/02/25 �@�\�ǉ�(Hiro OTSUKA) �L�[��Ԃ��m�F����@�\��ǉ�
 *  2017/04/07 �\���ύX(Hiro OTSUKA) �s���ݒ�̌݊����ƃG���[�`�F�b�N������
 *  2017/04/09 �\���ύX(Hiro OTSUKA) �����������𐮗����A�҂����Ԑݒ���ς�
 *  2017/04/22 �\���ύX(Hiro OTSUKA) �s����PU�v�ۂ��p�����[�^�Ŏw��ł���悤�ύX
 *  2017/05/13 �@�\�ύX(Hiro OTSUKA) �s���̐ݒ�����s���Ɏw��ł���悤�ύX
 *  2017/05/19 �@�\�ǉ�(Hiro OTSUKA) �s���̐ݒ�ɁA�]��s�������̂��߂̖������t���O��ǉ�
 *  2017/05/19 �\���ύX(Hiro OTSUKA) �R�[�h�T�C�Y�팸�����{
 *
 */

#ifndef PIN_Control_H_
#define PIN_Control_H_

//********** �ǂݍ��� **********//
#include "config.h"	//�v���W�F�N�g���Ƃ̃s���g�p��`���K�v
#include <util/delay.h>
#include <avr/io.h>
#include "PWM_PCMPlay.h"

//********** �萔��` **********//
//���ʒ�`
// �s���ݒ�p�̃r�b�g�ԍ�
#define PIN_SETBIT_BTN	0	// 1=BTN / 0=LED
#define PIN_SETBIT_NOPU	1	// 1="NO"PullUp / 0=PullUP
#define PIN_SETBIT_INT	2	// 1=Interrupt / 0="NO"Interrupt
#define PIN_SETBIT_ENBL	3	// 1=Enable / 0=Disable
#define PIN_SETBIT_DEF	0b11110000	//�f�t�H���g�͂��ׂ� 1

#define PIN_WAIT_OFF	0
#define PIN_WAIT_ON		1

//�}�C�R���ʒ�`(�Ή��}�C�R���𑝂₷�ꍇ�͕ҏW���K�v)
// ATTiny85�n -----------------
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

//�f�o�b�O�s���g�p�̏ꍇ��PIN1�������������
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

// ATTiny861�n -----------------
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

//********** �ϐ���` **********//
//BTN�̉�����Ԃ��Q�Ɖ\�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
extern volatile uint8_t PIN_Control_Key;
extern volatile uint8_t PIN_Control_SetBit[];

//********** �֐���` **********//
//�s���A�T�C���Ɗ��荞�ݏ����̏�����
void PIN_Control_Init();

//�e�s����I/O�ݒ�
//	�����F�s���ԍ��A�ݒ�i�r�b�g�ݒ�� PIN_SETBIT_* �萔���Q�Ɓj
void PIN_Control_SetIO(uint8_t, uint8_t);

//�{�^���҂����Ԑݒ�
//	�����F�{�^������܂ł̑҂����ԁims�j
void PIN_Control_SetWait(uint8_t);

//BTN�����ׂė������^�������̂�҂�
//	�����F���[�h�i0=OFF�҂��^1=ON�҂��j�A�Ώۃ}�X�N 0=Wait�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
void PIN_Control_WaitKey(uint8_t, uint8_t );

//BTN�̂��̏u�Ԃ̉�����Ԃ𒲂ׂ�
//	�ߒl�FBTN�̉�����ԁiBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
volatile uint8_t PIN_Control_GetKey();

//�O���̃I�y�A���v��ON/OFF����(�s�����L���ȏꍇ)
//	�����F0=OFF/1=ON
void PIN_Control_Playing(uint8_t);

//LED��ON/OFF����(�s�����o�͐ݒ�̏ꍇ)
//	�����FPIN�ԍ��A0=OFF/1=ON
void PIN_Control_SetLED(uint8_t, uint8_t);

//LED�̑S�N���A�i�����ݒ�ɖ߂��j
void PIN_Control_ClearLED();

#endif /* PIN_Control_H_ */