libmeng��һ����ƽ̨��������Э�̿�

����ʾ����
	// ��������Э�̣�ָ��������ڡ���ջ��С������
	meng * m1 = meng_create(func1, 8 * 1024, (void*)1);
	meng * m2 = meng_create(func2, 8 * 1024, (void*)2);
	
	// ѭ��ִ������Э��
	while (!meng_end(m1) || !meng_end(m2))
	{
		meng_run(m1);
		meng_run(m2);
	}
	
	// ����
	meng_delete(m1);
	meng_delete(m2);