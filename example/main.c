#include <stdio.h>
#include "log.fs.h"
#include "log.fs.platformdepend.h"


int main()
{
	printf("This is example of using the file system\n");

	LogFs_format();                                                // Formating file system before first use

	const char* text1 = "This is text for first file";
	if (LogFs_initialize() == FS_SUCCESS)                          // Initialize file system
	{
		LogFs_create();                                        // Open file
		LogFs_write((uint8_t*)text1, strlen(text1));   // Write to file
	}
	const char* text2 = "This is text for second file";
	if (LogFs_initialize() == FS_SUCCESS)                          // Reinitialize file system (and close previous file)
	{
		LogFs_create();                                        // Open new file
		LogFs_write((uint8_t*)text2, strlen(text2));  // Write to file
	}
	LogFs_initialize();                                            // Reinitialize file system (and close previous file)

	printf("Full flash space: %d bytes \n", (int)LogFs_totalSize());            // Check the total file system space
	printf("Free flash space: %d bytes \n", (int)LogFs_freeSpace());           // Check the free  file system space
	printf("The file system contains %d files\n", LogFs_getFileNumber());      // Сheck how many files are in the directory

	if (LogFs_findFile(FIRST_FILE) != FS_ERROR)                             // Initialize file selector at first file
	{
		do {
			printf("File ID: %d......size: %d bytes\n",     
				LogFs_getFileID(),       // Get id of the selected file
				LogFs_getFileSize());    // Get size of the selected file
			printf("         ");
			for (uint32_t i = 0; i < LogFs_getFileSize(); i++)
			{
				char byte;
				LogFs_read(&byte, i, 1);         // Read selected file byte by byte
				printf("%c", byte);                  
			}
			printf("\n");
		} while ((LogFs_findFile(NEXT_FILE) != FS_ERROR));  //  Find next file if it's possible
	}
}


