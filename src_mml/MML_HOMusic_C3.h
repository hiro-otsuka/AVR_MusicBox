/*
 * MML_HOMusic_C3.h
 *
 * 概要：
 *  サンプルミュージック
 *
 * 使用方法等：
 *  main.c から include し、MusicBox_SetScore の引数として Setup 関数へのポインタを渡す
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/01/29 新規作成(Hiro OTSUKA)
 *  2017/02/17 構成変更(Hiro OTSUKA) EEPROMからのMML再生およびWAVとの自動判別に対応
 *
 */

#ifndef MML_HOMUSIC_C3_H_
#define MML_HOMUSIC_C3_H_

static void MML_HOMusic_C3_Setup()
{
	#undef N_LEN
	#undef N_KEY
	#define N_LEN	128		//1分音符の長さ
	#define N_KEY	0		//キー変更

	const static uint16_t Buff1[] PROGMEM = {
		M_CTRL(C_VOL, 6),
		M_CTRL(C_DIVST, 12),
		M_CTRL(C_DIVWT, 12),
		M_CTRL(C_OFF, N_KEY+12),
		M_CTRL(C_TYPE, 2),
		
		M_NOTE(N_E, 1, N_LEN/4),
		M_NOTE(N_A, 1, N_LEN/4+N_LEN/8), M_NOTE(N_C, 2, N_LEN/4), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_C, 2, N_LEN/8),
		M_NOTE(N_B, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4+N_LEN/8), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_A, 1, N_LEN/2), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/8), M_REST(N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/2+N_LEN/4), M_NOTE(N_A, 0, N_LEN/4),
		M_NOTE(N_E, 1, N_LEN/4+N_LEN/8), M_NOTE(N_C, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8), 
		M_NOTE(N_D, 1, N_LEN/4+N_LEN/8), M_NOTE(N_B, 0, N_LEN/4), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/4+N_LEN/8), M_NOTE(N_C, 1, N_LEN/4), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/2), M_NOTE(N_Gs, 1, N_LEN/2),
		M_NOTE(N_B, 1, N_LEN/4+N_LEN/8), M_NOTE(N_Fs, 2, N_LEN/4), M_NOTE(N_E, 2, N_LEN/4), M_NOTE(N_Fs, 2, N_LEN/8), 
		M_NOTE(N_D, 2, N_LEN/2), M_NOTE(N_B, 2, N_LEN/4), M_NOTE(N_Fs, 2, N_LEN/4),
		M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/4), M_NOTE(N_G, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/4),
		M_NOTE(N_D, 2, N_LEN/2), M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_REST(N_LEN/8), M_NOTE(N_Fs, 2, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_REST(N_LEN/8), M_NOTE(N_C, 2, N_LEN/4), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_C, 2, N_LEN/8),
		M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_D, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_REST(N_LEN/8), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8), 
		M_NOTE(N_A, 1, N_LEN/2+N_LEN/4), M_NOTE(N_E, 1, N_LEN/4),

		M_NOTE(N_A, 1, N_LEN/4+N_LEN/8), M_NOTE(N_C, 2, N_LEN/4), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_C, 2, N_LEN/8),
		M_NOTE(N_B, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4+N_LEN/8), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_A, 1, N_LEN/2), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/8), M_REST(N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/2+N_LEN/4), M_NOTE(N_A, 0, N_LEN/4),
		M_NOTE(N_E, 1, N_LEN/4+N_LEN/8), M_NOTE(N_C, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/4+N_LEN/8), M_NOTE(N_B, 0, N_LEN/4), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/4+N_LEN/8), M_NOTE(N_C, 1, N_LEN/4), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/2), M_NOTE(N_Gs, 1, N_LEN/2),
		M_NOTE(N_B, 1, N_LEN/4+N_LEN/8), M_NOTE(N_Fs, 2, N_LEN/4), M_NOTE(N_E, 2, N_LEN/4), M_NOTE(N_Fs, 2, N_LEN/8),
		M_NOTE(N_D, 2, N_LEN/2), M_NOTE(N_B, 2, N_LEN/4), M_NOTE(N_Fs, 2, N_LEN/4),
		M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/4), M_NOTE(N_G, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/4),
		M_NOTE(N_D, 2, N_LEN/2), M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_REST(N_LEN/8), M_NOTE(N_Fs, 2, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_REST(N_LEN/8), M_NOTE(N_C, 2, N_LEN/4), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_C, 2, N_LEN/8),
		M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_D, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_REST(N_LEN/8), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_A, 1, N_LEN/2+N_LEN/4),
		
		M_CTRL(C_END, 0),
	};

	const static uint16_t Buff2[] PROGMEM = {
		M_CTRL(C_VOL, 5),
		M_CTRL(C_DIVST, 4),
		M_CTRL(C_DIVWT, 6),
		M_CTRL(C_OFF, N_KEY),
		
		M_REST(N_LEN/4),
		M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8),
		M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		 M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		 M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8),
		 M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8),
		 M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8),
		M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4),
		M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4),
		M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/4),
		M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4+N_LEN/8), M_NOTE(N_A, 1, N_LEN/4),
		
		M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8),
		M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/4), M_NOTE(N_F, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_B, 0, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8),
		M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8), M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_D, 0, N_LEN/8),
		M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4),
		M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/4),
		M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/4),
		M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/4+N_LEN/8), 
		
		M_CTRL(C_END, 0),
	};

	const static uint16_t Buff3[] PROGMEM = {
		M_CTRL(C_VOL, 4),
		M_CTRL(C_DIVST, 1),
		M_CTRL(C_DIVWT, 3),
		M_CTRL(C_OFF, N_KEY+12),

		M_REST(N_LEN/4),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		 M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		 M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		 M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		 M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		 M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		 M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		 M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_Gs, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		 M_NOTE(N_Gs, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_Gs, 2, N_LEN/8),
		M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8),
		 M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8),
		M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		 M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8),
		 M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8),
		M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		 M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_C, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/4),
		M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_B, 0, N_LEN/4), M_NOTE(N_D, 1, N_LEN/4),
		M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_B, 0, N_LEN/4), M_NOTE(N_G, 0, N_LEN/4), M_NOTE(N_B, 0, N_LEN/4),
		M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_A, 0, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8),
		 M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_C, 1, N_LEN/4),
		
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_E, 1, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_D, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_G, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_F, 1, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_Gs, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_Gs, 1, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_Gs, 2, N_LEN/8),
		M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8),
		M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_Fs, 2, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8),
		M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_B, 1, N_LEN/8),
		M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8),
		M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8), M_NOTE(N_E, 2, N_LEN/8), M_NOTE(N_C, 2, N_LEN/8),
		M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8), M_NOTE(N_D, 2, N_LEN/8), M_NOTE(N_A, 1, N_LEN/8),
		M_NOTE(N_A, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_C, 1, N_LEN/4), M_NOTE(N_E, 1, N_LEN/4),
		M_NOTE(N_G, 1, N_LEN/4), M_NOTE(N_D, 1, N_LEN/4), M_NOTE(N_B, 0, N_LEN/4), M_NOTE(N_D, 1, N_LEN/4),
		M_NOTE(N_E, 1, N_LEN/4), M_NOTE(N_B, 0, N_LEN/4), M_NOTE(N_G, 0, N_LEN/4), M_NOTE(N_B, 0, N_LEN/4),
		M_NOTE(N_E, 1, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8), M_NOTE(N_A, 0, N_LEN/8), M_NOTE(N_C, 1, N_LEN/8),
		M_NOTE(N_E, 1, N_LEN/4), 

		M_CTRL(C_END, 0),
	};

	PWM_PCM_MB_MEM_SetScoreChannel(0, Buff1, sizeof(Buff1)/2);
	PWM_PCM_MB_MEM_SetScoreChannel(1, Buff2, sizeof(Buff2)/2);
	PWM_PCM_MB_MEM_SetScoreChannel(2, Buff3, sizeof(Buff3)/2);
}

#endif
