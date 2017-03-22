/*
 * MML2BIN.cpp
 *
 * �T�v�F
 *  MML�ŋL�q���ꂽ�y�������A�Z���u�����A�o�C�i���`���ŏo�͂���c�[��
 *
 * �g�p���@���F
 *  �\�[�X�R�[�h�� Eclipse ���ŃR���p�C������B
 *  �R���p�C����A������ MML �t�@�C�������w�肵�Ď��s����B
 *  �������ꂽ�t�@�C���� EEPROM ���ɏ������ނɂ� HEX �t�@�C���ւ̕ϊ����K�v�B
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/02/17 PC-98����� C++�R�[�h�𐮗����AATTiny85 �̃\�t�g�Ɠ������Č��J
 *  2017/02/18 ����ɃR�[�h�𐮗����A�T�u�p�[�g��`�ƃ��[�v�̋@�\��ǉ�
 *  2017/02/19 ���A�e���|�A�L�[��r���ŕύX����@�\��ǉ��A���t�_�����ɑΉ�
 *  2017/03/11 GUI����̌Ăяo���ɑΉ����邽�ߏo�͕��@��ύX
 *
 */

#include <iostream>
#include <fstream>

using namespace std;

// �萔�錾 ===================================================================

#define		NO_NUM	10000		// "�����ȊO"��\������l

#define		OCT_MAX		4		// �I�N�^�[�u�̍ő�l
#define		OCT_NUM		12		// 1�I�N�^�[�u�̉������i�ύX�s�v�j
#define		SUB_MAX		1024	// ���[�v���̃R�s�[�o�b�t�@

// �G���[�E�x�����b�Z�[�W��`
#define		ERR_STATUS		"���݂��Ȃ��R�}���h�Ŏn�܂��Ă��܂�."
#define		ERR_NOTNUM		"�����Ŏw�肵�ĉ�����."
#define		ERR_NOTNUM2		"�J�b�R����2�̐������w�肵�ĉ�����."
#define		ERR_NOTCOM		"�R�}���h���Ⴂ�܂�."
#define		ERR_NOTDEF		"�錾���������n�܂��Ă��܂���."
#define		ERR_NUMOF		"�������傫�����܂�."
#define		ERR_NOTKEY		"�L�[������������܂���."
#define		ERR_NOTLEN		"����/�����Ƃ����悤�Ɏw�肵�ĉ�����."
#define		ERR_SUBPTOPEN	"�T�u�p�[�g�������Ă��܂���."
#define		ERR_SUBPTNONAME	"�T�u�p�[�g�����J�b�R���Ɏw�肵�Ă�������."
#define		ERR_SUBPTERR	"�T�u�p�[�g�������݂��܂���."
#define		ERR_SUBNOLABEL	"�T�u�p�[�g�����w�肳��Ă��܂���."
#define		ERR_LOOPOPEN	"[]���[�v�������Ă��܂���."
#define		ERR_LOOPNONUM	"[]���[�v�̉񐔂��w�肳��Ă��܂���."
#define		ERR_LOOPLONG	"[]���[�v���̃f�[�^���������܂�."
#define		ERR_VOLERR		"V�R�}���h�̌�� = �͎w��ł��܂���."
#define		ERR_LENGTH		"�����̒������������܂�. �x�����Œ��߂��Ă�������."

#define		WAR_NOTLEN		"�P���߂̒���������������܂���."
#define		WAR_OCTCHK		"�I�N�^�[�u�̃`�F�b�N�ɍ��ق�����܂�."
#define		WAR_OCTMAX		"�I�N�^�[�u��5�ȏ�ɂȂ��Ă��܂�."
#define		WAR_OCTMIN		"�I�N�^�[�u��0�����ɂȂ��Ă��܂�."

// MML���̃R�}���h��`
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
	���ʏ���===================================================================
