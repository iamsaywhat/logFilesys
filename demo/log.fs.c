/****************************************************************************************************************************

	log.fs - Модуль облегченной "файловой системы" для записи логов в "черном ящике".

*****************************************************************************************************************************/

#include "log.fs.h"
#include "log.fs.platformdepend.h"


/*----------------------------------- Приватная часть модуля---------------------------------------------------------------------*/


/************************************************************
	Управляющая структура для работы файловой системы
************************************************************/
LogFs_Info_Type LogFs_info;

/************************************************************
	Структура для обеспечения доступа к файлам
************************************************************/
LogFs_FileProperties_Type LogFs_fileProperties;


/***************************************************************************************************
	LogFs_deleteOldestFile - Функция Удаления самого старого файла в директории. Является
	приватной, ее самостоятельный вызов может привести к ошибке файловой системы.

	Примечание:  В случае полной заполненности памяти, при создании файла - произойдет вызов
	этой функции. Она удалит самый старый из имеющихся файлов и произведет очистку секторов,
	которые он занимал.

	Возвращает:
				FS_FINE  - Файл был удачно удален из директории
				FS_ERROR - Неизвестное форматирование (файловая система имеет внутреннюю ошибку
***************************************************************************************************/
static LogFs_Status LogFs_deleteOldestFile(void);


/***************************************************************************************************
	LogFs_getNumberSectorsFile - Узнать количество секторов которые занимает файл c началом
	в секторе с номером Sector;
	Параметры:
				Sector - Номер сектора в котором начинается файл (от 0 до FS_SECTORS_NUM - 1)
	Возвращает:
				Количество секторов которые занимает файл
***************************************************************************************************/
static uint16_t LogFs_getNumberSectorsFile(uint16_t Sector);



/*------------------------------------------Публичная часть модуля-------------------------------------------------------------*/



/***************************************************************************************************
	LogFs_format - Форматирование накопителя (полная очистка с потерей всех данных)
***************************************************************************************************/
void LogFs_format(void)
{
	eraseChip();
}


/***************************************************************************************************
	LogFs_getFileNumber - Получение информации о числе имеющихся файлов в директории
***************************************************************************************************/
uint16_t LogFs_getFileNumber(void)
{
	return LogFs_info.Files_Num;
}



/***************************************************************************************************
	LogFs_getLastFileId - Узнать порядковый последнего созданного файла
***************************************************************************************************/
uint16_t LogFs_getLastFileId(void)
{
	uint32_t Address;            // Вычисляемый адрес для чтения
	uint8_t  buff[4];            // Буфер для чтения заголовка и номера файла
	uint16_t i;                  // Счетчик циклов

	for (i = 0; i < 4; i++) buff[i] = 0;

	// Определяем стартовый адрес расположения последнего созданного файла
	Address = FS_SECTOR_SIZE * LogFs_info.LastFile_Sector;
	// Читаем залоговок (2 байта) и номер файла (2 байта)
	readMemory(Address, buff, 4);

	// Возвращаем порядкой номер файла
	return *(uint16_t*)(buff + 2);
}



/***************************************************************************************************
	LogFs_getCurrentFileId - Узнать порядковый номер открытого на запись файла
***************************************************************************************************/
uint16_t LogFs_getCurrentFileId(void)
{
	uint32_t Address;            // Вычисляемый адрес для чтения
	uint8_t  buff[4];            // Буфер для чтения заголовка и номера файла
	uint16_t i;                  // Счетчик циклов

	for (i = 0; i < 4; i++) buff[i] = 0;
	// Определяем стартовый адрес расположения последнего созданного файла
	Address = FS_SECTOR_SIZE * LogFs_info.CurrentFile_Sector;
	// Читаем залоговок (2 байта) и номер файла (2 байта)
	readMemory(Address, buff, 4);

	// Возвращаем порядковый номер файла
	return *(uint16_t*)(buff + 2);
}




