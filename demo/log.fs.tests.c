#include "log.fs.tests.h"
#include "log.fs.h"
#include "log.fs.platformdepend.h"
#include "string.h" 


uint8_t MEMORY[FS_SECTORS_NUM * FS_SECTOR_SIZE];
uint8_t buffer[FS_SECTOR_SIZE];
uint8_t writeBuffer[FS_SECTORS_NUM * FS_SECTOR_SIZE];
uint8_t readBuffer[FS_SECTORS_NUM * FS_SECTOR_SIZE];
uint8_t* text = "The most delicious dumplings out of dough in the boiling water. Boil the water and pour a Cup of boiling"
"water in a deep bowl.Add a teaspoon of salt and slowly add pre - sifted through a sieve flour.Knead by hand a soft dough."
"A wooden Board dusted with flour and knead the dough until it stops sticking to your hands.Roll into a ball, cover with a"
"towel made of natural fabricsand give 20 minutes to stand up. During this time prepare the meat filling.Through a meat"
"grinder to crank out beef and pork, and then again with onionand garlic.Received minced saltand pepper pritrusit."
"All the ingredients are thoroughly mixed. In the bowl of stuffing to pour so much milk that it was not too thick,"
"but did not spread. The rested dough is divided into 4 parts.Roll out long \"sausages\".Each divided into 15 parts"
"and roll it out thinly.In the middle, ready to put the stuffingand sumipntg edge.Tips to keep each otherand a good"
"squeeze, giving the classic dumpling shape. Boil waterand put the first batch of products in the boiling water. "
"Cook for 6 - 8 minutes on high heat.Serves homemade dumplings with butter and sour cream.";

void putTextToBuffer(uint8_t *_buffer, int size)
{
	int len = strlen(text);
	static int textIndex = 0;
	for (int i = 0; i < size; i++, textIndex++)
	{
		textIndex %= len;
		_buffer[i] = text[textIndex];
	}
}

LogFsTestStatus LogFs_plarformdependTest(void)
{
	uint32_t address;
	LogFsTestStatus status = PASSED;
	
	printf("log.fs.tests: LogFs_plarformdependTest......started!\n");

	eraseChip();
	for (int i = 0; i < FS_SECTORS_NUM; i++)
	{
		address = FS_SECTOR_SIZE * i;
		readMemory(address, buffer, FS_SECTOR_SIZE);
		if (buffer[i] != 0xFF)
		{
			printf(" *  The chip is not erased by eraseChip()!\n");
			status = FAILED;
			break;
		}
	}

	for (int j = 0; j < FS_SECTORS_NUM; j++)
	{
		address = j * FS_SECTOR_SIZE;
		for (int i = 0; i < (FS_SECTOR_SIZE); i++)
			buffer[i] = (uint8_t)(i + j);
		writeMemory(address, buffer, FS_SECTOR_SIZE);
	}
	for (int j = 0; j < FS_SECTORS_NUM; j++)
	{
		address = j * FS_SECTOR_SIZE;
		readMemory(address, buffer, FS_SECTOR_SIZE);
		for (int i = 0; i < (FS_SECTOR_SIZE); i++)
		{
			if (buffer[i] != (uint8_t)(i + j))
			{
				printf(" *  Recorded data does not match read data!\n");
				status = FAILED;
				break;
			}
		}
	}
	for (int i = 0; i < FS_SECTORS_NUM; i++)
	{
		address = FS_SECTOR_SIZE * i;
		eraseSector(address);
		readMemory(address, buffer, FS_SECTOR_SIZE);
		for (int j = 0; j < FS_SECTOR_SIZE; j++)
		{
			if (buffer[i] != 0xFF)
			{
				printf(" *  The Sector is not erased by eraseSector()!\n");
				status = FAILED;
				break;
			}
		}
	}
	printf("log.fs.tests: LogFs_plarformdependTest......finished!\n");
	return status;
}



