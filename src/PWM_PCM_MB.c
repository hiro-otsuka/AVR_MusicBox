/*
 * PWM_PCM_MB.c
 *
 * 概要：
 *  減衰サイン波によるオルゴール演奏を行うモジュール
 *
 * 使用方法等：
 *  PWM_PCM_MB を使用するプロジェクトにインポートして使用
 *  PwmSoundSin_Buff を書き換えることで、三角波や矩形波に変更することが可能⇒2017/02/12に選択機能追加
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *  2017/02/17 構成変更(Hiro OTSUKA) EEPROMからのMML再生およびWAVとの自動判別に対応
 *                                  音色の選択機能を追加
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "USI_TWI_Master.h"
#include "PWM_PCM_MB.h"

//********** 関数定義 **********//
void PwmSoundSin_Init();
void PwmSoundSin_StartWave(uint8_t , uint16_t , uint16_t , uint16_t , uint8_t, uint8_t );
void MusicScore_Init();
void MusicScore_PlayMain();
void MusicScore_SetLed(uint8_t, uint8_t );
void MusicScore_SetMML(uint8_t, uint16_t );

//********** 構造定義 **********//
//チャンネル管理用情報
typedef volatile struct tagPwmSoundChannel {
	volatile uint8_t  PhaseMax;
	volatile uint8_t  PhaseNow;
	
	volatile uint16_t IndexStep;
	volatile uint16_t IndexNow;

	volatile uint8_t VolMst;
	volatile uint8_t Type;
} strPwmSoundChannel;

//楽譜管理用情報
typedef volatile struct tagMusicScoreChannel {
	volatile uint16_t			MMLBuff[PWM_MUSICBOX_BUFF];
	volatile uint16_t			MMLBuff_head;
	volatile uint16_t			MMLBuff_tail;
	volatile uint8_t			MMLBuff_tail_step;
	
	const volatile uint16_t*	ScoreBuff;
	volatile uint8_t			ScoreI2C;
	volatile uint16_t			ScoreAddr;
	volatile uint16_t			ScoreMax;
	volatile uint16_t			ScoreIndex;
	
	volatile uint8_t			VolMst;			//現在のマスタボリューム
	volatile uint8_t			NoteOff;		//音符のオフセット
	volatile uint8_t			Type;			//音色番号
	
	volatile uint8_t			DivStart;
	volatile uint8_t			DivStartNow;
	
	volatile uint8_t			DivWait;
	volatile uint8_t			DivWaitNow;

} strMusicScoreChannel;

//音楽管理用情報（関数へのポインタを保存）
typedef volatile struct tagMusicScore {
	void (*Init)();
} strMusicScore;

//*********** マクロ ***********//
#define FREQ_2_PM(f) MUSIC_PLAY_FREQ/f
#define FREQ_2_IS(f) (uint16_t)((uint32_t)f*256*PWM_SOUND_SIN_NUMS/MUSIC_PLAY_FREQ)

//********** 定数定義 **********//
#define PWM_SOUND_SIN_DATA	64						//サイン波の1/4データ数（0 ⇒ 32000 までの波形）
#define PWM_SOUND_SIN_NUMS	PWM_SOUND_SIN_DATA*4	//サイン波の全データ数（0 ⇒ 32000 ⇒ 0 ⇒ -32000 ⇒ 0 まで）

#define MUSIC_NOTE_MAX	12*5+1						//音符数（キー範囲）