/***************************************************************************************************
	LogFs_getNumberSectorsFile - Узнать количество секторов которые занимает файл c началом
	в секторе с номером Sector
***************************************************************************************************/
static uint16_t LogFs_getNumberSectorsFile(uint16_t Sector)
{
	uint32_t Address;                       // Вычисляемый адрес чтения
	uint8_t  buff[LAYOUT_SIZE];             // Буфер для чтения заголовка и номера файла
	uint16_t NumSectors = 1;                // Число секторов для файла по-умолчанию
	uint16_t i;                             // Счетчик циклов
	uint16_t _sector = Sector;              // Сектор, где расположено начало файла

	for (i = 0; i < LAYOUT_SIZE; i++) 
		buff[i] = 0;

	for (i = 0; i < FS_SECTORS_NUM; i++)
	{
		// Так как файловая система реализована по типу кольцевого буфера,
		// необходимо осуществлять переходы от старшего сектора к нулевому непрерывно
		// Чего и добиваемся этим условием
		_sector = (++_sector) % FS_SECTORS_NUM;

		// Теперь вычисляем адрес сектора идущим следом за тем, который был передан аргументом
		Address = FS_SECTOR_SIZE * _sector;
		readMemory(Address, buff, LAYOUT_SIZE);
		// Ожидаем находить заголовки продолжения файла
		// Любой другой будет говорить о том, что файл кончился
		if (*(uint16_t*)(buff) == FILE_CONTINUATION)
			NumSectors++;
		else 
			break;
	}

	return NumSectors;
}




/***************************************************************************************************
	LogFs_getFileProperties - Функция позволяет узнать параметры файла, который был выбран
	функцией "LogFs_findFile": Размер в байтах и порядковый номер в хранилище
***************************************************************************************************/
uint32_t LogFs_getFileProperties(uint8_t CMD)
{
	uint32_t result = 0;

	// Необходимо проверить инициализирована ли структура выбора файла
	// Если нет, значит и файл не выбран, а значит и выводить нечего.
	//if (LogFs_fileProperties.InitStructState != FS_INIT_OK)
	//	return FS_ERROR;
	// Смотрим, какая информация интересует
	switch (CMD)
	{
		// Спрашивают номер файла
	case FILE_NUMBER:
		result = LogFs_fileProperties.FileNum;
		break;
		// Спрашивают размер файла файла
	case FILE_SIZE:
		result = LogFs_fileProperties.ByteSize;
		break;

	}
	return result;
}




