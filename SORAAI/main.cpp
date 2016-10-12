
// ****************************************************************
//	#include
// ****************************************************************
#include "main.h"

// ****************************************************************
//	�ύX�\�Ȓ�`
// ****************************************************************
char	gTitle[TITLE_MAX];							// �^�C�g��
char	dTitle[] = "�󗕉Ƃ̂����� -0.2.2- ";		// �f�t�H���g�̃^�C�g��

int		gWindowWidth = 640;							// �E�B���h�E�T�C�Y
int		gWindowHeight = 480;						// �E�B���h�E�T�C�Y

int		gWindowX = 0;								// Say �E�B���h�E X
int		gWindowY = 330;								// Say �E�B���h�E Y
int		gSayNameX = 30;								// Say ���O X
int		gSayNameY = 450;							// Say ���O Y
int		gSayTextX1 = 150;							// Say �e�L�X�g X
int		gSayTextY1 = 350;							// Say �e�L�X�g Y
int		gSayTextX2 = 150;							// Say �e�L�X�g X
int		gSayTextY2 = 375;							// Say �e�L�X�g Y
int		gSayTextX3 = 150;							// Say �e�L�X�g X
int		gSayTextY3 = 400;							// Say �e�L�X�g Y
int		gSayTextX4 = 150;							// Say �e�L�X�g X
int		gSayTextY4 = 425;							// Say �e�L�X�g Y
int		gSayImgX = 30;								// Say ��O�� X
int		gSayImgY = 350;								// Say ��O�� Y

int		gSaySpeed = 3;								// Say �ꕶ���������frame��
int		gSayOffset = 60;							// Say �Œ���\��fream��

int		gYukkuriYomiSend = 30;						// �_�ǂ� ���Mms
int		gYukkuriYomiUpdate = 50;					// �_�ǂ� �X�Vms


// ****************************************************************
//	�O���[�o���ϐ�
// ****************************************************************
int				gCallNum = 0;						// �X�^�b�N�K�w�̕ۑ�
Caller			gCall[CALL_STACK_MAX];				// �X�^�b�N
int				gHashNum = 0;						// ���[�U�֐��̐�
Hash			gHash[HASH_SIZE];					// ���[�U�֐�
char			gProjectPath[256];					// �v���W�F�N�g�̃p�X
int				gDefNum = 0;						// �\��֐��̐�
Def				gDef[DEF_SIZE];						// �\��֐�

int				gTaskP = 0;							// ���s����^�X�N
int				gTaskQ = 0;							// �ǉ�����^�X�N
Task*			gTask[TASK_SIZE];					// �^�X�N

int				gLayer[15];							// ���C���n���h��
int				gLayerAlpha[15];					// ���u�����h
int				gImgHandle[15][IMAGE_MAX];			// �摜�n���h��

int				gUseYukkuri = 0;					// �_�ǂ݂̗��p
int				gSpeed = -1;						// ���x
int				gTone = -1;							// ����
int				gVol = -1;							// ����
int				gVoice = 0;							// ����

BouyomiHelper	gBouyomi;							// �_�ǂ�

// ****************************************************************
//	�N���X�֐�
// ****************************************************************
BouyomiHelper::BouyomiHelper()
{

	handle = -1;
	IPDATA ip = { 127, 0, 0, 1 };

	ipdata = ip;

}

BouyomiHelper::~BouyomiHelper()
{



}

int BouyomiHelper::ResetConnect()
{

	CloseNetWork(handle);
	handle = ConnectNetWork(ipdata, 50001);

	return handle == -1 ? -1 : 0;

}

int BouyomiHelper::SendCommand(short command)
{

	char com[2];
	*((short*)&com[0]) = command;

	return NetWorkSend(handle, com, 2);

}

int BouyomiHelper::RecvCommand(void * p)
{

	int ret = 0;
	int rec = 0;

	ret = GetNetWorkDataLength(handle);
	ret = NetWorkRecv(handle, p, ret);

	return ret;

}

