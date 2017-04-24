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
 *
 */

#include <avr/interrupt.h>
#include "PIN_Control.h"

//********** �ϐ���` **********//
//BTN�̉�����Ԃ��Q�Ɖ\�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
volatile uint8_t PIN_Control_Key;
volatile uint8_t PIN_Control_Wait;

//********** �O���֐� **********//
//=============================
//�s���A�T�C���Ɗ��荞�ݏ����̏�����
void PIN_Control_Init(uint8_t isNotPU)
{
	PIN_Control_Key = 0;
	PIN_Control_Wait = 100;	//�҂����Ԃ͉��� 100ms �ɐݒ�
	
	//�s���A�T�C��������	
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

	//���荞�ݏ����̏�����
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
//BTN�����ׂĉ������̂�҂�
//	�����F�Ώۃ}�X�N 0=Wait�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
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
//BTN�̂��̏u�Ԃ̉�����Ԃ𒲂ׂ�
//	�ߒl�FBTN�̉�����ԁiBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
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