/***************************************************************************************************
	LogFs_initialize - Функция проверки и инициализации файловой системы. Производит просмотр
	и анализ файл файловой системы, ищет созданые файлы, определяет положения первого
	и последнего файла (по порядковому номеру), определяет количество занимаемых ими секторов,
	производит подсчет свободных секторов, подсчет имеющихся файлов, ищет свободное место
	в котором может быть создан файл. Заполняет управляющую структуру LogFs_info,
	которая обеспечивает работу файловой системы.
***************************************************************************************************/
LogFs_Status LogFs_initialize(void)
{
	uint32_t   i;                                 // Счетчик циклов
	uint32_t   Address;                           // Адрес для чтения из памяти
	uint8_t    buff[LAYOUT_SIZE];                 // Буфер под читаемый заголовок и номер файла
	uint32_t   LastFileNum = 0;                   // Номер самого свежего файла
	uint32_t   OldestFileNum = 0xFFFFFFFF;        // Номер самого старого файла
	uint8_t    first_step_flag = 1;               // Флаг, используется для определения свободного места для записи

	for (i = 0; i < LAYOUT_SIZE; i++) buff[i] = 0;

	// Инициализируем структуру с информацией о файловой системе
	LogFs_info.Files_Num = 0;
	LogFs_info.FreeSectors_Num = FS_SECTORS_NUM;
	LogFs_info.LastFile_Sector = 0;
	LogFs_info.LastFile_SectorNum = 0;
	LogFs_info.OldestFile_Sector = 0;
	LogFs_info.OldestFile_SectorNum = 0;
	LogFs_info.CurrentFile_Sector = 0xFFFF;  // Выставляем некорректное значение для защиты
	LogFs_info.CurrentWritePosition = 0;

	// Чтобы узнать состояние файловой системы необходимо из каждого сектора считать первые 4 байта
	for (i = 0; i < FS_SECTORS_NUM; i++)
	{
		// Вычисляем адрес сектора
		Address = FS_SECTOR_SIZE * i;
		// Читаем первые 4 байта сектора
		readMemory(Address, buff, LAYOUT_SIZE);
		// Проверяем заголовок сектора: содержит ли сектор файл
		if (*(uint16_t*)(buff) == FILE_EXIST_HANDLER)
		{
			// Инкрементируем счетчик файлов
			LogFs_info.Files_Num++;
			// Декрементируем счетчик свободных секторов
			LogFs_info.FreeSectors_Num--;

			// Определяем наиболее старый файл из имеющихся, для возможной перезаписи
			// Ищем наименьший порядковый номер файла
			if (*(uint16_t*)(buff + 2) < OldestFileNum)
			{
				// Запоминаем номер файла
				OldestFileNum = *(uint16_t*)(buff + 2);
				// Фиксируем номер сектора, где этот файл лежит
				LogFs_info.OldestFile_Sector = i;
			}
			// Определяем последний созданный файл (номер сектора содержащий этот файл)
			// Ищем наибольший порядковый номер файла
			if (*(uint16_t*)(buff + 2) > LastFileNum)
			{
				// Запоминаем номер файла
				LastFileNum = *(uint16_t*)(buff + 2);
				// Фиксируем номер сектора, где этот файл лежит
				LogFs_info.LastFile_Sector = i;
			}
		}
		// Возможно это продолжение файла с предыдущего сектора?
		else if (*(uint16_t*)(buff) == FILE_CONTINUATION)
		{
			LogFs_info.FreeSectors_Num--;
		}
		// Тогда здесь должно быть пусто
		else if (*(uint16_t*)(buff) == FREE_SPACE_HANDLER)
		{
			// Если попали сюда, значит текущий сектор будет пустым
			if (first_step_flag)
			{
				// Необходимо определиться со свободным местом для записи
				// Фиксируем первый попавшийся при анализе номер свободного сектора
				// Как место откуда начнём писать создаваемые файлы
				LogFs_info.CurrentFile_Sector = i;
			}
			// Этим гарантируем, что инструкция выше выполнится единственный раз
			first_step_flag = 0;
		}
		// Иначе файловая система повреждена
		else return FS_ERROR;
	}
	// Дополнительно определим размеры в секторах файлов (самого старого и самого свежего)
	LogFs_info.LastFile_SectorNum = LogFs_getNumberSectorsFile(LogFs_info.LastFile_Sector);
	LogFs_info.OldestFile_SectorNum = LogFs_getNumberSectorsFile(LogFs_info.OldestFile_Sector);

	// Информация о диске получена, возвращаем успех			
	return FS_FINE;
}



/***************************************************************************************************
	LogFs_check - Функция проверки разметки используемого носителя.
***************************************************************************************************/
LogFs_Status LogFs_check(void)
{
	uint32_t    i;                     // Счетчик циклов
	uint32_t    Address;               // Адрес для чтения из памяти
	uint8_t     buff[4];               // Буфер под читаемый заголовок и номер файла

	for (i = 0; i < 4; i++) buff[i] = 0;

	// Чтобы узнать состояние файловой системы необходимо из каждого сектора считать первые 4 байта
	for (i = 0; i < FS_SECTORS_NUM; i++)
	{
		Address = FS_SECTOR_SIZE * i;                    // Вычисляем адрес сектора
		readMemory(Address, buff, 4);                    // Читаем первые 4 байта сектора
		                                                 // Проверяем заголовки секторов: содержит ли сектор файл
		if (*(uint16_t*)(buff) != FILE_EXIST_HANDLER &&  // содержит ли сектор файл
			*(uint16_t*)(buff) != FILE_CONTINUATION  &&  // продолжение файла
			*(uint16_t*)(buff) != FREE_SPACE_HANDLER)    // пусто		                                     
			return FS_ERROR;                             // Если нет, то файловая система повреждена
	}
	return FS_FINE;			                             // Значит разметка носителя не нарушена	
}




/***************************************************************************************************
	LogFs_fullSize - Размер носителя, байт
***************************************************************************************************/
uint64_t LogFs_fullSize(void)
{
	return FS_SECTORS_NUM * (FS_SECTOR_SIZE - LAYOUT_SIZE);
}



