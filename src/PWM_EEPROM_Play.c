/*
 * PWM_EEPROM_Play.c
 *
 * �T�v�F
 *  I2C�ڑ���EEPROM����AWAV�܂���MML���������ʂ��čĐ����郂�W���[��
 *
 * �g�p���@���F
 *  PWM_EEPROM_Play ���g�p����v���W�F�N�g�ɃC���|�[�g���Ďg�p
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/02/17 �V�K�쐬(Hiro OTSUKA) EEPROM�����MML�Đ������WAV�Ƃ̎������ʂɑΉ�
 *
 */

#include "PWM_EEPROM_Play.h"

//********** �֐���` **********//
uint8_t EEPROM_Search(uint8_t );

//********** �萔��` **********//
#define	EEPROM_I2C_MAX	8

//********** �ϐ���` **********//
// �Đ��\�ő吔
volatile uint8_t EEPROM_Files;

uint8_t	EEPROM_File[EEPROM_I2C_MAX];

//********** �O���֐� **********//
//=============================
//EEPROM����̃t�@�C���Đ����J�n�iI2C�A�h���X�w��j
//	�����FI2C�X���[�u�A�h���X, ���Ԗڂ̏����Đ����邩
void EEPROM_PlayAt(uint8_t slaveaddr, uint8_t count)
{
	uint8_t PlayMode = 0;
	uint16_t StartAddr = 0;
	uint32_t inSize = 0;
	int TimeOut = 0;
	
	//�w�肳�ꂽ count �Ԗڂ̃t�@�C����������܂ŃX�L�b�v���Ă���
	do {
		if (inSize != 0) {
			StartAddr += inSize + 8;
			inSize = 0;
		}
		TimeOut = 0;
		do {
			USI_TWI_Master_tranBegin(slaveaddr);
			USI_TWI_Master_tranSend((uint8_t)(StartAddr >> 8));
			USI_TWI_Master_tranSend((uint8_t)(StartAddr & 0xFF));
			TimeOut ++;
		} while(USI_TWI_Master_tranCommit() && TimeOut < PWM_PCMPLAY_TIMEOUT);
	
		//WAV�t�H�[�}�b�g�̃w�b�_�t�@�C���ǂݍ���
		USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'RIFF' or 'MMLD'
		PlayMode = USI_TWI_Master_receive();		// 'R' or 'M'
		USI_TWI_Master_receiveFrom(slaveaddr, 4);	//Size(below)
		
		//�t�@�C���̃T�C�Y�𓾂�
		for (uint8_t i = 0; i < 4; i++) {
			inSize >>= 8;
			inSize += ((uint32_t)USI_TWI_Master_receive() << 24);
		}
	} while(count --);
	
	//�t�H�[�}�b�g���m�F
	switch(PlayMode) {
		case 'R':	//RIFF = WAV
			PWM_PCM_WAV_Play(slaveaddr, StartAddr + 4+4);
			break;
		case 'M':	//MMLD = MML
			PWM_PCM_MB_EEPROM_Play(slaveaddr, StartAddr + 4+4);
			break;
	}
}

//=============================
//EEPROM�̃t�@�C����������
void EEPROM_Init()
{
	for (uint8_t i = 0; i < EEPROM_I2C_MAX; i++) {
		uint8_t num = EEPROM_Search(0x50 + i);
		EEPROM_File[i] = num;
		EEPROM_Files += num;
	}
}

//=============================
//EEPROM����̃t�@�C���Đ����J�n
//	�����F���Ԗڂ̏����Đ����邩
void EEPROM_Play(uint8_t count)
{
	uint8_t index = 0;
	while(count >= EEPROM_File[index] && index < EEPROM_I2C_MAX) {
		count -= EEPROM_File[index];
		index ++;
	}
	if (index >= EEPROM_I2C_MAX || EEPROM_File[index] == 0) return;
	EEPROM_PlayAt(0x50 + index, count);
}

//********** �����֐� **********//
//-----------------------------
//EEPROM�����WAV�Đ����J�n
//	�����FI2C�X���[�u�A�h���X
uint8_t EEPROM_Search(uint8_t slaveaddr)
{
	uint8_t PlayMode = 0;
	uint16_t StartAddr = 0;
	uint32_t inSize = 0;
	int Files = 0;
	int TimeOut = 0;
	
	//�t�@�C�������݂������X�L�b�v���Ă����iI2C�A�h���X�������ꍇ�� 0 �j
	while(1) {
		if (inSize != 0) {
			StartAddr += inSize + 8;
			inSize = 0;
		}
		TimeOut = 0;
		do {
			USI_TWI_Master_tranBegin(slaveaddr);
			USI_TWI_Master_tranSend((uint8_t)(StartAddr >> 8));
			USI_TWI_Master_tranSend((uint8_t)(StartAddr & 0xFF));
			TimeOut ++;
		} while(USI_TWI_Master_tranCommit() && TimeOut < PWM_PCMPLAY_TIMEOUT);
		
		// I2C�A�h���X��������� 0 �Ƃ���
		if (TimeOut >= PWM_PCMPLAY_TIMEOUT) return 0;
		
		//WAV�t�H�[�}�b�g�̃w�b�_�t�@�C���ǂݍ���
		USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'RIFF' or 'MMLD'
		PlayMode = USI_TWI_Master_receive();		// 'R' or 'M'
		if (PlayMode != 'R' && PlayMode != 'M') return Files;
		USI_TWI_Master_receiveFrom(slaveaddr, 4);	//Size(below)
		
		//�t�@�C���̃T�C�Y�𓾂�
		for (uint8_t i = 0; i < 4; i++) {
			inSize >>= 8;
			inSize += ((uint32_t)USI_TWI_Master_receive() << 24);
		}
		Files ++;
	}
}
