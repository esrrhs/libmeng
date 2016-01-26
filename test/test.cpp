#include "../include/meng.h"
#include <stdio.h>
#include <iostream>
#include <time.h>

#ifndef WIN32
#include "gperftools/profiler.h"
#endif

#ifndef _DEBUG
#define LOOP_NUM 10000000
#else
#define LOOP_NUM 10
#endif

void func(meng * m, void * arg, size_t argsize)
{
	for (int i = 0; i < LOOP_NUM; i++)
	{
#ifdef _DEBUG
		printf("func %d %d\n", *(int*)arg, i); 
#endif
		meng_yield(m);
	}
}

int main(int argc, const char * argv[])
{
	int arg1 = 1;
	int arg2 = 2;
	meng * m1 = meng_create(func, 8 * 1024, &arg1, sizeof(arg1));
	meng * m2 = meng_create(func, 8 * 1024, &arg2, sizeof(arg2));

	unsigned int begin,end;

	begin = time(0);

#ifndef WIN32
#ifndef _DEBUG
	ProfilerStart("meng.prof");
#endif
#endif

	while (!meng_end(m1) || !meng_end(m2))
	{
		meng_run(m1);
		meng_run(m2);
	}

#ifndef WIN32
#ifndef _DEBUG
	ProfilerStop();
#endif
#endif

	end = time(0);

	meng_delete(m1);
	meng_delete(m2);

	printf("use %d\n", end - begin);
	char c;
	std::cin >> c;

	return 0;
}

