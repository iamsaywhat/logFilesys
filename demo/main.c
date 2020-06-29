#include <stdio.h>
#include "log.fs.h"
#include "log.fs.platformdepend.h"


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
	uint16_t temp;
	LogFs_Status status;
	uint64_t space;

	TimestampToDate(1203161493);
	TimestampToDate(1562279506);
	for (i = 0; i < 15; i++)
	{
		buffer[i] = i;
	}

	LogFs_format();

	///
	LogFs_findFileByNum(7);

	status = LogFs_initialize();
	status = LogFs_check();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);

	space = LogFs_freeBytes();
	space = LogFs_fullSize();
	LogFs_findFileByNum(0);
	LogFs_readFile(buffer, 0, 5);


	LogFs_initialize();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);


	LogFs_initialize();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);


	LogFs_initialize();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);

	LogFs_initialize();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);

	///
	LogFs_initialize();
	LogFs_findFileByNum(3);

	LogFs_initialize();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);
	/////// пересекли границу

	LogFs_initialize();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);


	LogFs_initialize();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);

	LogFs_initialize();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);
	LogFs_writeToCurrentFile(buffer, 5);

	LogFs_initialize();
	LogFs_createFile();
	temp = LogFs_getFileNumber();
	temp = LogFs_getLastFileId();
	temp = LogFs_getCurrentFileId();
	LogFs_writeToCurrentFile(buffer, 5);

	LogFs_initialize();
	LogFs_findFile(FIRST_FILE);
	while (FS_ALL_FILES_SCROLLS != LogFs_findFile(NEXT_FILE));



	for (i = 0; i < 100; i++)
	{
		buffer[i] = 0;
	}



	// Тестирование чтения
	LogFs_findFile(FIRST_FILE);
	size = LogFs_getFileProperties(FILE_SIZE);
	LogFs_readFile(buffer, 0, size - 7);
	LogFs_readFile(buffer, 0, 7);
	for (i = 0; i < 100; i++) buffer[i] = 0;


	LogFs_findFile(NEXT_FILE);
	size = LogFs_getFileProperties(FILE_SIZE);
	LogFs_readFile(buffer, 0, size);
	for (i = 0; i < 100; i++) buffer[i] = 0;

	LogFs_findFile(NEXT_FILE);
	size = LogFs_getFileProperties(FILE_SIZE);
	LogFs_readFile(buffer, 0, size);
	for (i = 0; i < 100; i++) buffer[i] = 0;


	LogFs_findFile(NEXT_FILE);
	size = LogFs_getFileProperties(FILE_SIZE);
	LogFs_readFile(buffer, 0, size);
	for (i = 0; i < 100; i++) buffer[i] = 0;


	LogFs_findFile(8);
	LogFs_readFile(buffer, 0, size);
	for (i = 0; i < 100; i++) buffer[i] = 0;


	/////////////////////
	LogFs_findFile(FIRST_FILE);
	LogFs_findFile(NEXT_FILE);
	size = LogFs_getFileProperties(FILE_SIZE);
	
	LogFs_readFile(buffer, 0, size);
	for (i = 0; i < 100; i++) buffer[i] = 0;

	LogFs_readFile(buffer, 8, 5);
	for (i = 0; i < 100; i++) buffer[i] = 0;

	LogFs_readFile(buffer, 21, 0);
	for (i = 0; i < 100; i++) buffer[i] = 0;

	LogFs_readFile(buffer, size - 1, 1);
	for (i = 0; i < 100; i++) buffer[i] = 0;

}


