/*
 * EEPROM_Array.c
 *
 * 概要：
 *  複数の EEPROM を、まとめてひとつのメモリアレイとして扱うモジュール
 *
 * 使用方法等：
 *  USI_TWI_Master を使用するプロジェクトにインポートして使用
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/04/01 新規作成(Hiro OTSUKA)
 *
 */

#include "EEPROM_Array.h"

//********** 関数定義 **********//

//********** 定数定義 **********//

//********** 変数定義 **********//
uint8_t EEPROM_Array_Addr[EEPROM_ARRAY_ADDR_MAX];   // holds I2C addr
uint8_t EEPROM_Array_Buf[EEPROM_ARRAY_BUFF_MAX];    // holds I2C send and receive data
uint8_t EEPROM_Array_LastRead = 0;                  // number of bytes read so far
uint8_t EEPROM_Array_BytesAvail = 0;                // number of bytes requested but not read

uint8_t EEPROM_Array_AddrAvail = 0;
uint32_t EEPROM_Array_NowAddr = 0;

//********** 外部関数 **********//
//=============================
//EEPROM アドレスの検索
//	戻値：検出されたI2Cアドレスの数
//	引数：CPU周波数(KHz), I2C周波数(KHz)
uint8_t	EEPROM_Array_Init(uint16_t f_cpu, uint16_t f_i2c )
{
	// I2Cモジュールの初期化
	USI_TWI_Master_Initialize(f_cpu, f_i2c);
	
	// I2Cアドレスを検索する
	for (uint8_t slaveaddr = EEPROM_ARRAY_ADDR_START; slaveaddr < EEPROM_ARRAY_ADDR_START + EEPROM_ARRAY_ADDR_MAX; slaveaddr ++) {
		uint8_t	TimeOut = 0;
		uint8_t Result = 0;
		do {
			USI_TWI_Master_tranBegin(slaveaddr);
			USI_TWI_Master_tranSend(0);
			USI_TWI_Master_tranSend(0);
			Result = USI_TWI_Master_tranCommit();
			TimeOut ++;
		} while(Result && TimeOut < EEPROM_ARRAY_I2C_TIMEOUT);
		if (Result == 0) EEPROM_Array_Addr[EEPROM_Array_AddrAvail++] = slaveaddr;
	}
	
	return EEPROM_Array_AddrAvail;
}

//=============================
//読み込み開始アドレスのセット
//	引数：開始アドレス
//	戻値：0=成功,0以外=エラーコード
uint8_t	EEPROM_Array_SetAddr( uint32_t addr )
{
	uint8_t	TimeOut = 0;
	uint8_t Result = 0;
	
	do {
		USI_TWI_Master_tranBegin(EEPROM_Array_Addr[(uint8_t)(addr >> 16)]);
		USI_TWI_Master_tranSend((uint8_t)((addr >> 8) & 0xFF) );
		USI_TWI_Master_tranSend((uint8_t)(addr & 0xFF));
		Result = USI_TWI_Master_tranCommit();
		TimeOut ++;
	} while(Result && TimeOut < EEPROM_ARRAY_I2C_TIMEOUT);
	
	EEPROM_Array_NowAddr = addr;
	
	return Result;
}

//=============================
//読み込みの実行
//	引数：読み込みサイズ
//	戻値：読み込めたデータ数
uint8_t	EEPROM_Array_Read( uint8_t number )
{
	EEPROM_Array_LastRead = 0;
	EEPROM_Array_BytesAvail = 0;
	
	if (number > EEPROM_ARRAY_BUFF_MAX) number = EEPROM_ARRAY_BUFF_MAX;
	
	uint32_t newAddr = EEPROM_Array_NowAddr + (uint32_t) number;
	
	//I2Cアドレスをまたがらない場合は純粋に読み込む
	if ((EEPROM_Array_NowAddr & 0xFF0000) == (newAddr & 0xFF0000)) {
		if (USI_TWI_Master_receiveFrom(EEPROM_Array_Addr[(uint8_t)(EEPROM_Array_NowAddr >> 16)], number) == 0) {
			uint8_t readNum = USI_TWI_Master_receiveAvailable();
			for (uint8_t i = 0; i < readNum; i ++) EEPROM_Array_Buf[EEPROM_Array_BytesAvail++] = USI_TWI_Master_receive();
		} else {
			return 0;
		}
	// またがっていた場合は2回に分けて読み込む
	} else {
		uint8_t readNum1 = (uint8_t)((uint16_t)0x100 - (uint16_t)(EEPROM_Array_NowAddr & 0xFF));
		uint8_t readNum2 = (uint8_t)(newAddr & 0xFF);
		
		if (USI_TWI_Master_receiveFrom(EEPROM_Array_Addr[(uint8_t)(EEPROM_Array_NowAddr >> 16)], readNum1) == 0) {
			uint8_t readNum = USI_TWI_Master_receiveAvailable();
			for (uint8_t i = 0; i < readNum; i ++) EEPROM_Array_Buf[EEPROM_Array_BytesAvail++] = USI_TWI_Master_receive();
		} else {
			return 0;
		}
		if (EEPROM_Array_SetAddr(newAddr & 0xFF0000)) return 0;
		if (readNum2 != 0) {
			if (USI_TWI_Master_receiveFrom(EEPROM_Array_Addr[(uint8_t)(newAddr >> 16)], readNum2) == 0) {
				uint8_t readNum = USI_TWI_Master_receiveAvailable();
				for (uint8_t i = 0; i < readNum; i ++) EEPROM_Array_Buf[EEPROM_Array_BytesAvail++] = USI_TWI_Master_receive();
			} else {
				return 0;
			}
		}
	}
	
	EEPROM_Array_NowAddr = newAddr;
	
	return EEPROM_Array_BytesAvail;
}

//=============================
//I2C から受信した情報を1バイト読み込む
//	戻値：受信情報1バイト分
uint8_t	EEPROM_Array_data()
{
	return EEPROM_Array_Buf[EEPROM_Array_LastRead++];
}

//=============================
//I2C から受信した情報を2バイト読み込む
//	戻値：受信情報2バイト分
uint16_t EEPROM_Array_data2()
{
	return (uint16_t)EEPROM_Array_data()
			+ ((uint16_t)EEPROM_Array_data() << 8);
}

//=============================
//I2C から受信した情報を4バイト読み込む
//	戻値：受信情報4バイト分
uint32_t EEPROM_Array_data4()
{
	return (uint32_t)EEPROM_Array_data()
			+ ((uint32_t)EEPROM_Array_data() << 8)
			+ ((uint32_t)EEPROM_Array_data() << 16)
			+ ((uint32_t)EEPROM_Array_data() << 24);
}