*/
namespace commonfunc {
	// �^�C�g��
	void	title()
	{
		cerr << "MML2BIN.EXE ATTiny85 MusicBox�pMML �A�Z���u���v���O���� Ver 1.00 (C)Hiro OTSUKA" << endl;
	}
	// �g������\��
	void	usage()
	{
		cerr << "  Usage: MML2BIN.EXE filename" << endl;
		cerr << "    MML �ŏ����ꂽ���y���A�Z���u�����A.BIN �t�@�C�����쐬���܂�." << endl;
		cerr << "    filename �ɂ́A�X�y�[�X��؂�ŕ����̃t�@�C�����w��ł��܂�." << endl;
		exit(1);
	}
	// �o�͊�����\��
	void	complete(string filename)
	{
		cout << "  " << filename << ".BIN �t�@�C�����o�͂��܂���." << endl;
	}
	// �v���I�G���[��\��
	void	error(string txt )
	{
		cout << txt << endl;
		exit(2);
	}
}

/*
	�t�@�C���Ǘ��p�N���X ======================================================
*/
class	clsFILE	{
	protected:
		int			linenum;	// ���ݏ������̃��C���i���o
		char		oldch;		// �ꎞ�ۑ��p����
		char		lastch;		// �G���[�\���p����
		string		inname;		// �ǂݍ��݃t�@�C����
		string		outmuname;	// �����o���t�@�C����
		ifstream	in;			// �ǂݍ��݃t�@�C��
		fstream		outmu;		// �����o���t�@�C���i�ǂݍ��݂��s���j
		enum chtype : int {	// �����R�[�h���
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
		enum ptype : int {		// �v���X�}�C�i�X���
			PT_PLUS, PT_MINUS, PT_EQUAL, PT_NONE
		};

