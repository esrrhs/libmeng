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
	return ret;
}

// ��ת����mִ�У������ı��浽m�У�ͬʱ�Ѿɵ�m��������ȡ��
MENG_API void meng_run(meng * m)
{
	// ����������
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

}
