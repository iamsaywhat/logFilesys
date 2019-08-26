/****************************************************************************************************************************
*Version 1.0
*
*			Log_FS - Модуль облегченной "файловой системы" для flash накопителей для записи логов на flash - память.
*					
*			Единицей объема для файла является размер сектора flash памяти, во избежание необходимости буферизации 
*			нестираемой	части сектора и повышения быстродействия. Запись ведется сессиями. В случае нехватки места 
*			в одном секторе файл, переходит на запись следующего по порядку сектора. Также реализовано некоторое подобие 
*			кольцевого буфера. При полной заполнености накопителя, новые файлы пишутся поверх старых по кругу:
*			от нулевого сектора до крайнего, потом переход на 0й и так далее 0....N -> 0.....N. 
*			Файлы (или сессии) не имеют имен, имеют только порядковый номер с момента форматирования накопителя.
*			Каждый сектор содержащий файл имеет заголовок "FILE_EXIST_HANDLER" - Начало файла, либо FILE_CONTINUATION - 
*			Продолжение файла с предыдущего сектора (для 0 сектора предыдущим является последний).
*
*			Для работы модуль хранит в ОЗУ только две управляющие структуры типа Log_Fs_Info_Union и Log_Fs_FileProperties_Type.
*           Log_Fs_Info_Union - является состоянием файловой системы, Log_Fs_FileProperties_Type используется для доступа к файлам.
*			Они являются приватными, и недоступны пользователю (описание полей стркутуры можно увидеть в typedef).
*           
*			Навигация по файлам осуществляется функцией Log_Fs_FindFIle(). Первое использование функции должно 
*			быть с командой FIRST_FILE, таким образом файловая система открывает для чтения самый старый файл-сессию
*			(с наименьшим порядковым номером), после этого используя функцию LogFs_ReadFile () мы можем прочитать содержимое файла
*           либо скопировать в необходимо место. Функция  LogFs_ReadFile () позволяет читать один и тот же файл в несколько запросов,
*           если нет возможности прочитать весь файл целиком (обычно в таком случае нужна буферизация в ОЗУ, что для больших файлов
*           может быть проблемой), для этого функция запоминает последнюю прочитанную позицию, и при следующем обращении позволит продолжить
*           с этого же места.
*           Так же навигация по файлам возможна с использованием функции Log_Fs_FindFile_ByNum. Данная функция открываает файл с
*           порядковым номером, переданным аргументом.
*
*			Последующее использование функции Log_Fs_GetFileProperties для доступа к остальным файлам должно происходить 
*           с командой NEXT_FILE. Таким образом мы получаем информацию о файле со следующим порядковым номером и так же можем его прочитать 
*           функцией LogFs_ReadFile.
*			То есть перемещение по директории между файлами происходит всегда от самого старого файла к самому новому.
*			
*			Внимание! 
*           - При использовании данной файловой системы на носителях отличных от flash памяти, нужно быть осторожным,
*           для файловой системы состояние пусто (стертая ячейка) - это "0xFF". Если на другом носителе это состояние отличается
*           файловая система будет работать некорректно.
*			- Запись кириллических символов в память недопустима. Конец файла определяется по признаку пустой ячейки (0xFF).
*			Буква "я" соответствует коду 0xFF, поэтому при записи "я" в память, конец файла может быть опреден неверно.
*           - Открытая сессия не отображается в числе доступных на чтение файлов (команда Log_Fs_FindFIle() не видит файл). 
*           На чтение доступны только неактивные сессии. Завершить сессию (закрыть файл) и сделать ее доступной на чтение
*           можно функцией Log_Fs_Info();
*
*			Пример использования:
*
	uint8_t buffer[100];
	uint32_t size;
	int i = 0;
	uint16_t num;
	// Готовим носитель для файловой системы форматированием
	LogFs_Formatting();
	// Инициализируем файловую систему, и проверяем получилось ли
	if (LogFs_Info() == FS_FINE)
	{
		// Создаём сессию
		LogFs_CreateFile();
		// Пишем из буфера в файл 5 байт
		LogFs_WriteToCurrentFile(buffer, 5);
		// Еще раз пишем из буфера в файл 5 байт
		LogFs_WriteToCurrentFile(buffer, 5);
		// Далее писать в файл можно сколько угодно
		// до следующего вызова LogFs_Info()
	}
	// Допустим понадобилось создать другой файл, а текущий закрыть
	// Запускаем переинициализацию - которая кроме этого закрывает открытую сессию
	// Если таковы имеются и конечно проверим, получилось ли
	if (LogFs_Info() == FS_FINE)
	{
		// Создаём новую сессию
		LogFs_CreateFile();
		// Пишем из буфера 4 раза в файл по 5 байт
		LogFs_WriteToCurrentFile(buffer, 5);
		LogFs_WriteToCurrentFile(buffer, 5);
		LogFs_WriteToCurrentFile(buffer, 5);
		LogFs_WriteToCurrentFile(buffer, 5);
		// Закроем сессию
		LogFs_Info();
	}
	// Перейдем к чтению
	// Запускаем навигацию по файлам, и попутно открываем самый старый файл на чтение, командой FIRST_FILE
	// Первое использование этой функции должно быть с командой FIRST_FILE
	Log_Fs_FindFIle(FIRST_FILE);
	// Узнаем размер открытого файла (команда FILE_SIZE)
	size = Log_Fs_GetFileProperties(FILE_SIZE);
	// Так же можно узнать порядковый номер открытого файла (команда FILE_NUMBER)
	num = Log_Fs_GetFileProperties(FILE_NUMBER);
	// И прочитаем файл в буфер
	LogFs_ReadFile(buffer, size);

	// Перейдем к следующему файлу (Внимание! Теперь функция с командой NEXT_FILE)
	// То есть этой функцией мы можем последовательно от самого старого к самому новому файлу переключаться между ними)
	// Так же можно проверять открылся ли файл
	if (Log_Fs_FindFIle(NEXT_FILE) != FS_ERROR)
	{
		// Узнаем размер открытого файла (команда FILE_SIZE)
		size = Log_Fs_GetFileProperties(FILE_SIZE);
		// Продемонстрируем, что функцией LogFs_ReadFile, можно читать файл в за несколько обращений
		// Будем читать по байту size раз (размер файла в байтах)
		for (i = 0; i < size; i++)
		{
			LogFs_ReadFile(&buffer[i], 1);
		}
	}
	// Так было создано два файла и два файла были прочитаны, то Log_Fs_FindFIle(NEXT_FILE) будет возвращать ошибку
	// Открывать файл на чтение можно так же и с помощью другой функции по порядковому номеру
	// Например, узнаем порядковый номер самого свежего файла
	num = LogFs_GetNumLastFile();
	// Если файл с таким номером существуствует, функция откроет его на чтение и вернет FS_FINE, в противном случае FS_ERROR
	if (Log_Fs_FindFile_ByNum(num) == FS_FINE)
	{
		// Читаем файл за раз
		LogFs_ReadFile(buffer, Log_Fs_GetFileProperties(FILE_SIZE));
	}
*
*			
****************************************************************************************************************************/

