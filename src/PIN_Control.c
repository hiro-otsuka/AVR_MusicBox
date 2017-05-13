/*
 * PIN_Control.c
 *
 * �T�v�F
 *  �v���W�F�N�g���Ƃɒ�`���ꂽ�s���A�T�C�����ɏ]���āA
 *  ���荞�ݏ�����L�[���͂̊Ǘ����s�����W���[��
 *
 * �g�p���@���F
 *  PIN_Control ���g�p����v���W�F�N�g�ɃC���|�[�g���Ďg�p
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
 *
 */

#include <avr/interrupt.h>
#include "PIN_Control.h"

//********** �ϐ���` **********//
//BTN�̉�����Ԃ��Q�Ɖ\�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
volatile uint8_t PIN_Control_Key;
volatile uint8_t PIN_Control_Wait;
volatile uint8_t PIN_Control_Int;
volatile uint8_t PIN_Control_SetBit[_PIN_NUM_MAX];

//********** �O���֐� **********//
//=============================
//�s���A�T�C���Ɗ��荞�ݏ����̏�����
void PIN_Control_Init()
{
	PIN_Control_Key = 0;
	PIN_Control_Int = 0;
	PIN_Control_Wait = 100;	//�҂����Ԃ͉��� 100ms �ɐݒ�
	
	//�s���A�T�C���������i�f�t�H���g�͓���/�v���A�b�v/���荞�݂Ȃ� �ɂ��Ă����j
	for (uint8_t i = 0; i < _PIN_NUM_MAX; i ++) {
		PIN_Control_SetIO(i, PIN_SETBIT_DEF | (1 << PIN_SETBIT_BTN) | (0 << PIN_SETBIT_NOPU) | (0 << PIN_SETBIT_INT));
	}
#ifdef _PLAYING_ENABLE
	PIN_PLAYING_DDR; PIN_PLAYING_ON;
#endif

	//���荞�ݏ����̏�����
	PIN_CHANGE_INT_INIT;
	PIN_CHANGE_INT_ENBL;
}

//=============================
//�e�s����I/O�ݒ�
//	�����F�s���ԍ��A�ݒ�i�r�b�g�ݒ�̓w�b�_�Q�Ɓj
void PIN_Control_SetIO(uint8_t pinNum, uint8_t pinSet)
{
	switch(pinNum) {
#if defined(PIN_PIN0_BTN)
		case 0:
			if (pinSet & (1 << PIN_SETBIT_BTN)) PIN_PIN0_BTN;
			else PIN_PIN0_LED;
			if (pinSet & (1 << PIN_SETBIT_NOPU)) PIN_PIN0_OFF;
			else PIN_PIN0_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 0);
				PIN_PIN0_INT;
			}
			break;
#endif
#if defined(PIN_PIN1_BTN)
		case 1:
			if (pinSet & (1 << PIN_SETBIT_BTN)) PIN_PIN1_BTN;
			else PIN_PIN1_LED;
			if (pinSet & (1 << PIN_SETBIT_NOPU)) PIN_PIN1_OFF;
			else PIN_PIN1_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 1);
				PIN_PIN1_INT;
			}
			break;
#endif
#if defined(PIN_PIN2_BTN)
		case 2:
			if (pinSet & (1 << PIN_SETBIT_BTN)) PIN_PIN2_BTN;
			else PIN_PIN2_LED;
			if (pinSet & (1 << PIN_SETBIT_NOPU)) PIN_PIN2_OFF;
			else PIN_PIN2_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 2);
				PIN_PIN2_INT;
			}
			break;
#endif
#if defined(PIN_PIN3_BTN)
		case 3:
			if (pinSet & (1 << PIN_SETBIT_BTN)) PIN_PIN3_BTN;
			else PIN_PIN3_LED;
			if (pinSet & (1 << PIN_SETBIT_NOPU)) PIN_PIN3_OFF;
			else PIN_PIN3_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 3);
				PIN_PIN3_INT;
			}
			break;
