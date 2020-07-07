#ifndef _LOG_FS_TESTS_
#define _LOG_FS_TESTS_

typedef enum{
	PASSED,
	FAILED
}LogFsTestStatus;


LogFsTestStatus LogFs_plarformdependTest(void);
LogFsTestStatus LogFs_formatTest(void);
LogFsTestStatus LogFs_rewriteTest(int cycles, int fileSize);

#endif
