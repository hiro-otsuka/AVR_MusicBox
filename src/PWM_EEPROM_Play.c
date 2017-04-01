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
 *  2017/02/25 �@�\���P(Hiro OTSUKA) MML��WAV�𕪗����čĐ��ł���悤�@�\���P
 *  2017/02/26 �@�\�ǉ�(Hiro OTSUKA) Init����EEPROM����p�����[�^��ǂݍ��ދ@�\��ǉ�
 *  2017/04/01 �@�\�ύX(Hiro OTSUKA) EEPROM Array �̎����ɑΉ�
 *
 */

#include "PWM_EEPROM_Play.h"

//********** �֐���` **********//
uint8_t EEPROM_Search(uint8_t, uint8_t*, uint8_t* );

//********** �萔��` **********//

//********** �ϐ���` **********//
// EEPROM �ɕۑ�����Ă���t�@�C���̐��i��ޕʁj
volatile uint8_t EEPROM_Files[PWM_PCMPLAY_TYPES];
// EEPROM �ɕۑ�����Ă���p�����[�^��ۑ�
volatile uint8_t EEPROM_Params[PWM_PCMPLAY_PARAMS];

//********** �O���֐� **********//
//=============================
//EEPROM�̃t�@�C����������
void EEPROM_Init()
{
	for (uint8_t i = 0; i < PWM_PCMPLAY_PARAMS; i++ ) EEPROM_Params[i] = 0;
	for (uint8_t i = 0; i < PWM_PCMPLAY_TYPES; i++ ) EEPROM_Files[i] = 0;
	
	uint8_t PlayMode = 0;
	uint32_t StartAddr = 0;
	uint32_t inSize = 0;
	
	//�t�@�C�������݂������X�L�b�v���Ă����iI2C�A�h���X�������ꍇ�� 0 �j
	while(1) {
		if (inSize != 0) {
			StartAddr += inSize + 8;
			inSize = 0;
		}
		if (EEPROM_Array_SetAddr(StartAddr)) return;

		//WAV�t�H�[�}�b�g�̃w�b�_�t�@�C���ǂݍ���
		EEPROM_Array_Read(4);	//'RIFF' or 'MMLD' or 'PARM'
		PlayMode = EEPROM_Array_data();		// 'R' or 'M' or 'P'
		//�ǂ�ł��Ȃ��ꍇ�͏I��
		if (PlayMode != 'R' && PlayMode != 'M' && PlayMode != 'P') break;
		//�t�@�C���̃T�C�Y�𓾂�
		EEPROM_Array_Read(4);	//Size(below)
		inSize = EEPROM_Array_data4();
		
		//��ނ��ƂɑΏۂ̃t�@�C�����J�E���g�A�b�v����
		if (PlayMode == 'R') EEPROM_Files[PWM_PCMPLAY_PCM] ++;
		else if (PlayMode == 'M') EEPROM_Files[PWM_PCMPLAY_MML] ++;
		else if (PlayMode == 'P') {
			//�p�����[�^���������ꍇ�̓J�E���g�����A���̏�œǂݍ���
			EEPROM_Array_Read(inSize);
			for (uint8_t i = 0; i < inSize; i++) {
				EEPROM_Params[i] = EEPROM_Array_data();
			}
		}
	}
}

//=============================
//EEPROM����̃t�@�C���Đ����J�n
//	�����F�Đ�����t�@�C���̎��, ���Ԗڂ̏����Đ����邩
void EEPROM_Play(uint8_t type, uint8_t count)
{

	uint8_t PlayMode = 0;
	uint32_t StartAddr = 0;
	uint32_t inSize = 0;
	
	//�w�肳�ꂽ count �Ԗڂ̃t�@�C����������܂ŃX�L�b�v���Ă���
	do {
		if (inSize != 0) {
			StartAddr += inSize + 8;
			inSize = 0;
		}
		if (EEPROM_Array_SetAddr(StartAddr)) return;

		//WAV�t�H�[�}�b�g�̃w�b�_�t�@�C���ǂݍ���
		EEPROM_Array_Read(4);	//'RIFF' or 'MMLD' or 'PARM'
		PlayMode = EEPROM_Array_data();		// 'R' or 'M' or 'P'
		//�ǂ�ł��Ȃ��ꍇ�͏I��
		if (PlayMode != 'R' && PlayMode != 'M' && PlayMode != 'P') break;

		//�t�@�C���̃T�C�Y�𓾂�
		EEPROM_Array_Read(4);	//Size(below)
		inSize = EEPROM_Array_data4();

		//�Ώۃt�@�C������Ȃ��ꍇ�̓��[�v�𔲂��Ȃ��i'P'�͏�ɂȂ��������Ƃɂ���j
		if (PlayMode == 'P') count++;
		else if (type == PWM_PCMPLAY_MML && PlayMode != 'M') count ++;
		else if (type == PWM_PCMPLAY_PCM && PlayMode != 'R') count ++;
	} while(count --);
	
	//�t�H�[�}�b�g���m�F
	switch(PlayMode) {
		case 'R':	//RIFF = WAV
		PWM_PCM_WAV_Play(StartAddr + 4+4);
		break;
		case 'M':	//MMLD = MML
		PWM_PCM_MB_EEPROM_Play(StartAddr + 4+4);
		break;
	}
}

