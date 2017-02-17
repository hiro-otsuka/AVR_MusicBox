/*
 * main.h
 *
 * �T�v�F
 *  PWM�ɂ�鉹�y�E�����Đ��e�X�g
 *
 * �g�p���@���F
 *  BTN0 �Ń�������̉��y���Đ��ABTN1 ��EEPROM �� WAV,MML ���Đ�
 *  �Ȃ��A����̓T���v���ł���A�g�p���͗v�C��
 *
 * ���C�Z���X�F
 *  �p�u���b�N�h���C��/Unlicense�i�T���v���R�[�h�̂��߁j
 *
 * �X�V�����F
 *  2017/01/29 �V�K�쐬(Hiro OTSUKA)
 *  2017/02/17 �\���ύX(Hiro OTSUKA) EEPROM�����MML�Đ������WAV�Ƃ̎������ʂɑΉ�
 *
 */

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include "PIN_Control.h"
#include "PWM_EEPROM_Play.h"

//�y������ include�i�����Ȃ邽�ߕ����j
#include "MML_HOMusic_C3.h"
#include "MML_Test.h"

int main(void)
{
	uint8_t MusicMax = 0;
	uint8_t MusicNum = 0;
	uint8_t VoiceNum = 0;
	
	//�}�C�R��������������
	PIN_Control_PinAssign();
	PIN_Control_IntAssign();
	
	//I2C ������������
	USI_TWI_Master_Initialize(16000, 1000);

	//���t���W���[��������������
	PWM_PCM_MB_Init();
	
	//�y������o�^����
	PWM_PCM_MB_MEM_SetScore(0, (void(*)())MML_HOMusic_C3_Setup);
	PWM_PCM_MB_MEM_SetScore(1, (void(*)())MML_Test_Setup);
	
	//EEPROM �̃t�@�C�����𓾂�
	EEPROM_Init();
	MusicMax = EEPROM_Files;
	
	sei();
	while (1) {
		//���t���ĂȂ��Ԃ̓X���[�v�ɓ���
		if (PIN_Control_Key == 0) {
			PIN_Control_Playing(0);	//�O���̃I�[�f�B�I�A���v��OFF
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_mode();
			PIN_Control_Playing(1);	//�O���̃I�[�f�B�I�A���v��ON
			_delay_ms(30);			//�I�[�f�B�I�A���v�̋N���҂�
		}
		
		//���S�ɃL�[���������������܂ő҂�
		PIN_Control_WaitKeyOff(0);

		//BTN0 �������ꂽ�ꍇ�� EEPROM ���t�ɍĐ�����
		if (PIN_Control_Key == (1<<0)) {
			PIN_Control_Key = 0;
			if (VoiceNum == 0) VoiceNum = MusicMax - 1;	// 0 �ɒB������ő吔�ɖ߂�
			else VoiceNum --;
			EEPROM_Play(VoiceNum);
		//BTN1 �������ꂽ�ꍇ�� EEPROM �����ɍĐ�����
		} else if(PIN_Control_Key == (1<<1)) {
			PIN_Control_Key = 0;
			EEPROM_Play(VoiceNum ++);
			if (VoiceNum >= MusicMax) VoiceNum = 0;	//�ő吔�ɒB������ 0 �ɖ߂�
		//BTN0��BTN1 �������ꂽ�ꍇ�� �����y�������ɍĐ�����
		} else if(PIN_Control_Key == ((1<<0)|(1<<1))) {
			PIN_Control_Key = 0;
			PWM_PCM_MB_MEM_Play(MusicNum ++);
			if (MusicNum >= 2) MusicNum = 0;
		//�ǂ�ł��Ȃ���΃L�[����������
		} else {
			PIN_Control_Key = 0;
		}
		//���S�ɃL�[���������������܂ő҂�
		PIN_Control_WaitKeyOff(0);
	}
}

