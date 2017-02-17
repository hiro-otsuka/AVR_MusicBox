/*
 * USI_TWI_Master.c
 *
 * �T�v�F
 *  USI ��p���� I2C �}�X�^�ڑ����s�����W���[��
 *
 * �g�p���@���F
 *  USI_TWI_Master ���g�p����v���W�F�N�g�ɃC���|�[�g���Ďg�p
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/01/29 �V�K�쐬(Hiro OTSUKA)
 *
 */

#include "USI_TWI_Master.h"
#include <avr/interrupt.h>

//********** �֐���` **********//
void USI_TWI_Master_Wait( uint16_t );
uint8_t USI_TWI_Master_Transfer( );
uint8_t USI_TWI_Master_Start_Read_Write( uint8_t );
uint8_t USI_TWI_Master_Stop( void );
uint8_t USI_TWI_Master_Start( void );

//********** �ϐ���` **********//
uint16_t USI_TWI_Master_Wait_H = 0;
uint16_t USI_TWI_Master_Wait_L = 0;

uint8_t USI_TWI_Master_Buf[USI_TWI_MASTER_BUF_SIZE];  // holds I2C send and receive data
uint8_t USI_TWI_Master_BufIdx = 0;                    // current number of bytes in the send buff
uint8_t USI_TWI_Master_LastRead = 0;                  // number of bytes read so far
uint8_t USI_TWI_Master_BytesAvail = 0;                // number of bytes requested but not read

//���p�̂ł̃G���[�E�X�e�[�^�X�Ǘ�
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

//********** �O���֐� **********//
//=============================
//I2C �̏������i��񂾂���OK�j
//	�����FCPU���g��(KHz), I2C���g��(KHz)
void USI_TWI_Master_Initialize(uint16_t f_cpu, uint16_t f_i2c )
{
	//�E�F�C�g���Ԃ��Z�o
	USI_TWI_Master_Wait_H = f_cpu / f_i2c / 16;
	USI_TWI_Master_Wait_L = f_cpu / f_i2c / 16;
	
	USI_TWI_Master_Restart();
}

//=============================
//I2C �̍ċN���iInitialize ��̍ď������Ɏg�p�j
void USI_TWI_Master_Restart()
{
	SET_USI_TO_INIT();
	SET_USI_TO_MASTER();
}

//=============================
//I2C �ł̑��M�J�n��錾�i���̎��_�ł͉����������Ȃ��j
//	�����FI2C�X���[�u�A�h���X
void USI_TWI_Master_tranBegin(uint8_t slaveAddr)
{
	USI_TWI_Master_BufIdx = 0; 
	USI_TWI_Master_Buf[ USI_TWI_Master_BufIdx ++] = (slaveAddr << TWI_ADR_BITS) | (0 << TWI_READ_BIT); 
}

//=============================
//I2C �ł�1�o�C�g���M��\��i���̎��_�ł͉����������Ȃ��j
//	�����F���M�f�[�^
void USI_TWI_Master_tranSend(uint8_t data)
{
	if (USI_TWI_Master_BufIdx >= USI_TWI_MASTER_BUF_SIZE) return;
	USI_TWI_Master_Buf[USI_TWI_Master_BufIdx ++] = data;
}

//=============================
//I2C �ł̑��M�����s
//	�ߒl�F0=����,0�ȊO=�G���[�R�[�h
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
//I2C �ł̑��M�����s�i�������X�g�b�v�������s��Ȃ��j
//	�ߒl�F0=����,0�ȊO=�G���[�R�[�h
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
//I2C ����̎�M�����s
//	�����FI2C�X���[�u�A�h���X, ��M��
//	�ߒl�F0=����,0�ȊO=�G���[�R�[�h
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
//I2C �����M��������1�o�C�g�ǂݍ���
//	�ߒl�F��M���1�o�C�g��
uint8_t USI_TWI_Master_receive()
{
	return USI_TWI_Master_Buf[USI_TWI_Master_LastRead ++];
}

//=============================
//I2C �����M������񂪎c���Ă��邩�ǂ����m�F
//	�ߒl�F��M�\�o�C�g��
uint8_t USI_TWI_Master_receiveAvailable()
{
	return USI_TWI_Master_BytesAvail - (USI_TWI_Master_LastRead); 
}