#ifndef _Log_FS_H_
#define _Log_FS_H_


#include "stdint.h"

#define FILE_EXIST_HANDLER  0xEFA7 // Заголовок - признак того, что данный сектор уже занят файлом
#define FILE_CONTINUATION   0xEFAC // Заголовок - признак того, что этот сектор не новый файл, а продолжение предыдущего
#define FREE_SPACE_HANDLER  0xFFFF // Заголовок - признак того, сектор свободен и тут может быть создан файл
 

/************************************************************
         Структура информации о файловой системе            *
************************************************************/
typedef struct{
	uint16_t Files_Num;             // Количество файлов на диске
	uint16_t FreeSectors_Num;       // Количество свободных секторов
	uint16_t LastFile_Sector;       // Сектор с последним созданным файлом
	uint16_t LastFile_SectorNum;    // Количество секторов, которые занимает последний созданный файл
	uint16_t OldestFile_Sector;     // Самый старый файл на диске
	uint16_t OldestFile_SectorNum;  // Количество секторов, которые занимает самый старый созданный файл
	uint16_t CurrentFile_Sector;    // Свободный сектор, где можно создать новый файл
	uint32_t CurrentWritePosition;  // Позиция в файле для записи
	} Log_Fs_Info_Type;

typedef union { 
	Log_Fs_Info_Type Struct;        // Объединение структуры с буффером, для простого доступа
	uint8_t Buffer[18];
  } Log_Fs_Info_Union;


/************************************************************
		 Структура информации о файле                       *
************************************************************/
typedef struct {
	uint32_t FileNum;				 // Порядковый номер файла
	uint32_t ByteSize;				 // Размер файла в байтах
	uint32_t ReadPosition;           // Текущая позиция для чтения (исп. в LogFs_ReadFile)
	uint16_t SectorNum;				 // Число секторов на которых размещен файл
	uint16_t StartSector;			 // Стартовый сектор файла
	uint8_t  InitStructState;        // Была ли структура проинициализрована (FS_INIT_OK)
} Log_Fs_FileProperties_Type;

	
	
