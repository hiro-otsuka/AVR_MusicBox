/*
 * MML2BIN.cpp
 *
 * 概要：
 *  MMLで記述された楽譜情報をアセンブルし、バイナリ形式で出力するツール
 *
 * 使用方法等：
 *  ソースコードを Eclipse 等でコンパイルする。
 *  コンパイル後、引数に MML ファイル名を指定して実行する。
 *  生成されたファイルを EEPROM 等に書き込むには HEX ファイルへの変換が必要。
 *
 * ライセンス：
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2条項BSDライセンス/2 clause BSD license（同梱のLICENSE.md参照/See LICENSE.md）
 *
 * 更新履歴：
 *  2017/02/17 PC-98時代の C++コードを整理し、ATTiny85 のソフトと統合して公開
 *  2017/02/18 さらにコードを整理し、サブパート定義とループの機能を追加
 *  2017/02/19 拍、テンポ、キーを途中で変更する機能を追加、複付点音符に対応
 *  2017/03/11 GUIからの呼び出しに対応するため出力方法を変更
 *
 */

#include <iostream>
#include <fstream>

using namespace std;

// 定数宣言 ===================================================================

#define		NO_NUM	10000		// "数字以外"を表現する値

#define		OCT_MAX		4		// オクターブの最大値
#define		OCT_NUM		12		// 1オクターブの音符数（変更不要）
#define		SUB_MAX		1024	// ループ内のコピーバッファ

// エラー・警告メッセージ定義
#define		ERR_STATUS		"存在しないコマンドで始まっています."
#define		ERR_NOTNUM		"数字で指定して下さい."
#define		ERR_NOTNUM2		"カッコ内に2個の数字を指定して下さい."
#define		ERR_NOTCOM		"コマンドが違います."
#define		ERR_NOTDEF		"宣言が正しく始まっていません."
#define		ERR_NUMOF		"数字が大きすぎます."
#define		ERR_NOTKEY		"キーが正しくありません."
#define		ERR_NOTLEN		"数字/数字というように指定して下さい."
#define		ERR_SUBPTOPEN	"サブパートが閉じられていません."
#define		ERR_SUBPTNONAME	"サブパート名をカッコ内に指定してください."
#define		ERR_SUBPTERR	"サブパート名が存在しません."
#define		ERR_SUBNOLABEL	"サブパート名が指定されていません."
#define		ERR_LOOPOPEN	"[]ループが閉じられていません."
#define		ERR_LOOPNONUM	"[]ループの回数が指定されていません."
#define		ERR_LOOPLONG	"[]ループ内のデータが多すぎます."
#define		ERR_VOLERR		"Vコマンドの後に = は指定できません."
#define		ERR_LENGTH		"音符の長さが長すぎます. 休符等で調節してください."

#define		WAR_NOTLEN		"１小節の長さが正しくありません."
#define		WAR_OCTCHK		"オクターブのチェックに差異があります."
#define		WAR_OCTMAX		"オクターブが5以上になっています."
#define		WAR_OCTMIN		"オクターブが0未満になっています."

// MML側のコマンド定義
#define		CMD_REST	0x00
#define		CMD_VOL		0x81
#define		CMD_DIVST	0x82
#define		CMD_DIVWT	0x83
#define		CMD_KEY		0x84
#define		CMD_END		0x85
#define		CMD_LED_ON	0x86
#define		CMD_LED_OFF	0x87
#define		CMD_TYPE	0x88

