/*
 * USI_TWI_Master.h
 *
 * 概要：
 *  USI_TWI_Master のための定義
 *
 * 使用方法等：
 *  USI_TWI_Master を使用するモジュールから include
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *  2017/04/01 機能変更(Hiro OTSUKA) EEPROM Array の実装に対応
 *
 */

#ifndef USI_TWI_Master_h
#define USI_TWI_Master_h

//********** 読み込み **********//
#include "USI_TWI_Defs.h"
#include <inttypes.h>

//********** 定数定義 **********//
#define USI_TWI_MASTER_BUF_SIZE    33     // bytes in message buffer

//エラーコード
#define USI_TWI_NO_DATA             0x08  // Transmission buffer is empty
#define USI_TWI_DATA_OUT_OF_BOUND   0x09  // Transmission buffer is outside SRAM space
#define USI_TWI_UE_START_CON        0x07  // Unexpected Start Condition
#define USI_TWI_UE_STOP_CON         0x06  // Unexpected Stop Condition
#define USI_TWI_UE_DATA_COL         0x05  // Unexpected Data Collision (arbitration)
#define USI_TWI_NO_ACK_ON_DATA      0x02  // The slave did not acknowledge  all data
#define USI_TWI_NO_ACK_ON_ADDRESS   0x01  // The slave did not acknowledge  the address
#define USI_TWI_MISSING_START_CON   0x03  // Generated Start Condition not detected on bus
#define USI_TWI_MISSING_STOP_CON    0x04  // Generated Stop Condition not detected on bus
#define USI_TWI_BAD_MEM_READ	    0x0A  // Error during external memory read

//********** 関数定義 **********//
//I2C の初期化（一回だけでOK）
//	引数：CPU周波数(KHz), I2C周波数(KHz)
void	USI_TWI_Master_Initialize(uint16_t , uint16_t );

//I2C の再起動（Initialize 後の再初期化に使用）
void	USI_TWI_Master_Restart();

//I2C での送信開始を宣言（この時点では何も処理しない）
//	引数：I2Cスレーブアドレス
void	USI_TWI_Master_tranBegin( uint8_t );

//I2C での1バイト送信を予定（この時点では何も処理しない）
//	引数：送信データ
void	USI_TWI_Master_tranSend( uint8_t );

//I2C での送信を実行
//	戻値：0=成功,0以外=エラーコード
uint8_t	USI_TWI_Master_tranCommit( );

//I2C での送信を実行（ただしストップ処理を行わない）
//	戻値：0=成功,0以外=エラーコード
uint8_t	USI_TWI_Master_tranCommitNoStop( );

//I2C からの受信を実行
//	引数：I2Cスレーブアドレス, 受信長
//	戻値：0=成功,0以外=エラーコード
uint8_t	USI_TWI_Master_receiveFrom( uint8_t , uint8_t );

//I2C から受信した情報を1バイト読み込む
//	戻値：受信情報1バイト分
uint8_t	USI_TWI_Master_receive();

//I2C から受信した情報が残っているかどうか確認
//	戻値：受信可能バイト数
uint8_t	USI_TWI_Master_receiveAvailable();

#endif

