/*
 * EEPROM_Array.h
 *
 * �T�v�F
 *  EEPROM_Array �̂��߂̒�`
 *
 * �g�p���@���F
 *  EEPROM_Array ���g�p���郂�W���[������ include
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/04/01 �V�K�쐬(Hiro OTSUKA)
 *
 */

#ifndef EEPROM_ARRAY_h
#define EEPROM_ARRAY_h

//********** �ǂݍ��� **********//
#include "USI_TWI_Master.h"
#include <inttypes.h>

//********** �萔��` **********//
#define EEPROM_ARRAY_ADDR_START			0x50	// EEPROM�A�h���X�̊J�n�ʒu
#define EEPROM_ARRAY_ADDR_MAX			8		// EEPROM�A�h���X�̍ő吔
#define	EEPROM_ARRAY_BUFF_MAX			32		// �ǂݍ��݃o�b�t�@�̃T�C�Y
#define	EEPROM_ARRAY_I2C_TIMEOUT		5		// EEPROM�A�h���X�������̃^�C���A�E�g��

//********** �ϐ���` **********//

//********** �֐���` **********//
//EEPROM �A�h���X�̌���
//	�ߒl�F���o���ꂽI2C�A�h���X�̐�
//	�����FCPU���g��(KHz), I2C���g��(KHz)
uint8_t	EEPROM_Array_Init(uint16_t , uint16_t );

//�ǂݍ��݊J�n�A�h���X�̃Z�b�g
//	�����F�J�n�A�h���X
//	�ߒl�F0=����,0�ȊO=�G���[�R�[�h
uint8_t	EEPROM_Array_SetAddr( uint32_t );

//�ǂݍ��݂̎��s
//	�����F�ǂݍ��݃T�C�Y
//	�ߒl�F�ǂݍ��߂��f�[�^��
uint8_t	EEPROM_Array_Read( uint8_t );

//I2C �����M��������1�o�C�g�ǂݍ���
//	�ߒl�F��M���1�o�C�g��
uint8_t	EEPROM_Array_data();

//I2C �����M��������2�o�C�g�ǂݍ���
//	�ߒl�F��M���2�o�C�g��
uint16_t EEPROM_Array_data2();

//I2C �����M��������4�o�C�g�ǂݍ���
//	�ߒl�F��M���4�o�C�g��
uint32_t EEPROM_Array_data4();

#endif