/***************************************************************************************************
	LogFs_freeBytes - Свободное место на носителе, байт
***************************************************************************************************/
uint64_t LogFs_freeBytes(void)
{
	return ((uint64_t)LogFs_info.FreeSectors_Num * (uint64_t)(FS_SECTOR_SIZE - LAYOUT_SIZE) +
		    (FS_SECTOR_SIZE - LogFs_info.CurrentWritePosition));
}




/***************************************************************************************************
	LogFs_deleteOldestFile - Функция Удаления самого старого файла в директории. Является
	приватной, ее самостоятельный вызов может привести к ошибке файловой системы.
***************************************************************************************************/
static LogFs_Status LogFs_deleteOldestFile(void)
{
	uint16_t i;                       // Счетчик циклов  
	uint16_t Count;                   // Количество секторов занимаемых файлом
	uint32_t Address;                 // Вычисляемый адрес чтения/записи
	uint32_t Sector;                  // Номер сектора
	uint8_t  buff[LAYOUT_SIZE];       // Буфер для чтения загловка и номера сектора

	for (i = 0; i < LAYOUT_SIZE; i++)
		buff[i] = 0;

	// Определяем сколько секторов занимает файл
	Count = LogFs_getNumberSectorsFile(LogFs_info.OldestFile_Sector);

	// Стираем сектора, которые занимает самый старый файл
	for (i = 0; i < Count; i++)
	{
		// Определяем адреса секторов, в которых он расположен
		Address = FS_SECTOR_SIZE * (LogFs_info.OldestFile_Sector + i);
		// Так как файловая система реализована по типу кольцевого буфера,
		// необходимо осуществлять переходы от старшего сектора к нулевому непрерывно
		// Чего и добиваемся этим условием
		if (Address >= FS_SECTOR_SIZE * FS_SECTORS_NUM) 
			Address = Address % (FS_SECTOR_SIZE * FS_SECTORS_NUM);
		// Стираем сектор
		eraseSector(Address);
	}
	// Обновляем информацию о свободных секторах - теперь их Count штук
	LogFs_info.FreeSectors_Num += Count;
	// Декрементируем счетчик файлов
	LogFs_info.Files_Num--;

	// Удалили самый старый файл, теперь необходимо обновить информацию о файловой системе
	// Теперь самым старым файлом в директории будет файл с порядковым номером OldestFile_SectorNum + 1
	// Но необходимо найти начало этого файла
	// Делаем шаг вперед на Count секторов (Count - количество секторов на которых располагался ныне удалённый файл) и смотрим заголовок и номер файла
	Sector = (LogFs_info.OldestFile_Sector + Count) % FS_SECTORS_NUM;
	Address = FS_SECTOR_SIZE * Sector;
	readMemory(Address, buff, LAYOUT_SIZE);
	// Если все в порядке, то здесь должен быть заголовок начала следующего файла
	if (*(uint16_t*)(buff) != FILE_EXIST_HANDLER)
		return FS_ERROR; // Если нет - файловая система имеет ошибку и завершаем
	
	// Фиксируем сектор нового "старого" файла
	LogFs_info.OldestFile_Sector = Sector;

	// Теперь необходимо определить сколько секторов занимает новый "старый" файл
	LogFs_info.OldestFile_SectorNum = LogFs_getNumberSectorsFile(LogFs_info.OldestFile_Sector);
	// Если попали сюда, значит все хорошо
	return FS_FINE;
}