int BouyomiHelper::Pause()
{

	if (ResetConnect() == -1)
		return -1;

	return SendCommand(0x0010);

}

int BouyomiHelper::Resume()
{

	if (ResetConnect() == -1)
		return -1;

	return SendCommand(0x0020);

}

int BouyomiHelper::Skip()
{

	if (ResetConnect() == -1)
		return -1;

	return SendCommand(0x0030);

}

int BouyomiHelper::Clear()
{

	if (ResetConnect() == -1)
		return -1;

	return SendCommand(0x0040);

}

int BouyomiHelper::Next()
{

	Clear();
	Skip();

	return 0;

}

int BouyomiHelper::GetPause()
{

	char ret = 0;

	if (ResetConnect() == -1)
		return -1;

	if (SendCommand(0x0110) == -1)
		return -1;

	if (RecvCommand(&ret) == -1)
		return -1;

	return (int)ret;

}

int BouyomiHelper::GetNowPlaying()
{

	char ret = 0;


	if (ResetConnect() == -1)
		return -1;

	if (SendCommand(0x0120) == -1)
		return -1;

	if (RecvCommand(&ret) == -1)
		return -1;

	return (int)ret;

}

int BouyomiHelper::GetTaskCount()
{

	int ret = 0;

	if (ResetConnect() == -1)
		return -1;

	if (SendCommand(0x0130) == -1)
		return -1;

	if (RecvCommand(&ret) == -1)
		return -1;

	return (int)ret;

}

int BouyomiHelper::Talk(char * message, int speed = -1, int tone = -1, int vol = -1, int voice = 0)
{

	long   len;
	char   msg[32768];

	if (ResetConnect() == -1)
		return -1;

	strcpy_s(msg, 32768, message);
	len = (long)strlen(msg);

	char buf[15];
	*((short*)&buf[0]) = 0x0001;
	*((short*)&buf[2]) = speed;
	*((short*)&buf[4]) = tone;
	*((short*)&buf[6]) = vol;
	*((short*)&buf[8]) = voice;
	*((char*)&buf[10]) = 2;
	*((long*)&buf[11]) = len;

	NetWorkSend(handle, buf, 15);
	NetWorkSend(handle, msg, len);

	return 0;

}

int	TaskSilent::Step()
{
	
	return YukkuriWait();

}



// ****************************************************************
//	�ʏ�֐�
// ****************************************************************
int	InitLayer()
{

	for(int i=0; i < 15; i++)
		gLayer[i] = MakeScreen(gWindowWidth, gWindowHeight, true);

	return 0;

}

int InitTask()
{

	for(int i=0; i < TASK_SIZE; i++)
		gTask[i] = nullptr;

	return 0;

}

void DeleteTask()
{

	for(int i=0; i < TASK_SIZE; i++){
		if(gTask[i] != nullptr){
			delete gTask[i];
			gTask[i] = nullptr;
		}
	}

}

void Cleanup()
{

	DeleteTask();

}

