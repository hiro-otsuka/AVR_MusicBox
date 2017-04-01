/*
 * EEPROM_Array.h
 *
 * 概要：
 *  EEPROM_Array のための定義
 *
 * 使用方法等：
 *  EEPROM_Array を使用するモジュールから include
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/04/01 新規作成(Hiro OTSUKA)
 *
 */

#ifndef EEPROM_ARRAY_h
#define EEPROM_ARRAY_h

//********** 読み込み **********//
#include "USI_TWI_Master.h"
#include <inttypes.h>

//********** 定数定義 **********//
#define EEPROM_ARRAY_ADDR_START			0x50	// EEPROMアドレスの開始位置
#define EEPROM_ARRAY_ADDR_MAX			8		// EEPROMアドレスの最大数
#define	EEPROM_ARRAY_BUFF_MAX			32		// 読み込みバッファのサイズ
#define	EEPROM_ARRAY_I2C_TIMEOUT		5		// EEPROMアドレス検索時のタイムアウト回数

//********** 変数定義 **********//

//********** 関数定義 **********//
//EEPROM アドレスの検索
//	戻値：検出されたI2Cアドレスの数
//	引数：CPU周波数(KHz), I2C周波数(KHz)
uint8_t	EEPROM_Array_Init(uint16_t , uint16_t );

//読み込み開始アドレスのセット
//	引数：開始アドレス
//	戻値：0=成功,0以外=エラーコード
uint8_t	EEPROM_Array_SetAddr( uint32_t );

//読み込みの実行
//	引数：読み込みサイズ
//	戻値：読み込めたデータ数
uint8_t	EEPROM_Array_Read( uint8_t );

//I2C から受信した情報を1バイト読み込む
//	戻値：受信情報1バイト分
uint8_t	EEPROM_Array_data();

//I2C から受信した情報を2バイト読み込む
//	戻値：受信情報2バイト分
uint16_t EEPROM_Array_data2();

//I2C から受信した情報を4バイト読み込む
//	戻値：受信情報4バイト分
uint32_t EEPROM_Array_data4();

#endif