//音符の周波数情報（Hz）
#define HZ_0_C	65
#define HZ_0_Cs	69
#define HZ_0_D	73
#define HZ_0_Ds	78
#define HZ_0_E	82
#define HZ_0_F	87
#define HZ_0_Fs	92
#define HZ_0_G	98
#define HZ_0_Gs	104
#define HZ_0_A	110
#define HZ_0_As	116
#define HZ_0_B	123
#define HZ_1_C	131
#define HZ_1_Cs	139
#define HZ_1_D	147
#define HZ_1_Ds	157
#define HZ_1_E	165
#define HZ_1_F	175
#define HZ_1_Fs	185
#define HZ_1_G	196
#define HZ_1_Gs	208
#define HZ_1_A	220
#define HZ_1_As	233
#define HZ_1_B	247
#define HZ_2_C	262
#define HZ_2_Cs	277
#define HZ_2_D	294
#define HZ_2_Ds	311
#define HZ_2_E	330
#define HZ_2_F	349
#define HZ_2_Fs	370
#define HZ_2_G	392
#define HZ_2_Gs	415
#define HZ_2_A	440
#define HZ_2_As	466
#define HZ_2_B	494
#define HZ_3_C	523
#define HZ_3_Cs	554
#define HZ_3_D	587
#define HZ_3_Ds	622
#define HZ_3_E	659
#define HZ_3_F	698
#define HZ_3_Fs	740
#define HZ_3_G	784
#define HZ_3_Gs	831
#define HZ_3_A	880
#define HZ_3_As	932
#define HZ_3_B	988
#define HZ_4_C	1047
#define HZ_4_Cs	1109
#define HZ_4_D	1175
#define HZ_4_Ds	1245
#define HZ_4_E	1319
#define HZ_4_F	1397
#define HZ_4_Fs	1480
#define HZ_4_G	1568
#define HZ_4_Gs	1661
#define HZ_4_A	1760
#define HZ_4_As	1864
#define HZ_4_B	1975

//波形の1/4データ数（0 ⇒ 32000 までの1/4波形を記述）
const uint16_t PwmSoundSin_Buff[3][PWM_SOUND_SIN_DATA+1] PROGMEM = {
	//サイン波
	{
		0,  785, 1570, 2354, 3136, 3917, 4695, 5470, 6242, 7011, 7775, 8534, 9289,10037,10780,11516,
		12245,12967,13681,14387,15084,15772,16451,17119,17778,18425,19062,19687,20300,20901,21489,22065,
		22627,23175,23710,24230,24736,25227,25702,26162,26607,27035,27447,27842,28221,28583,28927,29254,
		29564,29855,30129,30384,30622,30840,31041,31222,31385,31528,31653,31759,31845,31913,31961,31990,
		32000
	},
	//三角波
	{
		0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500,
		8000, 8500, 9000, 9500,10000,10500,11000,11500,12000,12500,13000,13500,14000,14500,15000,15500,
		16000,16500,17000,17500,18000,18500,19000,19500,20000,20500,21000,21500,22000,22500,23000,23500,
		24000,24500,25000,25500,26000,26500,27000,27500,28000,28500,29000,29500,30000,30500,31000,31500,
		32000
	},
	//矩形波
	{
		0, 5000,10000,15000,20000,25000,30000,31000,32000,32000,32000,32000,32000,32000,32000,32000,
		32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,
		32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,
		32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,32000,
		32000
	}
};

