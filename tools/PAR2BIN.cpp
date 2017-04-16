/*
 * PAR2BIN.cpp
 *
 * 概要：
 *  テキストで記述されたパラメータをバイナリ形式で出力するツール
 *
 * 使用方法等：
 *  ソースコードを Eclipse 等でコンパイルする。
 *  コンパイル後、引数に PAR ファイル名を指定して実行する。
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/04/16 新規作成
 *
 */

#include <iostream>
#include <fstream>

using namespace std;

// 定数宣言 ===================================================================

#define		NO_NUM	100000		// "数字以外"を表現する値

// エラー・警告メッセージ定義
#define		ERR_STATUS		"存在しないコマンドで始まっています."
#define		ERR_NUMOF		"数字が大きすぎます."
#define		ERR_NOX			"ここで X(16進数の開始) は使用できません."
#define		ERR_NOB			"ここで B(2進数の開始) は使用できません."

/*
	共通処理===================================================================
*/
namespace commonfunc {
	// タイトル
	void	title()
	{
		cerr << "PAR2BIN.EXE ATTiny85 MusicBox用パラメータ 変換プログラム Ver 1.00 (C)Hiro OTSUKA" << endl;
	}
	// 使い方を表示
	void	usage()
	{
		cerr << "  Usage: PAR2BIN.EXE filename" << endl;
		cerr << "    テキストで書かれたパラメータを変換し、.BIN ファイルを作成します." << endl;
		cerr << "    filename には、スペース区切りで複数のファイルを指定できます." << endl;
		exit(1);
	}
	// 出力完了を表示
	void	complete(string filename)
	{
		cout << "  " << filename << ".BIN ファイルを出力しました." << endl;
	}
	// 致命的エラーを表示
	void	error(string txt )
	{
		cout << txt << endl;
		exit(2);
	}
}

/*
	ファイル管理用クラス ======================================================
*/
class	clsFILE	{
	protected:
		int			linenum;	// 現在処理中のラインナンバ
		char		oldch;		// 一時保存用文字
		char		lastch;		// エラー表示用文字
		string		inname;		// 読み込みファイル名
		string		outmuname;	// 書き出しファイル名
		ifstream	in;			// 読み込みファイル
		fstream		outmu;		// 書き出しファイル（読み込みも行う）
		enum chtype : int {	// 文字コード種別
			CH_ETC, CH_KIGOU, CH_NUM, CH_ALPH
		};
		int	ch_check(char ch, int base = 10);
	public:
		clsFILE(string name) {
			linenum = 1;
			oldch = lastch = 0;
			inname = name + ".PAR";
			outmuname = name + ".BIN";
			in.open(inname, ifstream::in | ifstream::binary);
			if (in.fail())
				commonfunc::error("File [" + inname + "] can not open.");
			outmu.open(outmuname, ifstream::out | ifstream::in | ifstream::binary | ifstream::trunc );
			if (outmu.fail())
				commonfunc::error("File [" + outmuname + "] can not open.");
		}
		~clsFILE() {
			if (in.is_open()) in.close();
			if (outmu.is_open()) outmu.close();
		}

		char	getd(int check = 0);
		int		getnum();

		// 書き出しファイルに１バイト書き出す
		void	writemu(char ch) {
			outmu.write((const char*)&ch, sizeof(char));
		}
		// 書き出しファイルに１ワード書き出す
		void	writemuw(int data) {
			char	ch;
			ch = data & 0xFF;
			writemu(ch);
			ch = (data >> 8) & 0xFF;
			writemu(ch);
		}
		// エラー出力して終了する
		void	asm_error(string txt) {
			if (oldch != 0) {
				// 一時保存文字がある場合
				cout << "  <" << inname << ">line " << linenum << "[" << oldch << "]: " << txt << endl;
			} else if (lastch != 0) {
				// 読み込み文字がある場合
				cout << "  <" << inname << ">line " << linenum << "[" << lastch << "]: " << txt << endl;
			} else {
				cout << "  <" << inname << ">line " << linenum << " : " << txt << endl;
			}
			// 出力ファイルを閉じ、削除して終了
			outmu.close();
			remove(outmuname.c_str());
			exit(2);
		}
		// 警告出力して継続する
		void	asm_warning(string txt) {
			cout << "  警告<" << inname << ">line " << linenum << " : " << txt << endl;
		}
		void	asm_warning(string txt, int slen, int nlen) {
			cout << "  警告<" << inname << ">line " << linenum << " : " << txt << "(" << nlen << "/" << slen << ")" << endl;
		}
		// 書き出しファイルの位置を返す
		streampos	getoutaddr() {
			return outmu.tellp();
		}
		// 読み込みファイルの位置と行番号を返す
		streampos	getinaddr(int* retnum) {
			*retnum = linenum;
			return in.tellg();
		}
		// 読み込みファイルの位置と行番号を設定する
		void	setinaddr(streampos addr, int setnum) {
			linenum = setnum;
			in.seekg(addr);
		}
		// 書き出しファイルの指定された位置に１バイト書き込む
		void	writemu_at(streampos addr, char ch) {
			streampos tmp = outmu.tellp();
			outmu.seekp(addr);
			writemu(ch);
			outmu.seekp(tmp);
		}
		// 書き出しファイルの指定された位置に１ワード書き込む
		void	writemuw_at(streampos addr, int data) {
			streampos tmp = outmu.tellp();
			outmu.seekp(addr);
			writemuw(data);
			outmu.seekp(tmp);
		}
};

