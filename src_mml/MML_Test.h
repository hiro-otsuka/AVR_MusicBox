/*
 * MML_Test.h
 *
 * 概要：
 *  サンプルデータ
 *
 * 使用方法等：
 *  main.c から include し、MusicBox_SetScore の引数として Setup 関数へのポインタを渡す
 *
 * ライセンス：
 *  パブリックドメイン/Unlicense（サンプルコードのため）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *  2017/02/17 構成変更(Hiro OTSUKA) EEPROMからのMML再生およびWAVとの自動判別に対応
 *  2017/02/18 機能追加(Hiro OTSUKA) 鋸波の追加に対応
 *
 */

#ifndef MML_TEST_H_
#define MML_TEST_H_

static void MML_Test_Setup()
{
	#undef N_LEN
	#undef N_KEY
	#define N_LEN	128		//1分音符長：16MHz 動作では 128 * (1/16000000) * (1024 * 256) = 128 * 0.016384 = 約2.097秒
	#define N_KEY	0		//キー変更

	const static uint16_t Buff1[] PROGMEM = {
		M_CTRL(C_VOL, 16),			//ボリュームを設定
		
		M_CTRL(C_DIVST, 12),		//減衰開始待ち（12 * 0.016384 = 約0.2秒）
		M_CTRL(C_DIVWT, 4),			//減衰間隔（4 * 0.016384 = 0.065秒、ボリューム 16 から 0 になるまで 約 1 秒）
		M_CTRL(C_TYPE, 0),			//正弦波
		M_NOTE(N_C, 0, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_E, 0, N_LEN/8), M_NOTE(N_F, 0, N_LEN/8), M_NOTE(N_G, 0, N_LEN/8), M_NOTE(N_A, 0, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_F, 2, N_LEN/8), M_NOTE(N_G, 2, N_LEN/8), M_NOTE(N_A, 2, N_LEN/8), M_NOTE(N_B, 2, N_LEN/8),
		M_NOTE(N_C, 3, N_LEN/8), M_NOTE(N_D, 3, N_LEN/8), M_NOTE(N_E, 3, N_LEN/8), M_NOTE(N_F, 3, N_LEN/8), M_NOTE(N_G, 3, N_LEN/8), M_NOTE(N_A, 3, N_LEN/8), M_NOTE(N_B, 3, N_LEN/8),
		M_NOTE(N_C, 4, N_LEN/8), M_NOTE(N_D, 4, N_LEN/8), M_NOTE(N_E, 4, N_LEN/8), M_NOTE(N_F, 4, N_LEN/8), M_NOTE(N_G, 4, N_LEN/8), M_NOTE(N_A, 4, N_LEN/8), M_NOTE(N_B, 4, N_LEN/8),

		M_CTRL(C_TYPE, 1),			//三角波
		M_CTRL(C_DIVST, 16),		//減衰開始待ち（16 * 0.016384 = 約0.25秒）
		M_CTRL(C_DIVWT, 1),			//減衰間隔（1 * 0.016384 = 0.016秒、ボリューム 16 から 0 になるまで 約 0.2 秒）
		M_NOTE(N_C, 0, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_E, 0, N_LEN/8), M_NOTE(N_F, 0, N_LEN/8), M_NOTE(N_G, 0, N_LEN/8), M_NOTE(N_A, 0, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_F, 2, N_LEN/8), M_NOTE(N_G, 2, N_LEN/8), M_NOTE(N_A, 2, N_LEN/8), M_NOTE(N_B, 2, N_LEN/8),
		M_NOTE(N_C, 3, N_LEN/8), M_NOTE(N_D, 3, N_LEN/8), M_NOTE(N_E, 3, N_LEN/8), M_NOTE(N_F, 3, N_LEN/8), M_NOTE(N_G, 3, N_LEN/8), M_NOTE(N_A, 3, N_LEN/8), M_NOTE(N_B, 3, N_LEN/8),
		M_NOTE(N_C, 4, N_LEN/8), M_NOTE(N_D, 4, N_LEN/8), M_NOTE(N_E, 4, N_LEN/8), M_NOTE(N_F, 4, N_LEN/8), M_NOTE(N_G, 4, N_LEN/8), M_NOTE(N_A, 4, N_LEN/8), M_NOTE(N_B, 4, N_LEN/8),
		
		M_CTRL(C_DIVST, 1),			//減衰開始待ち（1 * 0.016384 = 約0.016秒）
		M_CTRL(C_DIVWT, 6),			//減衰間隔（6 * 0.016384 = 0.1秒、ボリューム 16 から 0 になるまで 約 1.5 秒）
		M_CTRL(C_TYPE, 2),			//矩形波
		M_NOTE(N_C, 0, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_E, 0, N_LEN/8), M_NOTE(N_F, 0, N_LEN/8), M_NOTE(N_G, 0, N_LEN/8), M_NOTE(N_A, 0, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_F, 2, N_LEN/8), M_NOTE(N_G, 2, N_LEN/8), M_NOTE(N_A, 2, N_LEN/8), M_NOTE(N_B, 2, N_LEN/8),
		M_NOTE(N_C, 3, N_LEN/8), M_NOTE(N_D, 3, N_LEN/8), M_NOTE(N_E, 3, N_LEN/8), M_NOTE(N_F, 3, N_LEN/8), M_NOTE(N_G, 3, N_LEN/8), M_NOTE(N_A, 3, N_LEN/8), M_NOTE(N_B, 3, N_LEN/8),
		M_NOTE(N_C, 4, N_LEN/8), M_NOTE(N_D, 4, N_LEN/8), M_NOTE(N_E, 4, N_LEN/8), M_NOTE(N_F, 4, N_LEN/8), M_NOTE(N_G, 4, N_LEN/8), M_NOTE(N_A, 4, N_LEN/8), M_NOTE(N_B, 4, N_LEN/8),
		
		M_CTRL(C_DIVST, 1),			//減衰開始待ち（1 * 0.016384 = 約0.016秒）
		M_CTRL(C_DIVWT, 6),			//減衰間隔（6 * 0.016384 = 0.1秒、ボリューム 16 から 0 になるまで 約 1.5 秒）
		M_CTRL(C_TYPE, 3),			//鋸波
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
