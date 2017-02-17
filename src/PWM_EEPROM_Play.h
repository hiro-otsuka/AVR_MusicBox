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

//********** �ϐ���` **********//
extern volatile uint8_t EEPROM_Files;

//********** �֐���` **********//
//EEPROM����̃t�@�C���Đ����J�n�iI2C�A�h���X�w��j
//	�����FI2C�X���[�u�A�h���X, ���Ԗڂ̏����Đ����邩
void EEPROM_PlayAt(uint8_t, uint8_t );

//EEPROM�̃t�@�C����������
void EEPROM_Init();

//EEPROM����̃t�@�C���Đ����J�n
//	�����F���Ԗڂ̏����Đ����邩
void EEPROM_Play(uint8_t );

#endif /* PWM_EEPROM_PLAY_H_ */
