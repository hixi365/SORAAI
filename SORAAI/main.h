
// ****************************************************************
//	#include
// ****************************************************************
#include <dxlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

// ****************************************************************
//	#define
// ****************************************************************
#define CALL_STACK_MAX 10							// �X�^�b�N�̊K�w
#define CALL_VAL_MAX 10								// �X�^�b�N�̗v�f��
#define STRING_MAX 256								// �֐����̒���
#define HASH_SIZE 1000								// ���[�U��`�֐��̐�
#define FUNCTION_SIZE 100000						// ���[�U��`�֐��̕�����
#define LINE_MAX 256								// ��s�ɋL�q�ł��镶����
#define PATH_MAX 256								// �p�X
#define DEF_SIZE 500								// �\��֐��̐�
#define TASK_SIZE 500								// �^�X�N�̐�
#define IMAGE_MAX 100								// �e���C�����̉摜�n���h��
#define TITLE_MAX 256								// �^�C�g���̍ő啶����

// ****************************************************************
//	�\����
// ****************************************************************
typedef struct Caller						// �X�^�b�N
{
	char buf[CALL_VAL_MAX][STRING_MAX];
}Caller;

typedef struct Hash							// ���[�U��`�֐�
{
	char name[STRING_MAX];
	char buf[FUNCTION_SIZE];
}Hash;

typedef struct Def
{
	char name[STRING_MAX];
	int num;
}Def;

// ****************************************************************
//	enum
// ****************************************************************
struct defunc{

	// �����֐��̒�`
	// �ǉ��̎菇
	// 1. Enum �ɒǉ�
	// 2. SetDef() �Ŗ��O�̓o�^
	// 3. RunS() �Ŏ��s���e�̋L�q
	enum Enum{
		_Go,			// �^�X�N���s				()
		_SayMostEasy,	// �Z���t�̊ȈՕ\��			(name, graph, 1, 2, 3, 4)
		_LoadImg,		// �摜�ǂݍ���				(path, layer, index)
		_DrawImg,		// �摜�̕\��				(x, y, layer, index)
		_SayNMostEasy,	// �i���[�V�����̊ȈՕ\��	(1, 2, 3, 4)
		_ClearLayer,	// ���C���̏�����			(layer)
		_Wait,			// �E�G�C�g					(frame)
		_Fade,			// �t�F�[�h					(frame, alpha, layer)
		_SetTitle,		// �^�C�g���̕ύX			(name)
		_EnableYukkuri, // �������̗L����			()
		_SetYukkuri,	// ������̐ݒ�				(speed, tone, vol, voice)
		_SayYukkuriEasy,// �Z���t�̊ȈՓǂݏグ�\�� (name, graph, 1, 2, 3, 4)
		_SetYukkuriYomi,// ������莫���ǉ�			(kaki, yomi)
		_DelYukkuriYomi,// ������莫���폜			(kaki)

	};

};

struct tasktype{

	// �^�X�N
	// �ǉ��̎菇
	// 1. Enum �ɒǉ�
	// 2. �N���X��`
	// 3. TaskFunc() �ɓ���̋L�q
	enum Enum{
		Non,			// ����
		Wait,			// �E�G�C�g
		Say,			// ������`��
		Img,			// �摜�`��
		Clear,			// ���C���̏�����
		Fade,			// �t�F�[�h
		Silent,			// �������҂�		
		Yukkuri,		// �������

	};
};

// ****************************************************************
//	�N���X
// ****************************************************************
class BouyomiHelper
{

private:

	int handle = -1;
	IPDATA ipdata;

	int ResetConnect();
	int SendCommand(short command);
	int RecvCommand(void * p);

public:

	BouyomiHelper();
	~BouyomiHelper();

	int Pause();
	int Resume();
	int Skip();
	int Clear();
	int Next();
	int GetPause();
	int GetNowPlaying();
	int GetTaskCount();

	int Talk(char * message, int speed, int tone, int vol, int voice);

};

class Task{

protected:

	int mType;

public:

	Task(){ mType = tasktype::Non; }
	virtual ~Task(){}

	int		getType(){ return mType; }
	void	setType(int type){ mType = type; }

};

class TaskWait : public Task
{

private:

	int mFrame;

public:

