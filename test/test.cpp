#include "../include/meng.h"
#include <stdio.h>
#include <iostream>

void func1(meng * m, void * arg)
{
	for (int i = 0; i < 10; i++)
	{
		printf("func1 %d\n", i);
		meng_yield(m);
	}
}

void func2(meng * m, void * arg)
{
	for (int i = 0; i < 10; i++)
	{
		printf("func2 %d\n", i);
		meng_yield(m);
	}
}

int main(int argc, const char * argv[])
{
	meng * m1 = meng_create(func1, 8 * 1024, 0);
	meng * m2 = meng_create(func2, 8 * 1024, 0);
	while (!meng_end(m1) && !meng_end(m2))
	{
		meng_run(m1);
		meng_run(m2);
	}
	meng_delete(m1);
	meng_delete(m2);

	char c;
	std::cin >> c;

	return 0;
}