		// �v���X�}�C�i�X�̎w���ǂݍ���
		int		getplus() {
			lastch = getd();
			if (lastch == '+') return PT_PLUS;
			if (lastch == '-') return PT_MINUS;
			if (lastch == '=') return PT_EQUAL;
			oldch = lastch;
			return PT_NONE;
		}
		// �t�_���ǂ����𒲂ׂ�
		bool	isfuten() {
			if ('.' == (lastch = getd())) return true;
			oldch = lastch;
			return false;
		}
		// �ׂ��i�^�C�j���ǂ����𒲂ׂ�
		bool	isbeki() {
			lastch = getd();
			if (lastch == '^') return true;
			oldch = lastch;
			return false;
		}
		// �����o���t�@�C���ɂP�o�C�g�����o��
		void	writemu(char ch) {
			outmu.write((const char*)&ch, sizeof(char));
		}
		// �����o���t�@�C���ɂP���[�h�����o��
		void	writemuw(int data) {
			char	ch;
			ch = data & 0xFF;
			writemu(ch);
			ch = (data >> 8) & 0xFF;
			writemu(ch);
		}
		// �G���[�o�͂��ďI������
		void	asm_error(string txt) {
			if (oldch != 0) {
				// �ꎞ�ۑ�����������ꍇ
				cout << "  <" << inname << ">line " << linenum << "[" << oldch << "]: " << txt << endl;
			} else if (lastch != 0) {
				// �ǂݍ��ݕ���������ꍇ
				cout << "  <" << inname << ">line " << linenum << "[" << lastch << "]: " << txt << endl;
			} else {
				cout << "  <" << inname << ">line " << linenum << " : " << txt << endl;
			}
			// �o�̓t�@�C������A�폜���ďI��
			outmu.close();
			remove(outmuname.c_str());
			exit(2);
		}
		// �x���o�͂��Čp������
		void	asm_warning(string txt) {
			cout << "  �x��<" << inname << ">line " << linenum << " : " << txt << endl;
		}
		void	asm_warning(string txt, int slen, int nlen) {
			cout << "  �x��<" << inname << ">line " << linenum << " : " << txt << "(" << nlen << "/" << slen << ")" << endl;
		}
		// �����o���t�@�C���̈ʒu��Ԃ�
		streampos	getoutaddr() {
			return outmu.tellp();
		}
		// �ǂݍ��݃t�@�C���̈ʒu�ƍs�ԍ���Ԃ�
		streampos	getinaddr(int* retnum) {
			*retnum = linenum;
			return in.tellg();
		}
		// �ǂݍ��݃t�@�C���̈ʒu�ƍs�ԍ���ݒ肷��
		void	setinaddr(streampos addr, int setnum) {
			linenum = setnum;
			in.seekg(addr);
		}
		// �����o���t�@�C���̎w�肳�ꂽ�ʒu�ɂP�o�C�g��������
		void	writemu_at(streampos addr, char ch) {
			streampos tmp = outmu.tellp();
			outmu.seekp(addr);
			writemu(ch);
			outmu.seekp(tmp);
		}
		// �����o���t�@�C���̎w�肳�ꂽ�ʒu�ɂP���[�h��������
		void	writemuw_at(streampos addr, int data) {
			streampos tmp = outmu.tellp();
			outmu.seekp(addr);
			writemuw(data);
			outmu.seekp(tmp);
		}
};

/*
	�ǂ񂾕������������ׂ�
*/
int		clsFILE :: ch_check(char ch)
{
	// �A���t�@�x�b�g
	if ('A' <= ch && ch <= 'Z') return CH_ALPH;
	// ����
	if ('0' <= ch && ch <= '9') return CH_NUM;
	// �L���ȋL��
	if (string(":|/^+-=.'\%@#$[]{}<>()~").find(ch) != string::npos) return CH_KIGOU;

	return CH_ETC;
}

/*
	�P�����ǂ�(SPC �� TAB �͔�΂�)
*/
char	clsFILE :: getd(int check)
{
	int		done;
	char	ch;

	// �L���Ȑ���������܂œǂݍ��݂𑱂���
	do {
		done = 0;
		// �ꎞ�ۑ��������������ꍇ�͂����Ԃ�
		if (oldch != 0) {
			ch = oldch;
			oldch = 0;
		}
		// �Ȃ��ꍇ��1�����ǂݍ���
		else
			in.read(&ch, 1);

		// �R�����g�J�n�̏ꍇ�͍s���܂œǂݎ̂Ă�
		if (ch == ';') {
			do {
				in.read(&ch, 1);
			} while(ch != '\n');
		}
		// ���s�̏ꍇ�͍s�������Z���Ă���
		if (ch == '\n') linenum ++;
		// �����ȕ����̏ꍇ�͑����ď�������
		if (ch == ' ' || ch == '\t' || ch == '\r' || (ch=='\n' && check==0))
			done = 1;
	} while(done == 1 && !in.eof());

	// �A���t�@�x�b�g�̏ꍇ�͑啶���ɕϊ�����
	if ('a' <= ch && ch <= 'z')
		ch = ch - 'a' + 'A';

	// �t�@�C���̏I�������o����
	else if (in.eof())
		ch = EOF;

	// �G���[�\���̂��ߕ�����ۑ����A�Ԃ�
	lastch = ch;
	return ch;
}

/*
	������ǂݍ���
*/
int		clsFILE :: getnum()
{
	int		nums = 0, kake = 1;

	// �ꕶ���ǂݍ���
	char ch = getd();

	// �}�C�i�X�����̊J�n
	if (ch == '-') {
		kake = -1;
		ch = getd();
	}
	// �����ł͂Ȃ��ꍇ
	if (ch_check(ch) != CH_NUM) {
		// �ꎞ�ۑ����āA�G���[��Ԃ�
		oldch = ch;
		return NO_NUM;
	}
	// ������ǂݑ�����
	do {
		nums = nums * 10 + (ch - '0');
		if (nums >= NO_NUM)
			asm_error(ERR_NUMOF);
		ch = getd(1);
	} while(ch_check(ch) == CH_NUM);

	// �s�v�ȕ����̏ꍇ�͓ǂݎ̂�
	if (ch_check(ch) == CH_ETC)
		oldch = 0;
	// �K�v�ȕ����̏ꍇ�͈ꎞ�ۑ�
	else
		oldch = ch;

	// �}�C�i�X���l�����ĕԂ�
	return nums * kake;
}

/*
	���x����ǂݍ���
*/
string	clsFILE :: getlabel()
{
	string	buff = "";

	// ���x���I���܂œǂݍ���
	char ch = getd();
	while(ch != '(' && ch != ')' && ch != '{' && ch != '}' && ch != '[' && ch != ']') {
		char	tmp[2];
		tmp[0] = ch;
		tmp[1] = '\0';
		buff = buff + string((const char*)tmp);
		ch = getd();
	}

	// ���x���������ꍇ�̓G���[
	if (buff.length() == 0)
		asm_error(ERR_SUBNOLABEL);

	return buff;
}