LogFsTestStatus LogFs_formatTest(void)
{
	LogFsTestStatus status = PASSED;
	printf("log.fs.tests: LogFs_formatTest......started!\n");
	LogFs_format();
	for (int i = 0; i < (FS_SECTORS_NUM * FS_SECTOR_SIZE); i++)
	{
		uint8_t byte;
		readMemory(i, &byte, 1);
		if (byte != 0xFF)
		{
			printf(" *  The file system couldn't format flash!\n");
			status = FAILED;
			break;
		}
	}
	if (LogFs_initialize() != FS_SUCCESS)
	{
		printf(" *  The file system is corrupted after formatting. Failed to initialize!\n");
		status = FAILED;
	}
	if (LogFs_getFileNumber() != 0)
	{
		printf(" *  The number of files is not zero!\n");
		status = FAILED;
	}
	if (LogFs_findFile(FIRST_FILE) != FS_ERROR)
	{
		printf(" *  Function \"LogFs_findFile(FIRST_FILE)\" didn't return error!\n");
		status = FAILED;
	}
	if (LogFs_readFile(buffer, 0, 0) != FS_ERROR)
	{
		printf(" *  Function \"LogFs_readFile\" didn't return error at FIRST_FILE iterator!\n");
		status = FAILED;
	}
	if (LogFs_findFile(NEXT_FILE) != FS_ERROR)
	{
		printf(" *  Function \"LogFs_findFile(NEXT_FILE)\" didn't return error!\n");
		status = FAILED;
	}
	if (LogFs_readFile(buffer, 0, 0) != FS_ERROR)
	{
		printf(" *  Function \"LogFs_readFile\" didn't return error at NEXT_FILE iterator!\n");
		status = FAILED;
	}
	if (LogFs_findFile(LAST_FILE) != FS_ERROR)
	{
		printf(" *  Function \"LogFs_findFile(LAST_FILE)\" didn't return error!\n");
		status = FAILED;
	}
	if (LogFs_readFile(buffer, 0, 0) != FS_ERROR)
	{
		printf(" *  Function \"LogFs_readFile\" didn't return error at LAST_FILE iterator!\n");
		status = FAILED;
	}
	if (LogFs_fullSize() != (FS_SECTORS_NUM * (FS_SECTOR_SIZE - HANDLER_SIZE)))
	{
		printf(" *  Flash size calculated incorrectly!\n");
		status = FAILED;
	}
	if (LogFs_freeBytes() != (FS_SECTORS_NUM * (FS_SECTOR_SIZE- HANDLER_SIZE)))
	{
		printf(" *  Flash free size calculated incorrectly!\n");
		status = FAILED;
	}
	printf("log.fs.tests: LogFs_formatTest......finished!\n");
	return status;
}



LogFsTestStatus LogFs_cycleRewriteTest(int cycles)
{
	LogFsTestStatus status = PASSED;
	printf("log.fs.tests: LogFs_circleRewriteTest......started!\n");

	LogFs_format();
	LogFs_initialize();
	
	int sectors = 0;

	for (int count = 0; count < cycles; count++)
	{
		int action = 0;
		int files = LogFs_getFileNumber();
		while (sectors < FS_SECTORS_NUM)
		{
			if (action == 0)
			{
				LogFs_createFile();
				LogFs_writeToCurrentFile(text, 0.5*(FS_SECTOR_SIZE - HANDLER_SIZE));
				sectors += 1;
				files++;
			}
			else if (action == 1)
			{
				LogFs_createFile();
				LogFs_writeToCurrentFile(text, (FS_SECTOR_SIZE - HANDLER_SIZE));
				sectors += 1;
				files++;
			}
			else if (action == 2)
			{
				LogFs_createFile();
				LogFs_writeToCurrentFile(text, 1.5*(FS_SECTOR_SIZE - HANDLER_SIZE));
				sectors += 2;	
				files++;
			}
			else if (action == 3)
			{
				LogFs_createFile();
				LogFs_writeToCurrentFile(text, 3.5*(FS_SECTOR_SIZE - HANDLER_SIZE));
				sectors += 4;
				files++;
			}
			else
				action = 0;

			LogFs_getFileNumber();
			LogFs_initialize();
			action++;

			if (sectors < FS_SECTORS_NUM)
			{
				if (files != LogFs_getFileNumber())
					printf("  * File counter is wrong!\n");
			}
			else
			{
				if (LogFs_getFileNumber() > (files - 1) || LogFs_getFileNumber() < (files - 4))
					printf("  * File couter is wrong!\n");

				break;
			}
			sectors -= FS_SECTORS_NUM;
		}
	}
	printf("log.fs.tests: LogFs_circleRewriteTest......finished!\n");
	return status;
}


