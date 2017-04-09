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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include "PIN_Control_PUI.h"

int main(void)
{
	//�}�C�R��������������
	PIN_Control_PUI_Init();
	
	PIN_Control_PUI_SetWait(PIN_CONTROL_PUI_WAIT, PIN_SERIAL_WAIT_BOOT, PIN_SERIAL_WAIT_SERIAL);
	
	sei();
	while (1) {
		//���t���ĂȂ��Ԃ̓X���[�v�ɓ���
		if (PIN_Control_PUI_Key == 0) {
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_mode();
		}
		
		//���S�ɃL�[���������������܂ő҂�
		PIN_Control_PUI_WaitKeyOff(0);
		
		//�Đ����̏ꍇ�͈ꎞ��~����
		if (PIN_Control_PUI_isPlaying()) PIN_Control_PUI_Serial(0);
		//�����ꂽ�{�^���ɂ��A�V���A���ʐM���s��
		uint16_t btnTmp = PIN_Control_PUI_Key;
		uint8_t musicNum = 1;
		for (int i = 0; i < 16; i++) {
			if (btnTmp & 1) break;
			musicNum ++;
			btnTmp >>= 1;
		}
		if (btnTmp != 0) PIN_Control_PUI_Serial(musicNum);
		
		PIN_Control_PUI_Key = 0;
		//���S�ɃL�[���������������܂ő҂�
		PIN_Control_PUI_WaitKeyOff(0);
	}
}

