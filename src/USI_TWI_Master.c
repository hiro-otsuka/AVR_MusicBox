/*
 * USI_TWI_Master.c
 *
 * 概要：
 *  USI を用いた I2C マスタ接続を行うモジュール
 *
 * 使用方法等：
 *  USI_TWI_Master を使用するプロジェクトにインポートして使用
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *
 */

#include "USI_TWI_Master.h"
#include <avr/interrupt.h>

//********** 関数定義 **********//
void USI_TWI_Master_Wait( uint16_t );
uint8_t USI_TWI_Master_Transfer( );
uint8_t USI_TWI_Master_Start_Read_Write( uint8_t );
uint8_t USI_TWI_Master_Stop( void );
uint8_t USI_TWI_Master_Start( void );

//********** 変数定義 **********//
uint16_t USI_TWI_Master_Wait_H = 0;
uint16_t USI_TWI_Master_Wait_L = 0;

uint8_t USI_TWI_Master_Buf[USI_TWI_MASTER_BUF_SIZE];  // holds I2C send and receive data
uint8_t USI_TWI_Master_BufIdx = 0;                    // current number of bytes in the send buff
uint8_t USI_TWI_Master_LastRead = 0;                  // number of bytes read so far
uint8_t USI_TWI_Master_BytesAvail = 0;                // number of bytes requested but not read

//共用体でのエラー・ステータス管理
union  USI_TWI_Master_state
{
	unsigned char errorState;         // Can reuse the TWI_state for error states since it will not be needed if there is an error.
	struct {
		unsigned char addressMode         : 1;
		unsigned char masterWriteDataMode : 1;
		unsigned char memReadMode		  : 1;
		unsigned char unused              : 5;
	};
} USI_TWI_Master_state;

//********** 外部関数 **********//
//=============================
//I2C の初期化（一回だけでOK）
//	引数：CPU周波数(KHz), I2C周波数(KHz)
void USI_TWI_Master_Initialize(uint16_t f_cpu, uint16_t f_i2c )
{
	//ウェイト時間を算出
	USI_TWI_Master_Wait_H = f_cpu / f_i2c / 16;
	USI_TWI_Master_Wait_L = f_cpu / f_i2c / 16;
	
	USI_TWI_Master_Restart();
}

//=============================
//I2C の再起動（Initialize 後の再初期化に使用）
void USI_TWI_Master_Restart()
{
	SET_USI_TO_INIT();
	SET_USI_TO_MASTER();
}

//=============================
//I2C での送信開始を宣言（この時点では何も処理しない）
//	引数：I2Cスレーブアドレス
void USI_TWI_Master_tranBegin(uint8_t slaveAddr)
{
	USI_TWI_Master_BufIdx = 0; 
	USI_TWI_Master_Buf[ USI_TWI_Master_BufIdx ++] = (slaveAddr << TWI_ADR_BITS) | (0 << TWI_READ_BIT); 
}

//=============================
//I2C での1バイト送信を予定（この時点では何も処理しない）
//	引数：送信データ
void USI_TWI_Master_tranSend(uint8_t data)
{
	if (USI_TWI_Master_BufIdx >= USI_TWI_MASTER_BUF_SIZE) return;
	USI_TWI_Master_Buf[USI_TWI_Master_BufIdx ++] = data;
}

//=============================
//I2C での送信を実行
//	戻値：0=成功,0以外=エラーコード
uint8_t USI_TWI_Master_tranCommit()
{
	if (USI_TWI_Master_Start_Read_Write(0)) {
		USI_TWI_Master_BufIdx = 0;
		return 0;
	} else {
		USI_TWI_Master_BufIdx = 0;
		return USI_TWI_Master_state.errorState;
	}
}

//=============================
//I2C での送信を実行（ただしストップ処理を行わない）
//	戻値：0=成功,0以外=エラーコード
uint8_t USI_TWI_Master_tranCommitNoStop()
{
	if (USI_TWI_Master_Start_Read_Write(1)) {
		USI_TWI_Master_BufIdx = 0;
		return 0;
	} else {
		USI_TWI_Master_BufIdx = 0;
		return USI_TWI_Master_state.errorState;
	}
}

