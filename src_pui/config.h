/*
 * config.h
 *
 * �T�v�F
 *  PIN_Control_PUI.h ���̋K��ɏ]���āA�v���W�F�N�g�̃s���A�T�C�����`����
 *
 * �g�p���@���F
 *  �\�[�X���̃R�����g���Q��
 *  �Ȃ��A����̓T���v���v���W�F�N�g�̂��߂̒�`�ł���A�g�p���͗v�C��
 *
 * ���C�Z���X�F
 *  �p�u���b�N�h���C��/Unlicense�i�T���v���R�[�h�̂��߁j
 *
 * �X�V�����F
 *  2017/01/29 �V�K�쐬(Hiro OTSUKA)
 *  2017/02/17 �\���ύX(Hiro OTSUKA) EEPROM�����MML�Đ������WAV�Ƃ̎������ʂɑΉ�
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//********** �@�\��` **********//

//********** ���\��` **********//
#define PIN_CONTROL_PUI_WAIT	50		//BTN�������̃`���^�����O�h�~���Ԃ̒���(ms)���ŒZ�������x
#define PIN_SERIAL_WAIT_BOOT	30		//Serial�ʐM���̊O�����W���[���N���҂�����
#define PIN_SERIAL_WAIT_SERIAL	10		//Serial�ʐM���̑ҋ@���Ԃ̒���(ms)��AVR_MB��PIN_CONTROL_WAIT�Ƃ��킹��

//********** �s����` **********//
// BTN/LED �̑I��
// _PINx_LED ���`= PINx ���o��(LED)�Ƃ��Ďg��
// _PINx_BTN ���`= PINx �����(BTN)�Ƃ��Ďg��
// _PINx_BTN_INT ���`= BTNx �������ꂽ�Ƃ��Ɋ��荞�ݏ���(sleep ����̕��A�܂�)���s��
#define _PIN0_BTN
#define _PIN0_BTN_INT
#define _PIN1_BTN
#define _PIN1_BTN_INT
#define _PIN2_BTN
#define _PIN2_BTN_INT
#define _PIN3_BTN
#define _PIN3_BTN_INT
#define _PIN4_BTN
#define _PIN4_BTN_INT
#define _PIN5_BTN
#define _PIN5_BTN_INT
//#define _PIN6_BTN
//#define _PIN6_BTN_INT
//#define _PIN7_BTN
//#define _PIN7_BTN_INT
//#define _PIN8_BTN
//#define _PIN8_BTN_INT
//#define _PIN9_BTN
//#define _PIN9_BTN_INT
//#define _PIN10_BTN
//#define _PIN10_BTN_INT
//#define _PIN11_BTN
//#define _PIN11_BTN_INT

//#define _PIN0_LED
//#define _PIN1_LED
//#define _PIN2_LED
//#define _PIN3_LED
//#define _PIN4_LED
//#define _PIN5_LED
//#define _PIN6_LED
//#define _PIN7_LED
//#define _PIN8_LED
//#define _PIN9_LED
//#define _PIN10_LED
//#define _PIN11_LED

#endif /* CONFIG_H_ */
