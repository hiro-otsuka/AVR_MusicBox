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
 *  2017/02/25 �@�\���P(Hiro OTSUKA) MML��WAV�𕪗����čĐ����郂�[�h�ƃV���A���ʐM�w��̃��[�h��ǉ�
 *  2017/02/26 �@�\�ǉ�(Hiro OTSUKA) Init����EEPROM����p�����[�^��ǂݍ��ދ@�\��ǉ�
 *  2017/04/01 �@�\�ύX(Hiro OTSUKA) EEPROM Array �̎����ɑΉ�
 *
 */

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include "PIN_Control.h"
#include "PWM_EEPROM_Play.h"

//�Đ����[�h��ݒ�
//#define _MODE_PLAYSERIAL	//�V���A���w�胂�[�h(BTN0 ������ BTN1 ���������񐔂Ŏw��)
//#define _MODE_PLAYALL		//�S�Đ����[�h(BTN0 = �߂�ABTN1 = �i��)
#define _MODE_PLAYSPLIT		//�����Đ����[�h(BTN0 = PCM�ABTN1 = MML)
//#define _MODE_DEBUG			//�����Đ����[�h(BTN0 = �����Đ��ABTN1 = EEPROM���珇�Đ�)

//�e�X�g���s����ꍇ�͊y������ include�i�����Ȃ邽�ߕ����j
#ifdef _MODE_DEBUG
 #include "MML_Test.h"
#endif

int main(void)
{
	uint8_t MusicNum = 0;
	uint8_t VoiceNum = 0;
	
	//�}�C�R��������������
	PIN_Control_PinAssign();
	PIN_Control_IntAssign();
	
	//I2C ������������
	EEPROM_Array_Init(16000, 1000);

	//���t���W���[��������������
	PWM_PCM_MB_Init();
	
	//�e�X�g���s����ꍇ�͊y������o�^����
#ifdef _MODE_DEBUG
	PWM_PCM_MB_MEM_SetScore(0, (void(*)())MML_Test_Setup);
#endif
	
	//EEPROM �̃t�@�C�����𓾂�
	EEPROM_Init();
	
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

#if defined _MODE_PLAYSERIAL
		//�V���A���ʐM�ŏ��Ԃ��w�肷�郂�[�h====================
		if (PIN_Control_Key == (1<<0)) {
			VoiceNum = 0;
			//���S�ɉ������܂ő҂�
			PIN_Control_WaitKeyOn(~(1 << 0));
			//BTN0 ���������܂ŁABTN1 �������ꂽ�񐔂��v������
			while (PIN_Control_GetKey() & (1 << 0)) {
				//BTN1 �������ꂽ��A�������܂ő҂�
				if(PIN_Control_GetKey() & (1 << 1)) {
					//���S�ɉ������܂ő҂�
					PIN_Control_WaitKeyOn(~(1 << 1));
					VoiceNum ++;
					//�������܂ő҂�
					while(PIN_Control_GetKey() & (1 << 1));
					//���S�ɗ������܂ő҂�
					PIN_Control_WaitKeyOff(~(1 << 1));
				}
			}
			//���S�ɃL�[���������������܂ő҂�
			PIN_Control_WaitKeyOff(0);
			//BTN1 ��������Ȃ������ꍇ�͉��t��~���邾���ŏI��
			PIN_Control_Key = 0;
			if (VoiceNum > 0) {
				while(VoiceNum > EEPROM_Files[PWM_PCMPLAY_ANY]) VoiceNum -= EEPROM_Files[PWM_PCMPLAY_ANY];
				EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum-1);
			}
#elif defined _MODE_PLAYALL
		//�����f�[�^�����ɍĐ����郂�[�h========================
		//���S�ɃL�[���������������܂ő҂�
		PIN_Control_WaitKeyOff(0);
		//BTN0 �������ꂽ�ꍇ�� EEPROM ���t�ɍĐ�����
		if (PIN_Control_Key == (1<<0)) {
			PIN_Control_Key = 0;
			if (VoiceNum == 0) VoiceNum = EEPROM_Files[PWM_PCMPLAY_ANY] - 1;	// 0 �ɒB������ő吔�ɖ߂�
			else VoiceNum --;
			EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum);
		//BTN1 �������ꂽ�ꍇ�� EEPROM �����ɍĐ�����
		} else if(PIN_Control_Key == (1<<1)) {
			PIN_Control_Key = 0;
			EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum ++);
			if (VoiceNum >= EEPROM_Files[PWM_PCMPLAY_ANY]) VoiceNum = 0;	//�ő吔�ɒB������ 0 �ɖ߂�
		//BTN0��BTN1 �������ꂽ�ꍇ�� ��~����
		} else if(PIN_Control_Key == ((1<<0)|(1<<1))) {
			PIN_Control_Key = 0;
#elif defined _MODE_PLAYSPLIT
		//�����f�[�^�� MML�� PCM �ɕ����čĐ����郂�[�h=========
		//���S�ɃL�[���������������܂ő҂�
		PIN_Control_WaitKeyOff(0);
		//BTN0 �������ꂽ�ꍇ�� PCM �����ɍĐ�����
		if (PIN_Control_Key == (1<<0) && EEPROM_Files[PWM_PCMPLAY_PCM] > 0) {
			PIN_Control_Key = 0;
			EEPROM_Play(PWM_PCMPLAY_PCM, VoiceNum ++);
			if (VoiceNum >= EEPROM_Files[PWM_PCMPLAY_PCM]) VoiceNum = 0;	//�ő吔�ɒB������ 0 �ɖ߂�
		//BTN1 �������ꂽ�ꍇ�� MML �����ɍĐ�����
		} else if(PIN_Control_Key == (1<<1) && EEPROM_Files[PWM_PCMPLAY_MML] > 0) {
			PIN_Control_Key = 0;
			EEPROM_Play(PWM_PCMPLAY_MML, MusicNum ++);
			if (MusicNum >= EEPROM_Files[PWM_PCMPLAY_MML]) MusicNum = 0;	//�ő吔�ɒB������ 0 �ɖ߂�
		//BTN0��BTN1 �������ꂽ�ꍇ�̓p�����[�^0 �̒l�i�ʏ��0�j�Ԗڂ��Đ�����
		} else if(PIN_Control_Key == ((1<<0)|(1<<1))) {
			PIN_Control_Key = 0;
			EEPROM_Play(PWM_PCMPLAY_ANY, EEPROM_Params[0]);
#elif defined _MODE_DEBUG
		//�����f�[�^���f�o�b�O�Đ����郂�[�h====================
		//BTN0 �������ꂽ�ꍇ�� �����y�� ���Đ�����
		if (PIN_Control_Key == (1<<0)) {
			PIN_Control_Key = 0;
			PWM_PCM_MB_MEM_Play(0);
		//BTN1 �������ꂽ�ꍇ�� EEPROM �����ɍĐ�����
		} else if(PIN_Control_Key == (1<<1)) {
			PIN_Control_Key = 0;
			EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum ++);
			if (VoiceNum >= EEPROM_Files[PWM_PCMPLAY_ANY]) VoiceNum = 0;	//�ő吔�ɒB������ 0 �ɖ߂�
		//BTN0��BTN1 �������ꂽ�ꍇ�� ��~����
		} else if(PIN_Control_Key == ((1<<0)|(1<<1))) {
			PIN_Control_Key = 0;
#endif
		//�ǂ�ł��Ȃ���΃L�[����������
		} else {
			PIN_Control_Key = 0;
		}
		//���S�ɃL�[���������������܂ő҂�
		PIN_Control_WaitKeyOff(0);
	}
}