/************************************************************
                   Команды функций                          *
************************************************************/
typedef enum {
	FIRST_FILE,		                 // Команда для Log_Fs_FIndFile(FIRST_FILE); запускающая просмотр файлов и показывает свойства самого раннего файла в директории
	NEXT_FILE,                       // Команда для Log_Fs_FIndFile(NEXT_FILE); позволяющая перейти к следующему файлу и узнать его параметры
	LAST_FILE,                       // Команда для Log_Fs_FIndFile(LAST_FILE); позволяющая перейти сразу к просмотру последнего созданного файла
	FILE_NUMBER,                     // Комагда для Log_Fs_GetFileProperties(FILE_NUM); позволяющая узнать номер файла, который был найден Log_Fs_FIndFile();
	FILE_SIZE                        // Комагда для Log_Fs_GetFileProperties(FILE_SIZE); позволяющая узнать размер файла, который был найден Log_Fs_FIndFile();
}Log_Fs_CMD;

/************************************************************
	      Коды статуса в работе файловой системы            *
************************************************************/
typedef enum {
	FS_FINE,                         // Успех
	FS_ERROR,                        // Ошибка
	FS_NOT_OVER,                     // Имеются еще файлы, которые не были показаны
	FS_ALL_FILES_SCROLLS,            // Все файлы были просмотрены. Возвращается LogFs_FindFile, когда был найден последний файл в директории
	FS_INIT_OK,                      // Флаг состояния, означающий, что для чтения файл выбран
	FS_INIT_NO,                      // Флаг состояния, означающий, что для чтения файл не выбран
	FS_FILE_END                      // Конец файла, возвращается когда функцией LogFs_ReadFile(); файл был прочитан целиком
}Log_Fs_Status;





/***************************************************************************************************

       LogFs_Info - Функция проверки и инициализации файловой системы. Производит просмотр
	   и анализ файл файловой системы, ищет созданые файлы, определяет положения первого
	   и последнего файла (по порядковому номеру), определяет количество занимаемых ими секторов,
	   производит подсчет свободных секторов, подсчет имеющихся файлов, ищет свободное место
	   в котором может быть создан файл. Заполняет управляющую структуру Log_Fs_Info, 
	   которая обеспечивает работу файловой системы.
	   
	   Примечания:   Необходимо вызывать эту функцию при начале работы с файловой системы 
	   (инициализация), и при закрытии текущей сессии/файла, перед созданием новой сессии/файла

	   Возвращает: 
					FS_FINE - Система инициализированна удачно и готова к работе
					FS_ERROR - Неизвестное форматирование
		  
***************************************************************************************************/
Log_Fs_Status LogFs_Info(void);




/***************************************************************************************************

	   LogFs_Formatting - Форматирование накопителя (полная очистка с потерей всех данных)

	   Параметры: NONE

	   Возвращает: NONE

***************************************************************************************************/
void LogFs_Formatting(void);



/***************************************************************************************************

	   LogFs_GetFileNum - Получение информации о числе имеющихся файлов в директории

	   Возвращает:
					Количество файлов в директории

***************************************************************************************************/
uint16_t LogFs_GetFileNum(void);



/***************************************************************************************************

	   LogFs_GetFileNum - Узнать порядковый номер последнего созданного файла

	   Возвращает:
					Порядковый номер последнего созданного файла

***************************************************************************************************/
uint16_t LogFs_GetNumLastFile(void);



/***************************************************************************************************

	   LogFs_GetNumCurrentFile - Узнать порядковый номер открытого на запись файла

	   Возвращает:
					Порядковый номер открытого на запись файла

***************************************************************************************************/
uint16_t LogFs_GetNumCurrentFile(void);




/***************************************************************************************************

	   LogFs_CreateFile - Функция создания сессии/файла. Ищет свободное место не диске, если 
	   его нет, удаляет самый старший файл в директории, создает в свободном месте заголовок файла
	   и присваивает файлу порядковый номер с момета создания первого файла в ФС.

	   Примечание:  В случае полной заполненности памяти, при создании файла - произойдет вызов
	   этой функции. Она удалит самый старый из имеющихся файлов и произведет очистку секторов,
	   которые он занимал.

	   Возвращает:
					NONE

***************************************************************************************************/
void LogFs_CreateFile(void);


/***************************************************************************************************

	  LogFs_WriteToCurrentFile - Функция записи информации в файл.

	  Примечания: Должна вызываться только после LogFs_Info(); и LogFs_CreateFile(); иначе 
	  поведение не предсказуемо 

	   Параметры:
					Log_Fs_Info - Управляющая структура файловой системы
					Buffer      - Указатель на буфер, откуда брется информация для запили
					Size        - Размер в байтах, записываемой информации

	   Возвращает:
					NONE

***************************************************************************************************/
void LogFs_WriteToCurrentFile(uint8_t* Buffer, uint32_t Size);