void TaskFunc(int p)
{

	int done = 1;

	if(gTask[p] == nullptr)
		return;

	switch(gTask[p]->getType()){
	case tasktype::Wait:
		// �E�G�C�g
		done = !(((TaskWait*)gTask[p])->Step());
		break;
	case tasktype::Say:
		// ������̕`��
		// x, y, �e�L�X�g
		SetDrawScreen(gLayer[6]);
		DrawString(((TaskSay*)gTask[p])->getX(), ((TaskSay*)gTask[p])->getY(), ((TaskSay*)gTask[p])->getStr(), ((TaskSay*)gTask[p])->getCr());
		break;
	case tasktype::Img:
		// �摜�̕`��
		// x, y, layer, handle
		SetDrawScreen(gLayer[((TaskImg*)gTask[p])->getLayer()]);
		DrawGraph(((TaskImg*)gTask[p])->getX(), ((TaskImg*)gTask[p])->getY(),gImgHandle[((TaskImg*)gTask[p])->getLayer()][((TaskImg*)gTask[p])->getHandle()], true);
		break;
	case tasktype::Clear:
		// ���C���̏�����
		SetDrawScreen(gLayer[((TaskClear*)gTask[p])->getLayer()]);
		ClearDrawScreen();
		break;
	case tasktype::Fade:
		// �t�F�[�h
		gLayerAlpha[((TaskFade*)gTask[p])->getLayer()] += (((TaskFade*)gTask[p])->getAlpha() - gLayerAlpha[((TaskFade*)gTask[p])->getLayer()])/(((TaskFade*)gTask[p])->getFrame() > 0 ? ((TaskFade*)gTask[p])->getFrame() : 1);
		done = !(((TaskFade*)gTask[p])->Step());
		break;
	case tasktype::Silent:
		// �������҂�
		done = !(((TaskSilent*)gTask[p])->Step());
		break;
	case tasktype::Yukkuri:
		// �������
		if (!gUseYukkuri) break;
		gBouyomi.Talk(((TaskYukkuri*)gTask[p])->getStr(), gSpeed, gTone, gVol, gVoice);
		break;
	default:
		break;
	}

	if(done){
		delete gTask[p];
		gTask[p] = nullptr;
	}

}

void DeleteTimer()
{

	int p = gTaskP;

	while(p != gTaskQ){
		if(gTask[p] != nullptr){
			if (gTask[p]->getType() == tasktype::Wait)
				((TaskWait*)gTask[p])->setFrame(1);
			else if (gTask[p]->getType() == tasktype::Fade)
				((TaskFade*)gTask[p])->setFrame(1);
			else if (gTask[p]->getType() == tasktype::Silent)
				gBouyomi.Next();

		}
		p = (p+1)%TASK_SIZE;
	}

}

