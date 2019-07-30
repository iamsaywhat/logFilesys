#include <stdio.h>
#include "Log_FS.h"
#include "Log_FS_llio.h"


uint8_t MEMORY[FS_SECTORS_NUM * FS_SECTOR_SIZE];


void TimestampToDate(long timestamp)
{
	char m[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	char i = 0;

	int Year, Month, Day, Hours, Minutes, Seconds;

	int Step1 = (int)(timestamp / 31436000)+ 1970;

	int Step2 = (int)((Step1 - 1969)/4); 

	int Step3 = (int)(timestamp / 86400);

	int Step4 = (int)(Step3 - Step2);

	int Step5 = Step4 % 365;


	if (Step1 % 4 == 0)
	{
		if (Step1 % 100 != 0)
		{
			if (Step5 > 59)
				Step5 += 1;
		}

		else if (Step1 % 400 == 0)
		{
			if (Step5 > 59)
				Step5 += 1;
		}
	}

	int Step6 = Step5;
	for (i = 0; i < 12; i++)
	{
		if (Step6 > m[i])
			Step6 -= m[i];
		else
			break;
	}

	int Step7 = (int)(timestamp - (Step3 * 86400));

	int Step8 = (int)(Step7 / 3600);

	int Step9 = (int)(Step7 - (Step8 * 3600));

	int Step10 = (int)(Step9 / 60);

	int Step11 = (int)(Step9 - (Step10 * 60));

	Year = Step1;
	Month = i + 1;
	Day = Step6;
	Hours = Step8;
	Minutes = Step10;
	Seconds = Step11;

}


int main()
{
	uint8_t buffer[100];
	int i = 0;
	uint8_t flag = 0;
	uint16_t count = 0;
	uint8_t res;
	uint32_t size;

	TimestampToDate(1203161493);
	TimestampToDate(1562279506);
	for (i = 0; i < 15; i++)
	{
		buffer[i] = i;
	}

	LogFs_Formatting();

	///
	Log_Fs_FindFile_ByNum(7);

	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);


	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);


	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);


	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);

	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);

	///
	LogFs_Info();
	Log_Fs_FindFile_ByNum(3);

	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);
	/////// пересекли границу

	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);


	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);

	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);
	LogFs_WriteToCurrentFile(buffer, 5);

	LogFs_Info();
	LogFs_CreateFile();
	LogFs_WriteToCurrentFile(buffer, 5);

	LogFs_Info();
	Log_Fs_FindFIle(FIRST_FILE);
	while (FS_ALL_FILES_SCROLLS != Log_Fs_FindFIle(NEXT_FILE));



	for (i = 0; i < 100; i++)
	{
		buffer[i] = 0;
	}



	// Тестирование чтения
	Log_Fs_FindFIle(FIRST_FILE);
	size = Log_Fs_GetFileProperties(FILE_SIZE);
	LogFs_ReadFile(buffer, size - 7);
	LogFs_ReadFile(buffer , 7);
	for (i = 0; i < size - 7; i++)
	{
		printf("%d", buffer[i]);
	}
	printf("\n");
	for (i = 0; i < 100; i++)
	{
		buffer[i] = 0;
	}


	Log_Fs_FindFIle(NEXT_FILE);
	size = Log_Fs_GetFileProperties(FILE_SIZE);
	LogFs_ReadFile(buffer, size);
	for (i = 0; i < size; i++)
	{
		printf("%d", buffer[i]);
	}
	printf("\n");
	for (i = 0; i < 100; i++)
	{
		buffer[i] = 0;
	}

	Log_Fs_FindFIle(NEXT_FILE);
	size = Log_Fs_GetFileProperties(FILE_SIZE);
	LogFs_ReadFile(buffer, size);
	for (i = 0; i < size; i++)
	{
		printf("%d", buffer[i]);
	}
	printf("\n");
	for (i = 0; i < 100; i++)
	{
		buffer[i] = 0;
	}


	Log_Fs_FindFIle(NEXT_FILE);
	size = Log_Fs_GetFileProperties(FILE_SIZE);
	LogFs_ReadFile(buffer, size );
	for (i = 0; i < size; i++)
	{
		printf("%d", buffer[i]);
	}
	printf("\n");
	for (i = 0; i < 100; i++)
	{
		buffer[i] = 0;
	}


	Log_Fs_FindFile_ByNum(8);
	LogFs_ReadFile(buffer, size);
	for (i = 0; i < size; i++)
	{
		printf("%d", buffer[i]);
	}
	printf("\n");
	for (i = 0; i < 100; i++)
	{
		buffer[i] = 0;
	}
}


