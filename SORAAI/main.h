
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
#define CALL_STACK_MAX 10							// スタックの階層
#define CALL_VAL_MAX 10								// スタックの要素数
#define STRING_MAX 256								// 関数名の長さ
#define HASH_SIZE 1000								// ユーザ定義関数の数
#define FUNCTION_SIZE 100000						// ユーザ定義関数の文字数
#define LINE_MAX 256								// 一行に記述できる文字数
#define PATH_MAX 256								// パス
#define DEF_SIZE 500								// 予約関数の数
#define TASK_SIZE 500								// タスクの数
#define IMAGE_MAX 100								// 各レイヤ毎の画像ハンドル
#define TITLE_MAX 256								// タイトルの最大文字長

// ****************************************************************
//	構造体
// ****************************************************************
typedef struct Caller						// スタック
{
	char buf[CALL_VAL_MAX][STRING_MAX];
}Caller;

typedef struct Hash							// ユーザ定義関数
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

	// 内部関数の定義
	// 追加の手順
	// 1. Enum に追加
	// 2. SetDef() で名前の登録
	// 3. RunS() で実行内容の記述
	enum Enum{
		_Go,			// タスク実行				()
		_SayMostEasy,	// セリフの簡易表示			(name, graph, 1, 2, 3, 4)
		_LoadImg,		// 画像読み込み				(path, layer, index)
		_DrawImg,		// 画像の表示				(x, y, layer, index)
		_SayNMostEasy,	// ナレーションの簡易表示	(1, 2, 3, 4)
		_ClearLayer,	// レイヤの初期化			(layer)
		_Wait,			// ウエイト					(frame)
		_Fade,			// フェード					(frame, alpha, layer)
		_SetTitle,		// タイトルの変更			(name)
		_EnableYukkuri, // ゆっくりの有効化			()
		_SetYukkuri,	// 喋り方の設定				(speed, tone, vol, voice)
		_SayYukkuriEasy,// セリフの簡易読み上げ表示 (name, graph, 1, 2, 3, 4)
		_SetYukkuriYomi,// ゆっくり辞書追加			(kaki, yomi)
		_DelYukkuriYomi,// ゆっくり辞書削除			(kaki)

	};

};

struct tasktype{

	// タスク
	// 追加の手順
	// 1. Enum に追加
	// 2. クラス定義
	// 3. TaskFunc() に動作の記述
	enum Enum{
		Non,			// 無し
		Wait,			// ウエイト
		Say,			// 文字列描画
		Img,			// 画像描画
		Clear,			// レイヤの初期化
		Fade,			// フェード
		Silent,			// ゆっくり待ち		
		Yukkuri,		// ゆっくり

	};
};

// ****************************************************************
//	クラス
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
//	通常関数
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