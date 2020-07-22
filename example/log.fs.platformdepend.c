#include "log.fs.platformdepend.h"

uint8_t MEMORY[FS_SECTORS_NUM * FS_SECTOR_SIZE];

/******************************************************************************************
*                         Приватные функции нижнего уровня                                *
******************************************************************************************/
void writeMemory(uint32_t Address, uint8_t* buffer, uint32_t size)
{
	uint32_t i = 0;
	for (i = 0; i < size; i++)
		MEMORY[Address + i] = buffer[i];
}


void readMemory(uint32_t Address, uint8_t* buffer, uint32_t size)
{
	uint32_t i = 0;
	for (i = 0; i < size; i++)
		buffer[i] = MEMORY[Address + i];
}

void eraseChip(void)
{
	uint32_t i = 0;
	for (i = 0; i < FS_SECTORS_NUM * FS_SECTOR_SIZE; i++)
		MEMORY[i] = 0xff;
}

void eraseSector(uint32_t Address)
{
	uint32_t i = 0;
	for (i = 0; i < FS_SECTOR_SIZE; i++)
		MEMORY[Address + i] = 0xff;
}
