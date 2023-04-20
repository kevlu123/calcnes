/*****************************************************************/
/*                                                               */
/*   CASIO fx-9860G SDK Library                                  */
/*                                                               */
/*   File name : [ProjectName].c                                 */
/*                                                               */
/*   Copyright (c) 2006 CASIO COMPUTER CO., LTD.                 */
/*                                                               */
/*****************************************************************/
#include "io.h"
#include "emulator.h"
#include "controller.h"

extern int colour;

extern "C" {
#include "fxlib.h"
#include <stdlib.h>
#include <string.h>

int Bkey_GetKeyWait(
	int* k1,
	int* k2,
	int sel,
	int timeout,
	int menu,
	short* unused
);

static void PrintNum(int num) {
    for (int i = 0; i < sizeof(int); i++) {
        unsigned char s[2] = {
            (unsigned char)(num % 10 + '0'),
            '\0'
        };
        Print(s);
        num /= 10;
    }
}

static bool GetKeyDown(unsigned int key)
{
	short unused;
	int k1, k2;
	int status = Bkey_GetKeyWait(
		&k1,
		&k2,
		KEYWAIT_HALTOFF_TIMEROFF,
		0,
		0,
		&unused
	);

	return (status == KEYREP_KEYEVENT)
		&& (k1 == key / 10)
		&& (k2 == key % 10 + 1);
}

#define KEY_EXE 31
#define KEY_UP 28
#define KEY_DOWN 37
#define KEY_LEFT 38
#define KEY_RIGHT 27
#define KEY_F1 79
#define KEY_F2 69
#define KEY_F3 59
#define KEY_F4 49
#define KEY_F6 29

//****************************************************************************
//  AddIn_main (Sample program main function)
//
//  param   :   isAppli   : 1 = This application is launched by MAIN MENU.
//                        : 0 = This application is launched by a strip in eACT application.
//
//              OptionNum : Strip number (0~3)
//                         (This parameter is only used when isAppli parameter is 0.)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************
int AddIn_main(int isAppli, unsigned short OptionNum)
{
    Emulator* emu = (Emulator*)mymalloc(sizeof(Emulator));
    if (emu == NULL || InitEmu(emu)) {
        locate(0, 1);
        Print((unsigned char*)"Malloc failed");
        locate(0, 2);
        Print((unsigned char*)"Allocated ");
        PrintNum(allocated);
        locate(0, 3);
        Print((unsigned char*)"Tried ");
        PrintNum(lastMalloc);

        unsigned int k;
        GetKey(&k);
        return 0;
    }
    
    int frameNumber = 0;
    bool f6WasDown = false;
    DISPGRAPH graph;
    graph.WriteKind = IMB_WRITEKIND_OVER;
    graph.WriteModify = IMB_WRITEMODIFY_NORMAL;
    graph.x = 0;
    graph.y = 0;
    graph.GraphData.width = 128;
    graph.GraphData.height = 64;
    graph.GraphData.pBitmap = emu->ppu.screen;

    while (!GetKeyDown(KEY_EXE)) {
        frameNumber++;

        emu->mem.joy1.status = 0;
        if (GetKeyDown(KEY_UP)) emu->mem.joy1.status |= UP;
        if (GetKeyDown(KEY_DOWN)) emu->mem.joy1.status |= DOWN;
        if (GetKeyDown(KEY_LEFT)) emu->mem.joy1.status |= LEFT;
        if (GetKeyDown(KEY_RIGHT)) emu->mem.joy1.status |= RIGHT;
        if (GetKeyDown(KEY_F1)) emu->mem.joy1.status |= BUTTON_B;
        if (GetKeyDown(KEY_F2)) emu->mem.joy1.status |= BUTTON_A;
        if (GetKeyDown(KEY_F3)) emu->mem.joy1.status |= SELECT;
        if (GetKeyDown(KEY_F4)) emu->mem.joy1.status |= START;

        bool f6 = GetKeyDown(KEY_F6);
        if (f6 && !f6WasDown) colour = 1 - colour;
        f6WasDown = f6;

        memset(emu->ppu.screen, 0, sizeof(emu->ppu.screen));
        TickEmu(emu);
        Bdisp_WriteGraph_DD(&graph);
        Bdisp_SetPoint_DD(0, 62, frameNumber % 2);
    }

    QuitEmu(emu);
    return 1;
}




//****************************************************************************
//**************                                              ****************
//**************                 Notice!                      ****************
//**************                                              ****************
//**************  Please do not change the following source.  ****************
//**************                                              ****************
//****************************************************************************


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section


#pragma section _TOP

//****************************************************************************
//  InitializeSystem
//
//  param   :   isAppli   : 1 = Application / 0 = eActivity
//              OptionNum : Option Number (only eActivity)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************
int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section

}