 /*
	�o�̓t�@�C���̈ꕔ���R�s�[����
 */
void clsFILE :: outcopy(streampos addr, int num)
{
	char	buff[SUB_MAX];

	// ���݂̏o�͈ʒu�𓾂�
	streampos nowpos = getoutaddr();
	// �R�s�[�J�n�ʒu����̃T�C�Y�𓾂�
	int	loopsize = nowpos - addr;

	// �T�C�Y�`�F�b�N
	if (loopsize >= SUB_MAX)
		asm_error(ERR_LOOPLONG);

	// �w��񐔕��R�s�[����
	while(num != 0) {
		outmu.seekp(addr);
		outmu.read(buff, loopsize);
		outmu.seekp(0, ios_base::end);
		outmu.write(buff, loopsize);
		num --;
	}
}

/*
	�L�[�f�t�H���g�Ǘ��N���X ==================================================
*/
class	clsKEY	{
	protected:
		clsFILE*	fileio;				// �t�@�C��IO�Ǘ�
		int			akeys[OCT_NUM+1];	// �L�[�̃f�t�H���g��ۑ��i�S�̏����ݒ�j
		int			alen;				// �ꏬ�߂̒����i�S�̏����ݒ�j
		int			atempo;				// �ꕪ�����̒����i�S�̏����ݒ�j
		int			keys[OCT_NUM+1];	// �L�[�̃f�t�H���g��ۑ��i�r���ύX����j
		int			slen;				// �ꏬ�߂̒����i�r���ύX����j
		int			tempo;				// �ꕪ�����̒����i�r���ύX����j
		int			nlen;				// ���݂̈ꏬ�߂̒���
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

		// �ꕪ�����̒�����Ԃ�
		int		getalllen() {
			return tempo;
		}
		// �ꏬ�߂̒���������������
		void	sleninit() {
			// �S�̐ݒ��K�p
			slen = alen;
			tempo = atempo;
			for (int tmp = 0; tmp < OCT_NUM; tmp ++) {
				keys[tmp] = akeys[tmp];
			}
			// ���̒����͏�����
			nlen = 0;
		}
		// �ꏬ�߂̒����ɉ��Z����
		void	slenadd(int len) {
			nlen += len;
		}
		int		slenget() {
			return nlen;
		}
		// �ꏬ�߂̒������`�F�b�N����i�G���[�o�͂������w����\�j
		void	slencheck(int noerr = 0) {
			if (nlen != slen && noerr == 0)
				fileio->asm_warning(WAR_NOTLEN, slen, nlen);
			nlen = 0;
		}
		// �L�[�̃f�t�H���g�𓾂�
		int		getplus(int key) {
			return keys[key];
		}
};

 /*
	������
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
	���������K�ɕϊ�
*/
int	clsKEY :: tranch(char ch)
{
	int	param[] = { 10, 12, 1, 3, 5, 6, 8 };

	// 'A' ����n�܂�
	ch -= 'A';

	// �͈͊O�̓G���[
	if (ch > 6 || ch < 0) fileio->asm_error(ERR_NOTKEY);

	return param[(int)ch];
}

