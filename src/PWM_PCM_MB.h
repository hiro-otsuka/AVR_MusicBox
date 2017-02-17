/*
 * PWM_PCM_MB.h
 *
 * �T�v�F
 *  PWM_PCM_MB ���W���[���̂��߂̊e���`
 *
 * �g�p���@���F
 *  PWM_PCM_MB ���g�p���郂�W���[������ include
 *  �ʓr�y����`���s���K�v������i�����̃T���v���Q�Ɓj
 *  �����}�N���ɗ^���钷���̊�{�l�i�ꕪ�����ɑ������钷���j�̋��ߕ�
 *     ((60 / �l�������̃��Y����) * 4) / ((1,024 * 256) / CPU�̎��g��) 
 *      �˂�������3*16�̔{���̈�ԋ߂������Ɋۂ߂�i3�A�����g��Ȃ��ꍇ��16�̔{���ł��悢�j
 *        CPU�̎��g����20MHz�̎� �� �ꕪ�����̒���(�b) / 0.0131072
 *        CPU�̎��g����16MHz�̎� �� �ꕪ�����̒���(�b) / 0.016384
 *     ��j�l������=120�̊y���ŁACPU=16MHz �̏ꍇ
 *         ((60/120)*4)/0.016384=122 �� 3�A�����g���ꍇ�� 96 or 144�A�g��Ȃ��ꍇ�� 128
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/01/29 �V�K�쐬(Hiro OTSUKA)
 *  2017/02/17 �\���ύX(Hiro OTSUKA) EEPROM�����MML�Đ������WAV�Ƃ̎������ʂɑΉ�
 *                                  ���F�̑I���@�\��ǉ�
 *
 */

#ifndef PWM_PCM_MB_H_
#define PWM_PCM_MB_H_

//********** �ǂݍ��� **********//
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "PWM_PCMPlay.h"

//********** �萔��` **********//
// �e��T�C�Y
#define PWM_MUSICBOX_BUFF	32	// MML�i�[�p�o�b�t�@�̃T�C�Y�i2�̔{���ł��邱�Ɓj

// �������
#define N_C		0+1
#define N_Cs	1+1
#define N_Db	1+1
#define N_D		2+1
#define N_Ds	3+1
#define N_Eb	3+1
#define N_E		4+1
#define N_F		5+1
#define N_Fs	6+1
#define N_Gb	6+1
#define N_G		7+1
#define N_Gs	8+1
#define N_Ab	8+1
#define N_A		9+1
#define N_As	10+1
#define N_Bb	10+1
#define N_B		11+1

// ���t����
#define C_CMD	0x80
#define C_VOL	1	//�{�����[���ݒ�i�t�����F����0-16�j
#define C_DIVST	2	//�����E�F�C�g�i�t�����F�E�F�C�g���j
#define C_DIVWT	3	//�����Ԋu�i�t�����F�Ԋu���j
#define C_OFF	4	//�����I�t�Z�b�g�i�t�����F�L�[���Z�l�j
#define C_END	5	//�Đ��̏I��
#define C_L_ON	6	//LEDx ��HIGH���o�́i�t�����FLED�ԍ��j
#define C_L_OFF	7	//LEDx ��LOW ���o�́i�t�����FLED�ԍ��j
#define C_TYPE	8	//���F�ԍ��̕ύX

//*********** �}�N�� ***********//
//�ʏ�̉����i�����F�������,�I�N�^�[�u,�����j
#define M_NOTE(n,o,l) (uint16_t)(n+o*12)*256+l
//�X�^�b�J�[�g�i�����F�������,�I�N�^�[�u,�����j
#define M_NOTE_S(n,o,l) (uint16_t)(n+o*12)*256+N_LEN/16,(uint16_t)l-N_LEN/16
//�x���i�����F�����j
#define M_REST(l) l
//�R���g���[���R�}���h�i�����F���t���, �t�����j
#define M_CTRL(x,y) (uint16_t)(C_CMD+x)*256+y

//********** �֐���` **********//
//�I���S�[�����W���[���̏������i�ŏ��Ɉ�x�����Ăяo����OK�j
void PWM_PCM_MB_Init();

//�y�����ݒ�
//	�����F�y���ԍ�, �������p�R�[���o�b�N�֐�
void PWM_PCM_MB_MEM_SetScore(uint8_t , void (*)());

//�`�����l�����ݒ�
//	�����F�`�����l���ԍ�, �y���A�h���X, �y����(!=�o�C�g��)
void PWM_PCM_MB_MEM_SetScoreChannel(uint8_t , const uint16_t* , uint16_t );

//�I���S�[�����W���[���̍Đ��J�n�iC_END�܂ōĐ�����ƏI���j ��������
//	�����F�y���ԍ�
void PWM_PCM_MB_MEM_Play(uint8_t );

//�I���S�[�����W���[���̍Đ��J�n�iC_END�܂ōĐ�����ƏI���j EEOROM��
//	�����FI2C�X���[�u�A�h���X, �J�n�A�h���X
void PWM_PCM_MB_EEPROM_Play(uint8_t , uint16_t );

#endif //PWM_PCM_MB_H_
