// 文件系统.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"filesystem.h"
using namespace std;

int main()
{
	//初始化文件系统
	FILE* file = fopen("C:\\Users\\lenovo\\Desktop\\Disk.txt", "rb");
	fseek(file, 0, SEEK_END);  //先用fseek将文件指针移到文件末尾
	int n = ftell(file);
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (n == 0)
	{
		init();
		cout << "initial" << endl;
	}
	cout << "-------------初始化完成--------------"<<endl;
	road[0] = 0;
	num = 1;
	login();
	readSuper();
	parseCommand();
	updateSuper();
    return 0;
}