/***************************************************************************************************
	LogFs_createFile - Функция создания сессии/файла. Ищет свободное место не диске, если
	его нет, удаляет самый старший файл в директории, создает в свободном месте заголовок файла
	и присваивает файлу порядковый номер с момета создания первого файла в ФС.
***************************************************************************************************/
void LogFs_createFile(void)
{
	uint32_t Address;                     // Вычисляемый адрес чтения
	uint8_t  buff[LAYOUT_SIZE];           // Буфер для чтения заголовка и номера файла
	uint16_t i;                           // Счетчик циклов

	for (i = 0; i < LAYOUT_SIZE; i++) buff[i] = 0;

	// Чтобы узнать порядковый номер создаваемого файла
	// Воспользуемся информацией о последнем созданном файле
	// К его номеру сделаем икремент - и получим номер нового файла
	Address = FS_SECTOR_SIZE * LogFs_info.LastFile_Sector;
	readMemory(Address, buff, LAYOUT_SIZE);

	// Если создаётся первый файл в директории, то никакого LastFile не существует 
	// (только в этом случае LastFile будет с заголовком FREE_SPACE_HANDLER).
	// Тогда номер создаеваемого файла точно будет = 0
	if (*(uint16_t*)(buff) == FREE_SPACE_HANDLER)
		*(uint16_t*)(buff + 2) = 0;

	// Создаётся не первый файл, в этом случае порядковый номер создаваемого файла
	// будем выбирать как номер последнего созданного + 1
	else
	{
		// А тут к полученному номеру добавляем 1
		*(uint16_t*)(buff + 2) = *(uint16_t*)(buff + 2) + 1;
	}

	// А в первые два байта заносим заголовок начала нашего нового файла
	*(uint16_t*)(buff) = FILE_EXIST_HANDLER;

	// Если свободных секторов нет 
	if (LogFs_info.FreeSectors_Num == 0)
	{
		// Попав сюда, необходимо освободить сектор(а) с самым старым файлом
		// На его месте будет создан новый файл, поэтому сразу фиксируем сектор старого файла, как текущий рабочий
		LogFs_info.CurrentFile_Sector = LogFs_info.OldestFile_Sector;
		// И стираем сектора со старым файлом
		LogFs_deleteOldestFile();
	}
	// Создаем файл - Размещаем заголовок и номер на секторе диска
	Address = FS_SECTOR_SIZE * LogFs_info.CurrentFile_Sector;
	// Записываем заголовок в файл
	writeMemory(Address, buff, LAYOUT_SIZE);
	// Смещаем указатель для последующей записи на LAYOUT_SIZE байт (в каждом секторе первые LAYOUT_SIZE байт - резерв файловой системы)
	LogFs_info.CurrentWritePosition = LAYOUT_SIZE;
	// Раз заняли один сектор новым файлом, то нужно декрементировать счетчик свободных секторов
	LogFs_info.FreeSectors_Num--;
	// Файл создан, нужно обновить счетчик файлов
	LogFs_info.Files_Num++;
}



