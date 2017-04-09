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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include "PIN_Control_PUI.h"

int main(void)
{
	//マイコンを初期化する
	PIN_Control_PUI_Init();
	
	PIN_Control_PUI_SetWait(PIN_CONTROL_PUI_WAIT, PIN_SERIAL_WAIT_BOOT, PIN_SERIAL_WAIT_SERIAL);
	
	sei();
	while (1) {
		//演奏してない間はスリープに入る
		if (PIN_Control_PUI_Key == 0) {
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_mode();
		}
		
		//完全にキー押下が解除されるまで待つ
		PIN_Control_PUI_WaitKeyOff(0);
		
		//再生中の場合は一時停止する
		if (PIN_Control_PUI_isPlaying()) PIN_Control_PUI_Serial(0);
		//押されたボタンにより、シリアル通信を行う
		uint16_t btnTmp = PIN_Control_PUI_Key;
		uint8_t musicNum = 1;
		for (int i = 0; i < 16; i++) {
			if (btnTmp & 1) break;
			musicNum ++;
			btnTmp >>= 1;
		}
		if (btnTmp != 0) PIN_Control_PUI_Serial(musicNum);
		
		PIN_Control_PUI_Key = 0;
		//完全にキー押下が解除されるまで待つ
		PIN_Control_PUI_WaitKeyOff(0);
	}
}