 /*
 	 �L�[��ǂݍ���
 */
void	clsKEY :: asmbl_key(int isall)
{
	char	ch;
	int		tmp, keytmp;

	// ��`�J�n��������΃G���[
	if (fileio->getd() != '{')
		fileio->asm_error(ERR_NOTDEF);

	// ��`�I���܂œǂݍ���
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
	// �S�̐ݒ�̏ꍇ�͏����l��ۑ�
	if (isall) {
		for (tmp = 0; tmp < OCT_NUM; tmp ++) {
			akeys[tmp] = keys[tmp];
		}
	}
}

/*
	 ����ǂݍ���
*/
void	clsKEY :: asmbl_beat(int isall)
{
	int		tmp, tmp2;

	// ���q��ǂݍ���
	if (NO_NUM == (tmp = fileio->getnum()))
		fileio->asm_error(ERR_NOTLEN);
	// ���Z��������΃G���[
	if (fileio->getd() != '/')
		fileio->asm_error(ERR_NOTLEN);
	// �����ǂݍ���
	if (NO_NUM == (tmp2 = fileio->getnum()))
		fileio->asm_error(ERR_NOTLEN);
	// �ꕪ�����̒�������ꏬ�߂̒������Z�o����
	slen = (getalllen() / tmp2) * tmp;
	// �S�̐ݒ�̏ꍇ�͏����l��ۑ�
	if (isall) alen = slen;
}

/*
	 �e���|��ǂݍ���
*/
void	clsKEY :: asmbl_tempo(int isall)
{
	int		tmp;

	if (NO_NUM == (tmp = fileio->getnum()))
		fileio->asm_error(ERR_NOTLEN);
	slen = tempo = tmp;
	// �S�̐ݒ�̏ꍇ�͏����l��ۑ�
	if (isall) alen = atempo = tempo;
}

/*
	SML�W�J�^�N���X	===========================================================
*/
class	clsPART	{
	protected:
		clsFILE* 	fileio;		// �t�@�C��IO�Ǘ�
		clsKEY*		keys;		// �L�[�Ǘ�
		clsPART*	next;		// ���̃f�[�^
		int			nowOct;		// ���݂̃I�N�^�[�u
		int			nowVol;		// ���݂̃{�����[��
		int			deflen;		// ���݂̃f�t�H���g��

		int		getlength();

		// ���ɋL�q����Ă��鐔�������̂܂܏�������
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

