# LogFs - file system for serial flash

LogFs is a light log based file system for serial flash memory chips. 

## Features
* Provides even wear of sectors and cells, due to the fact that files are written sequentially;
* Requires only ~ 30 bytes of RAM;
* The speed of work is limited only by low-level platform-dependent functions;
* Power disruption or unplanned reset tolerance (no lost).

## Description
The unit of size for a file is the size of the flash memory sector, in order to avoid the need to buffer the non-erasable part of the sector and increase performance. Files are created by sessions. When the device runs out of free space, new files overwrite old ones (like FIFO or circular buffer). Files (or sessions) have no names, they have only a serial number (ID) since the device was formatted.

Each sector of the flash contains markers:
`FILE_EXIST_HANDLER` - start of file  OR
`FILE_CONTINUATION`- file continuation  OR
`FILE_FREE_SPACE`- free sector.
If the sector contains a file, it will contain the file ID following the marker.

The file system uses only two private control structures `core` and `fileSelector` for operation. Struct `core` is the core of the filesystem, `fileSelector` is used to access files (points to one of the files). The file is selected using the `LogFs_findFIle()` function, with the parameters:
* `FIRST_FILE` - Oldest file
* `NEXT_FILE` - Next file
* `LAST_FILE` - Last created file

Also, you can search a file using the `LogFs_findFileByNum()` function, which allows you to open a file with an explicitly specified ID.

You can read the file using `LogFs_read()`. The `LogFs_read()` function allows you to read the same file in several requests, if it is not possible to read the entire file as a whole (usually in this case, buffering in RAM is needed, which can be a problem for large files), for this the function allows you to specify the read position from the file to continue reading the file from the specified location.


## How to use

1) Copy the contents of the "source" folder;
2) Define the parameters of your memory chip in `log.fs.platformdepend.h` , 
where 
    - `FS_SECTORS_NUM` - Number of sectors;
    - `FS_SECTOR_SIZE` - Sector size;
3) Implement your low-level read/write/eraseChip/eraseSector functions based on your platform in `log.fs.platformdepend.c`.

## Caution!
- When using this file system on media other than flash memory, you need to be careful for the file system, the empty state (erased cell) is `0xFF`. If this condition is not met on your device, `the file system won't work correctly`;
- You cannot delete files of your choice, you can only erase the whole device, not individual files. Thefile system itself takes care of overwriting files when the device is full.
- To close the file, you need to reinitialize the file system using `LogFs_initialize()`. If the file (session) is already open, the call to `LogFs_createFile()` will return an error.