#include"struct.h"
#include <tchar.h>
#include<iostream>
#include<string>
#include<fstream>
#include<algorithm>
#include<ctime>
#include<math.h>
#include<iomanip>
using namespace std;
#pragma warning( disable : 4996)
int userNum=2;
char currentUser[6];
string currentName;
char currentUmask[4]="022";
char currentGroup[6];
SuperBlock superblock;
int road[112];//目录路径
int num = 0;//目录层数
int t = 0;//递归层数
void BinaryRecursion(int n, string& temp)//转化为2进制
{
	int a;
	string temp1="";
	for (int a = n; a; a = a / 2)
		temp1= temp1 + (a % 2 ? '1' : '0');
	reverse(temp1.begin(), temp1.end());
	temp += temp1;
}
void turnMode(char* mode)//根umask确定mode
{
	string bumask = "";
	string temp1 = "";
	string def;
	
	if (mode[0] == 'd')
		def = "777";
	else
		def = "666";
	
	for (int n = 0; n < 3; n++)
	{
		int j = (def[n] - 48)-(currentUmask[n]-48);

		BinaryRecursion(j,bumask);
	}
	for (int i = 0; i < 9; i++)
	{
		if (i % 3 == 0)
		{
			if (bumask[i]-48 ==0)
				mode[i + 1] = '-';
			if (bumask[i] - 48 == 1)
				mode[i + 1] = 'r';
		}
		if (i % 3 == 1)
		{
			if (bumask[i] - 48 == 0)
				mode[i + 1] = '-';
			if (bumask[i] - 48 == 1)
				mode[i + 1] = 'w';
		}
		if (i % 3 == 2)
		{
			if (bumask[i] - 48 == 0)
				mode[i + 1] = '-';
			if (bumask[i] - 48 == 1)
				mode[i + 1] = 'x';
		}	
	}
}
string turnUmask(char* mode)
{
	string umask="";
	int temp1;
	for (int j = 0; j < 9; j++)
	{
		int temp2 = j % 3;
		if (temp2 == 0)temp1 = 0;
		if (mode[j + 1] == '-')
			temp1 += temp2*pow(2, 3 - temp2);	
		if(temp2==2)
			umask += to_string(temp1);
	}
	return umask;
}
void readInode(int index, Inode& temp)
{
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in|ios::out);
	if (!disk)
	{
		cout << "读取索引结点时，访问磁盘失败！";
		exit(1);
	}
	disk.seekg(512 + 64 * index);
	disk >> temp;
	disk.close();
}
void showSuperBlock()
{
	cout << "空闲结点号栈(-1表示已使用)：" << endl;
	for (int i = 0; i < 112; i++)
	{
		cout << superblock.freeInode[i]<<" ";
		if (i % 20 == 0)cout << endl;
	}
	cout <<endl<< "索引结点总数：" << superblock.freeICount << endl;
	cout << "空闲结点总数：" << superblock.surFICount << endl;
	cout << "空闲块栈(-1表示已使用):" << endl;
	for (int i = 0; i < 5; i++)
		cout << superblock.freeBlock[i] << " ";
	cout << endl << "数据盘快总数：" << superblock.freeBCount << endl;
	cout  << "空闲盘块总数： " << superblock.surFBCount << endl;
}
void writeInode(Inode inode, int index)
{
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "写入索引结点时，访问磁盘失败！";
		exit(1);
	}
	disk.seekg(512 + 64 * index);
	disk << inode;
}
bool havePower(Inode &inode)
{
	if (!strcmp(currentUser ,inode.owner))
	{
		if (inode.mode[2] == 'w')
			return true;
		else return false;
	}
	else if (!strcmp(currentGroup,inode.owner))
	{
		cout << "2:currentUser:" << currentGroup << endl;
		if (inode.mode[5] == 'w')
			return true;
		else return false;
	}
	else
	{
		//cout << "3:currentUser:" << currentGroup << endl;
		if (inode.mode[8] == 'w')
			return true;
		else return false;
	}
}
bool isExist(string name,Inode inode,int& i,int& subIindex,string &subname)
{
	bool isExist = false;
	string name1;
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in);
	if (!disk)
	{
		cout << "遍历目录时，访问磁盘失败！";
		exit(1);
	}
	for ( i = 0; i < inode.filesize / 36; i++)
	{
		disk.seekg(512 * inode.fAddress[0] + 36 * i);
		disk >> name1;
		if (name == name1)
		{
			isExist = true;
			subname = name1;
			disk >> subIindex;
			//cout << "subindex:" << subIindex << endl;
			break;
		}
	}
	return isExist;
}
bool find(string path)//查找文件路径
{
	int pos = 0;
	char name[14] = "";
	Inode inode;
	if (path[pos] == '/')
	{
		currentName = "/";
		road[0] = 0;
		num = 1;
		pos++;
	}
	while (true)
	{
		readInode(road[num - 1], inode);
		
		for (int i = 0; (path[pos] != '/') && (path[pos] != '\0'); pos++, i++)
		{
			if (i == 15)
			{
				cout << "错误的文件地址，文件大小不能超过14位" << endl;
				return false;
			}
			name[i] = path[pos];
		}

		int i, subindex;
		string subname;
		if (isExist(name, inode,i,subindex, subname))
		{
			currentName = subname;
			road[num] = subindex;
			num++;
			if (path[pos] == '\0')
				return true;
			else pos++;
		}
		else
			return false;
	}
}
int requestNode()//请求分配结点
{
	if(superblock.surFICount>0)
	{
		int index = superblock.freeInode[superblock.freeICount - superblock.surFICount];
		superblock.freeInode[superblock.freeICount - superblock.surFICount] = -1;
		superblock.surFICount--;
		return index;
	}
	else return -1;
}
int requestBlock()//请求分配块
{
	int index = superblock.freeBlock[5 - superblock.surFBCount];
	if (superblock.surFBCount == 1)
	{
		if (index == 0)//磁盘块已分配完
			return -1;
		superblock.freeBlock[5 - superblock.surFBCount] = -1;
		superblock.surFBCount = 0;
		fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in|ios::out);
		if (!disk)
		{
			cout << "分配磁盘块时，访问磁盘失败！";
			exit(1);
		}
			int index1, tnum=0;
			disk.seekg(512 * index);
			for (int i = 0; i < 5; i++)
			{
				disk >> index1;
				tnum++;
				if (index1 == 0)break;
			}
			disk.seekg(512 * index);
			for (int j = 0; j < tnum; j++)
			{
				disk >> index1;
				superblock.freeBlock[j] = index1;
			}
			superblock.surFBCount = tnum;	
			disk.seekp(512 * index);
			disk << setw(512) << " ";
		disk.close();
		return index;
	}
	else
	{
		superblock.freeBlock[5 - superblock.surFBCount] = -1;
		superblock.surFBCount--;
		return index;
	}
}
void freeInode(int Iindex)//释放索引结点
{
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "释放索引结点时，访问磁盘失败！";
		exit(1);
	}
	disk.seekp(512 + 64 * Iindex);
	disk << setw(64) << "";
	disk.close();
	for (int i = superblock.freeICount - superblock.surFICount; i < superblock.freeICount; i++)
	{
		if (superblock.freeInode[i] < Iindex)
			superblock.freeInode[i - 1] = superblock.freeInode[i];
		else
		{
			superblock.freeInode[i - 1] = Iindex;
			break;
		}
	}
	superblock.surFICount++;
}
void freeBlock(int Bindex)
{
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "释放盘块时，访问磁盘失败！";
		exit(1);
	}
	disk.seekp(512 * Bindex);
	disk << setw(512) << ' ';
	if (superblock.surFBCount == 5)
	{
		disk.seekp(512 * Bindex);
		for (int i = 0; i < 5; i++)
		{
			disk << setw(4) << superblock.freeBlock[i];
			superblock.freeBlock[i] = -1;
		}
		superblock.surFBCount = 0;
	}
	disk.close();
	superblock.freeBlock[4 - superblock.surFBCount] = Bindex;
	superblock.surFBCount++;
}
void readDir(Inode inode, int index, Direct& direct)
{
	fstream dir("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!dir)
	{
		cout << "读入目录失败！";
		exit(1);
	}
	dir.seekg(512 * inode.fAddress[0] + 36 * index);
	dir >> direct;
	dir.close();
}
void writeDir(Inode inode, Direct direct, int index)//写入目录
{
	fstream dir("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!dir)
	{
		cout << "写入目录失败！";
		exit(1);
	}
	dir.seekp(512 * inode.fAddress[0] + 36 * index);
	dir << direct;
	dir.close();
}
void updateCurrentInode(Inode&inode, int i)
{
	
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "删除文件时，访问磁盘失败！";
		exit(1);
	}
	disk.seekp(512 * inode.fAddress[0] + 36 * i);
	disk << setw(36) << ' ';
	disk.close();
	Direct dir;
	int j;
	for (j = i + 1; j < inode.filesize / 36; j++)
	{
		readDir(inode, j, dir);
		writeDir(inode, dir, j - 1);
	}
	fstream disk1("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	disk1.seekp(512 * inode.fAddress[0] + 36 * (j-1));
	disk1 << setw(36) << ' ';
	disk1.close();
	
	inode.filesize -= 36;
	char temp[9];
	time_t t;
	time(&t);
	_strtime_s(temp);
	strcpy(inode.modifyTime, temp);
	disk.close();
}
void mkdir(char* name)
{
	Inode inode;
	readInode(road[num - 1], inode);
	int i,subindex;
	string subname;
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "创建目录时，访问磁盘失败！";
		exit(1);
	}
	if (havePower(inode))//当有写权限时
	{
		if (inode.filesize / 36 == 14)
			cout << "当前目录已满！" << endl;
		else
		{
			if (isExist(name, inode,i,subindex,subname))
				cout << "该目录已存在！"<<endl;
			else
			{
				int Iindex = requestNode();
				if (Iindex != -1)
				{
					int Bindex = requestBlock();
					if (Bindex != -1)
						/*修改当前目录盘块*/
					{
						disk.seekp(512 * inode.fAddress[0] + inode.filesize);
						disk << setw(14) << name;//添加新创建的目录名
						disk << setw(4) << Iindex;//新建目录的索引结点号
						if (inode.filesize == 0)
							disk << setw(14) << "/";
						else
							disk << setw(14) << "..";
						disk << setw(4) << road[num - 1];
						inode.filesize += 36;
						char temp[9];
						time_t t;
						time(&t);
						_strtime_s(temp);
						strcpy(inode.modifyTime, temp);
						Inode newDirect;
						newDirect.filesize = 0;
						newDirect.fbnum = 1;
						newDirect.fAddress[0] = Bindex;
						strcpy(newDirect.owner, currentUser);
						strcpy(newDirect.group, currentGroup);
						char mode[11] = "drwxrwxrwx";
						turnMode(mode);
						strcpy(newDirect.mode, mode);
						strcpy(newDirect.modifyTime, temp);
						disk.seekp(512 + 64 * Iindex);
						disk << newDirect;//将新建目录的索引结点写入磁盘
						//disk.close();
						cout << "目录创建成功" << endl;
					}
					else
					{
						freeInode(Iindex);
						cout << "磁盘空间不足，创建失败!" << endl;
					}
				}
				else
				{
					cout << "无可用索引结点，创建失败！" << endl;
				}
			}
		}
	}
	else
		cout << "不具备创建权限" << endl;
	writeInode(inode, road[num - 1]);//更新当前目录的索引结点
	disk.close();
}
void rm(string filename)//删除文件
{
	Inode inode, inode1;
	readInode(road[num - 1], inode);
	if (havePower(inode))
	{
		int index1, i;
		string temp;
		if (isExist(filename, inode, i, index1, temp))
		{
			readInode(index1, inode1);
			if (havePower(inode1))
			{
				if (inode1.mode[0] = '_')
				{
					if (inode1.fbnum <= 4)
					{
						for (int j = inode1.fbnum; j >0 ; j--)
							freeBlock(inode1.fAddress[j-1]);
					}
					else if (inode1.fbnum <= 6)
					{	
						fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
						for (int i = 6 - inode1.fbnum; i > 0; i--)
						{
							int index;
							disk.seekg(inode1.fAddress[4] * 512+4*(i-1));
							disk >> index;
							freeBlock(index);
						}
						disk.close();
						freeBlock(inode1.fAddress[4]);
						for (int j = 4; j>0; j--)
							freeBlock(inode1.fAddress[j-1]);
					}
					else
					{
						fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
						int temp;
						disk.seekg(inode1.fAddress[5] * 512 + 4 * 0);
						disk >> temp;
						for (int n1 = 0; n1 < inode1.fbnum - 6; n1++)
						{
							int temp1;
							disk.seekg(temp * 512 + 4 * n1);
							disk >> temp1;
							freeBlock(temp1);
						}
						freeBlock(temp);
						freeBlock(inode1.fAddress[5]);
						for (int i = 2; i > 0; i--)
						{
							int index2;
							disk.seekg(inode1.fAddress[4] * 512 + 4 * (i - 1));
							disk >> index2;
							freeBlock(index2);
						}
						disk.close();
						freeBlock(inode1.fAddress[4]);
						for (int j = 4; j>0; j--)
							freeBlock(inode1.fAddress[j - 1]);
					}
					freeInode(index1);
					updateCurrentInode(inode, i);
					cout << "文件已删除" << endl;
				}
				else
					cout << "删除目录请使用命令：Rmdir" << endl;
			}
			else
				cout << "不具备删除权限！" << endl;
		}
		else
			cout << "该文件不存在！" << endl;
	}
	else
		cout << "不具备删除权限！" << endl;
	writeInode(inode, road[num - 1]);
}
void rmdir(char* name,int currentIndex)//删除目录
{
	t++;
	Inode inode,inode1;
	Direct dir;
	readInode(currentIndex, inode);

	if (havePower(inode))
	{
		int i, index1;
		string temp = "";
		if (isExist(name, inode, i, index1, temp))
		{
			readInode(index1, inode1);
			if (havePower(inode1))
			{
				if (inode1.mode[0] == 'd')
				{
					if (inode1.filesize != 0)
					{
						char subname[14];
						int index2;
						Inode inode2;
						for (int i = 0; i < inode1.filesize / 36; i++)
						{
							fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
							if (!disk)
							{
								cout << "删除目录时，访问磁盘失败！";
								exit(1);
							}
							disk.seekg(inode1.fAddress[0] * 512 + 36 * i);
							disk >> subname;
							disk >> index2;
							disk.close();
							
							road[num] = index1;
							num++;
							readInode(index2, inode2);
							if (inode2.mode[0] == 'd')
								rmdir(subname, index1);
							else
								rm(subname);
						}
						num--;
						road[num] = 0;
						rmdir(name, currentIndex);
					}
					else
					{
						freeBlock(inode1.fAddress[0]);
						freeInode(index1);
						updateCurrentInode(inode, i);
						cout << "目录删除成功" << endl;
					}
				}
				else
					cout << "删除文件请使用命令：rm" << endl;
			}
			else
				cout << "不具备删除权限！" << endl;
		}
		else
			cout << "该目录不存在！" << endl;
	}
	else
		cout << "不具备删除权限！" << endl;
	if (t == 1)
	{
		inode.filesize -= 36;
		char temp[9];
		time_t t;
		time(&t);
		_strtime_s(temp);
		strcpy(inode.modifyTime, temp);
	}
	writeInode(inode, currentIndex);
	t--;
}
void cd(string path)//切换目录
{
	if (path == ".")
	{
		cout << "已切换至当前目录" << endl;
	}
	else if (path == "/")
	{
		currentName = "/";
		road[0] = 0;
		num = 1;
		cout << "已切换至根目录" << endl;
	}
	else if (path == "..")
	{
		if (currentName != "/")
		{
			Inode inode;
			readInode(road[num - 2], inode);
			string name;
			fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
			if (!disk)
			{
				cout << "切换目录时，访问磁盘失败！";
				exit(1);
			}
			disk.seekg(inode.fAddress[0] * 512 + 18);
			disk >> name;
			disk.close();
			currentName = name;
			num--;
			cout << "已切换至父目录" << endl;
		}
	}
	else
	{
		string::size_type id;
		id = path.find("/");
		if (id == string::npos)//切换至子目录
		{
			Inode inode, inode1;
			int i, subindex;
			string subname;
			readInode(road[num - 1], inode);
			if (isExist(path, inode, i, subindex, subname))
			{
				readInode(subindex, inode1);
				if (inode1.mode[0] == 'd')
				{
					currentName = subname;
					road[num] = subindex;
					num++;
					cout << "已切换至子目录" << endl;
				}
				else
					cout << path << "为数据文件，无法切换" << endl;
			}
			else
				cout << "不存在该子目录" << endl;
		}
		else//根据路径切换
		{
			string temppath = currentName;
			int temproad[112];
			int tempnum = num;
			for (int i = 0; i < num; i++)
				temproad[i] = road[i];
			if (find(path))
			{
				Inode inode;
				readInode(road[num - 1], inode);
				if (inode.mode[0] = 'd')
					cout << "已切换至该目录" << endl;
			}
			else
			{

				currentName = temppath;
				num = tempnum;
				for (int i = 0; i < num; i++)
					road[i] = temproad[i];
			}
		}
	}
}
void ls()//浏览目录
{
	Inode inode,inode1;
	readInode(road[num - 1], inode);
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in|ios::out);
	if (!disk)
	{
		cout << "遍历目录时，访问磁盘失败！";
		exit(1);
	}
	string name;
	int index;
	for (int i = 0; i < inode.filesize / 36; i++)
	{
		disk.seekg(inode.fAddress[0] * 512 + 36 * i);	
		disk >> name;
		disk >> index;
		readInode(index, inode1);
		cout<<endl<< name<<" "<<inode1.filesize<<" "<<inode1.owner<<" "<<inode1.group<<" "<<inode1.mode<<" "<<inode1.modifyTime<<endl;
	}
	disk.close();
	cout << endl;
}
void chmod(string name,char* args)//修改文件权限
{
	Inode inode, inode1;
	readInode(road[num - 1], inode);
	int i, index1;
	string temp;
	if (isExist(name, inode, i, index1, temp))
	{
		readInode(index1, inode1);
		if (havePower(inode1))
		{
			switch (args[0])
			{
			case 'u':
			{
				switch (args[1])
				{
					case '+':
					{
						if (args[2] == 'r')
							inode1.mode[1] = 'r';
						if (args[2] == 'w')
							inode1.mode[2] = 'w';
						if (args[2] == 'x')
							inode1.mode[3] = 'x';
						break;
					}
					case '-':
					{
						if (args[2] == 'r')
							inode1.mode[1] = '-';
						if (args[2] == 'w')
							inode1.mode[2] = '-';
						if (args[2] == 'x')
							inode1.mode[3] = '-';
						break;
					}
					case '=':
					{
						for (int i = 2; args[i] != '\0'; i++)
						{
							if (i > 4)
								cout << "参数错误，权限位最多为3位" << endl;
							else
								inode1.mode[i-1] = args[i];
						}
						break;
					}
					default:
					{
						cout << "参数错误，输入-h查看命令文档" << endl;
						break;
					}
				}
				break;
			}
			case 'g':
			{
				switch (args[1])
				{
					case '+':
					{
						if (args[2] == 'r')
							inode1.mode[4] = 'r';
						if (args[2] == 'w')
							inode1.mode[5] = 'w';
						if (args[2] == 'x')
							inode1.mode[6] = 'x';
						break;
					}
					case '-':
					{

						if (args[2] == 'r')
							inode1.mode[4] = '-';
						if (args[2] == 'w')
							inode1.mode[5] = '-';
						if (args[2] == 'x')
							inode1.mode[6] = '-';
						break;
					}
					case '=':
					{
						for (int i = 2; args[i] != '\0'; i++)
						{
							if (i > 4)
								cout << "参数错误，权限位最多为3位" << endl;
							else
								inode1.mode[i+2] = args[i];
						}
						break;
					}
					default:
					{
						cout << "参数错误，输入-h查看命令文档" << endl;
						break;
					}
				}
				break;
			}
			case 'o':
			{
				switch (args[1])
				{
				case '+':
				{
					if (args[2] == 'r')
						inode1.mode[7] = 'r';
					if (args[2] == 'w')
						inode1.mode[8] = 'w';
					if (args[2] == 'x')
						inode1.mode[9] = 'x';
					break;
				}
				case '-':
				{

					if (args[2] == 'r')
						inode1.mode[7] = '-';
					if (args[2] == 'w')
						inode1.mode[8] = '-';
					if (args[2] == 'x')
						inode1.mode[9] = '-';
					break;
				}
				case '=':
				{
					for (int i = 2; args[i] != '\0'; i++)
					{
						if (i > 4)
							cout << "参数错误，权限位最多为3位" << endl;
						else
							inode1.mode[i + 5] = args[i];
					}
					break;
				}
				default:
				{
					cout << "参数错误，输入-h查看命令文档" << endl;
					break;
				}
					
				}
				break;
			}
			case 'a':
			{
				if (args[1] == '=')
				{
					for (int i = 2; args[i] != '\0'; i++)
					{
						if (i > 4)
							cout << "参数错误，权限位最多为3位" << endl;
						else
							inode1.mode[i-1] = args[i];
					}
				}
				break;
			}
				default:
				{
					cout << "参数错误，输入-h查看命令文档" << endl;
					break;
				}
					
			}
			//writeInode(inode1, index1);
			cout << "权限修改完成" << endl;
		}
		else
			cout << "不具备修改权限！" << endl;
	}
	else
		cout << "该目录或文件不存在！" << endl;
	char temp1[9];
	time_t t;
	time(&t);
	_strtime_s(temp1);
	strcpy(inode.modifyTime, temp1);
	strcpy(inode1.modifyTime, temp1);
	writeInode(inode, road[num - 1]);
	writeInode(inode1, index1);
}
void chown(string name,char* args)
{
	Inode inode, inode1;
	readInode(road[num - 1], inode);
	int i, index;
	string temp;
	if (havePower(inode))
	{
		if (isExist(name, inode, i, index, temp))
		{
			readInode(index, inode1);
			if (havePower(inode1))
			{
				char user[6];
				char group[6];
				bool isUser = false;
				fstream u("C:\\Users\\lenovo\\Desktop\\User.txt", ios::in | ios::out);
				for (int j = 0; j < userNum; j++)
				{
					u.seekg(22 * j+1);
					u >> user;
					if (!strcmp(args ,user))
					{
						isUser = true;
						u.seekg(22 * j + 13);
						u >> group;
						break;
					}
				}
				u.close();
				if (isUser)
				{
					strcpy_s(inode1.owner, args);
					strcpy_s(inode1.group, group);
					char temp1[9];
					time_t t;
					time(&t);
					_strtime_s(temp1);
					strcpy(inode.modifyTime, temp1);
					strcpy(inode1.modifyTime, temp1);
					writeInode(inode, road[num - 1]);
					writeInode(inode1, index);
					cout << "修改成功" << endl;
				}
				else
					cout << "参数错误，不存在该用户！输入help Chown查看帮助文档" << endl;
			}
			else
				cout << "不具备权限" << endl;
		}
		else
			cout << "该文件或目录不存在！" << endl;
	}
	else
		cout << "不存在权限！" << endl;
}
void chgrp(string name,char* args)
{
	Inode inode, inode1;
	readInode(road[num - 1], inode);
	int i, index;
	string temp;
	if (havePower(inode))
	{
		if (isExist(name, inode, i, index, temp))
		{
			readInode(index,inode1 );
			if (havePower(inode1))
			{
				char group[6];
				bool isGroup = false;
				fstream u("C:\\Users\\lenovo\\Desktop\\User.txt", ios::in | ios::out);
				for (int j = 0; j < userNum; j++)
				{
					u.seekg(22 * j+13);
					u >> group;
					if (!strcmp(args ,group))
					{
						isGroup = true;
						break;
					}
				}
				u.close();
				if (isGroup)
				{
					strcpy_s(inode1.group, group);
					char temp1[9];
					time_t t;
					time(&t);
					_strtime_s(temp1);
					strcpy(inode.modifyTime, temp1);
					strcpy(inode1.modifyTime, temp1);
					writeInode(inode, road[num - 1]);
					writeInode(inode1, index);
					cout << "修改成功" << endl;
				}
				else
					cout << "参数错误，不存在该用户！输入help Chgrp查看帮助文档" << endl;
			}
			else
				cout << "不具备权限！" << endl;
		}
		else
			cout << "该文件或目录不存在！" << endl;
	}
	else
		cout << "不具备权限！" << endl;
}
void mv(string name,char* args)
{
	Inode inode, inode1;
	readInode(road[num - 1], inode);
	int i, index;
	string tempname;
	if (havePower(inode))
	{
		if (isExist(name, inode, i, index, tempname))
		{
			readInode(index, inode1);
			if (havePower(inode1))
			{
				fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in|ios::out);
				disk.seekp(512 * inode.fAddress[0] + 36 * i);
				disk << setw(14) << args;
				if (inode1.mode[0] == 'd')
				{
					for (int j = 0; j < inode1.filesize / 36; j++)
					{
						disk.seekp(512 * inode1.fAddress[0] + 36 * j + 18);
						disk << setw(14) << args;
					}
				}
				disk.close();
				char temp1[9];
				time_t t;
				time(&t);
				_strtime_s(temp1);
				strcpy(inode.modifyTime, temp1);
				strcpy(inode1.modifyTime, temp1);
				writeInode(inode, road[num - 1]);
				writeInode(inode1, index);
				cout << "修改成功" << endl;
			}
			else
				cout << "不具备权限！" << endl;
		}
		else
			cout << "该文件或目录不存在！" << endl;
	}
	else
		cout << "不具备权限！" << endl;
}
void umask(char* args)
{

	if (args[0] =='n')
	{
		cout<<currentUmask << endl;
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			currentUmask[i] = args[i];
		}
	}
		
}
void mk(string name, string content)
{
	Inode inode, inode1;
	bool isCreate = false;
	readInode(road[num - 1], inode);
	if (havePower(inode))
	{
		if (512 - inode.filesize < 36)
			cout << "当前目录项已满，无法创建文件" << endl;
		else
		{
			int i, index;
			string temp;
			if (isExist(name, inode, i, index, temp))
				cout << "创建失败，该文件已存在" << endl;
			else
			{
				int blocknum;
				if (content.length() % 512 != 0)
					blocknum = content.length() / 512 + 1;
				else
					blocknum = content.length() / 512;
			
				//blocknum = 5;
				int block[6];
				int block1[2];//一级间接索引存储的盘块号
				int block2[4];//二级间接索引
				int newinode = requestNode();
				if (newinode != -1)
				{
					isCreate = true;
					int j = 0;
					for (int i = 0; i < blocknum; i++)
					{
						block[i] = requestBlock();
						
						if (block[i] == -1)
						{
							cout << "创建失败，剩余盘块不足！" << endl;
							isCreate = false;
							freeInode(newinode);
							for (int j = i - 1; j >= 0; j--)
								freeBlock(block[j]);
							break;
						}
						else
						{
							if (i <= 3)
							{
								fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
								inode1.filesize = content.length();
								inode1.fbnum = blocknum;
								for (int i = 0; i < 6; i++)
								{
									if (i < blocknum)inode1.fAddress[i] = block[i];
									else inode1.fAddress[i] = 0;
								}
								disk.seekp(512 * block[i]);
								for (; j < 512*(i+1); j++)
								{
									if (content[j] == '\0')break;
									disk << content[j];
									//cout << content[j];
								}
								cout << endl;
								disk.close();
							}
							else
							{
								inode1.fAddress[i] = block[i];
								fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
								disk.seekp(512 * block[i]);
								int t = 4;
								for (int n = 0; n < blocknum-4; n++)
								{		
									if (n <= 1)
									{	
										t++;
										block1[n] = requestBlock();
										//cout << "block1[" << n << "]:" << block1[n] << endl;
										disk << setw(4) << block1[n];
									}
									else
									{
										block[i + 1] = requestBlock();
										inode1.fAddress[i + 1] = block[i + 1];
										disk.seekp(512 * block[i+1]);
										//cout << "二级间址：" << block[i + 1]<<endl;
										int mn = 0;
										for (int m=0;m<blocknum-(4+2);m=m++)
										{
											if (m <=1)
											{
												int temp = requestBlock();
												disk << setw(4) << temp;
												//cout << "er1:" << temp << endl;
												for (int m1 = 0; m1 < blocknum - (4+2); m1++)
												{
													t++;
													mn++;
													disk.seekp(512 * temp);
													block2[m1] = requestBlock();
													//cout << "block2[" << m1 << "]:" << block2[m1] << endl;
													disk << setw(4) << block2[m1];
												}
											}
											else
											{
												int temp = requestBlock();
												disk << setw(4) << temp;
												for (int m1 = 0; m1 < blocknum -(4+2+2); m1++)
												{
													mn++;
													t++;
													disk.seekp(512 * temp);
													block2[mn] = requestBlock();
													//cout << "block2[" << mn << "]:" << block2[mn] << endl;
													disk << setw(4) << block2[mn];
												}
											}
										}
									}
								}
								if (t <= 6)
								{
									disk.seekp(512 * block1[0]);
									for (; j < content.length(); j++)
									{
										disk << content[j];
										//cout << content[j];
									}
								}
								else
								{
									disk.seekp(512 * block1[0]);
									for (; j < 512 * 6; j++)
									{
										disk << content[j];
										//cout << content[j];
									}
									disk.seekp(512 * block2[0]);
									for (; j <content.length(); j++)
									{
										disk << content[j];
										//cout << content[j];
									}
								}
								cout << endl;
								disk.close();
								break;
							}
						}
					}
					fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
					disk.seekp(512 * inode.fAddress[0] + inode.filesize);
					disk << setw(14) << name;
					disk << setw(4) << newinode;
					disk << setw(14) << currentName;
					disk << setw(4) << road[num - 1];
					inode.filesize += 36;
					char temp1[9];
					time_t t;
					time(&t);
					_strtime_s(temp1);
					strcpy(inode.modifyTime, temp1);
					strcpy(inode1.owner, currentUser);
					strcpy(inode1.group, currentGroup);
					char mode[11] = "-rw-rw-rw-";
					turnMode(mode);
					strcpy(inode1.mode, mode);
					strcpy(inode1.modifyTime, temp1);
					disk.close();
					writeInode(inode1, newinode);
					cout << "文件创建成功！" << endl;
				}
				else
					cout << "创建失败，无空闲索引结点！" << endl;
			}
		}
	}
	else
		cout << "不具备权限！" << endl;
	writeInode(inode, road[num - 1]);
}
void cp(string path)//复制文件
{
	bool isFind = false;
	string content="", name;
	string tempcurrentname=currentName;
	int temproad[112];
	int tempnum = num;
	for (int i = 0; i < num; i++)
	{
		temproad[i] = road[i];
	}
	if (find(path))
	{
		Inode inode;
		readInode(road[num - 1], inode);
		if (inode.mode[0] == '-')
		{
			isFind = true;
			name = currentName;
			char temp;
			fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
			for (int i = 0; i < inode.filesize; i++)
			{
				disk.seekg(512 * inode.fAddress[0] + i);
				disk >> temp;
				content += temp;
			}
			disk.close();
		}
		else
			cout << "该命令只能用于复制数据文件！" << endl;
	}
	else
		cout << "无法根据该路径找到文件！" << endl;
	currentName = tempcurrentname;
	num = tempnum;
	for (int i = 0; i < num; i++)
	{
		road[i] = temproad[i];
	}
	if (isFind)
	{
		mk(name, content);
		cout << "文件复制成功" << endl;
	}
}
void ln(string path,char* link)
{
	Inode inode, inode1;
	int i, index;
	string  name,temp;
	string tempcurrentname = currentName;
	int temproad[112];
	int tempnum = num;
	for (int i = 0; i < num; i++)
		temproad[i] = road[i];
	readInode(road[num - 1], inode);
	if (havePower(inode))
	{
		if (inode.filesize / 36 == 14)
			cout << "当前目录项已满，创建失败！" << endl;
		else
		{
			if (isExist(link, inode, i, index, temp))
				cout << "已存在同名文件，创建失败！" << endl;
			else
			{
				if (find(path))
				{
					readInode(road[num - 1], inode1);
					cout << "num: " << num << endl;
					cout << "road: " << road[num - 1] << endl;
					fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
					disk.seekp(512 * inode.fAddress[0] + inode.filesize);
					disk << setw(14) << link;//连接名
					disk << setw(4) << road[num - 1];//目标文件的索引结点号
					if (temproad[tempnum-1] == 0)
						disk << setw(14) << "/";
					else
						disk << setw(14) << "..";
					disk << setw(4) << temproad[tempnum - 1];
					char temp[9];
					time_t t;
					time(&t);
					_strtime_s(temp);
					inode.filesize += 36;
					strcpy(inode.modifyTime, temp);
					writeInode(inode, temproad[tempnum - 1]);
					cout << "创建连接成功！" << endl;
					disk.close();
					num = tempnum;
					currentName = tempcurrentname;
					for (int i = 0; i < num; i++)
						road[i] =temproad[i] ;
				}
				else
					cout << "找不到目标文件，创建失败！" << endl;	
			}
		}	
	}
	else
		cout << "不具备创建权限！" << endl;
}
void cat(string name)
{
	Inode inode, inode1;
	readInode(road[num - 1], inode);
	int i, index;
	string temp;
	if (isExist(name, inode, i, index, temp))
	{
		int n = 0;
		readInode(index, inode1);
		if (inode1.mode[0] = '-')
		{
			fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
			char content;
			for (int i = 0; i < inode1.fbnum; i++)
			{
				disk.seekg(inode1.fAddress[i] * 512);
				if (i <= 3)//直接索引
				{
					if (inode1.fbnum <= 4)
					{
						for (n = 0; n < inode1.filesize; n++)
						{
							disk >> content;
							cout << content;
						}
						break;
					}
					else
					{
						for (n = 0; n < 512; n++)
						{
							disk >> content;
							cout << content;
						}
					}
				}
				else//一级间址索引
				{
					for (int j = 0; j < inode1.fbnum - 4; j++)
					{
						int temp;
						if (j <= 1)
						{	disk.seekg(inode1.fAddress[4] * 512 + 4 * j);
							disk >> temp;
							disk.seekg(temp * 512);
							if (inode1.fbnum - 4 <= 2)
							{
								for (n = 0; n < inode1.filesize - 2048; n++)
								{
									disk >> content;
									cout << content;
								}
							}
							else
							{
								for (n = 0; n < 512; n++)
								{
									disk >> content;
									cout << content;
								}
							}
						}
						else//二级间址索引
						{
							
								int temp;
								disk.seekg(inode1.fAddress[5] * 512 + 4 * 0);
								disk >> temp;
								cout <<endl<< "faddress[5]: " << inode1.fAddress[5] << endl;
								cout << "index: " << temp << endl;
								for (int n1 = 0; n1 < inode1.fbnum - 6; n1++)
								{
									int temp1;
									disk.seekg(temp*512+4*n1);
									disk >> temp1;
									cout << "index1: " << temp1 << endl;
									disk.seekg(temp1 * 512);
									for (int m1 = 0; m1 < inode1.filesize - 3072; m1++)
									{
										disk >> content;
										cout << content;
									}
								}
							
						}
					}
					break;
				}
			}
			disk.close();
		}
		else
			cout << "显示目录文件，请使用ls命令！" << endl;
	}
	else
		cout << "当前目录不存在该数据文件！" << endl;
	cout << endl;
}
void alterPass(string password)
{
	fstream u("C:\\Users\\lenovo\\Desktop\\User.txt", ios::in | ios::out);
	int n = 0;
	char tempuser[6];
	for (; n < userNum; n++)
	{
		u.seekg(22 * n+1);
		u >> tempuser;
		if (!strcmp(currentUser , tempuser))
			break;
	}
	u.seekp(22 * n + 7);
	u << setw(6) << password;
	cout << "密码修改成功" << endl;
	u.close();
}
void help(string args)
{
	if (args == "")
	{
		cout << "ls: 查看当前目录下的所有子项" << endl;
		cout << "命令形式：		Ls" << endl;
		cout << "chmod: u表示用户，g表示所属组,o表示其他用户，+为增加权限，-为取消权限，=为重新赋予权限" << endl;
		cout << "命令形式：		Chmod u+r filename " << endl;
		cout << "chown: 改变文件拥有者" << endl;
		cout << "命令形式：		Chown filename ower" << endl;
		cout << "chgrp: 改变文件所属组" << endl;
		cout << "命令形式：		Chgrp filename group" << endl;
		cout << "pwd: 显示当前目录" << endl;
		cout << "命令形式：		Pwd" << endl;
		cout << "cd: 切换目录,path为目标路径" << endl;
		cout << "命令形式：		Cd path" << endl;
		cout << "mkdir: 创建目录" << endl;
		cout << "命令形式：		Mkdir directname" << endl;
		cout << "rmdir: 删除目录" << endl;
		cout << "命令形式：		Rmdir directname" << endl;
		cout << "umask: 文件创建屏蔽码" << endl;
		cout << "命令形式：		Umask -n,查看当前文件屏蔽码" << endl;
		cout << "命令形式：		Umask umask,修改文件屏蔽码" << endl;
		cout << "mv: 修改文件名" << endl;
		cout << "命令形式：		Mv filename newname" << endl;
		cout << "cp: 复制文件到当前目录" << endl;
		cout << "命令形式：		Cp filepath" << endl;
		cout << "rm: 删除当前目录下的文件" << endl;
		cout << "命令形式：		Rm  filename" << endl;
		cout << "ln: 创建文件连接,path为目标文件的路径，link为连接名" << endl;
		cout << "命令形式：		Ln path link" << endl;
		cout << "cat: 显示文件内容" << endl;
		cout << "命令形式：		Cat filename" << endl;
		cout << "password: 修改用户口令" << endl;
		cout << "命令形式：		Password newpass" << endl;
		cout << "exit: 退出文件系统" << endl;
		cout << "命令形式：		exit" << endl;
	}
	if (args == "ls")
	{
		cout << "Ls: 查看当前目录下的所有子项" << endl;
		cout << "命令形式：		Ls" << endl;
	}
	if (args == "chmod")
	{
		cout << "Chmod: u表示用户，g表示所属组,o表示其他用户，+为增加权限，-为取消权限，=为重新赋予权限" << endl;
		cout << "命令形式：		Chmod u+r filename " << endl;
	}
	if (args == "chown")
	{
		cout << "Chown: 改变文件拥有者" << endl;
		cout << "命令形式：		Chown filename ower" << endl;
	}
	if (args == "chgrp")
	{
		cout << "Chgrp: 改变文件所属组" << endl;
		cout << "命令形式：		Chgrp filename group" << endl;
	}
	if (args == "pwd")
	{
		cout << "Pwd: 显示当前目录" << endl;
		cout << "命令形式：		Pwd" << endl;
	}
	if (args == "cd")
	{
		cout << "Cd: 切换目录,path为目标路径" << endl;
		cout << "命令形式：		Cd path" << endl;
	}
	if (args == "mkdir")
	{
		cout << "Mkdir: 创建目录" << endl;
		cout << "命令形式：		Mkdir directname" << endl;
	}
	if (args == "rmdir")
	{
		cout << "Rmdir: 删除目录" << endl;
		cout << "命令形式：		Rmdir directname" << endl;
	}
	if (args == "umask")
	{
		cout << "Umask: 文件创建屏蔽码" << endl;
		cout << "命令形式：		Umask -n,查看当前文件屏蔽码" << endl;
		cout << "命令形式：		Umask umask,修改文件屏蔽码" << endl;
	}
	if (args == "mv")
	{
		cout << "Mv: 修改文件名" << endl;
		cout << "命令形式：		Mv filename newname" << endl;
	}
	if (args == "cp")
	{
		cout << "Cp: 复制文件到当前目录" << endl;
		cout << "命令形式：		Cp filepath" << endl;
	}
	if (args == "rm")
	{
		cout << "Rm: 删除当前目录下的文件" << endl;
		cout << "命令形式：		Rm  filename" << endl;
	}
	if (args == "ln")
	{
		cout << "Ln: 创建文件连接,path为目标文件的路径，link为连接名" << endl;
		cout << "命令形式：		Ln path link" << endl;
	}
	if (args == "cat")
	{
		cout << "Cat: 显示文件内容" << endl;
		cout << "命令形式：		Cat filename" << endl;
	}
	if (args == "password")
	{
		cout << "Password: 修改用户口令" << endl;
		cout << "命令形式：		Password newpass" << endl;
	}
	if (args == "exit")
	{
		cout << "exit: 退出文件系统" << endl;
		cout << "命令形式：		exit" << endl;
	}
}
void init()//初始化
{
	User user1 = { "admin","admin","admin","022" };
	User user2 = { "root","root","root","022" };
	cout << user1;
	cout<< user2;
	userNum = 2;
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "初始化磁盘失败！";
		exit(1);
	}
	for (int i = 0; i < 150; i++)
	{
		disk << setw(512) << "";
	}
	
	disk.seekp(0);
	disk << setw(4) << -1;
	for (int i = 1; i < 112; i++)
	{
		disk << setw(4) << i;
	}
	disk << setw(4) << 112;
	disk << setw(4) << 111;
	for (int i = 0; i < 5; i++)
	{
		disk << setw(4) << 16 + i;
	}
	disk << setw(4) << 135;
	disk << setw(4) << 5;
	disk.seekp(512);
	disk << setw(4) << 0;
	disk << setw(4) << 1;
	disk << setw(4) << 15;
	disk << setw(4) << 0 << setw(4) << 0 << setw(4) << 0 << setw(4) << 0 << setw(4) << 0;//0表示不指向任何盘块
	disk << setw(6) << "root";
	disk << setw(6) << "root";
	char mode[11] = "drwxrwxrwx";
	//turnMode(mode);
	disk << setw(11) <<mode ;
	char temp[9];
	time_t t;
	time(&t);
	_strtime_s(temp);
	disk << setw(9) << temp;
							
	for (int i = 20; i < 150; i++)
	{
		if (i % 5 == 0)
		{
			disk.seekp(512 * i);
			for (int j = 0; j < 5; j++)
			{
				int temp = i + j + 1;
				if (temp < 150)disk << setw(4) << temp;
			}
		}
	}
	disk << setw(4) << 0;
	disk.close();
}
void login()//登录
{
	bool isRight = false;
	while (!isRight)
	{
		cout << "Login as:";
		char user[6];
		cin >> user;
		cout << endl << "Password:";
		char password[6];
		cin >> password;

		fstream User("C:\\Users\\lenovo\\Desktop\\User.txt", ios::in|ios::out);
		if (!User)
		{
			cout << "登录时，读取User表失败！" << endl;
			exit(1);
		}
		User.seekp(0);
		User >> userNum;
		char user1[6];
		char password1[6];
		for (int i = 0; i < userNum; i++)
		{
			User.seekg(22 * i+1);
			User >> user1 >> password1;
			if (strcmp(user,user1)==0 && strcmp(password,password1)==0)
			{
				isRight = true;
				strcpy_s(currentUser,user);
				currentName = "/";
				User >> currentGroup;
				break;
			}
		}
		User.close();
		if (isRight == false)
			cout << endl << "-----用户名或密码错误，请重新输入" << endl;
		else
		{
			system("cls");
			cout << "---------成功登入文件系统--------" << endl;
		}			
	}
}
void readSuper()//将超级块读入主存
{
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in);
	if (!disk)
	{
		cout << "读入超级块时，访问磁盘失败！";
		exit(1);
	}
	int i = 0;
	for (; i < 112; i++)
	{
		disk >> superblock.freeInode[i];
	}
	disk >> superblock.freeICount;
	disk >> superblock.surFICount;
	for (i = 0; i < 5; i++)
		disk >> superblock.freeBlock[i];
	disk >> superblock.freeBCount;
	disk >> superblock.surFBCount;
	disk.close();
}
void updateSuper()
{
	cout << superblock;
}
void printRoad()
{
	Inode temp;
	int index;
	char filename[14];
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "在输出路径时，访问磁盘失败！";
		exit(1);
	}
	if (num == 1)
		cout << "/";
	for (int i = 0; i < num - 1; i++)
	{
		readInode(road[i], temp);

		for (int j = 0; j < temp.filesize / 36; j++)
		{
			disk.seekg(512 * temp.fAddress[0] + 36 * j);
			disk >> filename;
			disk >> index;
			if (index == road[i + 1])//存在下一层目录
			{
				cout << '/' << filename;
				break;
			}
		}
	}
	disk.close();
}
void parseCommand()
{
	string command;
	bool isParse = false;
	while (true)
	{
		cout <<endl<< currentUser<<":";
		printRoad();
		cout << "#  ";
		cin >> command;
		if (command == "ls")//显示当前目录
		{
			isParse = true;
			ls();
		}
		if (command == "chmod")//改变文件权限
		{
			isParse = true;
			char args[5];
			cin >> args;
			char name[14];
			cin >> name;
			chmod(name,args);
		}
		if (command == "chown")
		{
			isParse = true;
			char name[14];
			char user[6];
			cin >> name >> user;
			chown(name,user);
		}
		if (command == "chgrp")
		{
			isParse = true;
			char name[14];
			cin >> name;
			char group[6];
			cin >> group;
			chgrp(name,group);
		}
		if (command == "pwd")
		{
			isParse = true;
			printRoad();
		}
		if (command == "cd")//切换目录
		{
			isParse = true;
			string path;
			cin >> path;
			cd(path);
		}
		if (command == "mkdir")//创建子目录
		{
			isParse = true;
			char name[14];
			cin >> name;
			mkdir(name);
		}
		if (command == "rmdir")
		{
			isParse = true;
			char name[14];
			cin >> name;
			rmdir(name,road[num-1]);
		}
		if (command == "umask")
		{
			isParse = true;
			char args[3];
			if (cin.get() == '\n')
				args[0]='n';
			else
				cin >> args;
			umask(args);
		}
		if (command == "mk")
		{
			isParse = true;
			string name;
			cin >> name;
			cout << endl << "输入文件内容：" << endl;
			string content;
			cin >> content;
			mk(name, content);
		}
		if (command == "mv")
		{
			isParse = true;
			char name[14];
			cin >> name;
			char newname[14];
			cin >> newname;
			mv(name,newname);
		}
		if (command == "cp")
		{
			isParse = true;
			string path;
			cin >> path;
			cp(path);
		}
		if (command == "rm")
		{
			isParse = true;
			char name[14];
			cin >> name;
			rm(name);
		}
		if (command == "ln")
		{
			isParse = true;
			string path;
			cin >> path;
			char link[14];
			cin >> link;
			ln(path, link);
		}
		if (command == "cat")
		{
			isParse = true;
			char name[14];
			cin >> name;
			cat(name);
		}
		if (command == "password")
		{
			isParse = true;
			string newpassword;
			cin >> newpassword;
			alterPass(newpassword);
		}
		if (command == "showsuper")
		{
			isParse = true;
			showSuperBlock();
		}
		if (command == "help")
		{
			isParse = true;
			string args;
			if (cin.get() == '\n')
				args = "";
			else
				cin >> args;
			help(args);
		}
		if (command == "clear")
		{
			isParse = true;
			system("cls");
		}
		if (command == "exit")
		{
			isParse = true;
			return;
		}
		if (isParse == false)
			cout << "No command " + command + " found." << endl;

	}
}