//=============================
//I2C からの受信を実行
//	引数：I2Cスレーブアドレス, 受信長
//	戻値：0=成功,0以外=エラーコード
uint8_t USI_TWI_Master_receiveFrom(uint8_t slaveAddr, uint8_t numBytes)
{
	USI_TWI_Master_LastRead = 1;
	USI_TWI_Master_BytesAvail = numBytes + 1;
	USI_TWI_Master_Buf[0] = (slaveAddr << TWI_ADR_BITS) | (1 << TWI_READ_BIT);
	USI_TWI_Master_BufIdx = numBytes + 1;
	if (USI_TWI_Master_Start_Read_Write(0)) {
		return 0;
	} else {
		return USI_TWI_Master_state.errorState;
	}
}

//=============================
//I2C から受信した情報を1バイト読み込む
//	戻値：受信情報1バイト分
uint8_t USI_TWI_Master_receive()
{
	return USI_TWI_Master_Buf[USI_TWI_Master_LastRead ++];
}

//=============================
//I2C から受信した情報が残っているかどうか確認
//	戻値：受信可能バイト数
uint8_t USI_TWI_Master_receiveAvailable()
{
	return USI_TWI_Master_BytesAvail - (USI_TWI_Master_LastRead); 
}

//********** 内部関数 **********//
//-----------------------------
//CPU速度に応じたウェイト(ただし不正確)
//	引数：ウェイトカウント(1=約4CLK？)
void USI_TWI_Master_Wait( uint16_t time )
{
	for(; time ; time --)		//LOOP_CYCL = 4
	asm volatile("nop"::);
}

//-----------------------------
//I2C による送受信処理メイン
//	引数：STOP制御を行うかどうか（0=行う, 1=行わない）
//	戻値：USI_TWI_TRUE=成功,USI_TWI_FALSE=失敗
uint8_t USI_TWI_Master_Start_Read_Write(uint8_t isNoStop)
{
	unsigned char *msg;
	unsigned char msgSize; 
	
	msg = USI_TWI_Master_Buf;
	msgSize = USI_TWI_Master_BufIdx;
	
	//初期化
	USI_TWI_Master_state.errorState = 0;
	USI_TWI_Master_state.addressMode = USI_TWI_TRUE;
	
	//R/W の判定
	if ( !(*msg & (1<<TWI_READ_BIT)) ) {
		USI_TWI_Master_state.masterWriteDataMode = USI_TWI_TRUE;
	}

	//I2Cスタート
	if ( !USI_TWI_Master_Start( )) {
		return (USI_TWI_FALSE);
	}

	//R/W処理の実行
	do {
		//アドレス送信またはデータ書き込み処理の場合
		if (USI_TWI_Master_state.addressMode || USI_TWI_Master_state.masterWriteDataMode) {
			//1バイトのセットアップ
			USIDR     = *(msg++);
			//1バイトの送信
			SET_USI_TO_SEND_DATA(USI_TWI_MASTER);
			USI_TWI_Master_Transfer();
			//Slaveからの応答を待つ
			SET_USI_TO_READ_ACK(USI_TWI_MASTER);
			if( USI_TWI_Master_Transfer() & (1 << TWI_NACK_BIT) ) {
				if ( USI_TWI_Master_state.addressMode )
					USI_TWI_Master_state.errorState = USI_TWI_NO_ACK_ON_ADDRESS;
				else
					USI_TWI_Master_state.errorState = USI_TWI_NO_ACK_ON_DATA;
				if (isNoStop == 0) USI_TWI_Master_Stop();
				return (USI_TWI_FALSE);
			}
			//アドレスモードじゃなかった場合は？
			if ((!USI_TWI_Master_state.addressMode) && USI_TWI_Master_state.memReadMode) {
				msg = USI_TWI_Master_Buf;
				msgSize = USI_TWI_Master_BufIdx;
				
				*(msg) |= (1 << TWI_READ_BIT);		// set the Read Bit on Slave address
				USI_TWI_Master_state.errorState = 0;
				USI_TWI_Master_state.addressMode = USI_TWI_TRUE;	// Now set up for the Read cycle
				// Note that the length should be Slave adrs byte + # bytes to read + 1 (gets decremented below)
				if ( !USI_TWI_Master_Start( )) {
					USI_TWI_Master_state.errorState = USI_TWI_BAD_MEM_READ;
					return (USI_TWI_FALSE);					// Send a START condition on the TWI bus.
				}
			//アドレス送信だった場合は完了
			} else {
				USI_TWI_Master_state.addressMode = USI_TWI_FALSE;	// Only perform address transmission once.
			}
		}
		//読み込み処理の場合
		else {
			//1バイトの読み込み
			SET_USI_TO_READ_DATA(USI_TWI_MASTER);
			*(msg++)  = USI_TWI_Master_Transfer();
			//ACKまたはNACKを準備する
			if( msgSize == 1) {
				SET_USI_TO_SEND_NACK(USI_TWI_MASTER);
			} else {
				SET_USI_TO_SEND_ACK(USI_TWI_MASTER);
			}
			//ACKまたはNACKを送信する
			USI_TWI_Master_Transfer();
		}
	} while( --msgSize);
	
	//I2Cストップ(例外を除く)
	if (isNoStop == 0) {
		if (!USI_TWI_Master_Stop()) {
			return (USI_TWI_FALSE);
		}
	}
	
	//終了
	return (USI_TWI_TRUE);
}