//********** �����֐� **********//
//-----------------------------
//CPU���x�ɉ������E�F�C�g(�������s���m)
//	�����F�E�F�C�g�J�E���g(1=��4CLK�H)
void USI_TWI_Master_Wait( uint16_t time )
{
	for(; time ; time --)		//LOOP_CYCL = 4
	asm volatile("nop"::);
}

//-----------------------------
//I2C �ɂ�鑗��M�������C��
//	�����FSTOP������s�����ǂ����i0=�s��, 1=�s��Ȃ��j
//	�ߒl�FUSI_TWI_TRUE=����,USI_TWI_FALSE=���s
uint8_t USI_TWI_Master_Start_Read_Write(uint8_t isNoStop)
{
	unsigned char *msg;
	unsigned char msgSize; 
	
	msg = USI_TWI_Master_Buf;
	msgSize = USI_TWI_Master_BufIdx;
	
	//������
	USI_TWI_Master_state.errorState = 0;
	USI_TWI_Master_state.addressMode = USI_TWI_TRUE;
	
	//R/W �̔���
	if ( !(*msg & (1<<TWI_READ_BIT)) ) {
		USI_TWI_Master_state.masterWriteDataMode = USI_TWI_TRUE;
	}

	//I2C�X�^�[�g
	if ( !USI_TWI_Master_Start( )) {
		return (USI_TWI_FALSE);
	}

	//R/W�����̎��s
	do {
		//�A�h���X���M�܂��̓f�[�^�������ݏ����̏ꍇ
		if (USI_TWI_Master_state.addressMode || USI_TWI_Master_state.masterWriteDataMode) {
			//1�o�C�g�̃Z�b�g�A�b�v
			USIDR     = *(msg++);
			//1�o�C�g�̑��M
			SET_USI_TO_SEND_DATA(USI_TWI_MASTER);
			USI_TWI_Master_Transfer();
			//Slave����̉�����҂�
			SET_USI_TO_READ_ACK(USI_TWI_MASTER);
			if( USI_TWI_Master_Transfer() & (1 << TWI_NACK_BIT) ) {
				if ( USI_TWI_Master_state.addressMode )
					USI_TWI_Master_state.errorState = USI_TWI_NO_ACK_ON_ADDRESS;
				else
					USI_TWI_Master_state.errorState = USI_TWI_NO_ACK_ON_DATA;
				if (isNoStop == 0) USI_TWI_Master_Stop();
				return (USI_TWI_FALSE);
			}
			//�A�h���X���[�h����Ȃ������ꍇ�́H
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
			//�A�h���X���M�������ꍇ�͊���
			} else {
				USI_TWI_Master_state.addressMode = USI_TWI_FALSE;	// Only perform address transmission once.
			}
		}
		//�ǂݍ��ݏ����̏ꍇ
		else {
			//1�o�C�g�̓ǂݍ���
			SET_USI_TO_READ_DATA(USI_TWI_MASTER);
			*(msg++)  = USI_TWI_Master_Transfer();
			//ACK�܂���NACK����������
			if( msgSize == 1) {
				SET_USI_TO_SEND_NACK(USI_TWI_MASTER);
			} else {
				SET_USI_TO_SEND_ACK(USI_TWI_MASTER);
			}
			//ACK�܂���NACK�𑗐M����
			USI_TWI_Master_Transfer();
		}
	} while( --msgSize);
	
	//I2C�X�g�b�v(��O������)
	if (isNoStop == 0) {
		if (!USI_TWI_Master_Stop()) {
			return (USI_TWI_FALSE);
		}
	}
	
	//�I��
	return (USI_TWI_TRUE);
}

//-----------------------------
//I2C �ʐM����
//	�ߒl�F��M�������i���M���� ACK, NACK���܂ށj
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
//I2C �ʐM�J�n����
//	�ߒl�FUSI_TWI_TRUE=����,USI_TWI_FALSE=���s
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
//I2C �ʐM�I������
//	�ߒl�FUSI_TWI_TRUE=����,USI_TWI_FALSE=���s
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

