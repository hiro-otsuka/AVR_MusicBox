/*
 * PIN_Control_PUI.c
 *
 * �T�v�F
 *  �v���W�F�N�g���Ƃɒ�`���ꂽ�s���A�T�C�����ɏ]���āA
 *  ���荞�ݏ�����L�[���͂̊Ǘ����s�����W���[��
 *
 * �g�p���@���F
 *  PIN_Control_PUI ���g�p����v���W�F�N�g�ɃC���|�[�g���Ďg�p
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/04/06 �V�K�쐬(Hiro OTSUKA)
 *  2017/04/09 �\���ύX(Hiro OTSUKA) �����������𐮗����A�҂����Ԑݒ���ς�
 *
 */

#include <avr/interrupt.h>
#include "PIN_Control_PUI.h"

//********** �ϐ���` **********//
//BTN�̉�����Ԃ��Q�Ɖ\�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
volatile uint16_t PIN_Control_PUI_Key;
volatile uint8_t  PIN_Control_PUI_Wait_Boot;
volatile uint8_t  PIN_Control_PUI_Wait_Btn;
volatile uint8_t  PIN_Control_PUI_Wait_Serial;

//********** �O���֐� **********//
//�s���A�T�C���Ɗ��荞�ݏ����̏�����
//=============================
void PIN_Control_PUI_Init()
{
	PIN_Control_PUI_Key = 0;
	PIN_Control_PUI_Wait_Boot = 100;	//�҂����Ԃ͉��� 100ms �ɐݒ�
	PIN_Control_PUI_Wait_Btn = 100;		//�҂����Ԃ͉��� 100ms �ɐݒ�
	PIN_Control_PUI_Wait_Serial = 100;	//�҂����Ԃ͉��� 100ms �ɐݒ�

	//�s���A�T�C���̏�����
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
	
	//���荞�ݏ�����������
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
//�{�^���҂����Ԑݒ�
//	�����F�{�^������܂ł̑҂����ԁims�j�A�Đ����W���[���̋N�����ԁims�j�A�V���A���ʐM�̑҂����ԁims�j
void PIN_Control_PUI_SetWait(uint8_t wait, uint8_t boot, uint8_t serial)
{
	PIN_Control_PUI_Wait_Btn = wait;		//�w�肳�ꂽ�҂����Ԃ�ݒ�
	PIN_Control_PUI_Wait_Boot = boot;		//�w�肳�ꂽ�҂����Ԃ�ݒ�
	PIN_Control_PUI_Wait_Serial = serial;	//�w�肳�ꂽ�҂����Ԃ�ݒ�
}

//=============================
//BTN�����ׂė������̂�҂�
//	�����F�Ώۃ}�X�N 0=Wait�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
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
//BTN�����ׂĉ������̂�҂�
//	�����F�Ώۃ}�X�N 0=Wait�iBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
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
//BTN�̂��̏u�Ԃ̉�����Ԃ𒲂ׂ�
//	�ߒl�FBTN�̉�����ԁiBTN0 �� 1�r�b�g��, BTN1 �� 2�r�b�g�� ...�j
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
//�O���̍Đ����W���[���ɃV���A���ʐM
//	�����F���M���鐔�l
void PIN_Control_PUI_Serial(uint8_t number)
{
	//�ʐM�J�n
	PIN_SERIAL1_OFF;
	PIN_SERIAL0_ON;
	for(uint8_t i = 0; i < PIN_Control_PUI_Wait_Boot; i++) _delay_ms(1);
	for(uint8_t i = 0; i < (PIN_Control_PUI_Wait_Serial << 1); i++) _delay_ms(1);
	//���l�𑗐M
	for (uint8_t i = 0; i < number; i++) {
		PIN_SERIAL1_ON;
		for(uint8_t i = 0; i < (PIN_Control_PUI_Wait_Serial << 1); i++) _delay_ms(1);
		PIN_SERIAL1_OFF;
		for(uint8_t i = 0; i < (PIN_Control_PUI_Wait_Serial << 1); i++) _delay_ms(1);
	}
	//�ʐM�I��
	PIN_SERIAL0_OFF;
	for(uint8_t i = 0; i < (PIN_Control_PUI_Wait_Serial << 1); i++) _delay_ms(1);
	for(uint8_t i = 0; i < PIN_Control_PUI_Wait_Boot; i++) _delay_ms(1);
}

//=============================
//�O���̍Đ����W���[����ON/OFF���擾
//	�ߒl�F0=OFF/1=ON
uint8_t PIN_Control_PUI_isPlaying()
{
	if (PIN_ISPLAYING) return 0;
	return 1;
}

//********** �����֐� **********//
//+++++++++++++++++++++++++++++
//�s���ω����荞�݂ɂ��L�[���͎�t
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
