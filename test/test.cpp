#include "../include/meng.h"
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <string.h>

#if defined(MENG_ENABLE_PROFILER) && !defined(WIN32)
#include "gperftools/profiler.h"
#endif

#ifndef _DEBUG
#define LOOP_NUM 10000000
#else
#define LOOP_NUM 10
#endif

void func(meng * m, void * arg, size_t argsize)
{
	(void)argsize;
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
	bool interactive = false;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
			interactive = true;
		}
	}

	int arg1 = 1;
	int arg2 = 2;
	meng * m1 = meng_create(func, 8 * 1024, &arg1, sizeof(arg1));
	meng * m2 = meng_create(func, 8 * 1024, &arg2, sizeof(arg2));

	time_t begin = time(0);

#if defined(MENG_ENABLE_PROFILER) && !defined(WIN32)
#ifndef _DEBUG
	ProfilerStart("meng.prof");
#endif
#endif

	while (!meng_end(m1) || !meng_end(m2))
	{
		meng_run(m1);
		meng_run(m2);
	}

#if defined(MENG_ENABLE_PROFILER) && !defined(WIN32)
#ifndef _DEBUG
	ProfilerStop();
#endif
#endif

	time_t end = time(0);

	meng_delete(m1);
	meng_delete(m2);

	printf("use %ld\n", (long)(end - begin));

	if (interactive) {
		char c;
		std::cin >> c;
	}

	return 0;
}