		// ���̃f�[�^�ɒǉ�
		void	addlink(clsPART* tmp) {
			if (next == NULL) next = tmp;
			else next->addlink(tmp);
		}
		// �f�[�^���𓾂�
		int		getnum(int num = 0) {
			if (next == NULL) return num + 1;
			else return next->getnum(num + 1);
		}
};

/*
	�������擾
*/
int	clsPART :: getlength()
{
	int	com, tmp, minus = 0;

	// �����w��̏ꍇ�͂��̐������g�p���Ē������Z�o
	if ((com = fileio->getnum()) != NO_NUM)
		com = keys->getalllen() / com;
	// �����������ꍇ�̓f�t�H���g�����g�p
	else
		com = keys->getalllen() / deflen;
	// �t�_�����̏ꍇ
	if (fileio->isfuten()) {
		// ���t�_�̏ꍇ�� 1.75�{
		if (fileio->isfuten())
			com = com + com/2 + com/4;
		// �t�_�����̏ꍇ�� 1.5�{
		else
			com = com + com/2;
	}
	// �^�C�������ꍇ�͉��Z���p��
	while (fileio->isbeki()) {
		// �������擾
		tmp = fileio->getnum();
		// �����ł͂Ȃ��ꍇ
		if (tmp == NO_NUM) {
			// ����������Ă��邩�𒲂ׂ�
			tmp = fileio->getd();
			// ���߂̏I���̏ꍇ
			if (tmp == '|' || tmp == ':') {
				// �O���ߕ���ۑ����Ă���
				minus += com;
				// �`�F�b�N�������s��
				keys->slenadd(com);
				if (tmp == '|') keys->slencheck();
				else keys->slencheck(1);
				// ���̎��ɐ���������͂�
				if ((tmp = fileio->getnum()) == NO_NUM)
					fileio->asm_error(ERR_NOTNUM);
			}
			// ����ȊO�̓G���[
			else
				fileio->asm_error(ERR_NOTNUM);
		}
		// �������Z�o����
		tmp = keys->getalllen() / tmp;
		// �t�_�����̏ꍇ
		if (fileio->isfuten()) {
			// ���t�_�̏ꍇ�� 1.75�{
			if (fileio->isfuten())
				tmp = tmp + tmp/2 + tmp/4;
			// �t�_�����̏ꍇ�� 1.5�{
			else
				tmp = tmp + tmp/2;
		}
		// ���Z����
		com += tmp;
	}
	// ��������ꍇ�̓G���[�ɂ���
	if (com >= 256)
		fileio->asm_error(ERR_LENGTH);

	// �O���ߕ��������āA���߂̒����ɑ����Ă���
	keys->slenadd(com - minus);

	return com;
}

/*
	�A�Z���u���T�u
*/
void	clsPART :: asmblpt(char ch)
{
	int		com, plus, num1;

	// �������
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
	// ���߂̏I���i�`�F�b�N����j
	else if (ch == '|')
		keys->slencheck();
	// ���߂̏I���i�`�F�b�N�Ȃ��j
	else if (ch == ':')
		keys->slencheck(1);
	// �f�t�H���g���̐ݒ�i�t�_�͎w��s�j
	else if (ch == 'L') {
		if ((num1 = fileio->getnum()) == NO_NUM)
			fileio->asm_error(ERR_NOTNUM);
		deflen = num1;
	}
	// �{�����[���ݒ�R�}���h�i�����\�j
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
	// �I�N�^�[�u�ݒ�R�}���h
	else if (ch == 'O') {
		if ((nowOct = fileio->getnum()) == NO_NUM)
			fileio->asm_error(ERR_NOTNUM);
	}
	// �I�N�^�[�u�����i�{�j
	else if (ch == '>') {
		nowOct ++;
		if (nowOct > OCT_MAX)
			fileio->asm_warning(WAR_OCTMAX);
	}
	// �I�N�^�[�u�����i�|�j
	else if (ch == '<') {
		nowOct --;
		if (nowOct < 0)
			fileio->asm_warning(WAR_OCTMIN);
	}
	// �����ݒ�
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
	// �I�t�Z�b�g�ݒ�R�}���h
	else if (ch == 'K') {
		writenum();
		fileio->writemu(CMD_KEY);
	}
	// ���F�^�C�v�ݒ�
	else if (ch == '@') {
		writenum();
		fileio->writemu(CMD_TYPE);
	}
	// �ڒ��R�}���h
	else if (ch == 'I') {
		keys->asmbl_key(0);
	}
	// ���̕ύX
	else if (ch == 'H') {
		keys->asmbl_beat(0);
	}
	// �e���|�̕ύX
	else if (ch == 'T') {
		keys->asmbl_tempo(0);
	}
	// ���t�I��
	else if (ch == 'Q') {
		fileio->writemu(0);
		fileio->writemu(CMD_END);
	}
	// LED�_��
	else if (ch == 'P') {
		writenum();
		fileio->writemuw(CMD_LED_ON);
	}
	// LED����
	else if (ch == 'F') {
		writenum();
		fileio->writemuw(CMD_LED_OFF);
	}
	// �I�N�^�[�u�̊m�F
	else if (ch == '%') {
		if (fileio->getnum() != nowOct) {
			fileio->asm_warning(WAR_OCTCHK);
		}
		fileio->getd();
	}
	// ����`�̃R�}���h
	else
		fileio->asm_error(ERR_NOTCOM);
}

/*
	�T�u�p�[�g�~���[�W�b�N�W�J�^�N���X ========================================
*/
class	clsSUBPT : public clsPART {
	protected:
		string		ptname;		//�p�[�g��
		streampos	ptstart;	//�p�[�g�J�n�ʒu
		int			ptnum;		//�p�[�g�J�n�s
	public:
		clsSUBPT(clsFILE* cfile, clsKEY* ckey)
									: clsPART(cfile, ckey) {
			ptname = "";
			ptstart = 0;
			ptnum = 0;
		}
		~clsSUBPT() {}
		void	preread();

