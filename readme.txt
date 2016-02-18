**libmeng**是一个跨平台的轻量级协程库，可以轻松创建协程及RPC

试用示例：

// 协程入口
void func(meng * m, void * arg, size_t argsize)
{
	for (int i = 0; i < LOOP_NUM; i++)
	{
		printf("func %d %d\n", *(int*)arg, i);
		meng_yield(m);
	}
}

int main(int argc, const char * argv[])
{
	// 创建两个协程，指定函数入口、堆栈大小及参数
	int arg1 = 1;
	int arg2 = 2;
	meng * m1 = meng_create(func, 8 * 1024, &arg1, sizeof(arg1));
	meng * m2 = meng_create(func, 8 * 1024, &arg2, sizeof(arg2));
	
	// 循环执行两个协程
	while (!meng_end(m1) || !meng_end(m2))
	{
		meng_run(m1);
		meng_run(m2);
	}
	
	// 销毁
	meng_delete(m1);
	meng_delete(m2);

	return 0;
}