void PlayTask()
{

	while(gTaskP != gTaskQ) // �^�X�N�����݂����
	{

		for(int p = gTaskP; p != gTaskQ; p = (p+1)%TASK_SIZE)
		{
			TaskFunc(p);	// �^�X�N�̎��s
		}

		SetDrawScreen(DX_SCREEN_BACK);
		if(ProcessMessage() == 0 && ScreenFlip() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
		{
			// �\�`��̍폜 
			ClearDrawScreen();

			// space�ɂ��^�C�}�����I��
			static int skip = 0;
			int now = CheckHitKey(KEY_INPUT_SPACE);
			if(skip == 0 && now == 1)
			{
				DeleteTimer();
			}
			skip = now;

			// <<���^�C�}�����I��
			if(CheckHitKey(KEY_INPUT_LSHIFT))
				DeleteTimer();

			// ���C���̕`��
			for(int i=0; i < 15; i++){
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, gLayerAlpha[14-i]);
				DrawGraph(0, 0, gLayer[14-i], true);
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		}
		else // ��肪����ΏI��
		{
			Cleanup();
			exit(1);
		}

		while(gTaskP != gTaskQ && gTask[gTaskP] == nullptr){
			gTaskP = (gTaskP+1)%TASK_SIZE;
		}

	}

}

void DeleteTab(char *p, char *q)
{

	while(*q){
		if(*q == '\t'){
			*q++;
			continue;
		}
		*p++ = *q++;
	}

}

void AddHash(char *name)
{
	
	int t = 0;
	char buf[LINE_MAX] = {};
	char wri[LINE_MAX] = {};
	FILE *fp;
	
	fopen_s(&fp, name, "r");
	if(fp == NULL) return;

	while(fgets(buf, LINE_MAX, fp) != NULL){
		memset(wri, 0, LINE_MAX);
		DeleteTab(wri, buf);
		if(wri[0] == '\0') continue;
		if(wri[0] == '/' && wri[1] == '/') continue;
		if(wri[0] == '\n') continue;
		if(wri[0] == '{'){
			t++;
		}
		else if(wri[0] == '}'){
			t--;
			if(t == 0)
				gHashNum++;
		}
		if(t == 0 && wri[0] == '}'){
			strcat_s(gHash[gHashNum-1].buf, wri);
			strcat_s(gHash[gHashNum-1].buf, "\n");
		}
		else if(t == 0){
			char name[256];
			int l = strnlen_s(wri, LINE_MAX);
			if (l > 0) wri[l - 1] = '\0';
			strcpy_s(name, wri);
			strcpy_s(gHash[gHashNum].name, name);
			memset(gHash[gHashNum].buf, 0, FUNCTION_SIZE);
		}
		else{
			strcat_s(gHash[gHashNum].buf, wri);
		}
	}

}

void LoadScript()
{
	char pPath[256];
	WIN32_FIND_DATA ffd;

	strcpy_s(pPath, gProjectPath);
	strcat_s(pPath, "Script\\*.txt");
	HANDLE h = FindFirstFile(pPath, &ffd);
	if ( h != INVALID_HANDLE_VALUE ) {
		do {
			char name[256];
			strcpy_s(name, gProjectPath);
			strcat_s(name, "Script\\");
			strcat_s(name, ffd.cFileName);
			AddHash(name);
		} while ( FindNextFile(h, &ffd) );
		FindClose(h);
	}

	h = FindFirstFile(".\\Common\\Script\\*.txt", &ffd);
	if ( h != INVALID_HANDLE_VALUE ) {
		do {
			char name[256];
			strcpy_s(name, ".\\Common\\Script\\");
			strcat_s(name, ffd.cFileName);
			AddHash(name);
		} while ( FindNextFile(h, &ffd) );
		FindClose(h);
	}
}

int SearchD(char *func)
{

	for(int i=0; i < gHashNum; i++){
		if(gHash[i].name[0] == 0)
			break;
		if(strcmp(gHash[i].name, func) == 0)
			return i;
	}

	return -1;
	
}

int SearchS(char *func)
{

	for(int i=0; i < gDefNum; i++){
		if(gDef[i].name[0] == 0)
			break;
		if(strcmp(gDef[i].name, func) == 0)
			return i;
	}

	return -1;
	
}

void AddDef(char *name, int num)
{

	strcpy_s(gDef[gDefNum].name, name);
	gDef[gDefNum].num = num;
	gDefNum++;

}

void SetDef()
{

	memset(gDef, 0, DEF_SIZE*sizeof(Def));
	AddDef("_Go", defunc::_Go);	
	AddDef("_SayMostEasy", defunc::_SayMostEasy);
	AddDef("_LoadImg", defunc::_LoadImg);	
	AddDef("_DrawImg", defunc::_DrawImg);
	AddDef("_SayNMostEasy", defunc::_SayNMostEasy);
	AddDef("_ClearLayer", defunc::_ClearLayer);
	AddDef("_Wait", defunc::_Wait);
	AddDef("_Fade", defunc::_Fade);
	AddDef("_SetTitle", defunc::_SetTitle);
	AddDef("_EnableYukkuri", defunc::_EnableYukkuri);
	AddDef("_SetYukkuri", defunc::_SetYukkuri);
	AddDef("_SayYukkuriEasy", defunc::_SayYukkuriEasy);
	AddDef("_SetYukkuriYomi", defunc::_SetYukkuriYomi);
	AddDef("_DelYukkuriYomi", defunc::_DelYukkuriYomi);


}

int AddTask()
{

	int q = gTaskQ;
	gTaskQ = (gTaskQ+1)%TASK_SIZE;
	return q;

}

int GetSayLong(int b)
{

	int n = 0;

	for(int i=0; i < 4; i++)
		n += strlen(gCall[gCallNum].buf[b+i]);

	return n;

}

void SetTitle(char *p)
{

	strcpy_s(gTitle, p);

}

int EnableYukkuri()
{

	IPDATA ip = { 127, 0, 0, 1 };
	int handle = ConnectNetWork(ip, 50001);
	if (handle == -1)
		return 0;

	gBouyomi.Clear();

	return 1;

}

int YukkuriWait()
{

	if (!gUseYukkuri)
		return 0;

	return gBouyomi.GetTaskCount();

}

void SetYukkuriYomi(char *p, char *q)
{

	char buf[LINE_MAX * 3] = {};

	strcat_s(buf, "����(");
	strcat_s(buf, p);
	strcat_s(buf, "=");
	strcat_s(buf, q);
	strcat_s(buf, ")");

	gBouyomi.Talk(buf);
	Sleep(gYukkuriYomiSend);
	gBouyomi.Next();
	Sleep(gYukkuriYomiUpdate);

}

void DeleteYukkuriYomi(char *p)
{

	char buf[LINE_MAX * 2] = {};

	strcat_s(buf, "�Y�p(");
	strcat_s(buf, p);
	strcat_s(buf, ")");

	gBouyomi.Talk(buf);
	Sleep(gYukkuriYomiSend);
	gBouyomi.Next();
	Sleep(gYukkuriYomiUpdate);

}

void RunS(int num)
{

	int p;

	switch(num){
	
	case defunc::_Go:

		PlayTask();
		break;

	case defunc::_SayMostEasy:
		
		p = AddTask(); gTask[p] = new TaskClear(6);
		p = AddTask(); gTask[p] = new TaskImg(gWindowX, gWindowY, 6, 0);
		p = AddTask(); gTask[p] = new TaskSay(gSayNameX, gSayNameY, gCall[gCallNum].buf[0], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX1, gSayTextY1, gCall[gCallNum].buf[2], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX2, gSayTextY2, gCall[gCallNum].buf[3], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX3, gSayTextY3, gCall[gCallNum].buf[4], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX4, gSayTextY4, gCall[gCallNum].buf[5], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskImg(gSayImgX, gSayImgY, 6, atoi(gCall[gCallNum].buf[1]));
		p = AddTask(); gTask[p] = new TaskWait(GetSayLong(2)*gSaySpeed + gSayOffset);
		break;

	case defunc::_LoadImg: // path, layer, index

		{
			int handle = LoadGraph(gCall[gCallNum].buf[0]);
			int layer = atoi(gCall[gCallNum].buf[1]);
			int index = atoi(gCall[gCallNum].buf[2]);
			if(gImgHandle[layer][index] != -1){
				DeleteGraph(gImgHandle[layer][index]);
			}
			gImgHandle[layer][index] = handle;
		}
		break;

	case defunc::_DrawImg: // x, y, layer, index

		p = AddTask(); gTask[p] = new TaskImg(atoi(gCall[gCallNum].buf[0]), atoi(gCall[gCallNum].buf[1]), atoi(gCall[gCallNum].buf[2]), atoi(gCall[gCallNum].buf[3]));
		break;

	case defunc::_SayNMostEasy:
		
		p = AddTask(); gTask[p] = new TaskClear(6);
		p = AddTask(); gTask[p] = new TaskImg(gWindowX, gWindowY, 6, 0);
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX1, gSayTextY1, gCall[gCallNum].buf[0], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX2, gSayTextY2, gCall[gCallNum].buf[1], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX3, gSayTextY3, gCall[gCallNum].buf[2], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX4, gSayTextY4, gCall[gCallNum].buf[3], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskWait(GetSayLong(0)*gSaySpeed + gSayOffset);
		break;

	case defunc::_ClearLayer:
		
		p = AddTask(); gTask[p] = new TaskClear(atoi(gCall[gCallNum].buf[0]));
		break;

	case defunc::_Wait:
		
		p = AddTask(); gTask[p] = new TaskWait(atoi(gCall[gCallNum].buf[0]));
		break;

	case defunc::_Fade:
		
		p = AddTask(); gTask[p] = new TaskFade(atoi(gCall[gCallNum].buf[0]), atoi(gCall[gCallNum].buf[1]), atoi(gCall[gCallNum].buf[2]));
		break;

	case defunc::_SetTitle:

		SetTitle(gCall[gCallNum].buf[0]);
		break;

	case defunc::_EnableYukkuri:

		gUseYukkuri = EnableYukkuri();
		break;
	
	case defunc::_SetYukkuri:

		if (!gUseYukkuri) break;

		gSpeed	= atoi(gCall[gCallNum].buf[0]);
		gTone	= atoi(gCall[gCallNum].buf[1]);
		gVol	= atoi(gCall[gCallNum].buf[2]);
		gVoice	= atoi(gCall[gCallNum].buf[3]);
		break;

	case defunc::_SayYukkuriEasy:

		if (!gUseYukkuri) { RunS(defunc::_SayMostEasy); break; }

		p = AddTask(); gTask[p] = new TaskClear(6);
		p = AddTask(); gTask[p] = new TaskImg(gWindowX, gWindowY, 6, 0);
		p = AddTask(); gTask[p] = new TaskSay(gSayNameX, gSayNameY, gCall[gCallNum].buf[0], GetColor(255, 255, 255));
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX1, gSayTextY1, gCall[gCallNum].buf[2], GetColor(255, 255, 255)); p = AddTask(); gTask[p] = new TaskYukkuri(gCall[gCallNum].buf[2]);
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX2, gSayTextY2, gCall[gCallNum].buf[3], GetColor(255, 255, 255));	p = AddTask(); gTask[p] = new TaskYukkuri(gCall[gCallNum].buf[3]);
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX3, gSayTextY3, gCall[gCallNum].buf[4], GetColor(255, 255, 255));	p = AddTask(); gTask[p] = new TaskYukkuri(gCall[gCallNum].buf[4]);
		p = AddTask(); gTask[p] = new TaskSay(gSayTextX4, gSayTextY4, gCall[gCallNum].buf[5], GetColor(255, 255, 255));	p = AddTask(); gTask[p] = new TaskYukkuri(gCall[gCallNum].buf[5]);
		p = AddTask(); gTask[p] = new TaskImg(gSayImgX, gSayImgY, 6, atoi(gCall[gCallNum].buf[1]));
		p = AddTask(); gTask[p] = new TaskSilent();
		break;

	case defunc::_SetYukkuriYomi:

		if (!gUseYukkuri) break;

		SetYukkuriYomi(gCall[gCallNum].buf[0], gCall[gCallNum].buf[1]);
		break;

	case defunc::_DelYukkuriYomi:

		if (!gUseYukkuri) break;

		DeleteYukkuriYomi(gCall[gCallNum].buf[0]);
		break;

	default:
		break;

	}

}


