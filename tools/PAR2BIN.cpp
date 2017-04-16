/*
 * PAR2BIN.cpp
 *
 * �T�v�F
 *  �e�L�X�g�ŋL�q���ꂽ�p�����[�^���o�C�i���`���ŏo�͂���c�[��
 *
 * �g�p���@���F
 *  �\�[�X�R�[�h�� Eclipse ���ŃR���p�C������B
 *  �R���p�C����A������ PAR �t�@�C�������w�肵�Ď��s����B
 *
 * ���C�Z���X�F
 *  Copyright (c) 2017, Hiro OTSUKA All rights reserved.
 *  2����BSD���C�Z���X/2 clause BSD license�i������LICENSE.md�Q��/See LICENSE.md�j
 *
 * �X�V�����F
 *  2017/04/16 �V�K�쐬
 *
 */

#include <iostream>
#include <fstream>

using namespace std;

// �萔�錾 ===================================================================

#define		NO_NUM	100000		// "�����ȊO"��\������l

// �G���[�E�x�����b�Z�[�W��`
#define		ERR_STATUS		"���݂��Ȃ��R�}���h�Ŏn�܂��Ă��܂�."
#define		ERR_NUMOF		"�������傫�����܂�."
#define		ERR_NOX			"������ X(16�i���̊J�n) �͎g�p�ł��܂���."
#define		ERR_NOB			"������ B(2�i���̊J�n) �͎g�p�ł��܂���."

/*
	���ʏ���===================================================================
*/
namespace commonfunc {
	// �^�C�g��
	void	title()
	{
		cerr << "PAR2BIN.EXE ATTiny85 MusicBox�p�p�����[�^ �ϊ��v���O���� Ver 1.00 (C)Hiro OTSUKA" << endl;
	}
	// �g������\��
	void	usage()
	{
		cerr << "  Usage: PAR2BIN.EXE filename" << endl;
		cerr << "    �e�L�X�g�ŏ����ꂽ�p�����[�^��ϊ����A.BIN �t�@�C�����쐬���܂�." << endl;
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
int		clsFILE :: ch_check(char ch, int base)
{
	// ����
	if (base == 10 && '0' <= ch && ch <= '9') return CH_NUM;
	if (base == 16 && (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'F'))) return CH_NUM;
	if (base == 2  && '0' <= ch && ch <= '1') return CH_NUM;
	// �A���t�@�x�b�g
	if ('A' <= ch && ch <= 'Z') return CH_ALPH;
	// �L���ȋL���i�Ȃ��j
	if (string("\n\r").find(ch) != string::npos) return CH_KIGOU;

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
			if (ch == '\n') linenum --;	//��œ�d���Z�ɂȂ�̂Ō��Z���Ă���
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
	int		base = 10, nums = 0;

	// �ꕶ���ǂݍ���
	char ch = getd();

	// �������n�܂�Ȃ���ΏI���
	if (ch_check(ch, base) != CH_NUM) {
		// �ꎞ�ۑ����āA�G���[��Ԃ�
		oldch = ch;
		return NO_NUM;
	}
	// ������ǂݑ�����
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
				// 10�i������ 16�i���̊J�n
				if (base == 10) {
					base = 16;
					ch = getd(1);
				} else {
					asm_error(ERR_NOX);
				}
			} else if (ch == 'B') {
				// 10�i������2�i���̊J�n
				if (base == 10) {
					base = 2;
					ch = getd(1);
				// 16�i���̏ꍇ�� B �͒ʏ�̕���
				} else if (base != 16) {
					asm_error(ERR_NOB);
				}
			}
		}
	} while(ch_check(ch, base) == CH_NUM);

	// �s�v�ȕ����̏ꍇ�͓ǂݎ̂�
	if (ch_check(ch) == CH_ETC)
		oldch = 0;
	// �K�v�ȕ����̏ꍇ�͈ꎞ�ۑ�
	else
		oldch = ch;

	return nums;
}

/*
	�p�����[�^�Ǘ��^�N���X ================================================
*/
class	clsPARAM	{
	protected:
		clsFILE*	fileio;		// �t�@�C��IO�Ǘ�
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
	�A�Z���u�����s
*/
void	clsPARAM :: asmbl(char ch)
{
	// �P�o�C�g�w��
	if (ch == 'B') {
		fileio->writemu((char)fileio->getnum());
	}
	// �Q�o�C�g�w��
	else if (ch == 'W') {
		fileio->writemuw(fileio->getnum());
	}
	// ����`�̃R�}���h
	else
		fileio->asm_error(ERR_STATUS);
}

/*
	�A�Z���u�����Ăяo��
*/
void	clsPARAM :: run()
{
	char	ch;

	// �w�b�_�o��('PARM')
	fileio->writemu('P');
	fileio->writemu('A');
	fileio->writemu('R');
	fileio->writemu('M');
	// �T�C�Y�i���j
	fileio->writemuw(0);
	fileio->writemuw(0);

	// �t�@�C���̏I���܂ŏ������s��
	ch = fileio->getd();
	while (ch != EOF) {
		asmbl(ch);
		ch = fileio->getd();
	};

	// �T�C�Y����������
	int size = (int)(fileio->getoutaddr()) - 8;
	fileio->writemuw_at(4, size);
}

/*
	���C�� ====================================================================
*/
int	main(int argc, char* argv[]) {
	clsFILE*	fileio;	//�t�@�C��IO�Ǘ�
	clsPARAM*	param;	//�p�����[�^�Ǘ�

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
		param = new clsPARAM(fileio);
		param->run();
		delete param;
		delete fileio;
		commonfunc::complete(filename);
	}
	return 0;
}
