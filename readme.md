# libmeng #
**libmeng**是一个跨平台的轻量级协程库，可以轻松组建协程、RPC

# 特性 #
* 支持win32、linux64
* 支持协程中创建协程
* 支持信号处理
* 支持自定义参数

# API说明 #
### meng_create ###
创建协程对象，用来保存参数及栈信息，参数分别为：入口函数、栈大小、参数地址、参数大小
### meng_run ###
执行一个协程，直到调用meng_yield或者函数执行完
### meng_yield ###
释放执行权，跳转到meng_run的地方
### meng_end ###
判断一个协程是否结束
### meng_delete ###
释放协程对象

# 示例 #

```
#!c++


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
```