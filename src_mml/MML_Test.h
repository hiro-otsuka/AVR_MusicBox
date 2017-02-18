/*
 * MML_Test.h
 *
 * �T�v�F
 *  �T���v���f�[�^
 *
 * �g�p���@���F
 *  main.c ���� include ���AMusicBox_SetScore �̈����Ƃ��� Setup �֐��ւ̃|�C���^��n��
 *
 * ���C�Z���X�F
 *  �p�u���b�N�h���C��/Unlicense�i�T���v���R�[�h�̂��߁j
 *
 * �X�V�����F
 *  2017/01/29 �V�K�쐬(Hiro OTSUKA)
 *  2017/02/17 �\���ύX(Hiro OTSUKA) EEPROM�����MML�Đ������WAV�Ƃ̎������ʂɑΉ�
 *  2017/02/18 �@�\�ǉ�(Hiro OTSUKA) ���g�̒ǉ��ɑΉ�
 *
 */

#ifndef MML_TEST_H_
#define MML_TEST_H_

static void MML_Test_Setup()
{
	#undef N_LEN
	#undef N_KEY
	#define N_LEN	128		//1���������F16MHz ����ł� 128 * (1/16000000) * (1024 * 256) = 128 * 0.016384 = ��2.097�b
	#define N_KEY	0		//�L�[�ύX

	const static uint16_t Buff1[] PROGMEM = {
		M_CTRL(C_VOL, 16),			//�{�����[����ݒ�
		
		M_CTRL(C_DIVST, 12),		//�����J�n�҂��i12 * 0.016384 = ��0.2�b�j
		M_CTRL(C_DIVWT, 4),			//�����Ԋu�i4 * 0.016384 = 0.065�b�A�{�����[�� 16 ���� 0 �ɂȂ�܂� �� 1 �b�j
		M_CTRL(C_TYPE, 0),			//�����g
		M_NOTE(N_C, 0, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_E, 0, N_LEN/8), M_NOTE(N_F, 0, N_LEN/8), M_NOTE(N_G, 0, N_LEN/8), M_NOTE(N_A, 0, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_F, 2, N_LEN/8), M_NOTE(N_G, 2, N_LEN/8), M_NOTE(N_A, 2, N_LEN/8), M_NOTE(N_B, 2, N_LEN/8),
		M_NOTE(N_C, 3, N_LEN/8), M_NOTE(N_D, 3, N_LEN/8), M_NOTE(N_E, 3, N_LEN/8), M_NOTE(N_F, 3, N_LEN/8), M_NOTE(N_G, 3, N_LEN/8), M_NOTE(N_A, 3, N_LEN/8), M_NOTE(N_B, 3, N_LEN/8),
		M_NOTE(N_C, 4, N_LEN/8), M_NOTE(N_D, 4, N_LEN/8), M_NOTE(N_E, 4, N_LEN/8), M_NOTE(N_F, 4, N_LEN/8), M_NOTE(N_G, 4, N_LEN/8), M_NOTE(N_A, 4, N_LEN/8), M_NOTE(N_B, 4, N_LEN/8),

		M_CTRL(C_TYPE, 1),			//�O�p�g
		M_CTRL(C_DIVST, 16),		//�����J�n�҂��i16 * 0.016384 = ��0.25�b�j
		M_CTRL(C_DIVWT, 1),			//�����Ԋu�i1 * 0.016384 = 0.016�b�A�{�����[�� 16 ���� 0 �ɂȂ�܂� �� 0.2 �b�j
		M_NOTE(N_C, 0, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_E, 0, N_LEN/8), M_NOTE(N_F, 0, N_LEN/8), M_NOTE(N_G, 0, N_LEN/8), M_NOTE(N_A, 0, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_F, 2, N_LEN/8), M_NOTE(N_G, 2, N_LEN/8), M_NOTE(N_A, 2, N_LEN/8), M_NOTE(N_B, 2, N_LEN/8),
		M_NOTE(N_C, 3, N_LEN/8), M_NOTE(N_D, 3, N_LEN/8), M_NOTE(N_E, 3, N_LEN/8), M_NOTE(N_F, 3, N_LEN/8), M_NOTE(N_G, 3, N_LEN/8), M_NOTE(N_A, 3, N_LEN/8), M_NOTE(N_B, 3, N_LEN/8),
		M_NOTE(N_C, 4, N_LEN/8), M_NOTE(N_D, 4, N_LEN/8), M_NOTE(N_E, 4, N_LEN/8), M_NOTE(N_F, 4, N_LEN/8), M_NOTE(N_G, 4, N_LEN/8), M_NOTE(N_A, 4, N_LEN/8), M_NOTE(N_B, 4, N_LEN/8),
		
		M_CTRL(C_DIVST, 1),			//�����J�n�҂��i1 * 0.016384 = ��0.016�b�j
		M_CTRL(C_DIVWT, 6),			//�����Ԋu�i6 * 0.016384 = 0.1�b�A�{�����[�� 16 ���� 0 �ɂȂ�܂� �� 1.5 �b�j
		M_CTRL(C_TYPE, 2),			//��`�g
		M_NOTE(N_C, 0, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_E, 0, N_LEN/8), M_NOTE(N_F, 0, N_LEN/8), M_NOTE(N_G, 0, N_LEN/8), M_NOTE(N_A, 0, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_F, 2, N_LEN/8), M_NOTE(N_G, 2, N_LEN/8), M_NOTE(N_A, 2, N_LEN/8), M_NOTE(N_B, 2, N_LEN/8),
		M_NOTE(N_C, 3, N_LEN/8), M_NOTE(N_D, 3, N_LEN/8), M_NOTE(N_E, 3, N_LEN/8), M_NOTE(N_F, 3, N_LEN/8), M_NOTE(N_G, 3, N_LEN/8), M_NOTE(N_A, 3, N_LEN/8), M_NOTE(N_B, 3, N_LEN/8),
		M_NOTE(N_C, 4, N_LEN/8), M_NOTE(N_D, 4, N_LEN/8), M_NOTE(N_E, 4, N_LEN/8), M_NOTE(N_F, 4, N_LEN/8), M_NOTE(N_G, 4, N_LEN/8), M_NOTE(N_A, 4, N_LEN/8), M_NOTE(N_B, 4, N_LEN/8),
		
		M_CTRL(C_DIVST, 1),			//�����J�n�҂��i1 * 0.016384 = ��0.016�b�j
		M_CTRL(C_DIVWT, 6),			//�����Ԋu�i6 * 0.016384 = 0.1�b�A�{�����[�� 16 ���� 0 �ɂȂ�܂� �� 1.5 �b�j
		M_CTRL(C_TYPE, 3),			//���g
		M_NOTE(N_C, 0, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_E, 0, N_LEN/8), M_NOTE(N_F, 0, N_LEN/8), M_NOTE(N_G, 0, N_LEN/8), M_NOTE(N_A, 0, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_F, 2, N_LEN/8), M_NOTE(N_G, 2, N_LEN/8), M_NOTE(N_A, 2, N_LEN/8), M_NOTE(N_B, 2, N_LEN/8),
		M_NOTE(N_C, 3, N_LEN/8), M_NOTE(N_D, 3, N_LEN/8), M_NOTE(N_E, 3, N_LEN/8), M_NOTE(N_F, 3, N_LEN/8), M_NOTE(N_G, 3, N_LEN/8), M_NOTE(N_A, 3, N_LEN/8), M_NOTE(N_B, 3, N_LEN/8),
		M_NOTE(N_C, 4, N_LEN/8), M_NOTE(N_D, 4, N_LEN/8), M_NOTE(N_E, 4, N_LEN/8), M_NOTE(N_F, 4, N_LEN/8), M_NOTE(N_G, 4, N_LEN/8), M_NOTE(N_A, 4, N_LEN/8), M_NOTE(N_B, 4, N_LEN/8),
		
		M_CTRL(C_END, 0),
	};
	
	//const static uint16_t Buff2[] PROGMEM = { };
	//const static uint16_t Buff3[] PROGMEM = { };
	
	PWM_PCM_MB_MEM_SetScoreChannel(0, Buff1, sizeof(Buff1)/2);
	//PWM_PCM_MB_MEM_SetScoreChannel(1, Buff2, sizeof(Buff2)/2);
	//PWM_PCM_MB_MEM_SetScoreChannel(2, Buff3, sizeof(Buff3)/2);
}

#endif
