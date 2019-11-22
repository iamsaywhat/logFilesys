#include "Log_FS_llio.h"

extern uint8_t MEMORY[FS_SECTORS_NUM * FS_SECTOR_SIZE];

/******************************************************************************************
*                         Приватные функции нижнего уровня                                *
******************************************************************************************/
void MemoryWrite(uint32_t Address, uint8_t* buffer, uint32_t size)
{
	uint32_t i = 0;
	for (i = 0; i < size; i++)
	{
		MEMORY[Address + i] = buffer[i];
	}
}


void MemoryRead(uint32_t Address, uint8_t* buffer, uint32_t size)
{
	uint32_t i = 0;
	for (i = 0; i < size; i++)
	{
		buffer[i] = MEMORY[Address + i];
	}
}

void EraseChip(void)
{
	uint32_t i = 0;
	for (i = 0; i < FS_SECTORS_NUM * FS_SECTOR_SIZE; i++)
	{
		MEMORY[i] = 0xff;
	}
}

void EraseSector(uint32_t Address)
{
	uint32_t i = 0;
	for (i = 0; i < FS_SECTOR_SIZE; i++)
	{
		MEMORY[Address + i] = 0xff;
	}
}