#endif
#if defined(PIN_PIN4_BTN)
		case 4:
			if (pinSet & (1 << PIN_SETBIT_BTN)) PIN_PIN4_BTN;
			else PIN_PIN4_LED;
			if (pinSet & (1 << PIN_SETBIT_NOPU)) PIN_PIN4_OFF;
			else PIN_PIN4_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 4);
				PIN_PIN4_INT;
			}
			break;
#endif
#if defined(PIN_PIN5_BTN)
		case 5:
			if (pinSet & (1 << PIN_SETBIT_BTN)) PIN_PIN5_BTN;
			else PIN_PIN5_LED;
			if (pinSet & (1 << PIN_SETBIT_NOPU)) PIN_PIN5_OFF;
			else PIN_PIN5_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 5);
				PIN_PIN5_INT;
			}
			break;
#endif
#if defined(PIN_PIN6_BTN)
		case 6:
			if (pinSet & (1 << PIN_SETBIT_BTN)) PIN_PIN6_BTN;
			else PIN_PIN6_LED;
			if (pinSet & (1 << PIN_SETBIT_NOPU)) PIN_PIN6_OFF;
			else PIN_PIN6_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 6);
				PIN_PIN6_INT;
			}
			break;
#endif
#if defined(PIN_PIN7_BTN)
		case 7:
			if (pinSet & (1 << PIN_SETBIT_BTN)) PIN_PIN7_BTN;
			else PIN_PIN7_LED;
			if (pinSet & (1 << PIN_SETBIT_NOPU)) PIN_PIN7_OFF;
			else PIN_PIN7_ON;
			if (pinSet & (1 << PIN_SETBIT_INT)) {
				PIN_Control_Int |= (1 << 7);
				PIN_PIN7_INT;
			}
			break;
#endif
	}
	PIN_Control_SetBit[pinNum] = pinSet;
}

//=============================
//�{�^���҂����Ԑݒ�
//	�����F�{�^������܂ł̑҂����ԁims�j
void PIN_Control_SetWait(uint8_t wait)
{
	PIN_Control_Wait = wait;	//�w�肳�ꂽ�҂����Ԃ�ݒ�
}

//=============================
//BTN�����ׂė������̂�҂�
//	�����F�Ώۃ}�X�N 0=Wait�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
void PIN_Control_WaitKeyOff(uint8_t waitmask)
{
	uint8_t WaitCount = 0;
	
	waitmask = ~waitmask;
	while(WaitCount < PIN_Control_Wait) {
		uint8_t nowPin = PIN_Control_GetKey() & waitmask;
		if (nowPin) WaitCount = 0;
		else WaitCount ++;
		_delay_ms(1);
	}
}

//=============================
//BTN�����ׂĉ������̂�҂�
//	�����F�Ώۃ}�X�N 0=Wait�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
void PIN_Control_WaitKeyOn(uint8_t waitmask)
{
	uint8_t WaitCount = 0;
	
	waitmask = ~waitmask;
	while(WaitCount < PIN_Control_Wait) {
		uint8_t nowPin = PIN_Control_GetKey() & waitmask;
		if (nowPin != waitmask) WaitCount = 0;
		else WaitCount ++;
		_delay_ms(1);
	}
}

//=============================
//BTN�̂��̏u�Ԃ̉�����Ԃ𒲂ׂ�
//	�ߒl�FBTN�̉�����ԁiBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
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
	return KeyNow;
}

//=============================
//LED��ON/OFF����(�s�����o�͐ݒ�̏ꍇ)
//	�����FPIN�ԍ��A0=OFF/1=ON
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
//�O���̃I�y�A���v��ON/OFF����(�s�����L���ȏꍇ)
//	�����F0=OFF/1=ON
void PIN_Control_Playing(uint8_t isOn)
{
#ifdef _PLAYING_ENABLE
	if (isOn) PIN_PLAYING_ON;
	else PIN_PLAYING_OFF;
#endif
}

