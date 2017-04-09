/*
 * config.h
 *
 * 概要：
 *  PIN_Control_PUI.h 等の規定に従って、プロジェクトのピンアサインを定義する
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
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

//********** 機能定義 **********//

//********** 性能定義 **********//
#define PIN_CONTROL_PUI_WAIT	50		//BTN押下時のチャタリング防止時間の長さ(ms)＝最短反応速度
#define PIN_SERIAL_WAIT_BOOT	30		//Serial通信時の外部モジュール起動待ち時間
#define PIN_SERIAL_WAIT_SERIAL	10		//Serial通信時の待機時間の長さ(ms)＝AVR_MBのPIN_CONTROL_WAITとあわせる

//********** ピン定義 **********//
// BTN/LED の選択
// _PINx_LED を定義= PINx を出力(LED)として使う
// _PINx_BTN を定義= PINx を入力(BTN)として使う
// _PINx_BTN_INT を定義= BTNx が押されたときに割り込み処理(sleep からの復帰含む)を行う
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
