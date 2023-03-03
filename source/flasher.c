#include <sys/types.h>
#include <libcd.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>

#define NULL 0
#define TRUE 1 
#define FALSE 0

#define DATA_BUFF (u_char *)0x80100000

#define BIOS_CE		0
#define EXT_CE		1

DRAWENV  draw[2];        
DISPENV  disp[2];        
long font;
int id = 0;

#define AMD_ID 0x01
#define ATM_ID 0x1F
#define SST_ID 0xBF
#define WIN_ID 0xDA

#define AMD_29F040B  0xA4
#define ATM_29C040A  0xA4
#define SST_28SF040  0x04
#define SST_39SF040  0xB7
#define SST_39VF040  0xD7
#define WIN_29C040   0x46

typedef struct
{
	u_char  cnt;
	u_short addr[8];
	u_char  cmd[8];
} E_CMD;

typedef struct
{
	u_char id1;
	u_char id2;
	u_int  romsize;
	u_int  BPS; // Bytes Per Sector
	u_char *base;
	char   desc[50];
	E_CMD  erase;
	E_CMD  program;
} EEPROM;

#define IC_CNT 6

EEPROM EepromArray[IC_CNT] = 
{ 
	{
		AMD_ID, AMD_29F040B,  512, 001, NULL, "AMD 29F040B",
		6, 
		0x555, 0x2AA, 0x555, 0x555, 0x2AA, 0x555, 0, 0,
		0xAA, 0x55, 0x80, 0xAA, 0x55, 0x10, 0, 0,
		3,
		0x555, 0x2AA, 0x555, 0, 0, 0, 0, 0,
		0xAA, 0x55, 0xA0, 0, 0, 0, 0, 0
	},
	{
		ATM_ID, ATM_29C040A,  512, 256, NULL, "ATMEL 29C040A",
		6, 
		0x5555, 0x2AAA, 0x5555, 0x5555, 0x2AAA, 0x5555, 0, 0,
		0xAA, 0x55, 0x80, 0xAA, 0x55, 0x10, 0, 0,
		3,
		0x5555, 0x2AAA, 0x5555, 0, 0, 0, 0, 0,
		0xAA, 0x55, 0xA0, 0, 0, 0, 0, 0
	},
	{
		SST_ID, SST_28SF040,  512, 001, NULL, "SST 28SF040",
		2,
		0, 0, 0, 0, 0, 0, 0, 0,
		0x30, 0x30, 0, 0, 0, 0, 0, 0,
		1,
		0, 0, 0, 0, 0, 0, 0, 0,
		0x10, 0, 0, 0, 0, 0, 0, 0
	},
	{
		SST_ID, SST_39SF040,  512, 001, NULL, "SST 39SF040",
		6, 
		0x5555, 0x2AAA, 0x5555, 0x5555, 0x2AAA, 0x5555, 0, 0,
		0xAA, 0x55, 0x80, 0xAA, 0x55, 0x10, 0, 0,
		3,
		0x5555, 0x2AAA, 0x5555, 0, 0, 0, 0, 0,
		0xAA, 0x55, 0xA0, 0, 0, 0, 0, 0
	},
	{
		SST_ID, SST_39VF040,  512, 001, NULL, "SST 39VF040",
		6,
		0x5555, 0x2AAA, 0x5555, 0x5555, 0x2AAA, 0x5555, 0, 0,
		0xAA, 0x55, 0x80, 0xAA, 0x55, 0x10, 0, 0,
		3,
		0x5555, 0x2AAA, 0x5555, 0, 0, 0, 0, 0,
		0xAA, 0x55, 0xA0, 0, 0, 0, 0, 0
	},
	{
		WIN_ID, WIN_29C040,   512, 256, NULL, "WINBOND 29C040",
		6, 
		0x5555, 0x2AAA, 0x5555, 0x5555, 0x2AAA, 0x5555, 0, 0,
		0xAA, 0x55, 0x80, 0xAA, 0x55, 0x10, 0, 0,
		3,
		0x5555, 0x2AAA, 0x5555, 0, 0, 0, 0, 0,
		0xAA, 0x55, 0xA0, 0, 0, 0, 0, 0
	}	
};

