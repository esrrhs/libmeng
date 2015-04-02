#pragma once

#include "meng.h"

#define MMALLOC(x) malloc(x)
#define MFREE(x) free(x)

enum meng_status
{
	ms_start,
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
	meng * father;
};

// save old, load new
extern "C" void swap_context(char * old_context, char * new_context);

// ini
extern "C" void ini_context(char * context);

#ifdef WIN32
#define CONTEXT_SIZE (52)
#define CONTEXT_IP_POS (32)
#define CONTEXT_SP_POS (24)
#else
#define CONTEXT_SIZE (136)
#define CONTEXT_IP_POS (64)
#define CONTEXT_SP_POS (48)
#endif
