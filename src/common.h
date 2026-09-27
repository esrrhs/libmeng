#pragma once

#include "meng.h"
#include <stdint.h>

#ifndef MMALLOC
#define MMALLOC(x) malloc(x)
#endif

#ifndef MFREE
#define MFREE(x) free(x)
#endif

#define MENG_MAGIC 0xDEADBEEF
#define MENG_DEFAULT_STACK_SIZE (64 * 1024)

enum meng_status
{
	ms_start = 0,
	ms_running = 1,
	ms_end = 2,
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
	size_t argsize;
	uint32_t magic;
};

// save old, load new
extern "C" void swap_context(char * old_context, char * new_context);

// ini
extern "C" void ini_context(char * context);

// get meng pointer (legacy ABI hook)
extern "C" meng * get_meng();

#if defined(__x86_64__) || defined(_M_X64)
#if defined(_WIN32)
#define CONTEXT_SIZE (256)
#define CONTEXT_RIP_POS (0)
#define CONTEXT_RSP_POS (8)
#define CONTEXT_RBP_POS (16)
#define CONTEXT_RBX_POS (24)
#define CONTEXT_RSI_POS (32)
#define CONTEXT_RDI_POS (40)
#define CONTEXT_R12_POS (48)
#define CONTEXT_R13_POS (56)
#define CONTEXT_R14_POS (64)
#define CONTEXT_R15_POS (72)
#else
// Linux / System V AMD64:
#define CONTEXT_SIZE (264)
#define CONTEXT_RIP_POS (64)
#define CONTEXT_RBP_POS (56)
#define CONTEXT_RSP_POS (48)
#define CONTEXT_RDI_POS (40)
#define CONTEXT_RSI_POS (32)
#define CONTEXT_RDX_POS (24)
#endif
#elif defined(__aarch64__) || defined(_M_ARM64)
#define CONTEXT_SIZE (176)
#define CONTEXT_LR_POS (88)
#define CONTEXT_FP_POS (80)
#define CONTEXT_SP_POS (96)
#elif defined(WIN32) || defined(__i386__) || defined(_M_IX86)
#define CONTEXT_SIZE (84)
#define CONTEXT_RIP_POS (32)
#define CONTEXT_RBP_POS (28)
#define CONTEXT_RSP_POS (24)
#define CONTEXT_RDI_POS (20)
#define CONTEXT_RSI_POS (16)
#define CONTEXT_RDX_POS (12)
#endif