	TaskWait(int frame){ mFrame = frame; mType = tasktype::Wait; }

	TaskWait(){ mType = tasktype::Wait; }
	~TaskWait(){}

	int		getFrame(){ return mFrame; }
	void	setFrame(int frame){ mFrame = frame; }

	int		Step(){ mFrame--; return mFrame > 0; }

};

class TaskSay : public Task
{

private:
	int mX, mY;
	char mBuf[LINE_MAX];
	int mCr;

public:

	TaskSay(int x, int y, char *str, int cr){ mX = x; mY = y; strcpy_s(mBuf, str); mCr = cr; mType = tasktype::Say; }

	TaskSay(){ mType = tasktype::Say; }
	~TaskSay(){}

	int		getX(){ return mX; }
	void	setX(int x){ mX = x; }
	int		getY(){ return mY; }
	void	setY(int y){ mY = y; }
	char*	getStr(){ return mBuf; }
	void	setStr(char *str){ strcpy_s(mBuf, str); }
	int		getCr(){ return mCr; }
	void	setCr(int cr){ mCr = cr; }

};

class TaskImg : public Task
{

private:

	int mX, mY;
	int mLayer;
	int mHandle;

public:

	TaskImg(int x, int y, int layer, int handle){ mX = x; mY = y; mLayer = layer; mHandle = handle; mType = tasktype::Img; }

	TaskImg(){ mType = tasktype::Img; }
	~TaskImg(){}

	int		getX(){ return mX; }
	void	setX(int x){ mX = x; }
	int		getY(){ return mY; }
	void	setY(int y){ mY = y; }
	int		getLayer(){ return mLayer; }
	void	setLayer(int layer){ mLayer = layer; }
	int		getHandle(){ return mHandle; }
	void	setHandle(int handle){ mHandle = handle; }

};

class TaskClear : public Task
{

private:

	int mLayer;

public:

	TaskClear(int layer){ mLayer = layer; mType = tasktype::Clear; }

	TaskClear(){ mType = tasktype::Clear; }
	~TaskClear(){}

	int		getLayer(){ return mLayer; }
	void	setLayer(int layer){ mLayer = layer; }

};

class TaskFade : public Task
{

private:

	int mFrame;
	int mAlpha;
	int mLayer;

public:

	TaskFade(int frame, int alpha, int layer){ mFrame = frame; mAlpha = alpha; mLayer = layer; mType = tasktype::Fade; }

	TaskFade(){ mType = tasktype::Fade; }
	~TaskFade(){}

	int		getFrame(){ return mFrame; }
	void	setFrame(int frame){ mFrame = frame; }
	int		getAlpha(){ return mAlpha; }
	void	setAlpha(int alpha){ mAlpha = alpha; }
	int		getLayer(){ return mLayer; }
	void	setLayer(int layer){ mLayer = layer; }

	int		Step(){ mFrame--; return mFrame > 0; }

};

class TaskSilent : public Task
{

private:

public:

	TaskSilent(){ mType = tasktype::Silent; }
	~TaskSilent(){}

	int		Step();

};

class TaskYukkuri : public Task
{

private:
	char mBuf[LINE_MAX];

public:

	TaskYukkuri(char *str){ strcpy_s(mBuf, str); mType = tasktype::Yukkuri; }

	TaskYukkuri(){ mType = tasktype::Yukkuri; }
	~TaskYukkuri(){}

	char*	getStr(){ return mBuf; }
	void	setStr(char *str){ strcpy_s(mBuf, str); }

};

// ****************************************************************
//	�ʏ�֐�
// ****************************************************************
int	InitLayer();
int InitTask();
void DeleteTask();
void Cleanup();
void TaskFunc(int p);
void DeleteTimer();
void PlayTask();
void DeleteTab(char *p, char *q);
void AddHash(char *name);
void LoadScript();
int SearchD(char *func);
int SearchS(char *func);
void AddDef(char *name, int num);
void SetDef();
int AddTask();
int GetSayLong(int b);
void SetTitle(char *p);
int EnableYukkuri();
int YukkuriWait();
void SetYukkuriYomi(char *p, char *q);
void DeleteYukkuriYomi(char *p);
void RunS(int num);
void RunD(char *func);
void LoadPath();
void InitImg();
void InitAlpha();