#include "pch.h"
#include "CppUnitTest.h"

extern "C" {
#include "..\source\log.fs.platformdepend.h"
#include "..\source\log.fs.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

char* text = "The most delicious dumplings out of dough in the boiling water. Boil the water and pour a Cup of boiling"
"water in a deep bowl.Add a teaspoon of salt and slowly add pre - sifted through a sieve flour.Knead by hand a soft dough."
"A wooden Board dusted with flour and knead the dough until it stops sticking to your hands.Roll into a ball, cover with a"
"towel made of natural fabricsand give 20 minutes to stand up. During this time prepare the meat filling.Through a meat"
"grinder to crank out beef and pork, and then again with onionand garlic.Received minced saltand pepper pritrusit."
"All the ingredients are thoroughly mixed. In the bowl of stuffing to pour so much milk that it was not too thick,"
"but did not spread. The rested dough is divided into 4 parts.Roll out long \"sausages\".Each divided into 15 parts"
"and roll it out thinly.In the middle, ready to put the stuffingand sumipntg edge.Tips to keep each otherand a good"
"squeeze, giving the classic dumpling shape. Boil waterand put the first batch of products in the boiling water. "
"Cook for 6 - 8 minutes on high heat.Serves homemade dumplings with butter and sour cream.";



namespace logfstests
{
	TEST_CLASS(logfstests)
	{

	private:

		uint8_t buffer[FS_SECTOR_SIZE];
		uint8_t writeBuffer[FS_SECTORS_NUM * FS_SECTOR_SIZE];
		uint8_t readBuffer[FS_SECTORS_NUM * FS_SECTOR_SIZE];

		enum LogFsTestStatus {
			PASSED,
			FAILED
		};

		void putTextToBuffer(uint8_t * _buffer, int size)
		{
			int len = strlen(text);
			static int textIndex = 0;
			for (int i = 0; i < size; i++, textIndex++)
			{
				textIndex %= len;
				_buffer[i] = text[textIndex];
			}
		}

		LogFsTestStatus LogFs_rewriteTest(int cycles, int fileSize)
		{
			LogFs_format();
			Assert::IsTrue(LogFs_initialize() != FS_ERROR, L"Initialization failed!\n");

			int fileSectorSize = (int)(0.5 + ((double)fileSize / (FS_SECTOR_SIZE - HANDLER_SIZE)));
			int fileCounter = 0;
			int sectors = 0;
			int freeSectors = FS_SECTORS_NUM;
			int currentFileId = 0;
			for (int count = 0; count < cycles; count++)
			{
				for (; sectors < FS_SECTORS_NUM; sectors += fileSectorSize)
				{
					if (freeSectors >= fileSectorSize)
					{
						fileCounter++;
						freeSectors -= fileSectorSize;
					}
					LogFs_createFile();
					putTextToBuffer(writeBuffer, fileSize);
					LogFs_writeToCurrentFile(writeBuffer, fileSize);
					Assert::IsTrue(LogFs_getFileNumber() == fileCounter, L"File counter is wrong!\n");
					Assert::IsTrue(LogFs_findFile(LAST_FILE) != FS_ERROR, L"Current file not found by LogFs_findFile(LAST_FILE)!\n");
					Assert::IsTrue(LogFs_getFileProperties(FILE_SIZE) == fileSize, L"Current file size is incorrect!\n");
					Assert::IsTrue(LogFs_getFileProperties(FILE_NUMBER) == currentFileId, L"Current file id is incorrect!\n");
					Assert::IsTrue(LogFs_findFileByNum(LogFs_getFileProperties(FILE_NUMBER)) != FS_ERROR, L"Current file not found by LogFs_findFileByNum()!\n");
					Assert::IsTrue(LogFs_getFileProperties(FILE_SIZE) == fileSize, L"Current file size is incorrect!\n");
					Assert::IsTrue(LogFs_getFileProperties(FILE_NUMBER) == currentFileId, L"Current file id is incorrect!\n");
					Assert::IsTrue(LogFs_readFile(readBuffer, 0, fileSize) != FS_ERROR, L"Current file read error!\n");
					for (int i = 0; i < fileSize; i++)
						Assert::IsTrue(readBuffer[i] == writeBuffer[i], L"Read data and write data doesn't match!\n");
					Assert::IsTrue(LogFs_initialize() != FS_ERROR, L"Initialization failed!\n");
					currentFileId++;
				}
				sectors -= FS_SECTORS_NUM;
			}
			return PASSED;
		}
	public:
		TEST_METHOD(plarformdependTest_eraseChip)
		{
			uint32_t address;
			eraseChip();
			for (int i = 0; i < FS_SECTORS_NUM; i++)
			{
				address = FS_SECTOR_SIZE * i;
				readMemory(address, buffer, FS_SECTOR_SIZE);
				for(int j = 0; j < FS_SECTOR_SIZE; j++)
					Assert::IsTrue(buffer[j] == 0xFF, L"The chip is not erased by eraseChip()!\n");
			}
		}
		TEST_METHOD(plarformdependTest_readWrite)
		{
			uint32_t address;
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
					Assert::IsTrue(buffer[i] == (uint8_t)(i + j), L"Recorded data does not match read data!\n");
			}
		}
		TEST_METHOD(plarformdependTest_eraseSector)
		{
			for (int i = 0; i < FS_SECTORS_NUM; i++)
			{
				uint32_t address;
				address = FS_SECTOR_SIZE * i;
				eraseSector(address);
				readMemory(address, buffer, FS_SECTOR_SIZE);
				for (int j = 0; j < FS_SECTOR_SIZE; j++)
					Assert::IsFalse(buffer[i] != 0xFF, L"The Sector is not erased by eraseSector()!\n");
			}
		}

		TEST_METHOD(formatTest)
		{
			LogFs_format();
			for (int i = 0; i < (FS_SECTORS_NUM * FS_SECTOR_SIZE); i++)
			{
				uint8_t byte;
				readMemory(i, &byte, 1);
				Assert::IsTrue(byte == 0xFF, L"The file system couldn't format flash!\n");
			}
			Assert::IsTrue(LogFs_initialize() == FS_SUCCESS, L"The file system is corrupted after formatting. Failed to initialize!\n");
			Assert::IsTrue(LogFs_getFileNumber() == 0, L"The number of files is not zero!\n");
			Assert::IsTrue(LogFs_findFile(FIRST_FILE) == FS_ERROR, L"Function \"LogFs_findFile(FIRST_FILE)\" didn't return error!\n");
			Assert::IsTrue(LogFs_findFile(NEXT_FILE) == FS_ERROR, L" Function \"LogFs_findFile(NEXT_FILE)\" didn't return error!\n");
			Assert::IsTrue(LogFs_readFile(buffer, 0, 0) == FS_ERROR, L"Function \"LogFs_readFile\" didn't return error at NEXT_FILE iterator!\n");
			Assert::IsTrue(LogFs_findFile(LAST_FILE) == FS_ERROR, L"Function \"LogFs_findFile(LAST_FILE)\" didn't return error!\n");
			Assert::IsTrue(LogFs_readFile(buffer, 0, 0) == FS_ERROR, L"Function \"LogFs_readFile\" didn't return error at LAST_FILE iterator!\n");
			Assert::IsTrue(LogFs_fullSize() == (FS_SECTORS_NUM * (FS_SECTOR_SIZE - HANDLER_SIZE)), L"Flash size calculated incorrectly!\n");
			Assert::IsTrue(LogFs_freeBytes() == (FS_SECTORS_NUM * (FS_SECTOR_SIZE - HANDLER_SIZE)), L"Flash free size calculated incorrectly!\n");
		}
		TEST_METHOD(rewriteTest)
		{
			Assert::IsTrue(LogFs_rewriteTest(1, 5 * (FS_SECTOR_SIZE - HANDLER_SIZE)) == PASSED);
			Assert::IsTrue(LogFs_rewriteTest(5, 3 * (FS_SECTOR_SIZE - HANDLER_SIZE)) == PASSED);
			Assert::IsTrue(LogFs_rewriteTest(6, 2 * (FS_SECTOR_SIZE - HANDLER_SIZE)) == PASSED);
		}
	};
}