EEPROM cur_ic = 
{ 
	0, 0, 0, 0, NULL, "Not detected", 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

#define ROM_CNT 1
int cur_romsel = 0;

char rom_list[ROM_CNT][16] =
{
	{ "PSX-XBOO.BIN" }
};

void DelayMs(u_long ms)
{
	u_long x,y;
	
	for (x=0; x < ms; x++);
	
	for (y=0; y < 500; y++);
}

void DrawDisp()
{
	id = id ? 0: 1;
	
	if (draw[id].dfe)
	{ 
		DrawSync(0); 
		VSync(0); 
	}
	else
	{ 
		VSync(0); 
		ResetGraph(1); 
	}
	
	PutDrawEnv(&draw[id]);
	PutDispEnv(&disp[id]);
	FntFlush(font);
}

void Progress(int x, int y, int w, int h, int value)
{
	TILE t;
	char d = 2;
	
	SetTile (&t);
	setXY0 (&t, x, y); 
	
	t.w = w; 
	t.h = h;
	
	setRGB0 (&t,255,255,255);
	DrawPrim (&t);
	
	setXY0 (&t,x+d,y+d); 
	
	t.w = w-d*2; 
	t.h = h-d*2;
	
	setRGB0 (&t,0,0,0);
	DrawPrim (&t);
	
	setXY0 (&t,x+d,y+d); 
	
	t.w = (w-d*2)*value/100; 
	t.h = h-d*2;
	
	setRGB0 (&t,255,128,64);
	DrawPrim (&t);
}

void InitDisp(int w, int h)
{
	if (*(char *)0xbfc7ff52 == 'E')
	{
		SetVideoMode(MODE_PAL);
	}
	else
	{
		SetVideoMode(MODE_NTSC);
	}
	
	ResetGraph(0);
	
	SetDefDrawEnv(&draw[0], 0, 0, w, h);
	SetDefDrawEnv(&draw[1], 0, h, w, h);
	SetDefDispEnv(&disp[0], 0, h, w, h);
	SetDefDispEnv(&disp[1], 0, 0, w, h);
	
	draw[0].isbg = draw[1].isbg = 1;
	
	setRGB0(&draw[0], 0, 0, 0);
	setRGB0(&draw[1], 0, 0, 0); 
	
	FntLoad(960, 256);
	font = FntOpen(16, 32, w, h, 0, 2048);
	SetDumpFnt(font);
	SetDispMask(1);
}

void WaitScreen (char *text)
{
	u_long padRead;
	
	do
	{
		padRead = PadRead(0);
		FntPrint(font, text);
		FntPrint(font, "\n\nPress <X> to continue");
		DrawDisp();
	} 
	while (!(padRead & PADRdown));
	
	while (PadRead(0) & PADRdown);
}

int Check_Toggle_Ready (u_char *Dst)
{
	u_char Loop = TRUE, PreData, CurrData;
	u_long TimeOut = 0;
	
	PreData = *Dst & 0x40;
	
	while ((TimeOut< 0x07FFFFFF) && (Loop))
	{
		CurrData = *Dst & 0x40;
		
		if (PreData == CurrData)
		{
			Loop = FALSE;
		}
		
		PreData = CurrData;
		TimeOut++;
	}
	
	if (TimeOut==0x07FFFFFF) 
	{
		return -1;
	}
	
	return 0;
}

void detect_ic(int memsel)
{
	u_char *base = (memsel == BIOS_CE) ? (u_char *) 0x1FC00000 : (u_char *) 0x1F000000;
	u_char rom_id[2];
	int i;
	
	base[0x5555] = 0xAA;
	base[0x2AAA] = 0x55;
	base[0x5555] = 0x90;
	DelayMs(100);
	
	rom_id[0] = base[0];
	rom_id[1] = base[1];
	
	if (rom_id[0] != 0xBF && rom_id[0] != 0x04)
	{
		base[0x5555] = 0xAA;
		base[0x2AAA] = 0x55;
		base[0x5555] = 0xF0;
	}
	else
	{
		base[0] = 0xFF;
		
		DelayMs(100);
		
		(void) base[0x1823];
		(void) base[0x1820];
		(void) base[0x1822];
		(void) base[0x0418];
		(void) base[0x041b];
		(void) base[0x0419];
		(void) base[0x041a];
	}
	
	DelayMs(100);
	
	for (i = 0; i < IC_CNT; i++)
	{
		if (EepromArray[i].id1 == rom_id[0] &&
			EepromArray[i].id2 == rom_id[1])
		{
			memcpy(&cur_ic, &EepromArray[i], sizeof(EEPROM));
			cur_ic.base = base;
			break;
		}
	}
	
	if (i == IC_CNT)
	{
		memset(&cur_ic, 0, sizeof(EEPROM));
		sprintf(cur_ic.desc, "Unknown IC %02X %02X", rom_id[0], rom_id[1]);
	}
}

int confirm_screen(const char *msg)
{
	u_long padRead;
	
	while (1)
	{
		padRead = PadRead(0);
		
		FntPrint(font,  msg);
		
		FntPrint(font, "      START - confirm, O - cancel\n");
		
		DrawDisp();
		
		if (padRead & PADstart)
		{
			while (PadRead(0) & PADstart);
			return 1;
		}
		else if(padRead & PADRright)
		{
			while (PadRead(0) & PADRright);
			break;
		}
	}
	
	return 0;
}

int LoadROM(char *name)
{
	CdlFILE cdi;
	char result;
	
	FntPrint(font, "      Read %s from CD\n", name);
	DrawDisp();
	DrawDisp();
	
	if (CdSearchFile(&cdi, name) == NULL)
	{
		return 0;
	}
	
	CdControl(CdlSetloc, (u_char*)&cdi.pos, 0);
	CdSync(0, &result);
	CdRead(256, (u_long*) DATA_BUFF, 0);
	CdReadSync(0, &result);
	
	return 1;
}

int WriteIC()
{
	u_char *base = cur_ic.base,
			*src = DATA_BUFF;
	u_int i, j, len, progress;
	
	FntPrint(font ,"\n\n\n                       ERASE EEPROM\n");
	DrawDisp();
	DrawDisp();
	
	for (j = 0; j < cur_ic.erase.cnt; j++)
	{
		base[cur_ic.erase.addr[j]] = cur_ic.erase.cmd[j];
	}
	
	DelayMs(1);
	
	if (Check_Toggle_Ready(base))
	{
		return -1;
	}
	
	len = (cur_ic.romsize * 1024) / cur_ic.BPS;
	
	for (i = 0; i < len; i += cur_ic.BPS)
	{
		progress = (i*100/len)+1;
			
		if (!(progress % 10))
		{
			FntPrint(font ,"\n\n\n                       UPDATING EEPROM");
			Progress(50, 80, 412, 20, progress);
			DrawDisp();
		}
		
		for (j = 0; j < cur_ic.program.cnt; j++)
		{
			base[cur_ic.program.addr[j]] = cur_ic.program.cmd[j];
		}
		
		for (j = 0; j < cur_ic.BPS; j++)
		{
			base[i+j] = src[i+j];
		}
		
		DelayMs(1);
		
		if (Check_Toggle_Ready(&base[i]))
		{
			return -1;
		}
	}
	
	return 0;
}

void Select_ROM()
{
	u_long padRead;
	int i;
	char temp[128];
	
	while(1)
	{
		padRead = PadRead(0);
		
		FntPrint(font, "         Select ROM file\n\n");
		
		for (i = 0; i < ROM_CNT; i++)
		{
			if (i == cur_romsel)
			{
				FntPrint(font, "   >>>%s<<<\n", rom_list[i]);
			}
			else
			{
				FntPrint(font, "   %s\n", rom_list[i]);
			}
		}
		
		FntPrint(font, "\nUP/DOWN - navigate, X - confirm, O - cancel\n");
		
		if (padRead & PADRdown)
		{
			while (PadRead(0) & PADRdown);
			// write flash
			
			sprintf(temp, "\n  write %s to %s?\n\n", rom_list[cur_romsel], cur_ic.desc);
			
			if (confirm_screen(temp))
			{
				sprintf(temp, "\\%s;1", rom_list[cur_romsel]);
				
				if (LoadROM(temp))
				{
					if (WriteIC())
					{
						sprintf(temp, "Error: Can't write %s", cur_ic.desc);
						WaitScreen(temp);
					}
				}
				else
				{
					sprintf(temp, "Error: Can't read %s", rom_list[cur_romsel]);
					WaitScreen(temp);
				}
			}
		}
		else if (padRead & PADRright)
		{
			return;
		}
		else if (padRead & PADLup)
		{
			cur_romsel--;
			
			if (cur_romsel < 0)
			{
				cur_romsel = ROM_CNT - 1;
			}
			
			while (PadRead(0) & PADLup);
		}
		else if (padRead & PADLdown)
		{
			cur_romsel++;
			
			if (cur_romsel >= ROM_CNT)
			{
				cur_romsel = 0;
			}
			
			while (PadRead(0) & PADLdown);
		}
		
		DrawDisp();
	}
}

int main()
{
	u_long padRead;
	
	CdInit();
	InitDisp(512, 240);
	PadInit(0);
	
	while(1)
	{
		padRead = PadRead(0);
		
		FntPrint(font, "         BIOS Flasher 0.1\n\n");
		FntPrint(font, "L1 - Detect IC on BIOS /CE\n");
		FntPrint(font, "R1 - Detect IC on EXP1 /CE\n");
		
		if (padRead & PADL1)
		{
			detect_ic(BIOS_CE);
			
			while (PadRead(0) & PADL1); 
		}
		else if (padRead & PADR1)
		{
			detect_ic(EXT_CE);
			
			while (PadRead(0) & PADR1); 
		}
		
		FntPrint(font, "\n%s\n", cur_ic.desc);
		
		if (cur_ic.romsize == 512)
		{
			FntPrint(font, "\npress X for select BIOS rom\n");
			
			if (padRead & PADRdown)
			{
				while (PadRead(0) & PADRdown);
				
				Select_ROM();
				
				memset(&cur_ic, 0, sizeof(EEPROM));
				sprintf(cur_ic.desc, "Not detected");
			}
		}
		
		DrawDisp();
	}
	
	return 0;
}