/*
	読んだ文字が何か調べる
*/
int		clsFILE :: ch_check(char ch, int base)
{
	// 数字
	if (base == 10 && '0' <= ch && ch <= '9') return CH_NUM;
	if (base == 16 && (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F'))) return CH_NUM;
	if (base == 2  && '0' <= ch && ch <= '1') return CH_NUM;
	// アルファベット
	if ('A' <= ch && ch <= 'Z') return CH_ALPH;
	// 有効な記号（ない）
	if (string("\n\r").find(ch) != string::npos) return CH_KIGOU;

	return CH_ETC;
}

/*
	１文字読む(SPC や TAB は飛ばす)
*/
char	clsFILE :: getd(int check)
{
	int		done;
	char	ch;

	// 有効な数字が来るまで読み込みを続ける
	do {
		done = 0;
		// 一時保存文字があった場合はそれを返す
		if (oldch != 0) {
			ch = oldch;
			oldch = 0;
			if (ch == '\n') linenum --;	//後で二重加算になるので減算しておく
		}
		// ない場合は1文字読み込む
		else
			in.read(&ch, 1);

		// コメント開始の場合は行末まで読み捨てる
		if (ch == ';') {
			do {
				in.read(&ch, 1);
			} while(ch != '\n');
		}
		// 改行の場合は行数を加算しておく
		if (ch == '\n') linenum ++;
		// 無効な文字の場合は続けて処理する
		if (ch == ' ' || ch == '\t' || ch == '\r' || (ch=='\n' && check==0))
			done = 1;
	} while(done == 1 && !in.eof());

	// アルファベットの場合は大文字に変換する
	if ('a' <= ch && ch <= 'z')
		ch = ch - 'a' + 'A';

	// ファイルの終了を検出する
	else if (in.eof())
		ch = EOF;

	// エラー表示のため文字を保存し、返す
	lastch = ch;
	return ch;
}

/*
	数字を読み込む
*/
int		clsFILE :: getnum()
{
	int		base = 10, nums = 0;

	// 一文字読み込む
	char ch = getd();

	// 数字が始まらなければ終わる
	if (ch_check(ch, base) != CH_NUM) {
		// 一時保存して、エラーを返す
		oldch = ch;
		return NO_NUM;
	}
	// 数字を読み続ける
	do {
		if (ch_check(ch, base) == CH_NUM) {
			if ('A' <= ch && ch <= 'F')
				nums = nums * base + (ch - 'A' + 10);
			else
				nums = nums * base + (ch - '0');
			if (nums >= NO_NUM)
				asm_error(ERR_NUMOF);
			ch = getd(1);
			if (ch == 'X') {
				// 10進数から 16進数の開始
				if (base == 10) {
					base = 16;
					ch = getd(1);
				} else {
					asm_error(ERR_NOX);
				}
			} else if (ch == 'B') {
				// 10進数から2進数の開始
				if (base == 10) {
					base = 2;
					ch = getd(1);
				// 16進数の場合は B は通常の文字
				} else if (base != 16) {
					asm_error(ERR_NOB);
				}
			}
		}
	} while(ch_check(ch, base) == CH_NUM);

	// 不要な文字の場合は読み捨て
	if (ch_check(ch) == CH_ETC)
		oldch = 0;
	// 必要な文字の場合は一時保存
	else
		oldch = ch;

	return nums;
}

/*
	パラメータ管理型クラス ================================================
*/
class	clsPARAM	{
	protected:
		clsFILE*	fileio;		// ファイルIO管理
	public:
		clsPARAM(clsFILE* cfile) {
			fileio = cfile;
		}
		~clsPARAM() {
		}
		void	asmbl(char ch);
		void	run();
};

/*
	アセンブル実行
*/
void	clsPARAM :: asmbl(char ch)
{
	// １バイト指定
	if (ch == 'B') {
		fileio->writemu((char)fileio->getnum());
	}
	// ２バイト指定
	else if (ch == 'W') {
		fileio->writemuw(fileio->getnum());
	}
	// 未定義のコマンド
	else
		fileio->asm_error(ERR_STATUS);
}

/*
	アセンブルを呼び出す
*/
void	clsPARAM :: run()
{
	char	ch;

	// ヘッダ出力('PARM')
	fileio->writemu('P');
	fileio->writemu('A');
	fileio->writemu('R');
	fileio->writemu('M');
	// サイズ（仮）
	fileio->writemuw(0);
	fileio->writemuw(0);

	// ファイルの終了まで処理を行う
	ch = fileio->getd();
	while (ch != EOF) {
		asmbl(ch);
		ch = fileio->getd();
	};

	// サイズを書き込む
	int size = (int)(fileio->getoutaddr()) - 8;
	fileio->writemuw_at(4, size);
}

/*
	メイン ====================================================================
*/
int	main(int argc, char* argv[]) {
	clsFILE*	fileio;	//ファイルIO管理
	clsPARAM*	param;	//パラメータ管理

	// タイトル表記
	commonfunc::title();

	// 引数が無い場合は使用方法表示して終了
	if (argc == 1)
		commonfunc::usage();

	// ファイルごとに処理を実行
	for (int i = 1; i < argc; i++) {
		string filename = argv[i];
		if (filename.find_last_of('.') != string::npos)
			filename = filename.substr(0, filename.find_last_of('.'));
		fileio = new clsFILE(filename);
		param = new clsPARAM(fileio);
		param->run();
		delete param;
		delete fileio;
		commonfunc::complete(filename);
	}
	return 0;
}