/***************************************************************************************************
	LogFs_writeToCurrentFile - Функция записи информации в файл
***************************************************************************************************/
void LogFs_writeToCurrentFile(uint8_t* Buffer, uint32_t Size)
{
	uint32_t i;                  // Счетчик циклов
	uint32_t Address;            // Вычисляемый адрес для чтения
	uint8_t  buff[LAYOUT_SIZE];  // Буфер для чтения заголовка и номера файла
	uint32_t adr;                // Вычисляемый адрес для реализации перехода между секторами

	for (i = 0; i < LAYOUT_SIZE; i++)  buff[i] = 0;

	// Определяем адрес для начала записи по выделенному сектору и смещению от его начала "CurrentWritePosition"
	Address = FS_SECTOR_SIZE * LogFs_info.CurrentFile_Sector + LogFs_info.CurrentWritePosition;

	// Проверяем хватит ли нам места в текущем секторе
	if ((LogFs_info.CurrentWritePosition + Size) >= FS_SECTOR_SIZE)
	{
		// Не хватает свободного места, необходимо освободить
		uint32_t currentSectorFreeSpace = (FS_SECTOR_SIZE - LogFs_info.CurrentWritePosition);
		uint16_t needFreeSector = (0.5 + ((double)(Size - currentSectorFreeSpace) / (FS_SECTOR_SIZE - LAYOUT_SIZE)));



		//// Посмотрим имеются ли свободные сектора
		//if (LogFs_info.FreeSectors_Num < needFreeSector)
		//{
		//	// Нет, нужно освободить место, удалив самый старый из имеющихся файлов, кроме случаев, когда он единственный
		//	if (LogFs_info.Files_Num > 1)
		//		LogFs_deleteOldestFile();
		//	else
		//	{
		//		writeMemory(Address, &Buffer[i], LogFs_freeBytes());
		//		return;
		//	}
		//}


		// Посмотрим имеются ли свободные сектора
		while (LogFs_info.FreeSectors_Num < needFreeSector && LogFs_info.Files_Num > 1)
		{
			// Нет, нужно освободить место, удалив самый старый из имеющихся файлов, кроме случаев, когда он единственный
			LogFs_deleteOldestFile();
		}
	}

	// Будем писать побайтово, так как нужен контроль над переходом на новый сектор
	// Там может быть, например, не пусто 
	for (i = 0; i < Size; i++)
	{
		if (LogFs_info.Files_Num <= 1 && LogFs_freeBytes() <= 0) 
			return;

		// Определяем адрес места записи
		Address = FS_SECTOR_SIZE * LogFs_info.CurrentFile_Sector + LogFs_info.CurrentWritePosition;

		// Условие для контроля перехода между секторами
		// Определяем границу перехода по позиции в секторе, если она равна размеру сектора, это и есть граница
		if (LogFs_info.CurrentWritePosition == FS_SECTOR_SIZE)
		{
			/* Попали на новый сектор, необходимо объявить о продолжении файла, создав в следующем секторе 
			 * соответствующий заголовок и дублировать номер файла, для чего возвращаемся к началу текущего сектора,
			 * чтобы клонировать его разметку */
			Address = FS_SECTOR_SIZE * LogFs_info.CurrentFile_Sector;     // Узнаем номер текущего файла в каталоге
			readMemory(Address, buff, LAYOUT_SIZE);                       // Читаем первые LAYOUT_SIZE байт разметки
			*(uint16_t*)(buff) = FILE_CONTINUATION;                       // Информацию получили, теперь подменим заголовок 
			                                                              // на заголовок продолжения файла, номер остаётся таким же

			// Если сектор последний - нужно перейти на первый (кольцевой буфер)
			if (LogFs_info.CurrentFile_Sector < (FS_SECTORS_NUM - 1))
				LogFs_info.CurrentFile_Sector++;
			else
				LogFs_info.CurrentFile_Sector = 0;

	
			Address = FS_SECTOR_SIZE * LogFs_info.CurrentFile_Sector;      // Вычисляем адрес для записи (начало нового сектора)
			writeMemory(Address, buff, LAYOUT_SIZE);                       // Размечаем этот сектор как продолжение нашего файла
			LogFs_info.FreeSectors_Num--;                                  // Декрементируем кол-во свободных секторов
			LogFs_info.CurrentWritePosition = LAYOUT_SIZE;                 // Задаем смещение на размер заголовка и номера от начала нового сектора
			//Вычисляем адрес для записи
			Address = FS_SECTOR_SIZE * LogFs_info.CurrentFile_Sector + LogFs_info.CurrentWritePosition;
		    // Теперь все готово к записи
		}

		// Пишем в файл, и делаем инкремент смещения от начала сектора
		writeMemory(Address, &Buffer[i], 1);
		LogFs_info.CurrentWritePosition++;
	}
}



