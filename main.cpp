#include<windows.h>
#include<commctrl.h>
#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME)&0x8000)?1:0)
#define abs(a,b) ((a)>(b)?(a)-(b):(b)-(a))
using namespace std;

const bool AI=false;	//AI mode
const double g=12.;		//gravity constant
const int UP=11;		//jump constant 1
const int BIG=9000;
const double JUMP=1.2;	//jump constant 2
const int size=50;
const int K=100;
int SPEED=8;			//scroll speed
const int GS=1;

double v=0;
bool an=false;
int center;
bool playing=false;
int tx;
int W,H;
int sc=0;
char tmp[500];
int X=0,Y=0;

void jump()
{
	v=-JUMP;
}

DWORD WINAPI mouse(LPVOID args)
{
	bool Key_ev;
	while(true)
	{
		Key_ev=KEY_DOWN(MOUSE_MOVED);
		while(!playing)
			Sleep(20);
		if(!Key_ev&&an) 
			an=false;
		else if(!an&&Key_ev)
		{
			an=true;
			jump();
		}
		Sleep(10);
	}
}

bool ai(int x,int y,int ox,int oy)
{
	if(ox<W/16-size/2)
	{
		if(H/2<y)
			return true;
		return false;
	}
	if(oy<y)
		return true;
	return false;
}

int main()
{
	HWND hwndParent=FindWindow("Progman","Program Manager"); //for win xp
	HWND hwndSHELLDLL_DefView=FindWindowEx(hwndParent,NULL,"SHELLDLL_DefView",NULL);
	if(hwndSHELLDLL_DefView==NULL)
	{
		hwndParent=FindWindowEx(NULL,NULL,"WorkerW",NULL);   //for win 7,8,10
		while ((!hwndSHELLDLL_DefView)&&hwndParent)
		{
			hwndSHELLDLL_DefView=FindWindowEx(hwndParent,NULL,"SHELLDLL_DefView",NULL);
			hwndParent=FindWindowEx(NULL,hwndParent,"WorkerW",NULL);
		}
	}
	HWND hwndSysListView32=FindWindowEx(hwndSHELLDLL_DefView,NULL,"SysListView32","FolderView");
	int Nm=ListView_GetItemCount(hwndSysListView32);
	CreateThread(NULL,0,mouse,NULL,0,0);
	HWND hDW=GetDesktopWindow();
	HDC hDC=GetWindowDC(hDW);
	W=GetDeviceCaps(hDC,DESKTOPHORZRES);
	H=GetDeviceCaps(hDC,DESKTOPVERTRES);
	srand(time(NULL));
	int ox=W/16,oy=H/2;
	center=rand()%(Nm-GS)+1;
	tx=W/16*15;
	double s=(H-K*2)/(Nm-1);
	int lll=W/16;
	playing=true;
		while(true)
		{
			if(rand()%256==0) SPEED++;
			oy+=BIG*v/2000;
			v+=UP*g/1000;
			if(oy+50>H||oy<0)
				oy-=BIG*v/2000,v=0;
			for(int i=0;i<center;i++)
				SendMessage(hwndSysListView32,LVM_SETITEMPOSITION,i+GS,MAKELPARAM(tx,i*s));
			for(int i=Nm-1;i>center;i--)
				SendMessage(hwndSysListView32,LVM_SETITEMPOSITION,i,MAKELPARAM(tx,(i-1)*s+2*K));
			for(int i=0;i<GS;i++)
				SendMessage(hwndSysListView32,LVM_SETITEMPOSITION,i,MAKELPARAM(ox,oy));
			ListView_RedrawItems(hwndSysListView32,0,Nm-1);
			UpdateWindow(hwndSysListView32);
			Sleep(UP);
			if((abs(ox,tx)<size&&(oy<=s*center||oy>=s*center+2*K-size))||(oy>=(Nm-2)*s+2*K))
			{
				playing=false;
				sprintf(tmp,"您得了%d分",sc);
				int ret=MessageBox(NULL,tmp,"提示",MB_RETRYCANCEL);
				if(ret==IDCANCEL)
					return 0;
				tx=W/16*15;
				ox=W/16,oy=H/2;
				sc=0;
				SPEED=8;
				center=rand()%(Nm-GS)+1;
				playing=true;
				continue;
			}
			tx-=SPEED;
			if(tx<lll)
			{
				tx=W/16*15;
				center=rand()%(Nm-GS)+1;
				sc++;
			}
			X=tx-ox;
			Y=s*center+K-oy;
			if(AI)
				if(ai(ox,oy,tx,s*center+K))
					jump();
		}
	return 0;
}
