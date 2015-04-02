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
};

// save old, load new
extern "C" void swap_context(char * old_context, char * new_context);

// ini
extern "C" void ini_context(char * context);

// get meng pointer
extern "C" meng * get_meng();

#ifdef WIN32
#define CONTEXT_SIZE (84)
#define CONTEXT_RIP_POS (32)
#define CONTEXT_RBP_POS (28)
#define CONTEXT_RSP_POS (24)
#define CONTEXT_RDI_POS (20)
#define CONTEXT_RSI_POS (16)
#else
#define CONTEXT_SIZE (264)
#define CONTEXT_RIP_POS (64)
#define CONTEXT_RBP_POS (56)
#define CONTEXT_RSP_POS (48)
#define CONTEXT_RDI_POS (40)
#define CONTEXT_RSI_POS (32)
#endif
