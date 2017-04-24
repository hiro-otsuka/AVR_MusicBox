/*
 * main.h
 *
 * 概要：
 *  PWMによる音楽・音声再生テスト
 *
 * 使用方法等：
 *  BTN0 でメモリ上の音楽を再生、BTN1 でEEPROM の WAV,MML を再生
 *  なお、これはサンプルであり、使用時は要修正
 *
 * ライセンス：
 *  パブリックドメイン/Unlicense（サンプルコードのため）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *  2017/02/17 構成変更(Hiro OTSUKA) EEPROMからのMML再生およびWAVとの自動判別に対応
 *  2017/02/25 機能改善(Hiro OTSUKA) MMLとWAVを分離して再生するモードとシリアル通信指定のモードを追加
 *  2017/02/26 機能追加(Hiro OTSUKA) Init時にEEPROMからパラメータを読み込む機能を追加
 *  2017/04/01 機能変更(Hiro OTSUKA) EEPROM Array の実装に対応
 *  2017/04/09 機能変更(Hiro OTSUKA) 再生モードを EEPROM のパラメータから読み込むよう変更
 *  2017/04/10 バグ修正(Hiro OTSUKA) 再生モード SERIAL_NUM 時の問題を修正
 *  2017/04/22 構成変更(Hiro OTSUKA) ピンのPU要否と再生モードを内蔵EEPROMで指定できるよう変更
 *
 */

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include "PIN_Control.h"
#include "PWM_EEPROM_Play.h"

//EEPROM上のパラメータアドレスを定義
#define PARAM_PLAYMODE	0x00
#define PARAM_ISPULLUP	0x01

//再生モードを定義（EEPROM内のパラメータファイル 0 バイト目で指定）
#define _PLAYMODE_SPLIT			0x00	//分離再生モード(BTN0 = PCM、BTN1 = MML) ※パラメータが無い場合のデフォルト
#define _PLAYMODE_ALL			0x01	//全再生モード(BTN0 = 戻る、BTN1 = 進む)
#define _PLAYMODE_SERIAL_NUM	0x02	//シリアル指定モード(BTN0 押下中 BTN1 押下した回数で指定)
#define _PLAYMODE_SERIAL_BTN	0x03	//シリアル指定モード(BTN0 押下中 BTN1 押下した回数でボタン番号を指定)