//-----------------------------
//I2C 通信処理
//	戻値：受信した情報（送信時は ACK, NACKを含む）
uint8_t USI_TWI_Master_Transfer()
{
	unsigned char temp;
	
//	PORT_USI &= ~(1<<PIN_USI_SCL);					// Pull SCL LOW.
	do {
		USI_TWI_Master_Wait(USI_TWI_Master_Wait_L);
		SET_USI_TO_CLOCK();						// Generate positve SCL edge.
		while( !(PIN_USI & (1<<PIN_USI_SCL)) );	// Wait for SCL to go high.
		USI_TWI_Master_Wait(USI_TWI_Master_Wait_H);
		SET_USI_TO_CLOCK();						// Generate negative SCL edge.
	} while( !(USISR & (1<<USIOIF)) );			// Check for transfer complete.
	
	USI_TWI_Master_Wait(USI_TWI_Master_Wait_L);
	temp  = USIDR;                           // Read out data.
	USIDR = 0xFF;                            // Release SDA.
	DDR_USI |= (1<<PIN_USI_SDA);             // Enable SDA as output.
	
	return temp;                             // Return the data from the USIDR
}

//-----------------------------
//I2C 通信開始処理
//	戻値：USI_TWI_TRUE=成功,USI_TWI_FALSE=失敗
uint8_t USI_TWI_Master_Start( void )
{
	/* Release SCL to ensure that (repeated) Start can be performed */
	PORT_USI |= (1<<PIN_USI_SCL);                     // Release SCL.
	while( !(PORT_USI & (1<<PIN_USI_SCL)) );          // Verify that SCL becomes high.
	USI_TWI_Master_Wait(USI_TWI_Master_Wait_L);
	/* Generate Start Condition */
	PORT_USI &= ~(1<<PIN_USI_SDA);                    // Force SDA LOW.
	USI_TWI_Master_Wait(USI_TWI_Master_Wait_H);
	PORT_USI &= ~(1<<PIN_USI_SCL);                    // Pull SCL LOW.
	PORT_USI |= (1<<PIN_USI_SDA);                     // Release SDA.

	if( !(USISR & (1<<USISIF)) ) {
		USI_TWI_Master_state.errorState = USI_TWI_MISSING_START_CON;  
		return (USI_TWI_FALSE);
	}
	
	return (USI_TWI_TRUE);
}

//-----------------------------
//I2C 通信終了処理
//	戻値：USI_TWI_TRUE=成功,USI_TWI_FALSE=失敗
uint8_t USI_TWI_Master_Stop( void )
{
	PORT_USI &= ~(1<<PIN_USI_SDA);           // Pull SDA low.
	PORT_USI |= (1<<PIN_USI_SCL);            // Release SCL.
	while( !(PIN_USI & (1<<PIN_USI_SCL)) );  // Wait for SCL to go high.  
	USI_TWI_Master_Wait(USI_TWI_Master_Wait_H);
	PORT_USI |= (1<<PIN_USI_SDA);            // Release SDA.
	USI_TWI_Master_Wait(USI_TWI_Master_Wait_L);
	
	if( !(USISR & (1<<USIPF)) ) {
		USI_TWI_Master_state.errorState = USI_TWI_MISSING_STOP_CON;    
		return (USI_TWI_FALSE);
	}
	
	return (USI_TWI_TRUE);
}

