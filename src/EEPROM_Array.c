/*
 * EEPROM_Array.c
 *
 * �T�v�F
 *  ������ EEPROM ���A�܂Ƃ߂ĂЂƂ̃������A���C�Ƃ��Ĉ������W���[��
 *
 * �g�p���@���F
 *  USI_TWI_Master ���g�p����v���W�F�N�g�ɃC���|�[�g���Ďg�p
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/04/01 �V�K�쐬(Hiro OTSUKA)
 *
 */

#include "EEPROM_Array.h"

//********** �֐���` **********//

//********** �萔��` **********//

//********** �ϐ���` **********//
uint8_t EEPROM_Array_Addr[EEPROM_ARRAY_ADDR_MAX];   // holds I2C addr
uint8_t EEPROM_Array_Buf[EEPROM_ARRAY_BUFF_MAX];    // holds I2C send and receive data
uint8_t EEPROM_Array_LastRead = 0;                  // number of bytes read so far
uint8_t EEPROM_Array_BytesAvail = 0;                // number of bytes requested but not read

uint8_t EEPROM_Array_AddrAvail = 0;
uint32_t EEPROM_Array_NowAddr = 0;

//********** �O���֐� **********//
//=============================
//EEPROM �A�h���X�̌���
//	�ߒl�F���o���ꂽI2C�A�h���X�̐�
//	�����FCPU���g��(KHz), I2C���g��(KHz)
uint8_t	EEPROM_Array_Init(uint16_t f_cpu, uint16_t f_i2c )
{
	// I2C���W���[���̏�����
	USI_TWI_Master_Initialize(f_cpu, f_i2c);
	
	// I2C�A�h���X����������
	for (uint8_t slaveaddr = EEPROM_ARRAY_ADDR_START; slaveaddr < EEPROM_ARRAY_ADDR_START + EEPROM_ARRAY_ADDR_MAX; slaveaddr ++) {
		uint8_t	TimeOut = 0;
		uint8_t Result = 0;
		do {
			USI_TWI_Master_tranBegin(slaveaddr);
			USI_TWI_Master_tranSend(0);
			USI_TWI_Master_tranSend(0);
			Result = USI_TWI_Master_tranCommit();
			TimeOut ++;
		} while(Result && TimeOut < EEPROM_ARRAY_I2C_TIMEOUT);
		if (Result == 0) EEPROM_Array_Addr[EEPROM_Array_AddrAvail++] = slaveaddr;
	}
	
	return EEPROM_Array_AddrAvail;
}

//=============================
//�ǂݍ��݊J�n�A�h���X�̃Z�b�g
//	�����F�J�n�A�h���X
//	�ߒl�F0=����,0�ȊO=�G���[�R�[�h
uint8_t	EEPROM_Array_SetAddr( uint32_t addr )
{
	uint8_t	TimeOut = 0;
	uint8_t Result = 0;
	
	do {
		USI_TWI_Master_tranBegin(EEPROM_Array_Addr[(uint8_t)(addr >> 16)]);
		USI_TWI_Master_tranSend((uint8_t)((addr >> 8) & 0xFF) );
		USI_TWI_Master_tranSend((uint8_t)(addr & 0xFF));
		Result = USI_TWI_Master_tranCommit();
		TimeOut ++;
	} while(Result && TimeOut < EEPROM_ARRAY_I2C_TIMEOUT);
	
	EEPROM_Array_NowAddr = addr;
	
	return Result;
}

//=============================
//�ǂݍ��݂̎��s
//	�����F�ǂݍ��݃T�C�Y
//	�ߒl�F�ǂݍ��߂��f�[�^��
uint8_t	EEPROM_Array_Read( uint8_t number )
{
	EEPROM_Array_LastRead = 0;
	EEPROM_Array_BytesAvail = 0;
	
	if (number > EEPROM_ARRAY_BUFF_MAX) number = EEPROM_ARRAY_BUFF_MAX;
	
	uint32_t newAddr = EEPROM_Array_NowAddr + (uint32_t) number;
	
	//I2C�A�h���X���܂�����Ȃ��ꍇ�͏����ɓǂݍ���
	if ((EEPROM_Array_NowAddr & 0xFF0000) == (newAddr & 0xFF0000)) {
		if (USI_TWI_Master_receiveFrom(EEPROM_Array_Addr[(uint8_t)(EEPROM_Array_NowAddr >> 16)], number) == 0) {
			uint8_t readNum = USI_TWI_Master_receiveAvailable();
			for (uint8_t i = 0; i < readNum; i ++) EEPROM_Array_Buf[EEPROM_Array_BytesAvail++] = USI_TWI_Master_receive();
		} else {
			return 0;
		}
	// �܂������Ă����ꍇ��2��ɕ����ēǂݍ���
	} else {
		uint8_t readNum1 = (uint8_t)((uint16_t)0x100 - (uint16_t)(EEPROM_Array_NowAddr & 0xFF));
		uint8_t readNum2 = (uint8_t)(newAddr & 0xFF);
		
		if (USI_TWI_Master_receiveFrom(EEPROM_Array_Addr[(uint8_t)(EEPROM_Array_NowAddr >> 16)], readNum1) == 0) {
			uint8_t readNum = USI_TWI_Master_receiveAvailable();
			for (uint8_t i = 0; i < readNum; i ++) EEPROM_Array_Buf[EEPROM_Array_BytesAvail++] = USI_TWI_Master_receive();
		} else {
			return 0;
		}
		if (EEPROM_Array_SetAddr(newAddr & 0xFF0000)) return 0;
		if (readNum2 != 0) {
			if (USI_TWI_Master_receiveFrom(EEPROM_Array_Addr[(uint8_t)(newAddr >> 16)], readNum2) == 0) {
				uint8_t readNum = USI_TWI_Master_receiveAvailable();
				for (uint8_t i = 0; i < readNum; i ++) EEPROM_Array_Buf[EEPROM_Array_BytesAvail++] = USI_TWI_Master_receive();
			} else {
				return 0;
			}
		}
	}
	
	EEPROM_Array_NowAddr = newAddr;
	
	return EEPROM_Array_BytesAvail;
}

//=============================
//I2C �����M��������1�o�C�g�ǂݍ���
//	�ߒl�F��M���1�o�C�g��
uint8_t	EEPROM_Array_data()
{
	return EEPROM_Array_Buf[EEPROM_Array_LastRead++];
}

//=============================
//I2C �����M��������2�o�C�g�ǂݍ���
//	�ߒl�F��M���2�o�C�g��
uint16_t EEPROM_Array_data2()
{
	return (uint16_t)EEPROM_Array_data()
			+ ((uint16_t)EEPROM_Array_data() << 8);
}

//=============================
//I2C �����M��������4�o�C�g�ǂݍ���
//	�ߒl�F��M���4�o�C�g��
uint32_t EEPROM_Array_data4()
{
	return (uint32_t)EEPROM_Array_data()
			+ ((uint32_t)EEPROM_Array_data() << 8)
			+ ((uint32_t)EEPROM_Array_data() << 16)
			+ ((uint32_t)EEPROM_Array_data() << 24);
}
