libmeng是一个跨平台的轻量级协程库

试用示例：

// 协程入口
void func(meng * m, void * arg)
{
	for (int i = 0; i < 10; i++)
	{
		printf("func %p %d\n", arg, i); 
		// 主动切换
		meng_yield(m);
	}
}

int main(int argc, const char * argv[])
{
	// 创建两个协程，指定函数入口、堆栈大小及参数
	meng * m1 = meng_create(func, 8 * 1024, (void*)1);
	meng * m2 = meng_create(func, 8 * 1024, (void*)2);
	
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
