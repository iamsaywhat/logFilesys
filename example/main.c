#include <stdio.h>
#include "..\source\log.fs.h"
#include "..\source\log.fs.platformdepend.h"


int main()
{
	uint8_t buffer[100];
	int i = 0;
	uint8_t flag = 0;
	uint16_t count = 0;
	uint32_t size;

	printf("Test started..\n\n");
	uint8_t status = 0;
	char* testText = "System clock selection is performed on startup, however the internal RC 8MHz oscillator is selected";

	LogFs_format();

	/* Creation first file (< 1 sector size) */
	printf("Test 2: file #1 (<1 sector size):\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();
	LogFs_writeToCurrentFile(testText, 5);
	LogFs_initialize();
	if (LogFs_getFileNumber() != 1)
	{
		printf("LogFs_getFileNumber(): file count  is incorrect!\n");
		status = 1;
	}
	if (LogFs_findFile(FIRST_FILE) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}
	if (LogFs_readFile(buffer, 0, 5) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 5; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 5)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 0)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");


	/* Creation file #2 (<2 sector size)*/
	printf("Test 3: file #2 (<2 sector size):\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();
	LogFs_writeToCurrentFile(testText, 10);
	LogFs_initialize();
	if (LogFs_getFileNumber() != 2)
	{
		printf("LogFs_getFileNumber(): file count  is incorrect!\n");
		status = 1;
	}
	if (LogFs_findFileByNum(1) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}
	if (LogFs_readFile(buffer, 0, 10) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 10; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 10)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 1)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");

	/* Creation file #3 (=2 sector size) */
	printf("Test 4: file #3 (=2 sector size):\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();
	LogFs_writeToCurrentFile(testText, 12);
	LogFs_initialize();
	if (LogFs_getFileNumber() != 3)
	{
		printf("LogFs_getFileNumber(): file count  is incorrect!\n");
		status = 1;
	}
	if (LogFs_findFileByNum(LogFs_getFileNumber() - 1) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}
	if (LogFs_readFile(buffer, 0, 12) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 12; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 12)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != (LogFs_getFileNumber() - 1))
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");


	/* Creation file #4 (<4 sector size) */
	printf("Test 5: file #4 (<4 sector size):\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();
	LogFs_writeToCurrentFile(testText, 15);
	LogFs_initialize();
	if (LogFs_getFileNumber() != 4)
	{
		printf("LogFs_getFileNumber(): file count  is incorrect!\n");
		status = 1;
	}
	if (LogFs_findFileByNum(LogFs_getFileNumber() - 1) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}
	if (LogFs_readFile(buffer, 0, 15) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 15; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 15)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != (LogFs_getFileNumber() - 1))
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");

	/* Файл 5 с переходом на 0 сектор и удалением файла */
	printf("Test 6: file #5 - trasition to the 0 sector (< 3 sector size):\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();
	LogFs_writeToCurrentFile(testText, 17);
	LogFs_initialize();
	if (LogFs_getFileNumber() != 4)
	{
		printf("LogFs_getFileNumber(): file count  is incorrect!\n");
		status = 1;
	}
	if (LogFs_findFileByNum(4) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}
	if (LogFs_readFile(buffer, 0, 17) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 17; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 17)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 4)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");




	/* Файл 6 - Большой объем одним обращением (до 7 секторов) */
	printf("Test 7: file #6 - Large data, one call (< 3 sector size):\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();
	LogFs_writeToCurrentFile(testText, 40);
	LogFs_initialize();
	if (LogFs_getFileNumber() != 2)
	{
		printf("LogFs_getFileNumber(): file count  is incorrect!\n");
		status = 1;
	}
	if (LogFs_findFileByNum(5) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}
	if (LogFs_readFile(buffer, 0, 40) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 40; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 40)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 5)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");


	/* Файл 7 - Текущий файл слишком большой (> чем носитель)
	 * Ожидаемое поведение: перезапись всех других файлов, а когда места больше не останется
	 * ограничит сам себя, запретит запись, дабы не сломать разметку
	 */
	printf("Test 8: file #7 - File >> flash chip:\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();
	LogFs_writeToCurrentFile(testText, 100);
	LogFs_initialize();
	if (LogFs_getFileNumber() != 1)
	{
		printf("LogFs_getFileNumber(): file count is incorrect!\n");
		status = 1;
	}
	if (LogFs_findFileByNum(6) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}
	if (LogFs_readFile(buffer, 0, 60) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 60; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 60)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 6)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");



	/* Файл 8 - Текущий файл слишком большой (> чем носитель)
	 * Ожидаемое поведение: перезапись всех других файлов, а когда места больше не останется
	 * ограничит сам себя, запретит запись, дабы не сломать разметку
	 */
	printf("Test 9: file #8 - Flash chip is full, create new file:\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();
	LogFs_writeToCurrentFile(testText, 6);
	LogFs_initialize();
	if (LogFs_getFileNumber() != 1)
	{
		printf("LogFs_getFileNumber(): file count  is incorrect!\n");
		status = 1;
	}
	if (LogFs_findFileByNum(7) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}
	if (LogFs_readFile(buffer, 0, 6) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 6; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 6)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 7)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");


	/* Creation file #8 (<4 sector size) */
	printf("Test 10: file #8 (<4 sector size):\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();
	LogFs_writeToCurrentFile(testText, 15);
	LogFs_initialize();
	if (LogFs_getFileNumber() != 2)
	{
		printf("LogFs_getFileNumber(): file count is incorrect!\n");
		status = 1;
	}
	if (LogFs_findFileByNum(8) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}
	if (LogFs_readFile(buffer, 0, 15) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 15; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 15)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 8)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");




	/* Тест 10: Чтение открытого файла */
	printf("Test 10: read open file:\n ");
	status = 0;
	LogFs_initialize();
	LogFs_createFile();

	LogFs_writeToCurrentFile(testText, 5);
	if (LogFs_findFileByNum(9) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}

	if (LogFs_readFile(buffer, 0, 5) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 5; i++)
	{
		if (buffer[i] != testText[i])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 5)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 9)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	LogFs_writeToCurrentFile(testText + 5, 5);
	if (LogFs_findFileByNum(9) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}

	if (LogFs_readFile(buffer, 5, 5) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 5; i++)
	{
		if (buffer[i] != testText[i + 5])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 10)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 9)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}

	LogFs_writeToCurrentFile(testText + 10, 5);
	if (LogFs_findFileByNum(9) == FS_ERROR)
	{
		printf("LogFs_findFile(FIRST_FILE): file not found!\n");
		status = 1;
	}

	if (LogFs_readFile(buffer, 10, 5) == FS_ERROR)
	{
		printf("LogFs_readFile(): fail!\n");
		status = 1;
	}
	for (int i = 0; i < 5; i++)
	{
		if (buffer[i] != testText[i + 10])
		{
			printf("LogFs_readFile(): data does not match!\n");
			status = 1;
			break;
		}
	}
	if (LogFs_getFileProperties(FILE_SIZE) != 15)
	{
		printf("LogFs_getFileProperties(FILE_SIZE): file size is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileProperties(FILE_NUMBER) != 9)
	{
		printf("LogFs_getFileProperties(FILE_NUMBER): file number is incorrect! \n");
		status = 1;
	}
	if (LogFs_getFileNumber() != 3)
	{
		printf("LogFs_getFileNumber(): file count is not 4!\n");
		status = 1;
	}
	if (status == 1)
		printf("  FAILED!\n");
	else
		printf("  PASSED!\n");
}


