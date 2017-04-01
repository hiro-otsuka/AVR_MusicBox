/*
 * PWM_PCM_WAV.c
 *
 * �T�v�F
 *  I2C�ڑ���EEPROM����WAV�Đ����s�����W���[��
 *
 * �g�p���@���F
 *  PWM_PCM_WAV ���g�p����v���W�F�N�g�ɃC���|�[�g���Ďg�p
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/01/29 �V�K�쐬(Hiro OTSUKA)
 *  2017/02/17 �\���ύX(Hiro OTSUKA) EEPROM�����MML�Đ������WAV�Ƃ̎������ʂɑΉ�
 *  2017/04/01 �@�\�ǉ�(Hiro OTSUKA) IMA-ADPCM�̍Đ��@�\��ǉ�
 *  2017/04/01 �@�\�ύX(Hiro OTSUKA) EEPROM Array �̎����ɑΉ�
 *
 */

#include "EEPROM_Array.h"
#include "PWM_PCM_WAV.h"

//********** �֐���` **********//
int16_t PWM_PCM_WAV_ADPCM(int16_t* , uint8_t* , uint8_t );

//********** �ϐ���` **********//
// ADPCM�p�̌W��
static const uint16_t ADPCM_Step[89] PROGMEM = {
	7, 8, 9,
	10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28,	31,
	34, 37, 41, 45, 50, 55, 60, 66,	73, 80, 88, 97,
	107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658,	724, 796, 876, 963,
	1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
	3327, 3660, 4026, 4428, 4871, 5358,	5894, 6484, 7132, 7845, 8630, 9493,
	10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350,
	22385, 24623, 27086, 29794, 32767
};

//********** �O���֐� **********//
//=============================
//EEPROM�����WAV�Đ����J�n
//	�����F�Đ��J�n�A�h���X
void PWM_PCM_WAV_Play(uint32_t StartAddr)
{
	// EEPROM ����w�b�_����ǂݍ���
	if (EEPROM_Array_SetAddr(StartAddr)) return;
	
	EEPROM_Array_Read(4);	//'WAVE'
	EEPROM_Array_Read(4);	//'fmt '
	
	//�g���G���A�̃T�C�Y���Z�o���Ă���
	EEPROM_Array_Read(4);	//Size(below)
	uint32_t inExtSize = EEPROM_Array_data4() - 0x10;
	
	//�t�H�[�}�b�g��ʂ𓾂Ă���
	EEPROM_Array_Read(2);	//format
	uint16_t inType = EEPROM_Array_data2();
	
	EEPROM_Array_Read(2);	//channels
	
	//�T���v�����O���[�g�𓾂�i8Kbps �Ŋm�F�ς݁j
	EEPROM_Array_Read(4);	//samplerate
	uint32_t inRate = EEPROM_Array_data4();
	
	EEPROM_Array_Read(4);	//bytepersec
	
	//�u���b�N�T�C�Y�𓾂Ă����iADPCM�p�j
	EEPROM_Array_Read(2);	//blockalign
	uint16_t inBlock = EEPROM_Array_data2();
	
	EEPROM_Array_Read(2);	//bitswidth
	
	//�g���G���A������ꍇ�͓ǂݔ�΂�
	if (inExtSize != 0) {
		EEPROM_Array_Read((uint8_t)inExtSize);
	}
	
	//fact ���������ꍇ�̓f�[�^�����܂܂�Ă���̂œǂ�ł����iADPCM�p�j
	EEPROM_Array_Read(4);	//'data' or 'fact'
	uint32_t inNumber = 0;
	if (EEPROM_Array_data() == 'f') {
		EEPROM_Array_Read(4);	//Size(below)
		uint32_t inFactSize = EEPROM_Array_data4();
		EEPROM_Array_Read((uint8_t)inFactSize);
		inNumber = EEPROM_Array_data4();
		
		EEPROM_Array_Read(4);	//'data'
	}
	
	//�����f�[�^�̃T�C�Y�𓾂�
	EEPROM_Array_Read(4);	//Size(below)
	uint32_t inSize = EEPROM_Array_data4();
	
	//PWM�J�n
	PwmSound_Start((uint16_t)inRate);
	
	PWM_PCM_Playing = 1;
	
	//�Đ����I������܂�EEPROM����ǂݍ��ݑ�����
	switch(inType) {
		case 0x0001:
			while (PWM_PCM_Playing) {
				EEPROM_Array_Read(PWM_PCMPLAY_READSIZE);
				for(uint8_t i = 0; i < PWM_PCMPLAY_READSIZE ; i++) {
					PwmSound_AddBuff(EEPROM_Array_data());
				}
				//�Ōオ���ꂢ�ɏI���Ȃ��̂ŗv���P
				if (inSize < PWM_PCMPLAY_READSIZE) PWM_PCM_Playing = 0;
				else inSize -= PWM_PCMPLAY_READSIZE;
			}
			break;
		case 0x0011:
			while (PWM_PCM_Playing) {
				//�w�b�_������ǂ�
				EEPROM_Array_Read(4);
				//�w�b�_���珉���f�[�^���Z�b�g����
				int16_t adVal = (int16_t)EEPROM_Array_data2();
				uint8_t adIdx = EEPROM_Array_data();
				uint16_t adSize = (inBlock - 4) * 2 + 1;
				if (adSize > inNumber) adSize = inNumber;
				inNumber -= adSize;
				adSize --;
				PwmSound_AddBuff((uint8_t)(adVal >> 8) + 0x80);
				while(adSize != 0 && PWM_PCM_Playing) {
					EEPROM_Array_Read(1);
					uint8_t inData = EEPROM_Array_data();
					PwmSound_AddBuff((uint8_t)(PWM_PCM_WAV_ADPCM(&adVal, &adIdx, (inData & 0xF)) >> 8) + 0x80);
					PwmSound_AddBuff((uint8_t)(PWM_PCM_WAV_ADPCM(&adVal, &adIdx, (inData >>  4)) >> 8) + 0x80);
					adSize -= 2;
				}
				if (inNumber < 4) PWM_PCM_Playing = 0;
			}
			break;
			
	}

	//PWM�I��
	PwmSound_Stop();
}

//********** �����֐� **********//
//-----------------------------
//ADPCM���f�R�[�h����
int16_t PWM_PCM_WAV_ADPCM(int16_t* adVal, uint8_t* adIdx, uint8_t inVal)
{
	int32_t tmpVal;
	uint8_t tmpIdx = *adIdx;
	uint16_t tmpStep = pgm_read_word(&ADPCM_Step[tmpIdx]);
	
	tmpVal = tmpStep >> 3;
	if (inVal & 0b0100) tmpVal += tmpStep;
	tmpStep >>= 1;
	if (inVal & 0b0010) tmpVal += tmpStep;
	tmpStep >>= 1;
	if (inVal & 0b0001) tmpVal += tmpStep;
	if (inVal & 0b1000) tmpVal = 1 - tmpVal;
	
	tmpVal += *adVal;
	if (tmpVal >  32767) tmpVal =  32767;
	if (tmpVal < -32768) tmpVal = -32768;
	*adVal = (int16_t)tmpVal;
	
	inVal = (inVal & 0b0111) - 4;
	if (inVal > 3) {
		if (tmpIdx) tmpIdx --;
	} else {
		tmpIdx += (inVal + 1) << 1;
		if (tmpIdx > 88) tmpIdx = 88;
	}
	*adIdx = tmpIdx;
	
	return *adVal;
}

