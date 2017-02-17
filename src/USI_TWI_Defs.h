/*
 * USI_TWI_Defs.h
 *
 * 概要：
 *  USI_TWI_Master, USI_TWI_Slave のためのピンアサインのHW差異を定義する
 *  実際に動作確認が取れているのは一部のマイコンのみ
 *
 * 使用方法等：
 *  別ライブラリから include 
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *
 */

#ifndef USI_TWI_Defs_h
#define USI_TWI_Defs_h

#include <inttypes.h>
#include <avr/io.h>

/****************************************************************************
  Bit and byte definitions
****************************************************************************/
#define TWI_READ_BIT	0       // Bit position for R/W bit in "address byte".
#define TWI_ADR_BITS	1       // Bit position for LSB of the slave address bits in the init byte.
#define TWI_NACK_BIT	0       // Bit position for (N)ACK bit.

#define USI_TWI_TRUE	1
#define USI_TWI_FALSE	0

#define USI_TWI_MASTER	1
#define USI_TWI_SLAVE	0

// Device dependant defines ADDED BACK IN FROM ORIGINAL ATMEL .H

#if defined(__AVR_AT90Mega169__) | \
	defined(__AVR_ATmega169__) | \
	defined(__AVR_ATmega329__) | \
	defined(__AVR_ATmega649__) | \
	defined(__AVR_ATmega3290__) | \
	defined(__AVR_ATmega6490__)
	defined(__AVR_AT90Mega165__) | \
	defined(__AVR_ATmega165__) | \
	defined(__AVR_ATmega325__) | \
	defined(__AVR_ATmega645__) | \
	defined(__AVR_ATmega3250__) | \
	defined(__AVR_ATmega6450__) | \
	#define DDR_USI             DDRE
	#define PORT_USI            PORTE
	#define PIN_USI             PINE
	#define PORT_USI_SDA        PORTE5
	#define PORT_USI_SCL        PORTE4
	#define PIN_USI_SDA         PINE5
	#define PIN_USI_SCL         PINE4
	#define USI_START_COND_INT  USISIF
	#define USI_START_VECTOR    USI_START_vect
	#define USI_OVERFLOW_VECTOR USI_OVF_vect
#endif

#if defined(__AVR_ATtiny25__) | \
	defined(__AVR_ATtiny45__) | \
	defined(__AVR_ATtiny85__) | \
	defined(__AVR_AT90Tiny26__) | \
	defined(__AVR_ATtiny26__)
	#define DDR_USI             DDRB
	#define PORT_USI            PORTB
	#define PIN_USI             PINB
	#define PORT_USI_SDA        PINB0
	#define PORT_USI_SCL        PINB2
	#define PIN_USI_SDA         PINB0
	#define PIN_USI_SCL         PINB2
	#define USI_START_COND_INT  USISIF
	#define USI_START_VECTOR    USI_START_vect
	#define USI_OVERFLOW_VECTOR USI_OVF_vect
#endif

#if defined(__AVR_AT90Tiny2313__) | \
	defined(__AVR_ATtiny2313__)
	#define DDR_USI             DDRB
	#define PORT_USI            PORTB
	#define PIN_USI             PINB
	#define PORT_USI_SDA        PORTB5
	#define PORT_USI_SCL        PORTB7
	#define PIN_USI_SDA         PINB5
	#define PIN_USI_SCL         PINB7
	#define USI_START_COND_INT  USISIF
	#define USI_START_VECTOR    USI_START_vect
	#define USI_OVERFLOW_VECTOR USI_OVERFLOW_vect
#endif

#if defined( __AVR_ATtiny261__ ) | \
	defined( __AVR_ATtiny461__ ) | \
	defined( __AVR_ATtiny861__ ) | \
	defined( __AVR_ATtiny261A__ ) | \
	defined( __AVR_ATtiny461A__ ) | \
	defined( __AVR_ATtiny861A__ )
	#define DDR_USI             DDRB
	#define PORT_USI            PORTB
	#define PIN_USI             PINB
	#define PORT_USI_SDA        PB0
	#define PORT_USI_SCL        PB2
	#define PIN_USI_SDA         PINB0
	#define PIN_USI_SCL         PINB2
	#define USI_START_COND_INT  USISIF
	#define USI_START_VECTOR    USI_START_vect
	#define USI_OVERFLOW_VECTOR USI_OVF_vect
#endif

static inline void SET_USI_TO_INIT( ) {
	// Enable SCL and SDA as output.
	DDR_USI |= ( 1 << PIN_USI_SCL) | ( 1 << PIN_USI_SDA );
	// set SCL and SDA high
	PORT_USI |= ( 1 << PORT_USI_SCL ) | ( 1 << PORT_USI_SDA );
}