void RunD(char *func)
{

	int num;
	int now = 0;
	int t = 0;
	int index;
	int stack = 0;
	int ch = -1;
	int cd = -1;
	char msg[256];
	char *read = new char[FUNCTION_SIZE];
	char *line = new char[FUNCTION_SIZE];
	char *buf = new char[FUNCTION_SIZE];

	index = SearchD(func);
	if(index == -1){
		if(strcmp(func, "SYSTEM_INIT") == 0)
			return;
		strcpy_s(msg, func);
		strcat_s(msg, " is not found");
		MessageBox(0, msg, "error", 0);
		exit(1);
	}
	else{
		memcpy(read, gHash[index].buf, FUNCTION_SIZE);
		while(read[now] != 0){
			memset(line, 0, FUNCTION_SIZE);
			for(num = 0; read[now+num] != '\n'; num++){
				line[num] = read[now+num];
			}
			now += num+1;
			if((line[0] == '{' || line[0] == '}')&& strlen(line) == 1){
				if(line[0] == '{')
					t++;
				else{
					t--;
					if(t == 1){
						gCallNum++;
						if(ch != -1){
							RunD(gHash[ch].name);
						}
						if(cd != -1){
							gCall[gCallNum] = gCall[gCallNum-1];
							RunS(cd);
						}
						cd = -1;
						ch = -1;
						stack = 0;
						gCallNum--;
					}
				}
				continue;
			}
			if(t == 1){
				if(line[0] == '_'){
					cd = SearchS(line);
					if(cd == -1){
						strcpy_s(msg, line);
						strcat_s(msg, " is not found");
						MessageBox(0, msg, "error", 0);
						exit(1);
					}
				}
				else{
					ch = SearchD(line);
					if(ch == -1){
						strcpy_s(msg, line);
						strcat_s(msg, " is not found");
						MessageBox(0, msg, "error", 0);
						exit(1);
					}
				}
				continue;
			}
			if(t == 2){
				if(stack == 0)
					memset(gCall[gCallNum].buf, 0, STRING_MAX*CALL_VAL_MAX);
				if(line[0] == '~'){
					if(gCallNum == 0){
						MessageBox(0, "array index is -1", "error", 0);
						exit(1);
					}
					memcpy(buf, gCall[gCallNum-1].buf[atoi(line+1)], STRING_MAX);
				}
				else{
					memcpy(buf, line, STRING_MAX);
				}
				memcpy(gCall[gCallNum].buf[stack], buf, STRING_MAX);
				stack++;
			}
		}
	}

	delete[] read;
	delete[] line;
	delete[] buf;

}