		// �A�Z���u�����s
		void	asmbl(string subpt) {
			// �Ώۂ̃T�u�p�[�g������������
			if (ptname != subpt) {
				// �T�u�p�[�g����`����Ă��Ȃ�����
				if (next == 0)
					fileio->asm_error(ERR_SUBPTERR);
				// ���g����v���Ȃ��ꍇ�͎��𒲂ׂ�
				else
					dynamic_cast<clsSUBPT*>(next)->asmbl(subpt);
			// ���g����v�����ꍇ�́A�T�u�p�[�g������ǂݍ���œW�J����
			} else {
				// ���݂̈ʒu��ۑ�
				int tmpnum;
				streampos nowtmp = fileio->getinaddr(&tmpnum);

				// �T�u�p�[�g�����Ɉړ�
				fileio->setinaddr(ptstart, ptnum);

				// ��`�I���܂ŏ������p������
				char ch = fileio->getd();
				while(ch != '}') {
					asmblpt(ch);
					ch = fileio->getd();
				}

				//���̈ʒu�ɖ߂�
				fileio->setinaddr(nowtmp, tmpnum);
			}
		}
};

 /*
	���O�ǂݍ��݁i�ʒu�m�F�j
 */
void	clsSUBPT :: preread()
{
	char	ch;

	//�錾������ǂݍ���ŕۑ����Ă���
	ptname = fileio->getlabel();

	//�J�n�ʒu��ǂݍ���
	ptstart = fileio->getinaddr(&ptnum);
	//�錾���I���܂œǂݍ��ނ��A�ȈՓI�ɃG���[�͌��m����
	while((ch = fileio->getd()) != '}') {
		if (ch == EOF || ch == '{' || ch == '#')
			fileio->asm_error(ERR_SUBPTOPEN);
	}
}

/*
	�`�����l���~���[�W�b�N�W�J�^�N���X ========================================
*/
class	clsCHAN	: public clsPART {
	protected:
		clsSUBPT*	subpt;	// �T�u�p�[�g�Ǘ�
	public:
		clsCHAN(clsFILE* cfile, clsKEY* ckey, clsSUBPT* csubpt)
										: clsPART(cfile, ckey) {
			subpt = csubpt;
		}
		~clsCHAN() { }
		void	asmbl(int depth = 0);
};

/*
	�A�Z���u�����s
*/
void	clsCHAN :: asmbl(int depth)
{
	// �o�͊J�n�ʒu��ۑ�
	streampos postmp = fileio->getoutaddr();
	int startlen = keys->slenget();
	// �g�b�v���x���̏ꍇ�A�T�C�Y�����L�����Ă���
	if (depth == 0) {
		fileio->writemuw(0);
	}
	// �`�����l����`�܂��̓��[�v��������܂ŏ������p��
	char ch = fileio->getd();
	while(ch != '}' && ch != ']') {
		// �T�u�p�[�g�w������m�����ꍇ�̓T�u�p�[�g�������J�n
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
		// ���[�v�J�n�����m�����ꍇ�͍ċN�Ăяo��
		} else if (ch == '[') {
			asmbl(depth + 1);
		// ���̑��̏ꍇ�͈ꕶ������
		} else {
			asmblpt(ch);
		}
		ch = fileio->getd();
	}

