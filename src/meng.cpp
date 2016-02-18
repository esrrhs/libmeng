#include "meng.h"
#include "common.h"
#include <assert.h>

void on_meng_main_quit()
{
	meng * p = get_meng();
	assert(p);
	p->status = ms_end;
	assert(p->magic == (int)0xDEADBEEF);
	swap_context(p->last_context, p->father_context);
}

MENG_API meng * meng_create(meng_main func, size_t stacksize, void * arg, size_t argsize)
{
	size_t wrapargsize = (argsize + 7) / 8 * 8;
	size_t size = sizeof(meng) + wrapargsize + stacksize + CONTEXT_SIZE * 2;
	meng * ret = (meng *)MMALLOC(size);
	memset(ret, 0, size);
	memcpy((char *)ret + sizeof(meng), arg, argsize);
	ret->func = func;
	ret->stack = (char *)ret + sizeof(meng) + wrapargsize;
	ret->father_context = (char *)ret + sizeof(meng) + wrapargsize + stacksize;
	ret->last_context = (char *)ret + sizeof(meng) + wrapargsize + stacksize + CONTEXT_SIZE;
	ret->stacksize = stacksize;
	ret->status = ms_start;
	ret->magic = 0xDEADBEEF;

	ini_context(ret->last_context);

	// ���ó�ʼֵ
	long * sp = (long*)(ret->stack + stacksize);
	sp -= 3; // arg
	*sp = (long)ret;
	*(sp + 1) = (long)((char *)ret + sizeof(meng));
	*(sp + 2) = (long)argsize;
	*--sp = (long)on_meng_main_quit; // return
	*(long *)(ret->last_context + CONTEXT_RIP_POS) = (long)func;
	*(long *)(ret->last_context + CONTEXT_RBP_POS) = (long)sp;
	*(long *)(ret->last_context + CONTEXT_RSP_POS) = (long)sp;
	*(long *)(ret->last_context + CONTEXT_RDI_POS) = (long)ret;
	*(long *)(ret->last_context + CONTEXT_RSI_POS) = (long)((char *)ret + sizeof(meng));
	*(long *)(ret->last_context + CONTEXT_RDX_POS) = (long)argsize;

	return ret;
}

// ��ת����mִ�У������ı��浽m�У�ͬʱ�Ѿɵ�m��������ȡ��
MENG_API void meng_run(meng * m)
{
	if (m->status == ms_start)
	{
		swap_context(m->father_context, m->last_context);
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