/*
	共通処理===================================================================
*/
namespace commonfunc {
	// タイトル
	void	title()
	{
		cerr << "MML2BIN.EXE ATTiny85 MusicBox用MML アセンブルプログラム Ver 1.00 (C)Hiro OTSUKA" << endl;
	}
	// 使い方を表示
	void	usage()
	{
		cerr << "  Usage: MML2BIN.EXE filename" << endl;
		cerr << "    MML で書かれた音楽をアセンブルし、.BIN ファイルを作成します." << endl;
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
		int	ch_check(char ch);
	public:
		clsFILE(string name) {
			linenum = 1;
			oldch = lastch = 0;
			inname = name + ".MML";
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
		string	getlabel();
		void	outcopy(streampos addr, int num);
		enum ptype : int {		// プラスマイナス種別
			PT_PLUS, PT_MINUS, PT_EQUAL, PT_NONE
		};

		// プラスマイナスの指定を読み込む
		int		getplus() {
			lastch = getd();
			if (lastch == '+') return PT_PLUS;
			if (lastch == '-') return PT_MINUS;
			if (lastch == '=') return PT_EQUAL;
			oldch = lastch;
			return PT_NONE;
		}
		// 付点かどうかを調べる
		bool	isfuten() {
			if ('.' == (lastch = getd())) return true;
			oldch = lastch;
			return false;
		}
		// べき（タイ）かどうかを調べる
		bool	isbeki() {
			lastch = getd();
			if (lastch == '^') return true;
			oldch = lastch;
			return false;
		}
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
int		clsFILE :: ch_check(char ch)
{
	// アルファベット
	if ('A' <= ch && ch <= 'Z') return CH_ALPH;
	// 数字
	if ('0' <= ch && ch <= '9') return CH_NUM;
	// 有効な記号
	if (string(":|/^+-=.'\%@#$[]{}<>()~").find(ch) != string::npos) return CH_KIGOU;

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
	int		nums = 0, kake = 1;

	// 一文字読み込む
	char ch = getd();

	// マイナス数字の開始
	if (ch == '-') {
		kake = -1;
		ch = getd();
	}
	// 数字ではない場合
	if (ch_check(ch) != CH_NUM) {
		// 一時保存して、エラーを返す
		oldch = ch;
		return NO_NUM;
	}
	// 数字を読み続ける
	do {
		nums = nums * 10 + (ch - '0');
		if (nums >= NO_NUM)
			asm_error(ERR_NUMOF);
		ch = getd(1);
	} while(ch_check(ch) == CH_NUM);

	// 不要な文字の場合は読み捨て
	if (ch_check(ch) == CH_ETC)
		oldch = 0;
	// 必要な文字の場合は一時保存
	else
		oldch = ch;

	// マイナスを考慮して返す
	return nums * kake;
}

/*
	ラベルを読み込む
*/
string	clsFILE :: getlabel()
{
	string	buff = "";

	// ラベル終了まで読み込む
	char ch = getd();
	while(ch != '(' && ch != ')' && ch != '{' && ch != '}' && ch != '[' && ch != ']') {
		char	tmp[2];
		tmp[0] = ch;
		tmp[1] = '\0';
		buff = buff + string((const char*)tmp);
		ch = getd();
	}

	// ラベルが無い場合はエラー
	if (buff.length() == 0)
		asm_error(ERR_SUBNOLABEL);

	return buff;
}

 /*
	出力ファイルの一部をコピーする
 */
void clsFILE :: outcopy(streampos addr, int num)
{
	char	buff[SUB_MAX];

	// 現在の出力位置を得る
	streampos nowpos = getoutaddr();
	// コピー開始位置からのサイズを得る
	int	loopsize = nowpos - addr;

	// サイズチェック
	if (loopsize >= SUB_MAX)
		asm_error(ERR_LOOPLONG);

	// 指定回数分コピーする
	while(num != 0) {
		outmu.seekp(addr);
		outmu.read(buff, loopsize);
		outmu.seekp(0, ios_base::end);
		outmu.write(buff, loopsize);
		num --;
	}
}

/*
	キーデフォルト管理クラス ==================================================
*/
class	clsKEY	{
	protected:
		clsFILE*	fileio;				// ファイルIO管理
		int			akeys[OCT_NUM+1];	// キーのデフォルトを保存（全体初期設定）
		int			alen;				// 一小節の長さ（全体初期設定）
		int			atempo;				// 一分音符の長さ（全体初期設定）
		int			keys[OCT_NUM+1];	// キーのデフォルトを保存（途中変更あり）
		int			slen;				// 一小節の長さ（途中変更あり）
		int			tempo;				// 一分音符の長さ（途中変更あり）
		int			nlen;				// 現在の一小節の長さ
	public:
		clsKEY(clsFILE* cfile) {
			fileio = cfile;
			alen = slen = tempo = atempo = 96;
			init();
		}
		void	init();
		int		tranch(char ch);
		void	asmbl_key(int isall = 1);
		void	asmbl_beat(int isall = 1);
		void	asmbl_tempo(int isall = 1);

		// 一分音符の長さを返す
		int		getalllen() {
			return tempo;
		}
		// 一小節の長さを初期化する
		void	sleninit() {
			// 全体設定を適用
			slen = alen;
			tempo = atempo;
			for (int tmp = 0; tmp < OCT_NUM; tmp ++) {
				keys[tmp] = akeys[tmp];
			}
			// 今の長さは初期化
			nlen = 0;
		}
		// 一小節の長さに加算する
		void	slenadd(int len) {
			nlen += len;
		}
		int		slenget() {
			return nlen;
		}
		// 一小節の長さをチェックする（エラー出力が無い指定も可能）
		void	slencheck(int noerr = 0) {
			if (nlen != slen && noerr == 0)
				fileio->asm_warning(WAR_NOTLEN, slen, nlen);
			nlen = 0;
		}
		// キーのデフォルトを得る
		int		getplus(int key) {
			return keys[key];
		}
};

 /*
	初期化
 */
void	clsKEY :: init()
{
	int i;

	for (i = 0; i < OCT_NUM+1; i ++) {
		keys[i] = 0;
		akeys[i] = 0;
	}
	sleninit();
}

/*
	文字を音階に変換
*/
int	clsKEY :: tranch(char ch)
{
	int	param[] = { 10, 12, 1, 3, 5, 6, 8 };

	// 'A' から始まる
	ch -= 'A';

	// 範囲外はエラー
	if (ch > 6 || ch < 0) fileio->asm_error(ERR_NOTKEY);

	return param[(int)ch];
}

 /*
 	 キーを読み込み
 */
void	clsKEY :: asmbl_key(int isall)
{
	char	ch;
	int		tmp, keytmp;

	// 定義開始が無ければエラー
	if (fileio->getd() != '{')
		fileio->asm_error(ERR_NOTDEF);

	// 定義終了まで読み込む
	while((ch = fileio->getd()) != '}') {
		tmp = tranch(ch);
		if (clsFILE::PT_NONE != (keytmp = fileio->getplus())) {
			if (keytmp == clsFILE::PT_PLUS) {
				keys[tmp] = 1;
			} else if (keytmp == clsFILE::PT_MINUS) {
				keys[tmp] = -1;
			} else if (keytmp == clsFILE::PT_EQUAL) {
				keys[tmp] = 0;
			}
		}
	}
	// 全体設定の場合は初期値を保存
	if (isall) {
		for (tmp = 0; tmp < OCT_NUM; tmp ++) {
			akeys[tmp] = keys[tmp];
		}
	}
}

/*
	 拍を読み込み
*/
void	clsKEY :: asmbl_beat(int isall)
{
	int		tmp, tmp2;

	// 分子を読み込む
	if (NO_NUM == (tmp = fileio->getnum()))
		fileio->asm_error(ERR_NOTLEN);
	// 除算が無ければエラー
	if (fileio->getd() != '/')
		fileio->asm_error(ERR_NOTLEN);
	// 分母を読み込む
	if (NO_NUM == (tmp2 = fileio->getnum()))
		fileio->asm_error(ERR_NOTLEN);
	// 一分音符の長さから一小節の長さを算出する
	slen = (getalllen() / tmp2) * tmp;
	// 全体設定の場合は初期値を保存
	if (isall) alen = slen;
}

/*
	 テンポを読み込み
*/
void	clsKEY :: asmbl_tempo(int isall)
{
	int		tmp;

	if (NO_NUM == (tmp = fileio->getnum()))
		fileio->asm_error(ERR_NOTLEN);
	slen = tempo = tmp;
	// 全体設定の場合は初期値を保存
	if (isall) alen = atempo = tempo;
}

/*
	SML展開型クラス	===========================================================
*/
class	clsPART	{
	protected:
		clsFILE* 	fileio;		// ファイルIO管理
		clsKEY*		keys;		// キー管理
		clsPART*	next;		// 次のデータ
		int			nowOct;		// 現在のオクターブ
		int			nowVol;		// 現在のボリューム
		int			deflen;		// 現在のデフォルト長

		int		getlength();

		// 次に記述されている数字をそのまま書き込む
		void	writenum() {
			int	com;

			if ((com = fileio->getnum()) == NO_NUM)
				fileio->asm_error(ERR_NOTNUM);
			fileio->writemu(com);
		}
	public:
		clsPART(clsFILE* cfile, clsKEY* ckey) {
			fileio = cfile;
			keys = ckey;
			next = NULL;
			nowOct = 0;
			nowVol = 0;
			deflen = 4;
		}
		virtual ~clsPART() {
			if (next != NULL) delete next;
		}
		void	asmblpt(char ch);

		// 次のデータに追加
		void	addlink(clsPART* tmp) {
			if (next == NULL) next = tmp;
			else next->addlink(tmp);
		}
		// データ数を得る
		int		getnum(int num = 0) {
			if (next == NULL) return num + 1;
			else return next->getnum(num + 1);
		}
};

/*
	音長を取得
*/
int	clsPART :: getlength()
{
	int	com, tmp, minus = 0;

	// 数字指定の場合はその数字を使用して長さを算出
	if ((com = fileio->getnum()) != NO_NUM)
		com = keys->getalllen() / com;
	// 数字が無い場合はデフォルト長を使用
	else
		com = keys->getalllen() / deflen;
	// 付点音符の場合
	if (fileio->isfuten()) {
		// 複付点の場合は 1.75倍
		if (fileio->isfuten())
			com = com + com/2 + com/4;
		// 付点音符の場合は 1.5倍
		else
			com = com + com/2;
	}
	// タイが続く場合は加算を継続
	while (fileio->isbeki()) {
		// 数字を取得
		tmp = fileio->getnum();
		// 数字ではない場合
		if (tmp == NO_NUM) {
			// 何が書かれているかを調べる
			tmp = fileio->getd();
			// 小節の終わりの場合
			if (tmp == '|' || tmp == ':') {
				// 前小節分を保存しておく
				minus += com;
				// チェック処理を行う
				keys->slenadd(com);
				if (tmp == '|') keys->slencheck();
				else keys->slencheck(1);
				// その次に数字が来るはず
				if ((tmp = fileio->getnum()) == NO_NUM)
					fileio->asm_error(ERR_NOTNUM);
			}
			// それ以外はエラー
			else
				fileio->asm_error(ERR_NOTNUM);
		}
		// 長さを算出する
		tmp = keys->getalllen() / tmp;
		// 付点音符の場合
		if (fileio->isfuten()) {
			// 複付点の場合は 1.75倍
			if (fileio->isfuten())
				tmp = tmp + tmp/2 + tmp/4;
			// 付点音符の場合は 1.5倍
			else
				tmp = tmp + tmp/2;
		}
		// 加算する
		com += tmp;
	}
	// 長すぎる場合はエラーにする
	if (com >= 256)
		fileio->asm_error(ERR_LENGTH);

	// 前小節分を除いて、小節の長さに足しておく
	keys->slenadd(com - minus);

	return com;
}

/*
	アセンブルサブ
*/
void	clsPART :: asmblpt(char ch)
{
	int		com, plus, num1;

	// 音符情報
	if (('A' <= ch && ch <= 'G') || ch == 'R') {
		if (ch == 'R')
			com = CMD_REST;
		else {
			com = keys->tranch(ch);
			plus = fileio->getplus();
			if (plus != clsFILE::PT_EQUAL) {
				if (plus == clsFILE::PT_NONE)
					com += keys->getplus(com);
				else if (plus == clsFILE::PT_PLUS)
					com ++;
				else if (plus == clsFILE::PT_MINUS)
					com --;
			}
			com += nowOct * OCT_NUM;
		}
		fileio->writemu(getlength());
		fileio->writemu(com);
	}
	// 小節の終わり（チェックあり）
	else if (ch == '|')
		keys->slencheck();
	// 小節の終わり（チェックなし）
	else if (ch == ':')
		keys->slencheck(1);
	// デフォルト長の設定（付点は指定不可）
	else if (ch == 'L') {
		if ((num1 = fileio->getnum()) == NO_NUM)
			fileio->asm_error(ERR_NOTNUM);
		deflen = num1;
	}
	// ボリューム設定コマンド（増減可能）
	else if (ch == 'V') {
		if ((ch = fileio->getplus()) != clsFILE::PT_NONE) {
			if (ch == clsFILE::PT_PLUS)
				nowVol ++;
			else if (ch == clsFILE::PT_MINUS)
				nowVol --;
			else
				fileio->asm_error(ERR_VOLERR);
		}
		else {
			if ((nowVol = fileio->getnum()) == NO_NUM)
				fileio->asm_error(ERR_NOTNUM);
		}
		fileio->writemu((char)nowVol);
		fileio->writemu(CMD_VOL);
	}
	// オクターブ設定コマンド
	else if (ch == 'O') {
		if ((nowOct = fileio->getnum()) == NO_NUM)
			fileio->asm_error(ERR_NOTNUM);
	}
	// オクターブ増減（＋）
	else if (ch == '>') {
		nowOct ++;
		if (nowOct > OCT_MAX)
			fileio->asm_warning(WAR_OCTMAX);
	}
	// オクターブ増減（－）
	else if (ch == '<') {
		nowOct --;
		if (nowOct < 0)
			fileio->asm_warning(WAR_OCTMIN);
	}
	// 減衰設定
	else if (ch == 'W') {
		if (fileio->getd() != '(')
			fileio->asm_error(ERR_NOTNUM2);
		if ((num1 = fileio->getnum()) == NO_NUM)
			fileio->asm_error(ERR_NOTNUM2);
		fileio->writemu(num1);
		fileio->writemu(CMD_DIVST);
		if ((num1 = fileio->getnum()) == NO_NUM)
			fileio->asm_error(ERR_NOTNUM2);
		fileio->writemu(num1);
		fileio->writemu(CMD_DIVWT);
		if (fileio->getd() != ')')
			fileio->asm_error(ERR_NOTNUM2);

	}
	// オフセット設定コマンド
	else if (ch == 'K') {
		writenum();
		fileio->writemu(CMD_KEY);
	}
	// 音色タイプ設定
	else if (ch == '@') {
		writenum();
		fileio->writemu(CMD_TYPE);
	}
	// 移調コマンド
	else if (ch == 'I') {
		keys->asmbl_key(0);
	}
	// 拍の変更
	else if (ch == 'H') {
		keys->asmbl_beat(0);
	}
	// テンポの変更
	else if (ch == 'T') {
		keys->asmbl_tempo(0);
	}
	// 演奏終了
	else if (ch == 'Q') {
		fileio->writemu(0);
		fileio->writemu(CMD_END);
	}
	// LED点灯
	else if (ch == 'P') {
		writenum();
		fileio->writemuw(CMD_LED_ON);
	}
	// LED消灯
	else if (ch == 'F') {
		writenum();
		fileio->writemuw(CMD_LED_OFF);
	}
	// オクターブの確認
	else if (ch == '%') {
		if (fileio->getnum() != nowOct) {
			fileio->asm_warning(WAR_OCTCHK);
		}
		fileio->getd();
	}
	// 未定義のコマンド
	else
		fileio->asm_error(ERR_NOTCOM);
}

/*
	サブパートミュージック展開型クラス ========================================
*/
class	clsSUBPT : public clsPART {
	protected:
		string		ptname;		//パート名
		streampos	ptstart;	//パート開始位置
		int			ptnum;		//パート開始行
	public:
		clsSUBPT(clsFILE* cfile, clsKEY* ckey)
									: clsPART(cfile, ckey) {
			ptname = "";
			ptstart = 0;
			ptnum = 0;
		}
		~clsSUBPT() {}
		void	preread();

		// アセンブル実行
		void	asmbl(string subpt) {
			// 対象のサブパート名を検索する
			if (ptname != subpt) {
				// サブパートが定義されていなかった
				if (next == 0)
					fileio->asm_error(ERR_SUBPTERR);
				// 自身が一致しない場合は次を調べる
				else
					dynamic_cast<clsSUBPT*>(next)->asmbl(subpt);
			// 自身が一致した場合は、サブパート部分を読み込んで展開する
			} else {
				// 現在の位置を保存
				int tmpnum;
				streampos nowtmp = fileio->getinaddr(&tmpnum);

				// サブパート部分に移動
				fileio->setinaddr(ptstart, ptnum);

				// 定義終了まで処理を継続する
				char ch = fileio->getd();
				while(ch != '}') {
					asmblpt(ch);
					ch = fileio->getd();
				}

				//元の位置に戻る
				fileio->setinaddr(nowtmp, tmpnum);
			}
		}
};

 /*
	事前読み込み（位置確認）
 */
void	clsSUBPT :: preread()
{
	char	ch;

	//宣言部分を読み込んで保存しておく
	ptname = fileio->getlabel();

	//開始位置を読み込み
	ptstart = fileio->getinaddr(&ptnum);
	//宣言が終わるまで読み混むが、簡易的にエラーは検知する
	while((ch = fileio->getd()) != '}') {
		if (ch == EOF || ch == '{' || ch == '#')
			fileio->asm_error(ERR_SUBPTOPEN);
	}
}

/*
	チャンネルミュージック展開型クラス ========================================
*/
class	clsCHAN	: public clsPART {
	protected:
		clsSUBPT*	subpt;	// サブパート管理
	public:
		clsCHAN(clsFILE* cfile, clsKEY* ckey, clsSUBPT* csubpt)
										: clsPART(cfile, ckey) {
			subpt = csubpt;
		}
		~clsCHAN() { }
		void	asmbl(int depth = 0);
};

/*
	アセンブル実行
*/
void	clsCHAN :: asmbl(int depth)
{
	// 出力開始位置を保存
	streampos postmp = fileio->getoutaddr();
	int startlen = keys->slenget();
	// トップレベルの場合、サイズを仮記入しておく
	if (depth == 0) {
		fileio->writemuw(0);
	}
	// チャンネル定義またはループが閉じられるまで処理を継続
	char ch = fileio->getd();
	while(ch != '}' && ch != ']') {
		// サブパート指定を検知した場合はサブパート処理を開始
		if (ch == '$') {
			if (fileio->getd() != '(') {
				fileio->asm_error(ERR_SUBPTNONAME);
			} else {
				string ptname = fileio->getlabel();
				if (subpt == NULL)
					fileio->asm_error(ERR_SUBPTERR);
				else
					subpt->asmbl(ptname);
			}
		// ループ開始を検知した場合は再起呼び出し
		} else if (ch == '[') {
			asmbl(depth + 1);
		// その他の場合は一文字処理
		} else {
			asmblpt(ch);
		}
		ch = fileio->getd();
	}

	//トップレベルの場合、サイズを記入する
	if (depth == 0) {
		int size = (int)(fileio->getoutaddr()) - postmp - 2;
		fileio->writemuw_at(postmp, size);
	//トップレベルではない場合は、ループの処理を行う
	} else {
		// ループが閉じられていない
		if (ch == '}')
			fileio->asm_error(ERR_LOOPOPEN);
		int loopnum = fileio->getnum();
		// ループ回数の指定がない
		if (loopnum == NO_NUM || loopnum == 0)
			fileio->asm_error(ERR_LOOPNONUM);
		// ループの指定回数分コピーする（1回分は出力済みなので -1）
		fileio->outcopy(postmp, loopnum - 1);
		// 小節内のループに対応するため長さを加算（小節が閉じられていないと矛盾）
		keys->slenadd((keys->slenget() - startlen) * (loopnum - 1));
	}
}

/*
	音楽パラメータ管理型クラス ================================================
*/
class	clsMUSIC	{
	protected:
		clsFILE*	fileio;		// ファイルIO管理
		clsCHAN*	s_chan;		// チャンネル保存
		clsSUBPT*	s_subpt;	// サブパート保存
		clsKEY*		keys;		// キー情報保持
	public:
		clsMUSIC(clsFILE* cfile) {
			fileio = cfile;
			s_chan = NULL;
			s_subpt = NULL;
			keys = new clsKEY(fileio);
		}
		~clsMUSIC() {
			if (s_chan != NULL) delete s_chan;
			if (s_subpt != NULL) delete s_subpt;
			if (keys != NULL) delete keys;
			s_chan = NULL;
			s_subpt = NULL;
			keys = NULL;
		}
		void	asmbl(char ch);
		void	run();
};

/*
	アセンブル実行
*/
void	clsMUSIC :: asmbl(char ch)
{
	// テンポ指定
	if (ch == 'T') {
		keys->asmbl_tempo();
	}
	// キー設定
	else if (ch == 'K' || ch == 'I') {
		keys->asmbl_key();
	}
	// 拍の設定
	else if (ch == 'B' || ch == 'H') {
		keys->asmbl_beat();
	}
	// サブパート
	else if (ch == '$') {
		// サブパートクラスを生成し、位置を保持しておく
		clsSUBPT* subpt = new clsSUBPT(fileio, keys);
		subpt->preread();
		if (s_subpt == NULL)	s_subpt = subpt;
		else					s_subpt->addlink(subpt);
	}
	// チャンネル
	else if (ch == '#') {
		// チャンネル定義を確認し、処理を開始する
		if (fileio->getd() != '{')
			fileio->asm_error(ERR_NOTDEF);
		// 小節の長さを初期化
		keys->sleninit();
		clsCHAN* chan = new clsCHAN(fileio, keys, s_subpt);
		chan->asmbl();
		if (s_chan == NULL) s_chan = chan;
		else				s_chan->addlink(chan);
	}
	// 未定義のコマンド
	else
		fileio->asm_error(ERR_STATUS);
}

/*
	アセンブルを呼び出す
*/
void	clsMUSIC :: run()
{
	char	ch;

	// ヘッダ出力('MMLD')
	fileio->writemu('M');
	fileio->writemu('M');
	fileio->writemu('L');
	fileio->writemu('D');
	// サイズ（仮）
	fileio->writemuw(0);
	fileio->writemuw(0);
	// チャンネル数出力（仮）
	fileio->writemu(0);

	// ファイルの終了まで処理を行う
	ch = fileio->getd();
	while (ch != EOF) {
		asmbl(ch);
		ch = fileio->getd();
	};

	// サイズを書き込む
	int size = (int)(fileio->getoutaddr()) - 8;
	fileio->writemuw_at(4, size);
	// チャンネル数を書き込む
	ch = s_chan->getnum();
	fileio->writemu_at(8, ch);
}

/*
	メイン ====================================================================
*/
int	main(int argc, char* argv[])
{
	clsFILE*	fileio;	//ファイルIO管理
	clsMUSIC*	music;	//音楽情報管理

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
		music = new clsMUSIC(fileio);
		music->run();
		delete music;
		delete fileio;
		commonfunc::complete(filename);
	}
	return 0;
}