const static uint16_t MusicNote_Buff_Freq[MUSIC_NOTE_MAX] PROGMEM = {
//    C    C#   D    D#   E    F    F#   G    G#   A    A#   B
	0,
	HZ_0_C, HZ_0_Cs, HZ_0_D, HZ_0_Ds, HZ_0_E, HZ_0_F, HZ_0_Fs, HZ_0_G, HZ_0_Gs, HZ_0_A, HZ_0_As, HZ_0_B,
	HZ_1_C, HZ_1_Cs, HZ_1_D, HZ_1_Ds, HZ_1_E, HZ_1_F, HZ_1_Fs, HZ_1_G, HZ_1_Gs, HZ_1_A, HZ_1_As, HZ_1_B,
	HZ_2_C, HZ_2_Cs, HZ_2_D, HZ_2_Ds, HZ_2_E, HZ_2_F, HZ_2_Fs, HZ_2_G, HZ_2_Gs, HZ_2_A, HZ_2_As, HZ_2_B,
	HZ_3_C, HZ_3_Cs, HZ_3_D, HZ_3_Ds, HZ_3_E, HZ_3_F, HZ_3_Fs, HZ_3_G, HZ_3_Gs, HZ_3_A, HZ_3_As, HZ_3_B,
	HZ_4_C, HZ_4_Cs, HZ_4_D, HZ_4_Ds, HZ_4_E, HZ_4_F, HZ_4_Fs, HZ_4_G, HZ_4_Gs, HZ_4_A, HZ_4_As, HZ_4_B,
};
const static uint16_t MusicNote_Buff_PhaseMax[MUSIC_NOTE_MAX] PROGMEM = {
	0,
	FREQ_2_PM(HZ_0_C), FREQ_2_PM(HZ_0_Cs), FREQ_2_PM(HZ_0_D), FREQ_2_PM(HZ_0_Ds), FREQ_2_PM(HZ_0_E), FREQ_2_PM(HZ_0_F), FREQ_2_PM(HZ_0_Fs), FREQ_2_PM(HZ_0_G), FREQ_2_PM(HZ_0_Gs), FREQ_2_PM(HZ_0_A), FREQ_2_PM(HZ_0_As), FREQ_2_PM(HZ_0_B),
	FREQ_2_PM(HZ_1_C), FREQ_2_PM(HZ_1_Cs), FREQ_2_PM(HZ_1_D), FREQ_2_PM(HZ_1_Ds), FREQ_2_PM(HZ_1_E), FREQ_2_PM(HZ_1_F), FREQ_2_PM(HZ_1_Fs), FREQ_2_PM(HZ_1_G), FREQ_2_PM(HZ_1_Gs), FREQ_2_PM(HZ_1_A), FREQ_2_PM(HZ_1_As), FREQ_2_PM(HZ_1_B),
	FREQ_2_PM(HZ_2_C), FREQ_2_PM(HZ_2_Cs), FREQ_2_PM(HZ_2_D), FREQ_2_PM(HZ_2_Ds), FREQ_2_PM(HZ_2_E), FREQ_2_PM(HZ_2_F), FREQ_2_PM(HZ_2_Fs), FREQ_2_PM(HZ_2_G), FREQ_2_PM(HZ_2_Gs), FREQ_2_PM(HZ_2_A), FREQ_2_PM(HZ_2_As), FREQ_2_PM(HZ_2_B),
	FREQ_2_PM(HZ_3_C), FREQ_2_PM(HZ_3_Cs), FREQ_2_PM(HZ_3_D), FREQ_2_PM(HZ_3_Ds), FREQ_2_PM(HZ_3_E), FREQ_2_PM(HZ_3_F), FREQ_2_PM(HZ_3_Fs), FREQ_2_PM(HZ_3_G), FREQ_2_PM(HZ_3_Gs), FREQ_2_PM(HZ_3_A), FREQ_2_PM(HZ_3_As), FREQ_2_PM(HZ_3_B),
	FREQ_2_PM(HZ_4_C), FREQ_2_PM(HZ_4_Cs), FREQ_2_PM(HZ_4_D), FREQ_2_PM(HZ_4_Ds), FREQ_2_PM(HZ_4_E), FREQ_2_PM(HZ_4_F), FREQ_2_PM(HZ_4_Fs), FREQ_2_PM(HZ_4_G), FREQ_2_PM(HZ_4_Gs), FREQ_2_PM(HZ_4_A), FREQ_2_PM(HZ_4_As), FREQ_2_PM(HZ_4_B),
};
const static uint16_t MusicNote_Buff_IndexStep[MUSIC_NOTE_MAX] PROGMEM = {
	0,
	FREQ_2_IS(HZ_0_C), FREQ_2_IS(HZ_0_Cs), FREQ_2_IS(HZ_0_D), FREQ_2_IS(HZ_0_Ds), FREQ_2_IS(HZ_0_E), FREQ_2_IS(HZ_0_F), FREQ_2_IS(HZ_0_Fs), FREQ_2_IS(HZ_0_G), FREQ_2_IS(HZ_0_Gs), FREQ_2_IS(HZ_0_A), FREQ_2_IS(HZ_0_As), FREQ_2_IS(HZ_0_B),
	FREQ_2_IS(HZ_1_C), FREQ_2_IS(HZ_1_Cs), FREQ_2_IS(HZ_1_D), FREQ_2_IS(HZ_1_Ds), FREQ_2_IS(HZ_1_E), FREQ_2_IS(HZ_1_F), FREQ_2_IS(HZ_1_Fs), FREQ_2_IS(HZ_1_G), FREQ_2_IS(HZ_1_Gs), FREQ_2_IS(HZ_1_A), FREQ_2_IS(HZ_1_As), FREQ_2_IS(HZ_1_B),
	FREQ_2_IS(HZ_2_C), FREQ_2_IS(HZ_2_Cs), FREQ_2_IS(HZ_2_D), FREQ_2_IS(HZ_2_Ds), FREQ_2_IS(HZ_2_E), FREQ_2_IS(HZ_2_F), FREQ_2_IS(HZ_2_Fs), FREQ_2_IS(HZ_2_G), FREQ_2_IS(HZ_2_Gs), FREQ_2_IS(HZ_2_A), FREQ_2_IS(HZ_2_As), FREQ_2_IS(HZ_2_B),
	FREQ_2_IS(HZ_3_C), FREQ_2_IS(HZ_3_Cs), FREQ_2_IS(HZ_3_D), FREQ_2_IS(HZ_3_Ds), FREQ_2_IS(HZ_3_E), FREQ_2_IS(HZ_3_F), FREQ_2_IS(HZ_3_Fs), FREQ_2_IS(HZ_3_G), FREQ_2_IS(HZ_3_Gs), FREQ_2_IS(HZ_3_A), FREQ_2_IS(HZ_3_As), FREQ_2_IS(HZ_3_B),
	FREQ_2_IS(HZ_4_C), FREQ_2_IS(HZ_4_Cs), FREQ_2_IS(HZ_4_D), FREQ_2_IS(HZ_4_Ds), FREQ_2_IS(HZ_4_E), FREQ_2_IS(HZ_4_F), FREQ_2_IS(HZ_4_Fs), FREQ_2_IS(HZ_4_G), FREQ_2_IS(HZ_4_Gs), FREQ_2_IS(HZ_4_A), FREQ_2_IS(HZ_4_As), FREQ_2_IS(HZ_4_B),
};

