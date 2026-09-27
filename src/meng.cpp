#include "meng.h"
#include "common.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static thread_local meng * g_current_meng = nullptr;

extern "C" meng * get_meng()
{
	return g_current_meng;
}

extern "C" void on_meng_main_quit()
{
	meng * p = g_current_meng;
	assert(p);
	p->status = ms_end;
	assert(p->magic == (uint32_t)MENG_MAGIC);
	swap_context(p->last_context, p->father_context);
}

static void meng_trampoline()
{
	meng * m = g_current_meng;
	assert(m);
	if (m->func)
	{
		m->func(m, m->arg, m->argsize);
	}
	on_meng_main_quit();
}

MENG_API meng * meng_create(meng_main func, size_t stacksize, const void * arg, size_t argsize)
{
	if (!func)
		return nullptr;

	if (stacksize == 0)
		stacksize = MENG_DEFAULT_STACK_SIZE;

	// Align stack size to 16 bytes
	stacksize = (stacksize + 15) / 16 * 16;

	size_t wrapargsize = (argsize > 0) ? ((argsize + 15) / 16 * 16) : 0;
	size_t total_size = sizeof(meng) + wrapargsize + stacksize + CONTEXT_SIZE * 2 + 16;
	meng * ret = (meng *)MMALLOC(total_size);
	if (!ret)
		return nullptr;

	memset(ret, 0, total_size);

	ret->func = func;
	ret->stacksize = stacksize;
	ret->argsize = argsize;
	ret->status = ms_start;
	ret->magic = MENG_MAGIC;

	char * ptr = (char *)ret + sizeof(meng);
	if (arg && argsize > 0)
	{
		memcpy(ptr, arg, argsize);
		ret->arg = ptr;
	}
	else
	{
		ret->arg = nullptr;
	}
	ptr += wrapargsize;

	// Align stack start to 16 bytes
	uintptr_t stack_addr = (uintptr_t)ptr;
	stack_addr = (stack_addr + 15) / 16 * 16;
	ret->stack = (char *)stack_addr;

	ret->father_context = ret->stack + stacksize;
	ret->last_context = ret->father_context + CONTEXT_SIZE;

	ini_context(ret->last_context);

#if defined(_WIN32)
	#if defined(__x86_64__) || defined(_M_X64)
	// Windows x64 ABI: 16-byte aligned stack + 32-byte shadow space
	uintptr_t sp = (uintptr_t)(ret->stack + stacksize);
	sp = (sp & ~0xFULL) - 40;

	*(long long *)(ret->last_context + CONTEXT_RIP_POS) = (long long)meng_trampoline;
	*(long long *)(ret->last_context + CONTEXT_RSP_POS) = (long long)sp;
	*(long long *)(ret->last_context + CONTEXT_RBP_POS) = (long long)sp;
	#else
	// Windows x86 32-bit
	uintptr_t sp = (uintptr_t)(ret->stack + stacksize);
	sp = (sp & ~0xFULL) - 16;

	*(long *)(ret->last_context + CONTEXT_RIP_POS) = (long)meng_trampoline;
	*(long *)(ret->last_context + CONTEXT_RSP_POS) = (long)sp;
	*(long *)(ret->last_context + CONTEXT_RBP_POS) = (long)sp;
	#endif
#elif defined(__x86_64__) || defined(_M_X64)
	// Linux / macOS System V AMD64 ABI:
	// Stack grows downward.
	// Function entry requirement: (%rsp + 8) % 16 == 0.
	uintptr_t sp = (uintptr_t)(ret->stack + stacksize);
	sp = (sp & ~0xFULL) - 8;

	*(long *)(ret->last_context + CONTEXT_RIP_POS) = (long)meng_trampoline;
	*(long *)(ret->last_context + CONTEXT_RSP_POS) = (long)sp;
	*(long *)(ret->last_context + CONTEXT_RBP_POS) = (long)sp;
#elif defined(__aarch64__) || defined(_M_ARM64)
	// ARM64 AAPCS64
	uintptr_t sp = (uintptr_t)(ret->stack + stacksize);
	sp = sp & ~0xFULL;

	*(long *)(ret->last_context + CONTEXT_LR_POS) = (long)meng_trampoline;
	*(long *)(ret->last_context + CONTEXT_FP_POS) = (long)sp;
	*(long *)(ret->last_context + CONTEXT_SP_POS) = (long)sp;
#elif defined(__i386__) || defined(_M_IX86)
	uintptr_t sp = (uintptr_t)(ret->stack + stacksize);
	sp = (sp & ~0xFULL) - 16;

	*(long *)(ret->last_context + CONTEXT_RIP_POS) = (long)meng_trampoline;
	*(long *)(ret->last_context + CONTEXT_RSP_POS) = (long)sp;
	*(long *)(ret->last_context + CONTEXT_RBP_POS) = (long)sp;
#endif

	return ret;
}

MENG_API void meng_run(meng * m)
{
	if (!m || m->status == ms_end)
		return;

	assert(m->magic == (uint32_t)MENG_MAGIC);

	meng * old = g_current_meng;
	g_current_meng = m;
	m->status = ms_running;

	swap_context(m->father_context, m->last_context);

	if (m->status != ms_end)
	{
		m->status = ms_start;
	}
	g_current_meng = old;
}

MENG_API bool meng_end(meng * m)
{
	return !m || m->status == ms_end;
}

MENG_API void meng_delete(meng * m)
{
	if (m)
	{
		MFREE(m);
	}
}

MENG_API void meng_yield(meng * m)
{
	if (!m)
		return;

	assert(m->magic == (uint32_t)MENG_MAGIC);
	swap_context(m->last_context, m->father_context);
}

MENG_API meng * meng_running(void)
{
	return g_current_meng;
}

MENG_API void meng_yield_current(void)
{
	if (g_current_meng)
	{
		meng_yield(g_current_meng);
	}
}

MENG_API void * meng_get_arg(meng * m)
{
	return m ? m->arg : nullptr;
}

MENG_API size_t meng_get_arg_size(meng * m)
{
	return m ? m->argsize : 0;
}

MENG_API size_t meng_get_stack_size(meng * m)
{
	return m ? m->stacksize : 0;
}
