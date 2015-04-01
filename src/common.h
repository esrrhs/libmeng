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
	char * father_context;
	char * last_context;
	char * stack;
	size_t stacksize;
	void * arg;
};

#define CONTEXT_SIZE (100)

// save old, load new
extern "C" void swap_context(char * old_context, char * new_context);