//********** 変数定義 **********//
static strPwmSoundChannel PwmSoundSin_Channel[MUSIC_SCORE_CHANNELS];
static strMusicScoreChannel MusicScore_Channel[MUSIC_SCORE_CHANNELS];
static strMusicScore MusicScore[MUSIC_SCORES];

//********** 外部関数 **********//
//=============================
//オルゴールモジュールの初期化（最初に一度だけ呼び出しでOK）
void PWM_PCM_MB_Init()
{
	PwmSound_Init();
	PwmSoundSin_Init();
	
	for (int MusicNum = 0; MusicNum < MUSIC_SCORES; MusicNum++) {
		MusicScore[MusicNum].Init = 0;
	}

	TCCR0A = TIM0_TCCR0A;
	TCCR0B = (1 << CS02) | (0 << CS01) | (1 << CS00);
	OCR0A = 255;
	TIM0_COUNT = 0;
}

//=============================
//楽譜情報設定
//	引数：楽譜番号, 初期化用コールバック関数
void PWM_PCM_MB_MEM_SetScore(uint8_t MusicNum, void (*func)())
{
	MusicScore[MusicNum].Init = func;
}

//=============================
//チャンネル情報設定
//	引数：チャンネル番号, 楽譜情報, 楽譜数(!=バイト数)
void PWM_PCM_MB_MEM_SetScoreChannel(uint8_t channel, const uint16_t* ScoreBuff, uint16_t ScoreMax)
{
	MusicScore_Channel[channel].ScoreBuff = ScoreBuff;
	MusicScore_Channel[channel].ScoreMax = ScoreMax;
}

//=============================
//オルゴールモジュールの再生開始（C_ENDまで再生すると終了） メモリ版
//	引数：楽譜番号
void PWM_PCM_MB_MEM_Play(uint8_t MusicNum)
{
	// メモリ上のスコアを初期化
	MusicScore_Init();
	
	// メモリアドレスをセット
	MusicScore[MusicNum].Init();
	
	// メインの再生モジュールをコール
	MusicScore_PlayMain();
}