int main(void)
{
	uint8_t MusicNum = 0;
	uint8_t VoiceNum = 0;
	uint8_t BtnNum[PIN_SERIAL_BTN];

	//内蔵EEPROMからパラメータを取得する
	eeprom_busy_wait();
	uint8_t PlayMode = eeprom_read_byte((uint8_t*)PARAM_PLAYMODE);
	eeprom_busy_wait();
	uint8_t IsNotPU = eeprom_read_byte((uint8_t*)PARAM_ISPULLUP);
	
	//マイコンを初期化する
	PIN_Control_Init(IsNotPU);
	
	//I2C を初期化する
	EEPROM_Array_Init(16000, 1000);

	//演奏モジュールを初期化する
	PWM_PCM_MB_Init();
	
	//EEPROM のファイル数を得る
	EEPROM_Init();
	
	//MMLを内蔵する場合は下記の通り実行する（プログラムメモリを使用するので要注意）
	//MMLの宣言----
	//#include "MML_Test.h"
	//MMLの登録----
	//PWM_PCM_MB_MEM_SetScore(0, (void(*)())MML_Test_Setup);
	//MMLの再生----
	//PWM_PCM_MB_MEM_Play(0);
	
	//再生前の初期設定	
	for(uint8_t i = 0; i < PIN_SERIAL_BTN; i ++) BtnNum[i] = 0;
	
	if (PlayMode == _PLAYMODE_SERIAL_NUM || PlayMode == _PLAYMODE_SERIAL_BTN)
		PIN_Control_SetWait(PIN_CONTROL_WAIT_SERIAL);
	else
		PIN_Control_SetWait(PIN_CONTROL_WAIT);
	
	sei();
	while (1) {
		//演奏してない間はスリープに入る
		if (PIN_Control_Key == 0) {
			PIN_Control_Playing(0);	//外部のオーディオアンプをOFF
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_mode();
			PIN_Control_Playing(1);	//外部のオーディオアンプをON
			_delay_ms(30);			//オーディオアンプの起動待ち
		}
		switch(PlayMode) {
			case _PLAYMODE_SERIAL_BTN:
			case _PLAYMODE_SERIAL_NUM:
				//シリアル通信で順番を指定するモード====================
				if (PIN_Control_Key == (1<<0)) {
					VoiceNum = 0;
					//完全に押されるまで待つ
					PIN_Control_WaitKeyOn(~(1 << 0));
					//BTN0 が離されるまで、BTN1 が押された回数を計測する
					while (PIN_Control_GetKey() & (1 << 0)) {
						//BTN1 が押されたら、離されるまで待つ
						if(PIN_Control_GetKey() & (1 << 1)) {
							//完全に押されるまで待つ
							PIN_Control_WaitKeyOn(~(1 << 1));
							VoiceNum ++;
							//離されるまで待つ
							while(PIN_Control_GetKey() & (1 << 1));
							//完全に離されるまで待つ
							PIN_Control_WaitKeyOff(~(1 << 1));
						}
					}
					//完全にキー押下が解除されるまで待つ
					PIN_Control_WaitKeyOff(0);
					//BTN1 が押されなかった場合は演奏停止するだけで終了
					PIN_Control_Key = 0;
					if (VoiceNum > 0) {
						VoiceNum --;
						//ボタン番号を指定するモードの場合
						if (PlayMode == _PLAYMODE_SERIAL_BTN) {
							//各ボタンに指定された範囲を演奏する
							MusicNum = BtnNum[VoiceNum];
							if (MusicNum < EEPROM_Params[VoiceNum]) MusicNum = EEPROM_Params[VoiceNum];
							BtnNum[VoiceNum] = MusicNum + 1;
							if (BtnNum[VoiceNum] >= EEPROM_Params[VoiceNum+1]) BtnNum[VoiceNum] = EEPROM_Params[VoiceNum];
						}
						while(MusicNum >= EEPROM_Files[PWM_PCMPLAY_ANY]) MusicNum -= EEPROM_Files[PWM_PCMPLAY_ANY];
						EEPROM_Play(PWM_PCMPLAY_ANY, MusicNum);
					}
				} else {
					//どれでもなければキー押下を解除
					PIN_Control_Key = 0;
				}
				break;
			case _PLAYMODE_ALL:
				//内蔵データを順に再生するモード========================
				//完全にキー押下が解除されるまで待つ
				PIN_Control_WaitKeyOff(0);
				//BTN0 が押された場合は EEPROM を逆に再生する
				if (PIN_Control_Key == (1<<0)) {
					PIN_Control_Key = 0;
					if (VoiceNum == 0) VoiceNum = EEPROM_Files[PWM_PCMPLAY_ANY] - 1;	// 0 に達したら最大数に戻す
					else VoiceNum --;
					EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum);
					//BTN1 が押された場合は EEPROM を順に再生する
				} else if(PIN_Control_Key == (1<<1)) {
					PIN_Control_Key = 0;
					VoiceNum ++;
					if (VoiceNum >= EEPROM_Files[PWM_PCMPLAY_ANY]) VoiceNum = 0;		//最大数に達したら 0 に戻す
					EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum);
				} else {
					//BTN0とBTN1 が押された場合は 停止する
					//どれでもなければキー押下を解除
					PIN_Control_Key = 0;
				}
				break;
			case _PLAYMODE_SPLIT:
			default:	//指定がない場合も同様に扱う
				//内蔵データを MMLと PCM に分けて再生するモード=========
				//完全にキー押下が解除されるまで待つ
				PIN_Control_WaitKeyOff(0);
				//BTN0 が押された場合は PCM を順に再生する
				if (PIN_Control_Key == (1<<0) && EEPROM_Files[PWM_PCMPLAY_PCM] > 0) {
					PIN_Control_Key = 0;
					EEPROM_Play(PWM_PCMPLAY_PCM, VoiceNum ++);
					if (VoiceNum >= EEPROM_Files[PWM_PCMPLAY_PCM]) VoiceNum = 0;	//最大数に達したら 0 に戻す
					//BTN1 が押された場合は MML を順に再生する
				} else if(PIN_Control_Key == (1<<1) && EEPROM_Files[PWM_PCMPLAY_MML] > 0) {
					PIN_Control_Key = 0;
					EEPROM_Play(PWM_PCMPLAY_MML, MusicNum ++);
					if (MusicNum >= EEPROM_Files[PWM_PCMPLAY_MML]) MusicNum = 0;	//最大数に達したら 0 に戻す
					//BTN0とBTN1 が押された場合はパラメータ0 の値（通常は0）番目を再生する
				} else if(PIN_Control_Key == ((1<<0)|(1<<1))) {
					PIN_Control_Key = 0;
					EEPROM_Play(PWM_PCMPLAY_ANY, EEPROM_Params[0]);
				} else {
					//どれでもなければキー押下を解除
					PIN_Control_Key = 0;
				}
				break;
		}
		//完全にキー押下が解除されるまで待つ
		PIN_Control_WaitKeyOff(0);
	}
}

