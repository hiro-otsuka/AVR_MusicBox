/*
 * PWM_EEPROM_Play.h
 *
 * �T�v�F
 *  PWM_EEPROM_Play ���W���[���̂��߂̊e���`
 *
 * �g�p���@���F
 *  PWM_EEPROM_Play ���g�p���郂�W���[������ include
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/02/17 �V�K�쐬(Hiro OTSUKA) EEPROM�����MML�Đ������WAV�Ƃ̎������ʂɑΉ�
 *  2017/02/25 �@�\���P(Hiro OTSUKA) MML��WAV�𕪗����čĐ��ł���悤�@�\���P
 *
 */

#ifndef PWM_EEPROM_PLAY_H_
#define PWM_EEPROM_PLAY_H_

//********** �ǂݍ��� **********//
#include "USI_TWI_Master.h"
#include "PWM_PCM_WAV.h"
#include "PWM_PCM_MB.h"

//********** �萔��` **********//
#define PWM_PCMPLAY_READSIZE	32		//EEPROM �����x�ɓǂݍ��ރT�C�Y
#define PWM_PCMPLAY_TIMEOUT		5		//EEPROM �̌��o�ɂ�����Ď��s��
#define PWM_PCMPLAY_ANY			0		//EEPROM �ōĐ�����t�@�C���̎�ށ�����
#define PWM_PCMPLAY_PCM			1		//EEPROM �ōĐ�����t�@�C���̎�ށ�PCM
#define PWM_PCMPLAY_MML			2		//EEPROM �ōĐ�����t�@�C���̎�ށ�MML
#define PWM_PCMPLAY_TYPES		3		//EEPROM �ōĐ�����t�@�C���̎�ސ�=2


//********** �ϐ���` **********//
// EEPROM �ɕۑ�����Ă���t�@�C���̐��i��ޕʁj
extern volatile uint8_t EEPROM_Files[PWM_PCMPLAY_TYPES];

//********** �֐���` **********//
//EEPROM����̃t�@�C���Đ����J�n�iI2C�A�h���X�w��j
//	�����FI2C�X���[�u�A�h���X, �Đ�����t�@�C���̎��, ���Ԗڂ̏����Đ����邩
void EEPROM_PlayAt(uint8_t, uint8_t, uint8_t );

//EEPROM�̃t�@�C����������
void EEPROM_Init();

//EEPROM����̃t�@�C���Đ����J�n
//	�����F�Đ�����t�@�C���̎��, ���Ԗڂ̏����Đ����邩
void EEPROM_Play(uint8_t, uint8_t );

#endif /* PWM_EEPROM_PLAY_H_ */
