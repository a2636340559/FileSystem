// �ļ�ϵͳ.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include"filesystem.h"
using namespace std;

int main()
{
	//��ʼ���ļ�ϵͳ
	FILE* file = fopen("C:\\Users\\lenovo\\Desktop\\Disk.txt", "rb");
	fseek(file, 0, SEEK_END);  //����fseek���ļ�ָ���Ƶ��ļ�ĩβ
	int n = ftell(file);
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (n == 0)
	{
		init();
		cout << "initial" << endl;
	}
	cout << "-------------��ʼ�����--------------"<<endl;
	road[0] = 0;
	num = 1;
	login();
	readSuper();
	parseCommand();
	updateSuper();
    return 0;
}

