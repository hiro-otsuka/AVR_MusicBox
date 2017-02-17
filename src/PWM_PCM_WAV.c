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
 *
 */

#include "USI_TWI_Master.h"
#include "PWM_PCM_WAV.h"

//********** �O���֐� **********//
//=============================
//EEPROM�����WAV�Đ����J�n
//	�����FI2C�X���[�u�A�h���X, �Đ��J�n�A�h���X
void PWM_PCM_WAV_Play(uint8_t slaveaddr, uint16_t StartAddr)
{
	// EEPROM ����w�b�_����ǂݍ���
	do {
		USI_TWI_Master_tranBegin(slaveaddr);
		USI_TWI_Master_tranSend((uint8_t)(StartAddr >> 8));
		USI_TWI_Master_tranSend((uint8_t)(StartAddr & 0xFF));
	} while(USI_TWI_Master_tranCommit());
	
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'WAVE'
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'fmt '
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//Size(below)
	USI_TWI_Master_receiveFrom(slaveaddr, 2);	//format
	USI_TWI_Master_receiveFrom(slaveaddr, 2);	//channels
	
	//�T���v�����O���[�g�𓾂�i8Kbps �Ŋm�F�ς݁j
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//samplerate
	
	uint32_t inRate = 0;
	for (uint8_t i = 0; i < 4; i++) {
		inRate >>= 8;
		inRate += ((uint32_t)USI_TWI_Master_receive() << 24);
	}
	
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//bytepersec
	USI_TWI_Master_receiveFrom(slaveaddr, 2);	//blockalign
	USI_TWI_Master_receiveFrom(slaveaddr, 2);	//bitswidth
	
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//'data'
	
	//�����f�[�^�̃T�C�Y�𓾂�
	USI_TWI_Master_receiveFrom(slaveaddr, 4);	//Size(below)

	uint32_t inSize = 0;
	for (uint8_t i = 0; i < 4; i++) {
		inSize >>= 8;
		inSize += ((uint32_t)USI_TWI_Master_receive() << 24);
	}
	
	//PWM�J�n
	PwmSound_Start((uint16_t)inRate);
	
	PWM_PCM_Playing = 1;
	
	//�Đ����I������܂�EEPROM����ǂݍ��ݑ�����
	while (PWM_PCM_Playing) {
		USI_TWI_Master_receiveFrom(slaveaddr, PWM_PCMPLAY_READSIZE);
		for(uint8_t i = 0; i < PWM_PCMPLAY_READSIZE ; i++) {
			PwmSound_AddBuff(USI_TWI_Master_receive());
		}
		//�Ōオ���ꂢ�ɏI���Ȃ��̂ŗv���P
		if (inSize < PWM_PCMPLAY_READSIZE) PWM_PCM_Playing = 0;
		else inSize -= PWM_PCMPLAY_READSIZE;
	}

	//PWM�I��
	PwmSound_Stop();
}
