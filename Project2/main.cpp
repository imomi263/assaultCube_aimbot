#include<Windows.h>
#include<iostream>
#include<math.h>
#include<algorithm>
#include<vector>
#include "header.h"
using namespace std;

#define F6_key 0x75
#define Right_Mouse 0x02

struct MyPlayer {
	DWORD player;
	int team;
	int health;
	float Position[3];

	void ReadInfo(){
		ReadProcessMemory(hProcess, (LPCVOID)(p_player + 0x00EC), &health, sizeof(int), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)(p_player + 0x0028), &Position, sizeof(Position), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)(p_player + 0x030C), &team, sizeof(int), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)(p_player + 0x0034), &anglee, sizeof(float), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)(p_player + 0x0038), &anglee2, sizeof(float), NULL);
	}

}Myplayer;
struct TargetList {
	float Distance;
	float AimbotAngle[3];
	TargetList() {

	}
	TargetList(float aimbotangle[],float myCoords[],float enemyCoords[]) {
		Distance = Get3dDistance(myCoords[0], myCoords[1], 
			myCoords[2], enemyCoords[0], enemyCoords[1], enemyCoords[2]);
		AimbotAngle[0] = aimbotangle[0];
		AimbotAngle[1] = aimbotangle[1];
		AimbotAngle[2] = aimbotangle[2];
	}
	float Get3dDistance(float myCoordsX,float myCoordsY,float myCoordsZ,
		float EnemyX, float EnemyY, float EnemyZ) {
		return sqrt((myCoordsX - EnemyX) * (myCoordsX - EnemyX) + 
					(myCoordsZ - EnemyZ) * (myCoordsZ - EnemyZ) + 
			        (myCoordsY - EnemyY) * (myCoordsY - EnemyY));
	}
};
struct PlayerList {
	DWORD CBaseEntity;
	int team;
	int health;
	float Position[3];

	float AimBotAngle[3];
	void ReadInfo(int player) {
		ReadProcessMemory(hProcess, (LPCVOID)(p_entityArray+ 0x4 * player), &entityObject, sizeof(int), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)(entityObject + 0x00EC), &health, sizeof(int), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)(entityObject + 0x030C), &team, sizeof(int), NULL);
		ReadProcessMemory(hProcess, (LPCVOID)(entityObject + 0x0028), &Position, sizeof(Position), NULL);

	}
}PlayerList1[16];

struct CompareTarget {
	bool operator () (TargetList & a, TargetList & b) {
		return a.Distance < b.Distance;
	}
};
void CalculateAngle(float * src,float *dst,float* angle){

	double delta[3] = { (src[0] - dst[0]),(src[1] - dst[1]),(src[2] - dst[2]) };
	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]
		 );
	angle[0] = (float)(abs(asinf(delta[1] / hyp)) * 57.295779513082f);
	angle[1] = (float)(abs(atanf(delta[2] / hyp)) * 57.295779513082f);
	angle[2] = (float)0;
	//if (delta[2] >= 0.0) {
	
	
	if (delta[0] >= 0 && delta[1] >= 0) {
		angle[0] = 270 + angle[0];
	}
	if (delta[0] < 0 && delta[1] >= 0) {
		angle[0] = 90 - angle[0];
	}
	if (delta[0] >= 0 && delta[1] < 0) {
		angle[0] = 270 - angle[0];
	}
	if (delta[0] < 0 && delta[1] < 0) {
		angle[0] = 90 + angle[0];
	}
	/*if (anglee2 >= angle[1])
		angle[1] = anglee2 - angle[1];
	else {
		angle[1] = (90 + anglee2 - angle[1]);
	}*/
	//}
	if (delta[2] > 0) {
		angle[1] = -angle[1];
	}

}
void Aimbot() {
	TargetList* Target = new TargetList[entity_count];
	int targetLoop = 0;
	for (int i = 0; i <entity_count ; i++) {
		PlayerList1[i].ReadInfo(i);

		if (PlayerList1[i].team == Myplayer.team) {
			continue;
		}
		if (PlayerList1[i].health <= 0) {
			continue;
		}
		CalculateAngle(Myplayer.Position, PlayerList1[i].Position
			, PlayerList1[i].AimBotAngle);
		Target[targetLoop++] = TargetList(PlayerList1[i].AimBotAngle
			, Myplayer.Position, PlayerList1[i].Position);
		
	}
	if (targetLoop > 0) {
		std::sort(Target, Target + targetLoop, CompareTarget());
		//if (GetAsyncKeyState(0x1)) {
			//WriteProcessMemory()

			//(PBYTE*)转角度的地址
			//TargetList[0].AimbotAngle,12,0
			WriteProcessMemory(hProcess, (PBYTE*)(p_player + 0x034), Target[0].AimbotAngle, 12, NULL);
		//}
	}
	targetLoop = 0;
	delete[] Target;
}
int main() {
	HWND hwnd = FindWindow(NULL, L"AssaultCube");
	if (!hwnd) {
		printf("没找到程序");
	}
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,pid);
	
	ReadProcessMemory(hProcess, (LPCVOID)Player_Address, &p_player, sizeof(int), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)ENTITY_Address, &p_entityArray, sizeof(int), NULL);
	ReadProcessMemory(hProcess, (LPCVOID)ENTITY_COUNT, &entity_count, sizeof(int), NULL);
	
	while (1) {
		if (GetAsyncKeyState(VK_RBUTTON) == 0) {
			Myplayer.ReadInfo();
			Aimbot();
		}
	}
	return 0;
}