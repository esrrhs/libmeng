#pragma once

#include "meng.h"

#define MMALLOC(x) malloc(x)
#define MFREE(x) free(x)

enum meng_status
{
	ms_start,
	ms_pause,
	ms_end,
};

struct meng
{
	meng_main func;
	meng_status status;
	char * stack;
	size_t stacksize;
	void * arg;
};
