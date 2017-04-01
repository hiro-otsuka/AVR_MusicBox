/*
 * PWM_PCM_WAV.h
 *
 * �T�v�F
 *  PWM_PCM_WAV ���W���[���̂��߂̊e���`
 *
 * �g�p���@���F
 *  PWM_PCM_WAV ���g�p���郂�W���[������ include
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

#ifndef PWM_PCM_WAV_H_
#define PWM_PCM_WAV_H_

//********** �ǂݍ��� **********//
#include "PWM_PCMPlay.h"

//********** �萔��` **********//
#define PWM_PCMPLAY_READSIZE	32		//EEPROM �����x�ɓǂݍ��ރT�C�Y�i <= EEPROM_ARRAY_BUFF_MAX �ł��邱�Ɓj

//********** �֐���` **********//
//EEPROM�����WAV�Đ����J�n
//	�����F�Đ��J�n�A�h���X
void PWM_PCM_WAV_Play( uint32_t );

#endif /* PWM_PCM_WAV_H_ */
