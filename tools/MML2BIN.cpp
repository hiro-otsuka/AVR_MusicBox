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
 *  2017/02/17 PC-98����� C++�R�[�h���ꕔ���C���AATTiny85 �̃\�t�g�Ɠ������Č��J
 *
 */

#include <iostream>
#include <fstream>

using namespace std;

// �萔�錾
#define		TRUE	-1
#define		FALSE	0

#define		NO_NUM	10000

#define		OCT_MAX		12

#define		ERR_STATUS		"���݂��Ȃ��R�}���h�Ŏn�܂��Ă��܂�."
#define		ERR_NOTNUM		"�����Ŏw�肵�ĉ�����."
#define		ERR_NOTNUM2		"�J�b�R����2�̐������w�肵�ĉ�����."
#define		ERR_NOTCOM		"�R�}���h���Ⴂ�܂�."
#define		ERR_NOTDEF		"�錾���������n�܂��Ă��܂���."
#define		ERR_NUMOF		"�������傫�����܂�."
#define		ERR_NOTKEY		"�L�[������������܂���."
#define		ERR_NOTLEN		"����/�����Ƃ����悤�Ɏw�肵�ĉ�����."

#define		WAR_LENGTH		"�����̒������������܂�. �^�C�łȂ��ĉ�����."
#define		WAR_NOTLEN		"�P���߂̒���������������܂���."
#define		WAR_OCTCHK		"�I�N�^�[�u�̃`�F�b�N�ɍ��ق�����܂�"

#define		CH_ETC		0
#define		CH_KIGOU	1
#define		CH_NUM		2
#define		CH_ALPH		3

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
	�G���[�I��
*/
namespace commonfunc {
	void	usage()
	{
		cerr << "  Usage: MML2BIN.EXE filename" << endl;
		cerr << "    MML �ŏ����ꂽ���y���A�Z���u�����A.BIN �t�@�C�����쐬���܂�." << endl;
		exit(1);
	}
	void	error(string txt )
	{
		cout << txt << endl;
		exit(2);
	}
}

/*
	�t�@�C���Ǘ��p�N���X	---------------------------------------------
*/
class	clsFILE	{
	protected:
		int		linenum;		//���ݏ������̃��C���i���o
		ifstream in;
		ofstream outmu;
		char	oldch;			//��ǂݗp
		char	lastch;			//�G���[�\���p
		string	outmuname;
		string	inname;
	public:
		clsFILE(string name);
		~clsFILE() {
			if (in.is_open()) in.close();
			if (outmu.is_open()) outmu.close();
		}
		char	getd(int check = 0);
		int		getnum();
		int		ch_check(char ch);
		int		getfuten() {
			if ('.' == (lastch = getd())) return TRUE;
			oldch = lastch;
			return FALSE;
		}
		int		getplus() {
			lastch = getd();
			if (lastch == '+') return 1;
			if (lastch == '-') return -1;
			if (lastch == '=') return NO_NUM;
			oldch = lastch;
			return 0;
		}
		int		getbeki() {
			lastch = getd();
			if (lastch == '^') return TRUE;
			oldch = lastch;
			return FALSE;
		}
		void	writemu(char ch) {
			outmu.write((const char*)&ch, sizeof(char));
		}
		void	writemuw(int data) {
			char	ch;
			ch = data & 0xFF;
			writemu(ch);
			ch = (data >> 8) & 0xFF;
			writemu(ch);
		}
		void	asm_error(string txt) {
			if (oldch != 0) {
				cout << "  <" << inname << ">line " << linenum << "[" << oldch << "]: " << txt << endl;
			} else if (lastch != 0) {
				cout << "  <" << inname << ">line " << linenum << "[" << lastch << "]: " << txt << endl;
			} else {
				cout << "  <" << inname << ">line " << linenum << " : " << txt << endl;
			}
			outmu.close();
			remove(outmuname.c_str());
			exit(2);
		}
		void	asm_warning(string txt) {
			cout << "  �x��<" << inname << ">line " << linenum << " : " << txt << endl;
		}
		streampos	getaddr() {
			return outmu.tellp();
		}
		void	writemu_at(streampos addr, char ch) {
			streampos tmp = outmu.tellp();
			outmu.seekp(addr);
			writemu(ch);
			outmu.seekp(tmp);
		}
		void	writemuw_at(streampos addr, int data) {
			streampos tmp = outmu.tellp();
			outmu.seekp(addr);
			writemuw(data);
			outmu.seekp(tmp);
		}
};

clsFILE :: clsFILE(string name)
{
	linenum = 1;
	oldch = lastch = 0;

	inname = name + ".MML";
	outmuname = name + ".BIN";
	in.open(inname, ifstream::in | ifstream::binary);
	if (in.fail())
		commonfunc::error("File [" + inname + "] can not open.");
	outmu.open(outmuname, ofstream::out | ofstream::binary);
	if (outmu.fail())
		commonfunc::error("File [" + outmuname + "] can not open.");
}

/*
	�ǂ񂾕������������ׂ�
*/
int		clsFILE :: ch_check(char ch)
{
	if ('A' <= ch && ch <= 'Z') return CH_ALPH;
	if ('0' <= ch && ch <= '9') return CH_NUM;
	if (string("*|\"/^=.'!&:@#$[]{}<>(~").find(ch) != string::npos) return CH_KIGOU;

	return CH_ETC;
}

/*
	�P�����ǂ�(SPC �� TAB �͔�΂�)
*/
char	clsFILE :: getd(int check)
{
	int		done;
	char	ch;

	do {
		done = 0;
		if (oldch != 0) {
			ch = oldch;
			oldch = 0;
		}
		else
			in.read(&ch, 1);
		if (ch == ';') {
			do {
				in.read(&ch, 1);
			} while(ch != '\n');
		}
		if (ch == '\n') linenum ++;
		if (ch == ' ' || ch == '\t' || ch == '\r' || (ch=='\n' && check==0))
			done = 1;
	} while(done == 1 && !in.eof());

	if ('a' <= ch && ch <= 'z')
		ch = ch - 'a' + 'A';
	else if (in.eof())
		ch = EOF;

	lastch = ch;
	return ch;
}

/*
	������ǂݍ���
*/
int		clsFILE :: getnum()
{
	int		nums = 0, kake = 1;
	char	ch;

	ch = getd();
	if (ch == '-') {
		kake = -1;
		ch = getd();
	}
	if (ch_check(ch) != CH_NUM) {
		oldch = ch;
		return NO_NUM;
	}
	do {
		nums = nums * 10 + (ch - '0');
		if (nums > 30000)
			asm_error(ERR_NUMOF);
		ch = getd(1);
	} while(ch_check(ch) == CH_NUM);

	if (ch_check(ch) == CH_ETC)
		oldch = 0;
	else
		oldch = ch;

	return nums * kake;
}

/*
	�L�[�f�t�H���g�ݒ�Ǎ���
*/
class	clsKEY	{
	protected:
		int			keys[OCT_MAX+1];
		int			slen, nlen;
		int			tempo;
		clsFILE*	fileio;
	public:
		clsKEY(clsFILE* tmp) {
			fileio = tmp;
			init();
			slen = tempo = 144;
		}
		int	tranch(char ch);
		void	asmbl_key();
		void	asmbl_beat();
		void	asmbl_tempo();
		int		getalllen() {
			return tempo;
		}
		void	slenadd(int len) {
			nlen += len;
		}
		void	slencheck(int noerr = 0) {
			if (nlen != slen && noerr == 0)
				fileio->asm_warning(WAR_NOTLEN);
			nlen = 0;
		}
		void	sleninit() {
			nlen = 0;
		}
		int		getplus(int key) {
			return keys[key];
		}
		void	init();
};

void	clsKEY :: init()
{
	int i;

	for (i = 0; i < OCT_MAX+1; i ++)
		keys[i] = 0;
	sleninit();
}

/*
	���������K�ɕϊ�
*/
int	clsKEY :: tranch(char ch)
{
	int	param[] = {
		10, 12, 1, 3, 5, 6, 8
	};

	ch -= 'A';
	if (ch > 6 || ch < 0) fileio->asm_error(ERR_NOTKEY);
	return param[(int)ch];
}

void	clsKEY :: asmbl_key()
{
	char	ch;
	int		tmp, keytmp;

	if (fileio->getd() != '{')
		fileio->asm_error(ERR_NOTDEF);
	ch = fileio->getd();
	while(ch != '}') {
		tmp = tranch(ch);
		if (NO_NUM != (keytmp = fileio->getplus()))
			keys[tmp] = keytmp;
		ch = fileio->getd();
	}
}

void	clsKEY :: asmbl_beat()
{
	int		tmp, tmp2;

	if (NO_NUM == (tmp = fileio->getnum()))
		fileio->asm_error(ERR_NOTLEN);
	if (fileio->getd() != '/')
		fileio->asm_error(ERR_NOTLEN);
	if (NO_NUM == (tmp2 = fileio->getnum()))
		fileio->asm_error(ERR_NOTLEN);
	tmp2 = getalllen() / tmp2;
	slen = tmp2 * tmp;
}

void	clsKEY :: asmbl_tempo()
{
	int		tmp;

	if (NO_NUM == (tmp = fileio->getnum()))
		fileio->asm_error(ERR_NOTLEN);
	slen = tempo = tmp;
}

/*
	SML�W�J�^�N���X	-----------------------------------------------------
*/
class	clsPART	{
	protected:
		int			nowOct;
		int			nowVol;

		int			len;
		clsFILE* 	fileio;
		clsKEY*		keys;

		void	get2num(int* num1, int* num2) {
			if (fileio->getd() != '(' ||
							(*num1 = fileio->getnum()) == NO_NUM ||
							(*num2 = fileio->getnum()) == NO_NUM)
				fileio->asm_error(ERR_NOTNUM);
		}
		void	writenum(int w = 0) {
			int	com;

			if ((com = fileio->getnum()) == NO_NUM)
				fileio->asm_error(ERR_NOTNUM);
			if (w)
				fileio->writemuw(com);
			else
				fileio->writemu(com);
		}
		void	writenum2(int w = 0) {
			int	com;

			if ((com = fileio->getnum()) == NO_NUM)
				com = 0;
			if (w)
				fileio->writemuw(com);
			else
				fileio->writemu(com);
		}
		int	getlength();
		unsigned int getsyuhasub(int num);
		int	getsyuha();
	public:
		clsPART(clsFILE* tmp, clsKEY* tmp3);
		virtual ~clsPART() {};
		virtual void	asmblpt(char ch);
};

clsPART :: clsPART(clsFILE* tmp, clsKEY* tmp3)
{
	fileio = tmp;
	keys = tmp3;
	nowOct = 0;
	nowVol = 0;
	len = keys->getalllen() / 4;
}

/*
	�������擾
*/
int	clsPART :: getlength()
{
	int	com, tmp, minus = 0;

	if ((com = fileio->getnum()) != NO_NUM)
		com = keys->getalllen() / com;
	else
		com = len;
	if (fileio->getfuten())
		com = com + com/2;
	while (fileio->getbeki()) {
		tmp = fileio->getnum();
		if (tmp == NO_NUM) {
			tmp = fileio->getd();
			if (tmp == '|' || tmp == ':') {
				minus += com;
				keys->slenadd(com);
				if (tmp == '|') keys->slencheck();
				else keys->slencheck(1);
				tmp = fileio->getnum();
			}
			else
				fileio->asm_error(ERR_NOTNUM);
		}
		tmp = keys->getalllen() / tmp;
		if (fileio->getfuten())
			tmp = tmp + tmp/2;
		com += tmp;
	}
	if (com >= 256)
		fileio->asm_warning(WAR_LENGTH);
	keys->slenadd(com - minus);

	return com;
}

/*
	�A�Z���u���T�u
*/
void	clsPART :: asmblpt(char ch)
{
	int		com, plus, num1;

	if (('A' <= ch && ch <= 'G') || ch == 'R') {
		if (ch == 'R')
			com = CMD_REST;
		else {
			com = keys->tranch(ch);
			plus = fileio->getplus();
			if (plus != NO_NUM) {
				if (plus == 0)
					com += keys->getplus(com);
				else
					com += plus;
			}
			com += nowOct * OCT_MAX;
		}
		fileio->writemu(getlength());
		fileio->writemu(com);
	}
	//���߂̏I���i�`�F�b�N����j
	else if (ch == '|')
		keys->slencheck();
	//���߂̏I���i�`�F�b�N�Ȃ��j
	else if (ch == ':')
		keys->slencheck(1);
	//�f�t�H���g���̐ݒ�
	else if (ch == 'L') {
		if ((num1 = fileio->getnum()) == NO_NUM)
			fileio->asm_error(ERR_NOTNUM);
		len = keys->getalllen() / num1;
		if (fileio->getfuten())
			len = len + len/2;
	}
	//�{�����[���ݒ�R�}���h�i�����\�j
	else if (ch == 'V') {
		if ((ch = fileio->getplus()) != 0) {
			if (ch == 2)
				nowVol ++;
			else
				nowVol --;
		}
		else {
			if ((nowVol = fileio->getnum()) == NO_NUM)
				fileio->asm_error(ERR_NOTNUM);
		}
		fileio->writemu((char)nowVol);
		fileio->writemu(CMD_VOL);
	}
	//�I�N�^�[�u�ݒ�R�}���h
	else if (ch == 'O') {
		if ((nowOct = fileio->getnum()) == NO_NUM)
			fileio->asm_error(ERR_NOTNUM);
	}
	//�I�N�^�[�u�����i�{�j
	else if (ch == '>') {
		nowOct ++;
	}
	//�I�N�^�[�u�����i�|�j
	else if (ch == '<') {
		nowOct --;
	}
	//�����ݒ�
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
	}
	//�L�[�ݒ�R�}���h
	else if (ch == 'K') {
		writenum();
		fileio->writemu(CMD_KEY);
	}
	//���F�^�C�v�ݒ�
	else if (ch == '@') {
		writenum();
		fileio->writemu(CMD_TYPE);
	}
	//���t�I��
	else if (ch == 'Q') {
		fileio->writemu(0);
		fileio->writemu(CMD_END);
	}
	//LED�_��
	else if (ch == 'P') {
		writenum();
		fileio->writemuw(CMD_LED_ON);
	}
	//LED����
	else if (ch == 'F') {
		writenum();
		fileio->writemuw(CMD_LED_OFF);
	}
	//�I�N�^�[�u�̊m�F
	else if (ch == '[') {
		if (fileio->getnum() != nowOct) {
			fileio->asm_warning(WAR_OCTCHK);
		}
		fileio->getd();
	}
	else
		fileio->asm_error(ERR_NOTCOM);
}

/*
	�`�����l���~���[�W�b�N�W�J�^�N���X	---------------------------------
*/
class	clsCHAN	: clsPART {
	protected:
		int			offset;
		clsCHAN*	next;
	public:
		clsCHAN(clsFILE* tmp, clsKEY* tmp4)
										: clsPART(tmp, tmp4) {
			next = NULL;
			offset = 0;
		}
		virtual ~clsCHAN() {
			if (next != NULL) delete next;
		}
		void	addlink(clsCHAN* tmp) {
			if (next == NULL) next = tmp;
			else next->addlink(tmp);
		}
		void	asmbl();
		int		getnum(int num = 0) {
			if (next == NULL) return num + 1;
			else return next->getnum(num + 1);
		}
		int		getaddr(int num) {
			if (num == 0) return offset;
			if (next != NULL) return next->getaddr(num - 1);
			return NO_NUM;
		}
};

/*
	�A�Z���u�����s
*/
void	clsCHAN :: asmbl()
{
	char	ch;

	offset = fileio->getaddr();
	//�T�C�Y�����L�����Ă���
	fileio->writemuw(0);

	if (fileio->getd() != '{')
		fileio->asm_error(ERR_NOTDEF);
	ch = fileio->getd();
	while(ch != '}') {
		asmblpt(ch);
		ch = fileio->getd();
	}

	int size = (int)(fileio->getaddr()) - offset - 2;
	fileio->writemuw_at(offset, size);
}

/*
	���y�p�����[�^�Ǘ��^�N���X	-----------------------------------------
*/
class	clsMUSIC	{
	protected:
		clsCHAN*	s_chan;
		clsCHAN*	chan;
		clsFILE*	fileio;
		clsKEY*		keys;
		int			status;
	public:
		clsMUSIC(clsFILE* data) {
			status = 0;
			chan = s_chan = NULL;
			fileio = data;
			keys = new clsKEY(fileio);
		}
		void	asmbl(char ch);
		void	run();
};

/*
	�A�Z���u�����s
*/
void	clsMUSIC :: asmbl(char ch)
{
	keys->sleninit();
	if (ch == 'T') {
		keys->asmbl_tempo();
	}
	else if (ch == 'K') {
		keys->asmbl_key();
	}
	else if (ch == 'B') {
		keys->asmbl_beat();
	}
	else if (ch == '#') {
		chan = new clsCHAN(fileio, keys);
		chan->asmbl();
		if (s_chan == NULL) s_chan = chan;
		else				s_chan->addlink(chan);
	}
	else
		fileio->asm_error(ERR_STATUS);
}

/*
	�A�Z���u�����Ăяo��
*/
void	clsMUSIC :: run()
{
	char	ch;

	keys->init();
	// �w�b�_�o��
	fileio->writemu('M');
	fileio->writemu('M');
	fileio->writemu('L');
	fileio->writemu('D');
	// �T�C�Y�i���j
	fileio->writemuw(0);
	fileio->writemuw(0);
	// �`�����l�����o�́i���j
	fileio->writemu(0);

	ch = fileio->getd();
	while (ch != EOF) {
		asmbl(ch);
		ch = fileio->getd();
	};
	//�T�C�Y����������
	int size = (int)(fileio->getaddr()) - 8;
	fileio->writemuw_at(4, size);
	//�`�����l��������������
	ch = s_chan->getnum();
	fileio->writemu_at(8, ch);

	//�N���[�Y
	if (s_chan != NULL) {
		delete s_chan;
		s_chan = NULL;
	}
}

/*
	���C��	-------------------------------------------------------------
*/
int	main(int argc, char* argv[])
{
	clsMUSIC*	music;
	clsFILE*	fileio;

	cerr << "MML2BIN.EXE ATTiny85 MusicBox�pMML �A�Z���u���v���O���� Ver 1.00 (C)Hiro OTSUKA" << endl;
	if (argc == 1)
		commonfunc::usage();
	for (int i = 1; i < argc; i++) {
		string filename = argv[i];
		if (filename.find_last_of('.') != string::npos)
			filename = filename.substr(0, filename.find_last_of('.'));
		fileio = new clsFILE(filename);
		music = new clsMUSIC(fileio);
		music->run();
		delete music;
		delete fileio;
		cerr << "  " << filename << ".BIN �t�@�C�����o�͂��܂���." << endl;
	}
	return 0;
}

