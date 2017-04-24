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
 *  2017/04/09 �@�\�ύX(Hiro OTSUKA) �Đ����[�h�� EEPROM �̃p�����[�^����ǂݍ��ނ悤�ύX
 *  2017/04/10 �o�O�C��(Hiro OTSUKA) �Đ����[�h SERIAL_NUM ���̖����C��
 *  2017/04/22 �\���ύX(Hiro OTSUKA) �s����PU�v�ۂƍĐ����[�h�����EEPROM�Ŏw��ł���悤�ύX
 *
 */

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include "PIN_Control.h"
#include "PWM_EEPROM_Play.h"

//EEPROM��̃p�����[�^�A�h���X���`
#define PARAM_PLAYMODE	0x00
#define PARAM_ISPULLUP	0x01

//�Đ����[�h���`�iEEPROM���̃p�����[�^�t�@�C�� 0 �o�C�g�ڂŎw��j
#define _PLAYMODE_SPLIT			0x00	//�����Đ����[�h(BTN0 = PCM�ABTN1 = MML) ���p�����[�^�������ꍇ�̃f�t�H���g
#define _PLAYMODE_ALL			0x01	//�S�Đ����[�h(BTN0 = �߂�ABTN1 = �i��)
#define _PLAYMODE_SERIAL_NUM	0x02	//�V���A���w�胂�[�h(BTN0 ������ BTN1 ���������񐔂Ŏw��)
#define _PLAYMODE_SERIAL_BTN	0x03	//�V���A���w�胂�[�h(BTN0 ������ BTN1 ���������񐔂Ń{�^���ԍ����w��)

int main(void)
{
	uint8_t MusicNum = 0;
	uint8_t VoiceNum = 0;
	uint8_t BtnNum[PIN_SERIAL_BTN];

	//����EEPROM����p�����[�^���擾����
	eeprom_busy_wait();
	uint8_t PlayMode = eeprom_read_byte((uint8_t*)PARAM_PLAYMODE);
	eeprom_busy_wait();
	uint8_t IsNotPU = eeprom_read_byte((uint8_t*)PARAM_ISPULLUP);
	
	//�}�C�R��������������
	PIN_Control_Init(IsNotPU);
	
	//I2C ������������
	EEPROM_Array_Init(16000, 1000);

	//���t���W���[��������������
	PWM_PCM_MB_Init();
	
	//EEPROM �̃t�@�C�����𓾂�
	EEPROM_Init();
	
	//MML���������ꍇ�͉��L�̒ʂ���s����i�v���O�������������g�p����̂ŗv���Ӂj
	//MML�̐錾----
	//#include "MML_Test.h"
	//MML�̓o�^----
	//PWM_PCM_MB_MEM_SetScore(0, (void(*)())MML_Test_Setup);
	//MML�̍Đ�----
	//PWM_PCM_MB_MEM_Play(0);
	
	//�Đ��O�̏����ݒ�	
	for(uint8_t i = 0; i < PIN_SERIAL_BTN; i ++) BtnNum[i] = 0;
	
	if (PlayMode == _PLAYMODE_SERIAL_NUM || PlayMode == _PLAYMODE_SERIAL_BTN)
		PIN_Control_SetWait(PIN_CONTROL_WAIT_SERIAL);
	else
		PIN_Control_SetWait(PIN_CONTROL_WAIT);
	
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
		switch(PlayMode) {
			case _PLAYMODE_SERIAL_BTN:
			case _PLAYMODE_SERIAL_NUM:
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
						VoiceNum --;
						//�{�^���ԍ����w�肷�郂�[�h�̏ꍇ
						if (PlayMode == _PLAYMODE_SERIAL_BTN) {
							//�e�{�^���Ɏw�肳�ꂽ�͈͂����t����
							MusicNum = BtnNum[VoiceNum];
							if (MusicNum < EEPROM_Params[VoiceNum]) MusicNum = EEPROM_Params[VoiceNum];
							BtnNum[VoiceNum] = MusicNum + 1;
							if (BtnNum[VoiceNum] >= EEPROM_Params[VoiceNum+1]) BtnNum[VoiceNum] = EEPROM_Params[VoiceNum];
						}
						while(MusicNum >= EEPROM_Files[PWM_PCMPLAY_ANY]) MusicNum -= EEPROM_Files[PWM_PCMPLAY_ANY];
						EEPROM_Play(PWM_PCMPLAY_ANY, MusicNum);
					}
				} else {
					//�ǂ�ł��Ȃ���΃L�[����������
					PIN_Control_Key = 0;
				}
				break;
			case _PLAYMODE_ALL:
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
					VoiceNum ++;
					if (VoiceNum >= EEPROM_Files[PWM_PCMPLAY_ANY]) VoiceNum = 0;		//�ő吔�ɒB������ 0 �ɖ߂�
					EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum);
				} else {
					//BTN0��BTN1 �������ꂽ�ꍇ�� ��~����
					//�ǂ�ł��Ȃ���΃L�[����������
					PIN_Control_Key = 0;
				}
				break;
			case _PLAYMODE_SPLIT:
			default:	//�w�肪�Ȃ��ꍇ�����l�Ɉ���
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
				} else {
					//�ǂ�ł��Ȃ���΃L�[����������
					PIN_Control_Key = 0;
				}
				break;
		}
		//���S�ɃL�[���������������܂ő҂�
		PIN_Control_WaitKeyOff(0);
	}
}