/***************************************************************************************************
	LogFs_findFile - Функция поиска файлов в директории
***************************************************************************************************/
LogFs_Status LogFs_findFile(uint8_t CMD)
{
	uint32_t           Address;            // Вычисляемый адрес для чтения                     
	uint8_t            buff[2];            // Буфер для чтения заголовка и номера файла
	uint32_t           i;                  // Счеттчик циклов
	static uint32_t    FileCount = 0;      // Количество просмотренных файлов

	// Посмотрим, что за команда
	// Если структура не была проинициализирована и вдруг приходит команда NEXT_FILE (Которая должна приходить только после FIRST_FILE)
	// Это ошибка
	if (CMD == NEXT_FILE && LogFs_fileProperties.InitStructState != FS_INIT_OK)
		return FS_ERROR;

	// Команда на запрос информации о самом старом файле в директории, равносильно команде на инициализацию структуры (отсчетной точкой будет старый файл)
	else if (CMD == FIRST_FILE)
	{
		// Сбрасываем и инициализируем структуру работы с файлами
		LogFs_fileProperties.ByteSize = 0;
		LogFs_fileProperties.FileNum = 0;
		LogFs_fileProperties.SectorNum = LogFs_info.OldestFile_SectorNum;
		LogFs_fileProperties.StartSector = LogFs_info.OldestFile_Sector;
		LogFs_fileProperties.InitStructState = FS_INIT_OK;

		// Сбрасываем счетчик просмотренных файлов
		FileCount = 0;
	}
	// Команда на запрос информации о самом новом файле в директории
	else if (CMD == LAST_FILE)
	{
		LogFs_fileProperties.ByteSize = 0;
		LogFs_fileProperties.FileNum = 0;
		LogFs_fileProperties.SectorNum = LogFs_info.LastFile_SectorNum;
		LogFs_fileProperties.StartSector = LogFs_info.LastFile_Sector;
		LogFs_fileProperties.InitStructState = FS_INIT_OK;

		// Сбрасываем счетчик просмотренных файлов
		FileCount = LogFs_info.Files_Num - 1;
	}
	// Переходим к следующему файлу, если структура была инициализирована (точка отсчета есть)
	else if (CMD == NEXT_FILE && LogFs_fileProperties.InitStructState == FS_INIT_OK)
	{
		LogFs_fileProperties.ByteSize = 0;
		LogFs_fileProperties.FileNum = 0;

		// Переходим к началу следующего файла, отталкиваясь от сектора предыдущего и количества секторов которые он занимал 
		LogFs_fileProperties.StartSector = (LogFs_fileProperties.StartSector + LogFs_fileProperties.SectorNum) % FS_SECTORS_NUM;

		// Теперь необходимо определить сколько секторов занимает этот файл
		for (i = 1; i < FS_SECTORS_NUM; i++)
		{
			Address = FS_SECTOR_SIZE * (LogFs_fileProperties.StartSector + i);
			// Контролируем переходы кольцевого буфера (чтобы избежать переполнения и обращения по несуществующему адресу)
			Address = Address % (FS_SECTORS_NUM * FS_SECTOR_SIZE);
			// Будем читать заголовки секторов после StartSector пока не найдем FILE_EXIST_HANDLER - признак начала следующего файла
			readMemory(Address, buff, 2);
			if (*(uint16_t*)(buff) == FILE_EXIST_HANDLER || *(uint16_t*)(buff) == 0xFFFF)
				break;

		}
		// Фиксируем число таких секторов
		LogFs_fileProperties.SectorNum = i;
	}
	// Иначе команда неопознана - возвращаем ошибку
	else
		return FS_ERROR;

	// Получаем адрес сектора расположения файла
	Address = FS_SECTOR_SIZE * LogFs_fileProperties.StartSector;

	// Проверим присутсвует ли по этому адресу заголовок начала файла
	readMemory(Address, buff, 2);
	if (*(uint16_t*)(buff) != FILE_EXIST_HANDLER)
		return FS_ERROR; // Заголовка нет, файловая система содержит ошибку

	// Узнаем порядковый номер этого файла в директории
	Address += 2;
	readMemory(Address, buff, 2);
	LogFs_fileProperties.FileNum = *(uint16_t*)(buff);

	// Если файл содержится на нескольких секторах, сразу переходим в последний
	if (LogFs_fileProperties.SectorNum > 1)
	{
		// Определяем адрес этого сектора
		Address = FS_SECTOR_SIZE * (LogFs_fileProperties.StartSector + LogFs_fileProperties.SectorNum - 1);
		// Контролируем переходы кольцевого буфера (чтобы избежать переполнения и обращения по несуществующему адресу)
		Address = Address % (FS_SECTORS_NUM * FS_SECTOR_SIZE);
		// А счетчик байт файла инкрементируем на (FS_SECTOR_SIZE - 4) * (Количество занятых файлом секторов - 1)
		LogFs_fileProperties.ByteSize = (FS_SECTOR_SIZE - 4) * (LogFs_fileProperties.SectorNum - 1);
		// Смещаем адрес на 4 заголовочных байта
		Address += 4;
	}
	// Если же файл содержится только в текущем секторе смещаем адрес еще на 2 байта (от номера файла; на первый байт данных)
	else
		Address += 2;

	// Дальше начинаем искать конец файла
	for (i = 0; i < FS_SECTOR_SIZE - 4; i++)
	{
		// Будем считывать байт за байтом из этого сектора и смотреть не упёрлись ли мы в конец (0xFF - форматированная ячейка)
		readMemory(Address, &buff[0], 1);
		// Если конец файла, то выходим
		if (*(uint8_t*)(buff) == 0xFF)
			break;
		// Если попали сюда, то полученный байт не признак конца и нужно инкрементировать счетчик байт и адрес
		Address++;
		LogFs_fileProperties.ByteSize++;
	}
	// Инкрементируем счетчик просмотренных файлов, так как если мы дошли сюда, значит файл просмотрен
	FileCount++;
	// Проверяем все ли файлы в хранилище были просмотрены
	if (LogFs_info.Files_Num <= FileCount)
	{
		LogFs_fileProperties.InitStructState = FS_INIT_NO;
		return FS_ALL_FILES_SCROLLS;
	}

	return FS_NOT_OVER;
}