static inline void SET_USI_TO_SEND_ACK(uint8_t isMaster ) {
	/* prepare ACK */
	USIDR = 0;
	/* set SDA as output */
	DDR_USI |= ( 1 << PORT_USI_SDA );
	/* clear all interrupt flags */
	USISR =
		( isMaster << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | ( 1 << USIDC ) |
		/* set USI counter to shift 1 bit */
		( 0x0E << USICNT0 );
}

static inline void SET_USI_TO_SEND_NACK(uint8_t isMaster ) {
	/* prepare NACK */
	USIDR = 0xFF;
	/* set SDA as output */
	DDR_USI |= ( 1 << PORT_USI_SDA );
	/* clear all interrupt flags */
	USISR =
		(isMaster << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | ( 1 << USIDC ) |
		/* set USI counter to shift 1 bit */
		( 0x0E << USICNT0 );
}

static inline void SET_USI_TO_READ_ACK(uint8_t isMaster ) {
	/* set SDA as input */
	DDR_USI &= ~( 1 << PORT_USI_SDA );
	/* prepare ACK */
	USIDR = 0;
	/* clear all interrupt flags, except Start Cond */
	USISR =
		(isMaster << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | ( 1 << USIDC ) |
		/* set USI counter to shift 1 bit */
		( 0x0E << USICNT0 );
}

static inline void SET_USI_TO_SEND_DATA(uint8_t isMaster) {
	/* set SDA as output */
	DDR_USI |=  ( 1 << PORT_USI_SDA );
	/* clear all interrupt flags */
	USISR =
	( isMaster << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | ( 1 << USIDC) |
	/* set USI to shift out 8 bits */
	( 0x0 << USICNT0 );
}

static inline void SET_USI_TO_READ_DATA(uint8_t isMaster) {
	/* set SDA as input */
	DDR_USI &= ~( 1 << PORT_USI_SDA );
	/* clear all interrupt flags */
	USISR =
	(isMaster << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | ( 1 << USIDC ) |
	/* set USI to shift out 8 bits */
	( 0x0 << USICNT0 );
}

static inline void SET_USI_TO_TWI_START_CONDITION_MODE( ) {
	USICR = 
		/* enable Start Condition Interrupt, disable Overflow Interrupt */
		( 1 << USISIE ) | ( 0 << USIOIE ) |
		/* set USI in Two-wire mode, no USI Counter overflow hold */
		( 1 << USIWM1 ) | ( 0 << USIWM0 ) |
		/* Shift Register Clock Source = External, positive edge */
		/* 4-Bit Counter Source = external, both edges */
		( 1 << USICS1 ) | ( 0 << USICS0 ) | ( 0 << USICLK ) |
		/* no toggle clock-port pin */
		( 0 << USITC );
	USISR =
		/* clear all interrupt flags */
		( 0 << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | ( 1 << USIDC ) | 
		/* set USI counter to shift 8 bit */
		( 0x0 << USICNT0 );
}

static inline void SET_USI_TO_TWI_HOLD_SCL_MODE( ) {
	USICR = 
		/* enable Start Condition Interrupt and Overflow Interrupt */
		( 1 << USISIE ) | ( 1 << USIOIE ) |
		/* set USI in Two-wire mode, USI Counter overflow hold */
		( 1 << USIWM1 ) | ( 1 << USIWM0 ) |
		/* Shift Register Clock Source = External, positive edge */
		/* 4-Bit Counter Source = external, both edges */
		( 1 << USICS1 ) | ( 0 << USICS0 ) | ( 0 << USICLK ) |
		/* no toggle clock-port pin */
		( 0 << USITC );
	USISR =
		/* clear all interrupt flags, except Start Cond */
		( 1 << USI_START_COND_INT ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | ( 1 << USIDC ) | 
		/* set USI counter to shift 8 bit */
		( 0x0 << USICNT0 );
}

static inline void SET_USI_TO_MASTER( ) {
	// Enable SCL and SDA as output.
	DDR_USI |= ( 1 << PIN_USI_SCL ) | ( 1 << PIN_USI_SDA );
	/* Preload dataregister with "released level" data. */
	USIDR = 0xFF;
	USICR =
	/* Disable Interrupts. */
	( 0 << USISIE ) | ( 0 << USIOIE ) |
	/* Set USI in Two-wire mode. */
	( 1 << USIWM1 ) | ( 0 << USIWM0 ) |
	/* Software stobe as counter clock source */
	( 1 << USICS1 ) | ( 0 << USICS0 ) | ( 1 << USICLK ) |
	/* no toggle clock-port pin */
	( 0 << USITC );
	USISR =
	/* Clear flags, */ \
	( 1 << USISIF ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | ( 1 << USIDC ) |
	/* and reset counter. */ \
	( 0x0 << USICNT0 );
}

static inline void SET_USI_TO_CLOCK( ) {
	USICR =
	/* Disable Interrupts. */
	( 0 << USISIE ) | ( 0 << USIOIE ) |
	/* Set USI in Two-wire mode. */
	( 1 << USIWM1 ) | ( 0 << USIWM0 ) |
	/* Software stobe as counter clock source */
	( 1 << USICS1 ) | ( 0 << USICS0 ) | ( 1 << USICLK ) |
	/* toggle clock-port pin */
	( 1 << USITC );
}

static inline void SET_USI_TO_SLAVE( ) {
	// Set SCL as output.
//	DDR_USI |= ( 1 << PIN_USI_SCL);
	// Set SDA as input.
	DDR_USI &= ~( 1 << PIN_USI_SDA );
	USICR = 
		/* Disable Interrupts. */ 
		( 1 << USISIE ) | ( 0 << USIOIE ) | 
		/* Set USI in Two-wire mode. */
		( 1 << USIWM1 ) | ( 0 << USIWM0 ) | 
		/* Software stobe as counter clock source */
		( 1 << USICS1 ) | ( 0 << USICS0 ) | ( 0 << USICLK ) | 
		/* no toggle clock-port pin */
		( 0 << USITC );
	USISR = 
		/* Clear flags, */ \
		( 1 << USISIF ) | ( 1 << USIOIF ) | ( 1 << USIPF ) | ( 1 << USIDC ) | 
		/* and reset counter. */ \
		( 0x0 << USICNT0 );
}

#endif

