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
 *  2017/02/25 機能改善(Hiro OTSUKA) MMLとWAVを分離して再生できるよう機能改善
 *
 */

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include "PIN_Control.h"
#include "PWM_EEPROM_Play.h"

//楽譜情報を include（長くなるため分離）
#include "MML_Test.h"

int main(void)
{
	uint8_t MusicNum = 0;
	uint8_t VoiceNum = 0;
	
	//マイコンを初期化する
	PIN_Control_PinAssign();
	PIN_Control_IntAssign();
	
	//I2C を初期化する
	USI_TWI_Master_Initialize(16000, 1000);

	//演奏モジュールを初期化する
	PWM_PCM_MB_Init();
	
	//楽譜情報を登録する
	PWM_PCM_MB_MEM_SetScore(0, (void(*)())MML_Test_Setup);
	
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
		
		//完全にキー押下が解除されるまで待つ
		PIN_Control_WaitKeyOff(0);

//
/*
		//内蔵データを順に再生するモード========================
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
//
*/
// /*
		//内蔵データを MMLと PCM に分けて再生するモード=========
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
// */

		//BTN0とBTN1 が押された場合は 内蔵楽譜を再生する
		} else if(PIN_Control_Key == ((1<<0)|(1<<1))) {
			PIN_Control_Key = 0;
			PWM_PCM_MB_MEM_Play(0);
		//どれでもなければキー押下を解除
		} else {
			PIN_Control_Key = 0;
		}
		//完全にキー押下が解除されるまで待つ
		PIN_Control_WaitKeyOff(0);
	}
}

