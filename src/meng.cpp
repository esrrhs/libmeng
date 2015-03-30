#include "meng.h"
#include "common.h"

MENG_API meng * meng_create(meng_main func, size_t stacksize, void * arg)
{
	meng * ret = (meng *)MMALLOC(sizeof(meng) + stacksize);
	ret->arg = arg;
	ret->func = func;
	ret->stack = (char *)ret + sizeof(meng);
	ret->stacksize = stacksize;
	ret->status = ms_start;
	return ret;
}

MENG_API void meng_run(meng * m)
{
	(m->func)(m, m->arg);
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
