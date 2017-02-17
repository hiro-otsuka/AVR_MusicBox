/*
 * PWM_PCMPlay.c
 *
 * �T�v�F
 *  PWM�ɂ��PCM�����̍Đ����s�����W���[��
 *
 * �g�p���@���F
 *  PWM_PCMPlay ���g�p����v���W�F�N�g�ɃC���|�[�g���Ďg�p
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/01/29 �V�K�쐬(Hiro OTSUKA)
 *  2017/02/17 �\���ύX(Hiro OTSUKA) EEPROM�����MML�Đ������WAV�Ƃ̎������ʂɑΉ�
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM_PCMPlay.h"

//********** �ϐ���` **********//
//PCM�Đ��p�̃o�b�t�@
volatile uint8_t PwmSoundBuff[PWM_SOUND_MAX_BUFF];
//PCM�Đ��p�o�b�t�@�̃C���f�b�N�X�i�擪���������ݐ�j
volatile uint8_t PwmSoundHead = 0;
//PCM�Đ��p�o�b�t�@�̃C���f�b�N�X�i�Ōぁ�ǂݍ��݌��j
volatile uint8_t PwmSoundTail = 0;
//PWM�Đ����̃t���O
volatile uint8_t PWM_PCM_Playing;

static volatile uint8_t PwmSoundWait = 0;
static volatile uint8_t PwmSoundNow  = 0;
static volatile uint8_t PwmSoundVal  = 0;

//********** �O���֐� **********//
//=============================
//PCM�Đ����W���[���̏������i�ŏ��Ɉ�x�����Ăяo����OK�j
void PwmSound_Init()
{
	DDRB |= (1 << PWM_PIN_OUT);
	PORTB |= (1 << PWM_PIN_OUT);
#ifdef _PWM_DEBUG
	DDRB |= (1 << PWM_DEBUG_OUT);
	PORTB &= ~(1 << PWM_DEBUG_OUT);
#endif

#if PWM_SOUND_FREQ == 32000
	PWM_SPEED_32000;
#elif PWM_SOUND_FREQ == 16000
	PWM_SPEED_16000;
#else
	PWM_SPEED_8000;
#endif
	PWM_TIM1B_ON;

	OCR1C = 0xFF;
	OCR1B = 0xFF;
	TCNT1 = 0;
}

//=============================
//PCM�Đ����W���[���̍Đ��J�n�i���荞��ON�̏ꍇ�ɍĐ��J�n�j
//	�����F�����̉𑜓x(�T���v�����O���[�g)
void PwmSound_Start(uint16_t MusicFreq)
{
	PwmSoundHead = 0;
	PwmSoundTail = 0;
	PwmSoundWait = (uint8_t)((uint16_t)PWM_SOUND_FREQ / MusicFreq);
	PwmSoundNow = 0;
	PwmSoundVal = 128;

	TIMSK |= (1 << TOIE1);
}

//=============================
//PCM�o�b�t�@�ւ̏��ǋL�i�o�b�t�@�ɋ󂫂������ꍇ�͑҂����荞��OFF�̏ꍇ�������i�܂Ȃ��̂Ńt���[�Y����j
//	�����F�������
void PwmSound_AddBuff(uint8_t val) {
	uint8_t PwmSoundTmp = (PwmSoundHead + 1) & (PWM_SOUND_MAX_BUFF-1);
	while(PwmSoundTmp == PwmSoundTail) {
#ifdef _PWM_DEBUG
		PORTB &= ~(1 << PWM_DEBUG_OUT);
#endif
	}
	PwmSoundBuff[PwmSoundTmp] = val;
	PwmSoundHead = PwmSoundTmp;
}

//=============================
//PCM�Đ����W���[���̍Đ���~
void PwmSound_Stop()
{
	TIMSK &= ~(1 << TOIE1);
}

//********** �����֐� **********//
//+++++++++++++++++++++++++++++
//�^�C�}1���荞�݂ɂ��PCM�Đ��iPwmSound �� Buff �� Tail ���� Head �܂œǂ�ł����j
ISR(TIMER1_OVF_vect)
{
	PwmSoundNow ++;
	if (PwmSoundNow >= PwmSoundWait) {
		PwmSoundNow = 0;
		if(PwmSoundHead != PwmSoundTail) {
			PwmSoundVal = PwmSoundBuff[PwmSoundTail];
			OCR1B = PwmSoundVal;
			PwmSoundTail = (PwmSoundTail + 1) & (PWM_SOUND_MAX_BUFF-1);
#ifdef _PWM_DEBUG
		} else {
			PORTB |= (1 << PWM_DEBUG_OUT);
#endif
		}
#ifdef _HIGH_RESO
	} else {
		//If next data is available
		if (PwmSoundHead != PwmSoundTail) {
			uint16_t MulTmp = 0;
			for (uint8_t i = 0; i < PwmSoundNow; i++) {
				MulTmp += (uint16_t)PwmSoundBuff[PwmSoundTail];
			}
			for (uint8_t i = PwmSoundNow; i < PwmSoundWait; i++) {
				MulTmp += (uint16_t)PwmSoundVal;
			}
			MulTmp >>= 2;
			OCR1B = MulTmp;
		}
#endif
	}
}