//=============================
//オルゴールモジュールの再生開始（C_ENDまで再生すると終了） EEOROM版
//	引数：I2Cスレーブアドレス, 開始アドレス
void PWM_PCM_MB_EEPROM_Play(uint8_t slaveaddr , uint16_t StartAddr)
{
	// メモリ上のスコアを初期化
	MusicScore_Init();

	// EEPROMを読み込んで必要情報をセット
	do {
		USI_TWI_Master_tranBegin(slaveaddr);
		USI_TWI_Master_tranSend((uint8_t)(StartAddr >> 8));
		USI_TWI_Master_tranSend((uint8_t)(StartAddr & 0xFF));
	} while(USI_TWI_Master_tranCommit());
	
	USI_TWI_Master_receiveFrom(slaveaddr, 1);	// チャンネル数を読み込み
	StartAddr ++;
	
	uint8_t channel_num = USI_TWI_Master_receive();
	for (uint8_t channel = 0; channel < channel_num; channel ++) {
		// チャンネルの情報開始位置に移動
		do {
			USI_TWI_Master_tranBegin(slaveaddr);
			USI_TWI_Master_tranSend((uint8_t)(StartAddr >> 8));
			USI_TWI_Master_tranSend((uint8_t)(StartAddr & 0xFF));
		} while(USI_TWI_Master_tranCommit());
		
		USI_TWI_Master_receiveFrom(slaveaddr, 2);	// チャンネルの長さ（Byte数）
		StartAddr += 2;
		uint16_t scoreNums = USI_TWI_Master_receive() + (USI_TWI_Master_receive() << 8);
		MusicScore_Channel[channel].ScoreI2C = slaveaddr;
		MusicScore_Channel[channel].ScoreAddr = StartAddr;
		MusicScore_Channel[channel].ScoreMax = scoreNums/2;
		StartAddr += scoreNums;
	}
	// メインの再生モジュールをコール
	MusicScore_PlayMain();
}

//********** 内部関数 **********//
//-----------------------------
//全チャンネルのPWMサイン波を初期化する
void PwmSoundSin_Init()
{
	for (uint8_t i = 0; i < MUSIC_SCORE_CHANNELS; i++) {
		PwmSoundSin_StartWave(i, 0, 0, 0, 0, 0);
	}
}

//-----------------------------
//指定したチャンネルのPWMサイン波を開始する
//	引数：チャンネル番号, 周波数有無（0なら停止）, 使用する波形情報数, 波形情報の加算数（256倍）, ボリューム, 音色番号
void PwmSoundSin_StartWave(uint8_t channel, uint16_t InHz, uint16_t PhaseMax, uint16_t IndexStep, uint8_t Vol, uint8_t Type)
{
	if (InHz != 0) {
		PwmSoundSin_Channel[channel].PhaseMax = PhaseMax;
		PwmSoundSin_Channel[channel].IndexStep = IndexStep;
		PwmSoundSin_Channel[channel].VolMst = Vol;
	} else {
		PwmSoundSin_Channel[channel].PhaseMax = 0;
		PwmSoundSin_Channel[channel].IndexStep = 0;
		PwmSoundSin_Channel[channel].VolMst = 0;
	}

	PwmSoundSin_Channel[channel].PhaseNow = 0;
	PwmSoundSin_Channel[channel].IndexNow = 0;
	PwmSoundSin_Channel[channel].Type = Type;
}

//-----------------------------
//楽譜の初期化
void MusicScore_Init()
{
	for (int channel = 0; channel < MUSIC_SCORE_CHANNELS; channel ++) {
		MusicScore_Channel[channel].ScoreBuff = 0;
		MusicScore_Channel[channel].ScoreMax = 0;
		MusicScore_Channel[channel].ScoreI2C = 0;
		MusicScore_Channel[channel].ScoreAddr = 0;
		MusicScore_Channel[channel].ScoreIndex = 0;
		MusicScore_Channel[channel].VolMst = 16;
		MusicScore_Channel[channel].DivStart = 255;
		MusicScore_Channel[channel].DivWait = 255;
		MusicScore_Channel[channel].NoteOff = 0;
		MusicScore_Channel[channel].Type = 0;
		MusicScore_Channel[channel].MMLBuff_head = 0;
		MusicScore_Channel[channel].MMLBuff_tail = 0;
		MusicScore_Channel[channel].MMLBuff_tail_step = 0;
	}
}

