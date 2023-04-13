#pragma once
#define Player_Address (0x400000+0x18AC00)
#define ENTITY_Address (0x400000+0x191FCC)
#define ENTITY_COUNT (ENTITY_Address+0x08)
#define MAX_ARRAY_LENGTH 16
// my player  angle 0x34
uintptr_t p_player;
uintptr_t entityObject;
uintptr_t p_entityArray;
uintptr_t p_player1;
float anglee = 0;
float anglee2 = 0;
HANDLE hProcess;
int entity_count = 0;