//********** �����֐� **********//
//+++++++++++++++++++++++++++++
//�s���ω����荞�݂ɂ��L�[���͎�t
ISR(PIN_CHANGE_INT_VECT)
{
	uint8_t nowBtn = PIN_Control_GetKey() & (~PIN_Control_Key);
	
	if (PWM_PCM_Playing == 0) PIN_Control_Key |= nowBtn;
	else if (nowBtn & PIN_Control_Int) {
		PWM_PCM_Playing = 0;
		PIN_Control_Key |= (nowBtn & PIN_Control_Int);
	}
/*	
#if defined(PIN_PIN0_BTN)
	if ((PIN_Control_SetBit[0] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN0_DATA == 0 && (PIN_Control_Key & (1 << 0)) == 0)  {
		if ((PIN_Control_SetBit[0] & (1 << PIN_SETBIT_INT)) == 0 || PWM_PCM_Playing == 0) {
			PIN_Control_Key |= (1 << 0);
			PWM_PCM_Playing = 0;
		}
	}
#endif
#if defined(PIN_PIN1_BTN)
	if ((PIN_Control_SetBit[1] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN1_DATA == 0 && (PIN_Control_Key & (1 << 1)) == 0)  {
		if ((PIN_Control_SetBit[1] & (1 << PIN_SETBIT_INT)) == 0 || PWM_PCM_Playing == 0) {
			PIN_Control_Key |= (1 << 1);
			PWM_PCM_Playing = 0;
		}
	}
#endif
#if defined(PIN_PIN2_BTN)
	if ((PIN_Control_SetBit[2] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN2_DATA == 0 && (PIN_Control_Key & (1 << 2)) == 0)  {
		if ((PIN_Control_SetBit[2] & (1 << PIN_SETBIT_INT)) == 0 || PWM_PCM_Playing == 0) {
			PIN_Control_Key |= (1 << 2);
			PWM_PCM_Playing = 0;
		}
	}
#endif
#if defined(PIN_PIN3_BTN)
	if ((PIN_Control_SetBit[3] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN3_DATA == 0 && (PIN_Control_Key & (1 << 3)) == 0)  {
		if ((PIN_Control_SetBit[3] & (1 << PIN_SETBIT_INT)) == 0 || PWM_PCM_Playing == 0) {
			PIN_Control_Key |= (1 << 3);
			PWM_PCM_Playing = 0;
		}
	}
#endif
#if defined(PIN_PIN4_BTN)
	if ((PIN_Control_SetBit[4] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN4_DATA == 0 && (PIN_Control_Key & (1 << 4)) == 0)  {
		if ((PIN_Control_SetBit[4] & (1 << PIN_SETBIT_INT)) == 0 || PWM_PCM_Playing == 0) {
			PIN_Control_Key |= (1 << 4);
			PWM_PCM_Playing = 0;
		}
	}
#endif
#if defined(PIN_PIN5_BTN)
	if ((PIN_Control_SetBit[5] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN5_DATA == 0 && (PIN_Control_Key & (1 << 5)) == 0)  {
		if ((PIN_Control_SetBit[5] & (1 << PIN_SETBIT_INT)) == 0 || PWM_PCM_Playing == 0) {
			PIN_Control_Key |= (1 << 5);
			PWM_PCM_Playing = 0;
		}
	}
#endif
#if defined(PIN_PIN6_BTN)
	if ((PIN_Control_SetBit[6] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN6_DATA == 0 && (PIN_Control_Key & (1 << 6)) == 0)  {
		if ((PIN_Control_SetBit[6] & (1 << PIN_SETBIT_INT)) == 0 || PWM_PCM_Playing == 0) {
			PIN_Control_Key |= (1 << 6);
			PWM_PCM_Playing = 0;
		}
	}
#endif
#if defined(PIN_PIN7_BTN)
	if ((PIN_Control_SetBit[7] & (1 << PIN_SETBIT_BTN)) != 0 && PIN_PIN7_DATA == 0 && (PIN_Control_Key & (1 << 7)) == 0)  {
		if ((PIN_Control_SetBit[7] & (1 << PIN_SETBIT_INT)) == 0 || PWM_PCM_Playing == 0) {
			PIN_Control_Key |= (1 << 7);
			PWM_PCM_Playing = 0;
		}
	}
#endif
*/
}