/***************************************************************************************************
	LogFs_findFileByNum - Функция осуществляет поиск файла по номеру.
***************************************************************************************************/
LogFs_Status LogFs_findFileByNum(uint16_t NUM)
{
	uint16_t files = 0;            // Количество файлов в хранилище
	uint16_t min_num, max_num;     // Минимальный и максимальный порядковые номера файлов
	uint16_t i;                    // Счетчик циклов


	// Если файлы отсутсвуют, то нечего искать
	if (LogFs_getFileNumber() == 0)
		return FS_ERROR;

	// Проверим существует ли такой номер
	// Посмотрим порядковый номер самого старого файла (номер должен быть самым маленьким)
	LogFs_findFile(FIRST_FILE);
	min_num = LogFs_getFileProperties(FILE_NUMBER);
	// Посмотрим порядковый номер самого свежего файла (номер должен быть самым большим)
	LogFs_findFile(LAST_FILE);
	max_num = LogFs_getFileProperties(FILE_NUMBER);

	// Номер запрашиваемого файла должен принадлжеать данному диапазону, иначе ошибка - файл не найден
	if (NUM > max_num || NUM < min_num)
		return FS_ERROR;

	// Получим число файлов в хранилище
	files = LogFs_getFileNumber();

	for (i = 0; i < files; i++)
	{
		if (i == 0)
			LogFs_findFile(FIRST_FILE);
		else
			LogFs_findFile(NEXT_FILE);
		// Сверяем номер файла. Если совпадает файл найден можно выходить
		if (LogFs_getFileProperties(FILE_NUMBER) == NUM)
			return FS_FINE;

	}
	// Если дошли сюда, значит файл не найден - ошибка
	return FS_ERROR;
}



/***************************************************************************************************
	LogFs_readFile - Функция чтения информации из файлов. Должна запускаться только после функции
	LogFs_findFile(), поскольку эта функция и позволяет нам определить какой файл будем
	читать. То, какой файл будем читать содержится в структуре LogFs_fileProperties.
***************************************************************************************************/
LogFs_Status LogFs_readFile(uint8_t* Buffer, uint32_t ByteNum, uint32_t Size)
{
	uint32_t Address;     // Адрес чтения 
	uint32_t i;           // Счетчик циклов
	uint32_t Sector;      // Номер сектора от начала файла

	// Проверим корректность параметров, и запретим чтение за пределами файла
	if (ByteNum + Size > LogFs_fileProperties.ByteSize)
		return FS_ERROR;

	// Определяем к какому сектору файла относится первый читаемый байт
	Sector = ByteNum / (FS_SECTOR_SIZE - 4);

	// Получаем текущий адрес ячейки для чтения
	// Сначала адрес нужного сектора с контролем переходов
	Address = (FS_SECTOR_SIZE * (LogFs_fileProperties.StartSector + Sector)) % (FS_SECTOR_SIZE * FS_SECTORS_NUM);
	// Затем задаем необходимое смещение от начала сектора
	Address += 4 + (ByteNum % (FS_SECTOR_SIZE - 4));

	// Начинаем побайтово читать файл
	for (i = 0; i < Size; i++)
	{
		// Читаем байт
		readMemory(Address, &Buffer[i], 1);
		// Инкрементируем адрес с контролем перехода кольцевого буфера
		Address = (Address + 1) % (FS_SECTOR_SIZE * FS_SECTORS_NUM);
		// Проверим, вдруг адрес указывает на первые 4 байта заголовка сектора
		if (Address % FS_SECTOR_SIZE == 0)
		{
			// Они нам не нужны, поэтому сразу перепрыгиваем на 4 байта вперед
			Address += 4;
		}
	}
	return FS_FINE;
}

