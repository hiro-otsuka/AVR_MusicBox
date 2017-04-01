/*
 * USI_TWI_Master.h
 *
 * �T�v�F
 *  USI_TWI_Master �̂��߂̒�`
 *
 * �g�p���@���F
 *  USI_TWI_Master ���g�p���郂�W���[������ include
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/01/29 �V�K�쐬(Hiro OTSUKA)
 *  2017/04/01 �@�\�ύX(Hiro OTSUKA) EEPROM Array �̎����ɑΉ�
 *
 */

#ifndef USI_TWI_Master_h
#define USI_TWI_Master_h

//********** �ǂݍ��� **********//
#include "USI_TWI_Defs.h"
#include <inttypes.h>

//********** �萔��` **********//
#define USI_TWI_MASTER_BUF_SIZE    33     // bytes in message buffer

//�G���[�R�[�h
#define USI_TWI_NO_DATA             0x08  // Transmission buffer is empty
#define USI_TWI_DATA_OUT_OF_BOUND   0x09  // Transmission buffer is outside SRAM space
#define USI_TWI_UE_START_CON        0x07  // Unexpected Start Condition
#define USI_TWI_UE_STOP_CON         0x06  // Unexpected Stop Condition
#define USI_TWI_UE_DATA_COL         0x05  // Unexpected Data Collision (arbitration)
#define USI_TWI_NO_ACK_ON_DATA      0x02  // The slave did not acknowledge  all data
#define USI_TWI_NO_ACK_ON_ADDRESS   0x01  // The slave did not acknowledge  the address
#define USI_TWI_MISSING_START_CON   0x03  // Generated Start Condition not detected on bus
#define USI_TWI_MISSING_STOP_CON    0x04  // Generated Stop Condition not detected on bus
#define USI_TWI_BAD_MEM_READ	    0x0A  // Error during external memory read

//********** �֐���` **********//
//I2C �̏������i��񂾂���OK�j
//	�����FCPU���g��(KHz), I2C���g��(KHz)
void	USI_TWI_Master_Initialize(uint16_t , uint16_t );

//I2C �̍ċN���iInitialize ��̍ď������Ɏg�p�j
void	USI_TWI_Master_Restart();

//I2C �ł̑��M�J�n��錾�i���̎��_�ł͉����������Ȃ��j
//	�����FI2C�X���[�u�A�h���X
void	USI_TWI_Master_tranBegin( uint8_t );

//I2C �ł�1�o�C�g���M��\��i���̎��_�ł͉����������Ȃ��j
//	�����F���M�f�[�^
void	USI_TWI_Master_tranSend( uint8_t );

//I2C �ł̑��M�����s
//	�ߒl�F0=����,0�ȊO=�G���[�R�[�h
uint8_t	USI_TWI_Master_tranCommit( );

//I2C �ł̑��M�����s�i�������X�g�b�v�������s��Ȃ��j
//	�ߒl�F0=����,0�ȊO=�G���[�R�[�h
uint8_t	USI_TWI_Master_tranCommitNoStop( );

//I2C ����̎�M�����s
//	�����FI2C�X���[�u�A�h���X, ��M��
//	�ߒl�F0=����,0�ȊO=�G���[�R�[�h
uint8_t	USI_TWI_Master_receiveFrom( uint8_t , uint8_t );

//I2C �����M��������1�o�C�g�ǂݍ���
//	�ߒl�F��M���1�o�C�g��
uint8_t	USI_TWI_Master_receive();

//I2C �����M������񂪎c���Ă��邩�ǂ����m�F
//	�ߒl�F��M�\�o�C�g��
uint8_t	USI_TWI_Master_receiveAvailable();

#endif

