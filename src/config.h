/*
 * config.h
 *
 * 概要：
 *  PIN_Control.h 等の規定に従って、プロジェクトのピンアサインを定義する
 *
 * 使用方法等：
 *  ソース内のコメントを参照
 *  なお、これはサンプルプロジェクトのための定義であり、使用時は要修正
 *
 * ライセンス：
 *  パブリックドメイン/Unlicense（サンプルコードのため）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *  2017/02/17 構成変更(Hiro OTSUKA) EEPROMからのMML再生およびWAVとの自動判別に対応
 *  2017/04/09 機能追加(Hiro OTSUKA) シリアル通信モード等のための設定項目を追加
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//#define F_CPU 16000000UL	//CPUスピードを定義 → プロジェクトで定義した方がよい

//********** 機能定義 **********//
//演奏中にPLAYINGピンがLowになる機能を有効化（オーディオアンプのCEピンへの接続を想定）
//	※ATTiny85の場合：ピン数が足りないためリセット無効化が必要＆BTN2/LED2と競合
#define _PLAYING_ENABLE

//PCM情報が間に合わない場合に PWM_DEBUG_OUT に 1 を出力する機能を有効化
//	※ATTiny85の場合：BTN1/LED1 と競合
//#define	_PWM_DEBUG

//フレーム間補完を有効化
//	※使用する場合：MUSIC_SCORE_CHANNELS の最大=2 が限界
//#define _HIGH_RESO

//********** 性能定義 **********//
#define MUSIC_SCORES			4		//登録する楽譜の最大数（8Kのメモリでは4曲程度が限界⇒EEPROM読込再生に対応したため影響は小）
#define	MUSIC_SCORE_CHANNELS	3		//チャネル数（CPU性能に依存. ATTiny85 16MHz で MUSIC_PLAY_FREQ=16000の場合、3が最大）
#define MUSIC_PLAY_FREQ			16000	//SINE波再生の解像度（8000にすると高音が歪む）
#define PWM_SOUND_FREQ			32000	//PCM再生解像度（8000,16000,32000が選択可能）
#define PIN_CONTROL_WAIT		50		//BTN押下時のチャタリング防止時間の長さ(ms)＝最短反応速度以外
#define PIN_CONTROL_WAIT_SERIAL	5		//シリアル通信時のチャタリング防止時間の長さ(ms)＝最短反応速度
#define PWM_PCMPLAY_PARAMS		32		//EEPROM から読み込むパラメータの最大サイズ（ <= EEPROM_ARRAY_BUFF_MAX であること）
#define MML_PREREAD_NUM			8		//EEPROM からメモリ上に一度に読み込む MML数
#define PIN_SERIAL_BTN			12		//シリアル通信時に指定されうるボタンの数

//********** ピン定義 **********//
// BTN/LED の選択
// _PINx_LED を定義= PINx を出力(LED)として使う
// _PINx_BTN を定義= PINx を入力(BTN)として使う
// _PINx_BTN_INT を定義= BTNx が押されたときに割り込み処理(sleep からの復帰含む)を行う
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
