#include "log.fs.tests.h"
#include "log.fs.h"
#include "log.fs.platformdepend.h"


uint8_t MEMORY[FS_SECTORS_NUM * FS_SECTOR_SIZE];
uint8_t buffer[FS_SECTOR_SIZE];


LogFsTestStatus LogFs_plarformdependTest(void)
{
	uint32_t address;
	LogFsTestStatus status = PASSED;

	printf("log.fs.tests: LogFs_plarformdependTest......started!\n");

	eraseChip();
	for (int i = 0; i < FS_SECTORS_NUM; i++)
	{
		address = FS_SECTOR_SIZE * i;
		readMemory(address, &buffer, FS_SECTOR_SIZE);
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
	if (LogFs_initialize() != FS_FINE)
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
	if (LogFs_fullSize() != (FS_SECTORS_NUM * FS_SECTOR_SIZE))
	{
		printf(" *  Flash size calculated incorrectly!\n");
		status = FAILED;
	}
	if (LogFs_freeBytes() != (FS_SECTORS_NUM * (FS_SECTOR_SIZE- LAYOUT_SIZE)))
	{
		printf(" *  Flash free size calculated incorrectly!\n");
		status = FAILED;
	}
	printf("log.fs.tests: LogFs_formatTest......finished!\n");
	return status;
}


