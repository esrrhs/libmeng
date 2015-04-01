#include "meng.h"
#include "common.h"

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

	// 设置初始值
	int * sp = (int*)(ret->stack + stacksize);
	sp -= 1; // arg
	*--sp = 0; // return
	*(int *)(ret->last_context + 32) = (int)func;
	*(int *)(ret->last_context + 24) = (int)sp;

	return ret;
}

// 跳转到新m执行，上下文保存到m中，同时把旧的m的上下文取出
MENG_API void meng_run(meng * m)
{
	// 保存上下文
	swap_context(m->father_context, m->last_context);
	m->status = ms_end;
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
	// 保存上下文
	swap_context(m->last_context, m->father_context);
}
