/*
 * PWM_PCMPlay.h
 *
 * �T�v�F
 *  PWM_PCMPlay ���W���[���̂��߂̊e���`
 *
 * �g�p���@���F
 *  PWM_PCMPlay ���g�p���郂�W���[������ include
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

#ifndef PWM_PCMPLAY_H_
#define PWM_PCMPLAY_H_

//********** �ǂݍ��� **********//
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "PIN_Control.h"

//********** �萔��` **********//
#define PWM_SOUND_MAX_BUFF	32		//PCM�Đ��p�o�b�t�@�i2�̔{���ł��邱�Ɓj

//********** �ϐ���` **********//
//PCM�Đ��p�̃o�b�t�@
extern volatile uint8_t PwmSoundBuff[];
//PCM�Đ��p�o�b�t�@�̃C���f�b�N�X�i�擪���������ݐ�j
extern volatile uint8_t PwmSoundHead;
//PCM�Đ��p�o�b�t�@�̃C���f�b�N�X�i�Ōぁ�ǂݍ��݌��j
extern volatile uint8_t PwmSoundTail;
//PWM�Đ����̃t���O
extern volatile uint8_t PWM_PCM_Playing;

//********** �֐���` **********//
//PCM�Đ����W���[���̏������i�ŏ��Ɉ�x�����Ăяo����OK�j
void PwmSound_Init();

//PCM�Đ����W���[���̍Đ��J�n�i���荞��ON�̏ꍇ�ɍĐ��J�n�j
//	�����F�����̉𑜓x(�T���v�����O���[�g)
void PwmSound_Start(uint16_t);

//PCM�o�b�t�@�ւ̏��ǋL�i�o�b�t�@�ɋ󂫂������ꍇ�͑҂����荞��OFF�̏ꍇ�������i�܂Ȃ��̂Ńt���[�Y����j
//	�����F�������
void PwmSound_AddBuff(uint8_t);

//PCM�Đ����W���[���̍Đ���~
void PwmSound_Stop();

#endif // PWM_PCMPLAY_H_