	//�g�b�v���x���̏ꍇ�A�T�C�Y���L������
	if (depth == 0) {
		int size = (int)(fileio->getoutaddr()) - postmp - 2;
		fileio->writemuw_at(postmp, size);
	//�g�b�v���x���ł͂Ȃ��ꍇ�́A���[�v�̏������s��
	} else {
		// ���[�v�������Ă��Ȃ�
		if (ch == '}')
			fileio->asm_error(ERR_LOOPOPEN);
		int loopnum = fileio->getnum();
		// ���[�v�񐔂̎w�肪�Ȃ�
		if (loopnum == NO_NUM || loopnum == 0)
			fileio->asm_error(ERR_LOOPNONUM);
		// ���[�v�̎w��񐔕��R�s�[����i1�񕪂͏o�͍ς݂Ȃ̂� -1�j
		fileio->outcopy(postmp, loopnum - 1);
		// ���ߓ��̃��[�v�ɑΉ����邽�ߒ��������Z�i���߂������Ă��Ȃ��Ɩ����j
		keys->slenadd((keys->slenget() - startlen) * (loopnum - 1));
	}
}

/*
	���y�p�����[�^�Ǘ��^�N���X ================================================
*/
class	clsMUSIC	{
	protected:
		clsFILE*	fileio;		// �t�@�C��IO�Ǘ�
		clsCHAN*	s_chan;		// �`�����l���ۑ�
		clsSUBPT*	s_subpt;	// �T�u�p�[�g�ۑ�
		clsKEY*		keys;		// �L�[���ێ�
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
	�A�Z���u�����s
*/
void	clsMUSIC :: asmbl(char ch)
{
	// �e���|�w��
	if (ch == 'T') {
		keys->asmbl_tempo();
	}
	// �L�[�ݒ�
	else if (ch == 'K' || ch == 'I') {
		keys->asmbl_key();
	}
	// ���̐ݒ�
	else if (ch == 'B' || ch == 'H') {
		keys->asmbl_beat();
	}
	// �T�u�p�[�g
	else if (ch == '$') {
		// �T�u�p�[�g�N���X�𐶐����A�ʒu��ێ����Ă���
		clsSUBPT* subpt = new clsSUBPT(fileio, keys);
		subpt->preread();
		if (s_subpt == NULL)	s_subpt = subpt;
		else					s_subpt->addlink(subpt);
	}
	// �`�����l��
	else if (ch == '#') {
		// �`�����l����`���m�F���A�������J�n����
		if (fileio->getd() != '{')
			fileio->asm_error(ERR_NOTDEF);
		// ���߂̒�����������
		keys->sleninit();
		clsCHAN* chan = new clsCHAN(fileio, keys, s_subpt);
		chan->asmbl();
		if (s_chan == NULL) s_chan = chan;
		else				s_chan->addlink(chan);
	}
	// ����`�̃R�}���h
	else
		fileio->asm_error(ERR_STATUS);
}

/*
	�A�Z���u�����Ăяo��
*/
void	clsMUSIC :: run()
{
	char	ch;

	// �w�b�_�o��('MMLD')
	fileio->writemu('M');
	fileio->writemu('M');
	fileio->writemu('L');
	fileio->writemu('D');
	// �T�C�Y�i���j
	fileio->writemuw(0);
	fileio->writemuw(0);
	// �`�����l�����o�́i���j
	fileio->writemu(0);

	// �t�@�C���̏I���܂ŏ������s��
	ch = fileio->getd();
	while (ch != EOF) {
		asmbl(ch);
		ch = fileio->getd();
	};

	// �T�C�Y����������
	int size = (int)(fileio->getoutaddr()) - 8;
	fileio->writemuw_at(4, size);
	// �`�����l��������������
	ch = s_chan->getnum();
	fileio->writemu_at(8, ch);
}

/*
	���C�� ====================================================================
*/
int	main(int argc, char* argv[])
{
	clsFILE*	fileio;	//�t�@�C��IO�Ǘ�
	clsMUSIC*	music;	//���y���Ǘ�

	// �^�C�g���\�L
	commonfunc::title();

	// �����������ꍇ�͎g�p���@�\�����ďI��
	if (argc == 1)
		commonfunc::usage();

	// �t�@�C�����Ƃɏ��������s
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