/***************************************************************************************************

	  Log_Fs_FindFIle - Функция осуществляет навигацию по файловому хранилищу.

	  Примечания: 
				1. Первый вызов этой функции должен сопровождаться командой "FIRST_FILE". Эта 
	  Команда инициализирует структуру "Log_Fs_FileProperties" и даёт нам информацию о самом 
	  старом файле в директории. 
			    2. Для получения информации о других файлах, после вызова с командой "FIRST_FILE"
	  можно просматривать последовательно от самого старого к самому новому (другой порядок 
	  невозможен), когда будет получена информация о самом новом файле, функция вернет
	  признак того, что был прочитан последний файл "FS_ALL_FILES_SCROLLS".
				3. Командой LAST_FILE можно так же инициализировать структуру (вместо "FIRST_FILE")
	  и получить информацию сразу о самом новом файле, после чего функция так же вернет 
	  "ALL_FILES_SCROLLS". 
				4. Для того, чтобы опять посмотреть информацию о файлах, после выдачи функцией 
	  "ALL_FILES_SCROLLS", структуру Log_Fs_FileProperties нужно снова инициализовать. См п.1 
	  или п.3.

	   Параметры:
					CMD   - Команда, определяющая  режим выполнения функции

	   Возвращает:
					ERROR - Структура не была проинициализирована
					      - Неопознаная команда
						  - Ошибка в файловой системе, заголовок начала данного файла не найден
		FS_ALL_FILES_SCROLLS - Все файлы были успешно просмотрены
		         FS_NOT_OVER - Еще остались файлы, которые можно просмотреть.

***************************************************************************************************/
Log_Fs_Status Log_Fs_FindFile(uint8_t CMD);



/***************************************************************************************************

	  Log_Fs_GetFileProperties - Функция позволяет узнать параметры файла, который был выбран 
	   функцией "Log_Fs_FindFIle": Размер в байтах и порядковый номер в хранилище

	   Параметры:
					CMD - Команда - запрос, что требуется узнать о файле 
					(FILE_SIZE - Размер в байтах, FILE_NUM - порядковый номер в хранилище)

	   Возвращает:
					FS_ERROR - Файл пуст, предвариельно не была использована Log_Fs_FindFIle();
					или файловая система повреждена;
					Размер в байтах файла ( FILE_SIZE );
					Порядковый номер файла в хранилище (FILE_NUM);

***************************************************************************************************/
uint32_t Log_Fs_GetFileProperties(uint8_t CMD);



/***************************************************************************************************

	  LogFs_ReadFile - Функция чтения информации из файлов. Должна запускаться только после функции 
	  Log_Fs_FindFile(), поскольку эта функция и позволяет нам определить какой файл будем 
	  читать. То, какой файл будем читать содержится в структуре Log_Fs_FileProperties.

	  Примечания:
					1. Перед использованием нуобходимо получить информацию о необходимом файле 
				с помощью Log_Fs_FindFile(). Как только желанный файл был найден
				и структура "Log_Fs_FileProperties" будет содежать информацию о нем, 
				файл готов к считыванию.
					2. Функция позволяет читать файл за несколько раз (даже по байту), так как
				запоминает, где чтение из файла прервалось в прошлый раз. При этом структура,
				"Log_Fs_FileProperties" должна быть такой же, что и при предыдущем обращении.
				Обращение к функции с целью "дочитать" файл, с информацией в "Log_Fs_FileProperties"
				о другом файле, перезагружает функцию, таким образом будет читаться с нуля файл 
				в "Log_Fs_FileProperties".

	   Параметры:
					Buffer                 - Указатель на буфер, откуда брется информация для запили
					Size                   - Размер в байтах, записываемой информации

	   Возвращает:
					FS_FILE_END - Файл был прочитан целиком
					FS_FINE	 - Цикл чтения завершен, но конец файла не достигнут

***************************************************************************************************/
Log_Fs_Status LogFs_ReadFile(uint8_t* Buffer, uint32_t Size);



/***************************************************************************************************

	   Log_Fs_FindFile_ByNum - Функция осуществляет поиск файла по номеру.

	   Параметры:
					NUM - Порядковый номер файла, который требуется найти в хранилище

	   Возвращает:
					FS_ERROR - Такого файла не существует
					FS_FINE  - Файл найден, готов к чтению

***************************************************************************************************/
Log_Fs_Status Log_Fs_FindFile_ByNum(uint16_t NUM);


#endif
