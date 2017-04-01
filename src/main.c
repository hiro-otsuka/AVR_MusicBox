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
 *
 */

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include "PIN_Control.h"
#include "PWM_EEPROM_Play.h"

//再生モードを設定
//#define _MODE_PLAYSERIAL	//シリアル指定モード(BTN0 押下中 BTN1 押下した回数で指定)
//#define _MODE_PLAYALL		//全再生モード(BTN0 = 戻る、BTN1 = 進む)
#define _MODE_PLAYSPLIT		//分離再生モード(BTN0 = PCM、BTN1 = MML)
//#define _MODE_DEBUG			//内蔵再生モード(BTN0 = 内蔵再生、BTN1 = EEPROMから順再生)

//テスト実行する場合は楽譜情報を include（長くなるため分離）
#ifdef _MODE_DEBUG
 #include "MML_Test.h"
#endif

int main(void)
{
	uint8_t MusicNum = 0;
	uint8_t VoiceNum = 0;
	
	//マイコンを初期化する
	PIN_Control_PinAssign();
	PIN_Control_IntAssign();
	
	//I2C を初期化する
	EEPROM_Array_Init(16000, 1000);

	//演奏モジュールを初期化する
	PWM_PCM_MB_Init();
	
	//テスト実行する場合は楽譜情報を登録する
#ifdef _MODE_DEBUG
	PWM_PCM_MB_MEM_SetScore(0, (void(*)())MML_Test_Setup);
#endif
	
	//EEPROM のファイル数を得る
	EEPROM_Init();
	
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

#if defined _MODE_PLAYSERIAL
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
				while(VoiceNum > EEPROM_Files[PWM_PCMPLAY_ANY]) VoiceNum -= EEPROM_Files[PWM_PCMPLAY_ANY];
				EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum-1);
			}
#elif defined _MODE_PLAYALL
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
			EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum ++);
			if (VoiceNum >= EEPROM_Files[PWM_PCMPLAY_ANY]) VoiceNum = 0;	//最大数に達したら 0 に戻す
		//BTN0とBTN1 が押された場合は 停止する
		} else if(PIN_Control_Key == ((1<<0)|(1<<1))) {
			PIN_Control_Key = 0;
#elif defined _MODE_PLAYSPLIT
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
#elif defined _MODE_DEBUG
		//内蔵データをデバッグ再生するモード====================
		//BTN0 が押された場合は 内蔵楽譜 を再生する
		if (PIN_Control_Key == (1<<0)) {
			PIN_Control_Key = 0;
			PWM_PCM_MB_MEM_Play(0);
		//BTN1 が押された場合は EEPROM を順に再生する
		} else if(PIN_Control_Key == (1<<1)) {
			PIN_Control_Key = 0;
			EEPROM_Play(PWM_PCMPLAY_ANY, VoiceNum ++);
			if (VoiceNum >= EEPROM_Files[PWM_PCMPLAY_ANY]) VoiceNum = 0;	//最大数に達したら 0 に戻す
		//BTN0とBTN1 が押された場合は 停止する
		} else if(PIN_Control_Key == ((1<<0)|(1<<1))) {
			PIN_Control_Key = 0;
#endif
		//どれでもなければキー押下を解除
		} else {
			PIN_Control_Key = 0;
		}
		//完全にキー押下が解除されるまで待つ
		PIN_Control_WaitKeyOff(0);
	}
}