void LoadPath()
{

	char buf[256];
	FILE *fp;

	fopen_s(&fp, ".\\ProjectPath.txt", "r");
	if(fp == NULL){
		Cleanup();
		exit(1);
	}

	fgets(buf, 256, fp);
	fclose(fp);

	strcpy_s(gProjectPath, buf);

}

void InitImg()
{

	for(int i=0; i < 15; i++){
		for(int j=0; j < IMAGE_MAX; j++){
			gImgHandle[i][j] = -1;
		}
	}

}

void InitAlpha()
{

	for(int i=0; i < 15; i++)
		gLayerAlpha[i] = 255;

}

// ****************************************************************
//	���C���֐�
// ****************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpCmdLine, int nCmdShow)
{

	// ������
	SetTitle(dTitle);								// �f�t�H���g�̃^�C�g���̎w��

	// �v���W�F�N�g�̓ǂݍ���
	LoadPath();										// �v���W�F�N�g�p�X�̓ǂݍ���
	SetDef();										// �\��֐��̃Z�b�g
	LoadScript();									// ���[�U�֐��̓ǂݍ���
	RunD("SYSTEM_INIT");							// ��ǂ݊֐��̎��s

	// DxLib�̏�����
	ChangeWindowMode(TRUE);							// �E�C���h�E���[�h
	SetGraphMode(gWindowWidth, gWindowHeight, 32);	// ��ʂ̃T�C�Y
	SetOutApplicationLogValidFlag(false);			// ���O���o�͂��Ȃ�
	SetMainWindowText(gTitle);						// �^�C�g��
	SetAlwaysRunFlag(true);							// �t�H�[�J�X
	SetDrawScreen(DX_SCREEN_BACK);					// ���`��
	DxLib_Init();									// dxlib������
	SetMouseDispFlag(false);						// �}�E�X�̔�\��
	SetUseDXNetWorkProtocol(false);					// �\�P�b�g�ʐM�v���g�R���w��

	// �`��֌W
	InitLayer();									// ���C���̏�����
	InitAlpha();									// ���u�����h�̏�����
	InitTask();										// �^�X�N�̏�����
	InitImg();										// �摜�̏�����

	// ���s
	RunD("MAIN");									// CALL MAIN

	// DxLib�̉��
	DxLib_End();

	Cleanup();

	return 0;

}