LogFsTestStatus LogFs_rewriteTest(int cycles, int fileSize)
{
	LogFsTestStatus status = PASSED;
	printf("log.fs.tests: LogFs_rewriteTest......started!\n");

	LogFs_format();
	if (LogFs_initialize() == FS_ERROR)
	{
		printf("  * Initialization failed!\n");
		status = FAILED;
	}
	int fileSectorSize = (int)(0.5 + ((double)fileSize/(FS_SECTOR_SIZE - HANDLER_SIZE)));
	int fileCounter = 0;
	int sectors = 0;
	int freeSectors = FS_SECTORS_NUM;
	int currentFileId = 0;
	for (int count = 0; count < cycles; count++)
	{
		for (; sectors < FS_SECTORS_NUM; sectors+= fileSectorSize)
		{
			if (freeSectors >= fileSectorSize)
			{
				fileCounter++;
				freeSectors -= fileSectorSize;
			}
			LogFs_createFile();
			putTextToBuffer(writeBuffer, fileSize);
			LogFs_writeToCurrentFile(writeBuffer, fileSize);
			if (LogFs_getFileNumber() != fileCounter)
			{
				printf("  * File counter is wrong!\n");
				status = FAILED;
			}	
			if (LogFs_findFile(LAST_FILE) == FS_ERROR)
			{
				printf("  * Current file not found by LogFs_findFile(LAST_FILE)!\n");
				status = FAILED;
			}
			if (LogFs_getFileProperties(FILE_SIZE) != fileSize)
			{
				printf("  * Current file size is incorrect!\n");
				status = FAILED;
			}
			if (LogFs_getFileProperties(FILE_NUMBER) != currentFileId)
			{
				printf("  * Current file id is incorrect!\n");
				status = FAILED;
			}
			if (LogFs_findFileByNum(LogFs_getFileProperties(FILE_NUMBER)) == FS_ERROR)
			{
				printf("  * Current file not found by LogFs_findFileByNum()!\n");
				status = FAILED;
			}
			if (LogFs_getFileProperties(FILE_SIZE) != fileSize)
			{
				printf("  * Current file size is incorrect!\n");
				status = FAILED;
			}
			if (LogFs_getFileProperties(FILE_NUMBER) != currentFileId)
			{
				printf("  * Current file id is incorrect!\n");
				status = FAILED;
			}
			if(LogFs_readFile(readBuffer, 0, fileSize) == FS_ERROR)
			{
				printf("  * Current file read error!\n");
				status = FAILED;
			}
			for (int i = 0; i < fileSize; i++)
			{
				if (readBuffer[i] != writeBuffer[i])
				{
					printf("  * Read data and write data doesn't match!\n");
					status = FAILED;
					break;
				}
			}
			if (LogFs_initialize() == FS_ERROR)
			{
				printf("  * Initialization failed!\n");
				status = FAILED;
			}
			currentFileId++;
		}
		sectors -= FS_SECTORS_NUM;
	}
	printf("log.fs.tests: LogFs_rewriteTest......finished!\n");
	return status;
}


LogFsTestStatus LogFs_readWriteTest(int cycles, int fileSize)
{
	LogFsTestStatus status = PASSED;
	printf("log.fs.tests: LogFs_rewriteTest......started!\n");

	LogFs_format();
	if (LogFs_initialize() == FS_ERROR)
	{
		printf("  * Initialization failed!\n");
		status = FAILED;
	}
}



