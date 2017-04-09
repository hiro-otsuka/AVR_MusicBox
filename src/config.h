/*
 * config.h
 *
 * �T�v�F
 *  PIN_Control.h ���̋K��ɏ]���āA�v���W�F�N�g�̃s���A�T�C�����`����
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
 *  2017/04/09 �@�\�ǉ�(Hiro OTSUKA) �V���A���ʐM���[�h���̂��߂̐ݒ荀�ڂ�ǉ�
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//#define F_CPU 16000000UL	//CPU�X�s�[�h���` �� �v���W�F�N�g�Œ�`���������悢

//********** �@�\��` **********//
//���t����PLAYING�s����Low�ɂȂ�@�\��L�����i�I�[�f�B�I�A���v��CE�s���ւ̐ڑ���z��j
//	��ATTiny85�̏ꍇ�F�s����������Ȃ����߃��Z�b�g���������K�v��BTN2/LED2�Ƌ���
#define _PLAYING_ENABLE

//PCM��񂪊Ԃɍ���Ȃ��ꍇ�� PWM_DEBUG_OUT �� 1 ���o�͂���@�\��L����
//	��ATTiny85�̏ꍇ�FBTN1/LED1 �Ƌ���
//#define	_PWM_DEBUG

//�t���[���ԕ⊮��L����
//	���g�p����ꍇ�FMUSIC_SCORE_CHANNELS �̍ő�=2 �����E
//#define _HIGH_RESO

//********** ���\��` **********//
#define MUSIC_SCORES			4		//�o�^����y���̍ő吔�i8K�̃������ł�4�Ȓ��x�����E��EEPROM�Ǎ��Đ��ɑΉ��������߉e���͏��j
#define	MUSIC_SCORE_CHANNELS	3		//�`���l�����iCPU���\�Ɉˑ�. ATTiny85 16MHz �� MUSIC_PLAY_FREQ=16000�̏ꍇ�A3���ő�j
#define MUSIC_PLAY_FREQ			16000	//SINE�g�Đ��̉𑜓x�i8000�ɂ���ƍ������c�ށj
#define PWM_SOUND_FREQ			32000	//PCM�Đ��𑜓x�i8000,16000,32000���I���\�j
#define PIN_CONTROL_WAIT		50		//BTN�������̃`���^�����O�h�~���Ԃ̒���(ms)���ŒZ�������x�ȊO
#define PIN_CONTROL_WAIT_SERIAL	5		//�V���A���ʐM���̃`���^�����O�h�~���Ԃ̒���(ms)���ŒZ�������x
#define PWM_PCMPLAY_PARAMS		32		//EEPROM ����ǂݍ��ރp�����[�^�̍ő�T�C�Y�i <= EEPROM_ARRAY_BUFF_MAX �ł��邱�Ɓj
#define MML_PREREAD_NUM			8		//EEPROM ���烁������Ɉ�x�ɓǂݍ��� MML��
#define PIN_SERIAL_BTN			12		//�V���A���ʐM���Ɏw�肳�ꂤ��{�^���̐�

//********** �s����` **********//
// BTN/LED �̑I��
// _PINx_LED ���`= PINx ���o��(LED)�Ƃ��Ďg��
// _PINx_BTN ���`= PINx �����(BTN)�Ƃ��Ďg��
// _PINx_BTN_INT ���`= BTNx �������ꂽ�Ƃ��Ɋ��荞�ݏ���(sleep ����̕��A�܂�)���s��
#define _PIN0_BTN
#define _PIN0_BTN_INT
#define _PIN1_BTN
#define _PIN1_BTN_INT
//#define _PIN2_BTN
//#define _PIN2_BTN_INT
//#define _PIN3_BTN
//#define _PIN3_BTN_INT
//#define _PIN4_BTN
//#define _PIN4_BTN_INT
//#define _PIN5_BTN
//#define _PIN5_BTN_INT
//#define _PIN6_BTN
//#define _PIN6_BTN_INT
//#define _PIN7_BTN
//#define _PIN7_BTN_INT

//#define _PIN0_LED
//#define _PIN1_LED
//#define _PIN2_LED
//#define _PIN3_LED
//#define _PIN4_LED
//#define _PIN5_LED
//#define _PIN6_LED
//#define _PIN7_LED

#endif /* CONFIG_H_ */
