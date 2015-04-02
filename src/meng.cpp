#include "meng.h"
#include "common.h"
#include <assert.h>

meng * g_running = 0;
void on_meng_main_quit()
{
	assert(g_running);
	g_running->status = ms_end;
	swap_context(g_running->last_context, g_running->father_context);
}

MENG_API meng * meng_create(meng_main func, size_t stacksize, void * arg)
{
	size_t size = sizeof(meng) + CONTEXT_SIZE * 2 + stacksize;
	meng * ret = (meng *)MMALLOC(size);
	memset(ret, 0, size);
	ret->arg = arg;
	ret->func = func;
	ret->father_context = (char *)ret + sizeof(meng);
	ret->last_context = (char *)ret + sizeof(meng) + CONTEXT_SIZE;
	ret->stack = (char *)ret + sizeof(meng) + CONTEXT_SIZE + CONTEXT_SIZE;
	ret->stacksize = stacksize;
	ret->status = ms_start;
	ret->father = g_running;

	ini_context(ret->last_context);

	// 设置初始值
	long * sp = (long*)(ret->stack + stacksize);
	sp -= 2; // arg
	*sp = (long)ret;
	*(sp + 1) = (long)arg;
	*--sp = (long)on_meng_main_quit; // return
	*(long *)(ret->last_context + CONTEXT_IP_POS) = (long)func;
	*(long *)(ret->last_context + CONTEXT_SP_POS) = (long)sp;

	return ret;
}

// 跳转到新m执行，上下文保存到m中，同时把旧的m的上下文取出
MENG_API void meng_run(meng * m)
{
	if (m->status == ms_start)
	{
		g_running = m;
		swap_context(m->father_context, m->last_context);
		g_running = m->father;
	}
}

MENG_API bool meng_end(meng * m)
{
	return m->status == ms_end;
}

MENG_API void meng_delete(meng * m)
{
	MFREE(m);
}

MENG_API void meng_yield(meng * m)
{
	swap_context(m->last_context, m->father_context);
}