//-----------------------------
//楽譜の再生メイン
void MusicScore_PlayMain()
{
	uint8_t nowread = 0;
	
	// 可能な限りメモリ上に読み込んでおく
	for (uint8_t channel = 0; channel < MUSIC_SCORE_CHANNELS; channel ++) {
		MusicScore_SetMML(channel, PWM_MUSICBOX_BUFF);
	}
	
	// PWMモジュールを初期化
	PwmSoundSin_Init();
	PWM_PCM_Playing = 1;
	
	//楽譜再生の開始
	PwmSound_Start(MUSIC_PLAY_FREQ);
	TIMSK |= (1 << OCIE0A);
	
	//再生ループ
	while(PWM_PCM_Playing) {
		uint8_t PwmSoundHeadTmp = (PwmSoundHead + 1) & (PWM_SOUND_MAX_BUFF - 1);
		if (PwmSoundHeadTmp != PwmSoundTail) {
			uint16_t ValNext = 0;
			for (uint8_t channel = 0; channel < MUSIC_SCORE_CHANNELS; channel ++) {
				if (PwmSoundSin_Channel[channel].IndexStep != 0) {
					uint8_t IndexTmp = (uint8_t)(PwmSoundSin_Channel[channel].IndexNow >> 8);
					uint16_t ValTmp;
					if(IndexTmp <= PWM_SOUND_SIN_DATA) {
						ValTmp = pgm_read_word(&PwmSoundSin_Buff[PwmSoundSin_Channel[channel].Type][IndexTmp]);
					} else if (IndexTmp <= PWM_SOUND_SIN_DATA * 2) {
						ValTmp = pgm_read_word(&PwmSoundSin_Buff[PwmSoundSin_Channel[channel].Type][PWM_SOUND_SIN_DATA*2 - IndexTmp]);
					} else if (IndexTmp <= PWM_SOUND_SIN_DATA * 3) {
						ValTmp = pgm_read_word(&PwmSoundSin_Buff[PwmSoundSin_Channel[channel].Type][IndexTmp - PWM_SOUND_SIN_DATA*2]);
					} else {
						ValTmp = pgm_read_word(&PwmSoundSin_Buff[PwmSoundSin_Channel[channel].Type][(PWM_SOUND_SIN_DATA*4-1) - IndexTmp + 1]);
					}
					uint8_t Vols = PwmSoundSin_Channel[channel].VolMst;
					if (Vols == 16) {
						//
					} else if (Vols == 0) {
						ValTmp = 0;
					} else {
						ValTmp =(Vols & 0b01000 ? ValTmp >> 1 : 0) +
						(Vols & 0b00100 ? ValTmp >> 2 : 0) +
						(Vols & 0b00010 ? ValTmp >> 3 : 0) +
						(Vols & 0b00001 ? ValTmp >> 4 : 0);
					}
					if (IndexTmp <= PWM_SOUND_SIN_DATA * 2) ValTmp += 32768;
					else ValTmp = 32768 - ValTmp;
#if MUSIC_SCORE_CHANNELS == 1
#elif MUSIC_SCORE_CHANNELS == 2
					ValTmp >>= 1;
#elif MUSIC_SCORE_CHANNELS == 3
					ValTmp >>= 2;
#elif MUSIC_SCORE_CHANNELS == 4
					ValTmp >>= 2;
#endif
					ValNext += ValTmp;
					PwmSoundSin_Channel[channel].PhaseNow ++;
					if (PwmSoundSin_Channel[channel].PhaseNow >= PwmSoundSin_Channel[channel].PhaseMax) {
						PwmSoundSin_Channel[channel].PhaseNow = 0;
						PwmSoundSin_Channel[channel].IndexNow = 0;
					} else {
						PwmSoundSin_Channel[channel].IndexNow += PwmSoundSin_Channel[channel].IndexStep;
					}
				}
			}
			PwmSoundBuff[PwmSoundHeadTmp] = (uint8_t)(ValNext >> 8);
			PwmSoundHead = PwmSoundHeadTmp;
		} else {
			//処理の余裕ができたらメモリ上に先読みしておく（各チャンネルを順に）
			MusicScore_SetMML(nowread++, MML_PREREAD_NUM);
			if (nowread >= MUSIC_SCORE_CHANNELS) nowread = 0;
#ifdef _PWM_DEBUG
			PORTB &= ~(1 << PWM_DEBUG_OUT);
#endif
		}
	}

	//楽譜の再生終了
	TIMSK &= ~(1 << OCIE0A);
	PwmSound_Stop();
}

//-----------------------------
//楽譜からのLED制御
//	引数：ON/OFF（0=OFF/1=ON）, ピン番号
void MusicScore_SetLed(uint8_t isOn, uint8_t pinnum)
{
#if defined(PIN_LED0_DDR)
	if (pinnum == 0) {
		if (isOn) PIN_LED0_ON;
		else PIN_LED0_OFF;
	}
#endif
#if defined(PIN_LED1_DDR)
	if (pinnum == 1) {
		if (isOn) PIN_LED1_ON;
		else PIN_LED1_OFF;
	}
#endif
#if defined(PIN_LED2_DDR)
	if (pinnum == 2) {
		if (isOn) PIN_LED2_ON;
		else PIN_LED2_OFF;
	}
#endif
#if defined(PIN_LED3_DDR)
	if (pinnum == 3) {
		if (isOn) PIN_LED3_ON;
		else PIN_LED3_OFF;
	}
#endif
#if defined(PIN_LED4_DDR)
	if (pinnum == 4) {
		if (isOn) PIN_LED4_ON;
		else PIN_LED4_OFF;
	}
#endif
#if defined(PIN_LED5_DDR)
	if (pinnum == 5) {
		if (isOn) PIN_LED5_ON;
		else PIN_LED5_OFF;
	}
#endif
#if defined(PIN_LED6_DDR)
	if (pinnum == 6) {
		if (isOn) PIN_LED6_ON;
		else PIN_LED6_OFF;
	}
#endif
#if defined(PIN_LED7_DDR)
	if (pinnum == 7) {
		if (isOn) PIN_LED7_ON;
		else PIN_LED7_OFF;
	}
#endif
}

//-----------------------------
//MMLバッファへの楽譜情報読み込み
//	引数：チャンネル番号, 読み込みサイズ
void MusicScore_SetMML(uint8_t channel, uint16_t readNum)
{
	// PRMMEMからの再生
	if (MusicScore_Channel[channel].ScoreBuff != 0) {
		while(readNum > 0 && MusicScore_Channel[channel].MMLBuff_tail != ((MusicScore_Channel[channel].MMLBuff_head+1) & (PWM_MUSICBOX_BUFF-1))) {
			readNum --;
			MusicScore_Channel[channel].MMLBuff[MusicScore_Channel[channel].MMLBuff_head++] = pgm_read_word(&MusicScore_Channel[channel].ScoreBuff[MusicScore_Channel[channel].ScoreIndex++]);
			if (MusicScore_Channel[channel].ScoreIndex >= MusicScore_Channel[channel].ScoreMax) MusicScore_Channel[channel].ScoreIndex = 0;
			MusicScore_Channel[channel].MMLBuff_head &= (PWM_MUSICBOX_BUFF-1);
		}
	// EEPROM空の再生
	} else if (MusicScore_Channel[channel].ScoreAddr != 0) {
		// 読み込み先アドレスに移動
		uint8_t slaveaddr = MusicScore_Channel[channel].ScoreI2C;
		uint16_t StartAddr = MusicScore_Channel[channel].ScoreAddr + MusicScore_Channel[channel].ScoreIndex * 2;
		if (readNum > MusicScore_Channel[channel].ScoreMax - MusicScore_Channel[channel].ScoreIndex) readNum = MusicScore_Channel[channel].ScoreMax - MusicScore_Channel[channel].ScoreIndex;
		do {
			USI_TWI_Master_tranBegin(slaveaddr);
			USI_TWI_Master_tranSend((uint8_t)(StartAddr >> 8));
			USI_TWI_Master_tranSend((uint8_t)(StartAddr & 0xFF));
		} while(USI_TWI_Master_tranCommit());
		USI_TWI_Master_receiveFrom(slaveaddr, readNum * 2);
		while(readNum > 0 && MusicScore_Channel[channel].MMLBuff_tail != ((MusicScore_Channel[channel].MMLBuff_head+1) & (PWM_MUSICBOX_BUFF-1))) {
			readNum --;
			MusicScore_Channel[channel].MMLBuff[MusicScore_Channel[channel].MMLBuff_head++] = USI_TWI_Master_receive() | (USI_TWI_Master_receive() << 8);
			MusicScore_Channel[channel].ScoreIndex ++;
			if (MusicScore_Channel[channel].ScoreIndex >= MusicScore_Channel[channel].ScoreMax) MusicScore_Channel[channel].ScoreIndex = 0;
			MusicScore_Channel[channel].MMLBuff_head &= (PWM_MUSICBOX_BUFF-1);
		}
	}
}
//********** 割込関数 **********//
//+++++++++++++++++++++++++++++
//タイマ0割り込みによる楽譜進行
ISR(TIMER0_COMPA_vect)
{
	for (uint8_t i = 0; i < MUSIC_SCORE_CHANNELS; i++ ) {
		if (MusicScore_Channel[i].MMLBuff_tail_step == 0 && MusicScore_Channel[i].ScoreMax != 0) {
			while(MusicScore_Channel[i].MMLBuff_tail_step == 0) {
				uint16_t ScoreCmd = MusicScore_Channel[i].MMLBuff[MusicScore_Channel[i].MMLBuff_tail];
				uint8_t ScoreNote = ScoreCmd >> 8;
				if (ScoreNote & C_CMD) {
					switch(ScoreNote & ~C_CMD){
						case C_VOL:
							MusicScore_Channel[i].VolMst = ScoreCmd &0xFF;
							break;
						case C_DIVST:
							MusicScore_Channel[i].DivStart = ScoreCmd &0xFF;
							MusicScore_Channel[i].DivStartNow = 0;
							break;
						case C_DIVWT:
							MusicScore_Channel[i].DivWait = ScoreCmd &0xFF;
							MusicScore_Channel[i].DivWaitNow = 0;
							break;
						case C_OFF:
							MusicScore_Channel[i].NoteOff = ScoreCmd &0xFF;
							break;
						case C_L_ON:
							MusicScore_SetLed(1, ScoreCmd & 0xFF);
							break;
						case C_L_OFF:
							MusicScore_SetLed(0, ScoreCmd & 0xFF);
							break;
						case C_TYPE:
							MusicScore_Channel[i].Type = ScoreCmd &0xFF;
							break;
						case C_END:
							PWM_PCM_Playing = 0;
							break;
					}
				} else {
					if (ScoreNote != 0) ScoreNote += MusicScore_Channel[i].NoteOff;
					MusicScore_Channel[i].MMLBuff_tail_step = (ScoreCmd & 0xFF)-1;
					PwmSoundSin_StartWave(i, pgm_read_word(&MusicNote_Buff_Freq[ScoreNote]),
											 pgm_read_word(&MusicNote_Buff_PhaseMax[ScoreNote]),
											 pgm_read_word(&MusicNote_Buff_IndexStep[ScoreNote]),
											 MusicScore_Channel[i].VolMst, MusicScore_Channel[i].Type);
					MusicScore_Channel[i].DivWaitNow = 0;
					MusicScore_Channel[i].DivStartNow = 0;
				}
				MusicScore_Channel[i].MMLBuff_tail = (MusicScore_Channel[i].MMLBuff_tail + 1) & (PWM_MUSICBOX_BUFF-1);
			}
		} else {
			MusicScore_Channel[i].MMLBuff_tail_step --;
			if (MusicScore_Channel[i].DivStartNow >= MusicScore_Channel[i].DivStart) {
				if (PwmSoundSin_Channel[i].VolMst != 0) {
					if (MusicScore_Channel[i].DivWaitNow >= MusicScore_Channel[i].DivWait) {
						MusicScore_Channel[i].DivWaitNow = 0;
						PwmSoundSin_Channel[i].VolMst --;
					} else {
						MusicScore_Channel[i].DivWaitNow ++;
					}
				}
			} else {
				MusicScore_Channel[i].DivStartNow ++;
			}
		}
	}
}

