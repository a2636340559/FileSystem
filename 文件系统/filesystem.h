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
int road[112];//Ŀ¼·��
int num = 0;//Ŀ¼����
int t = 0;//�ݹ����
void BinaryRecursion(int n, string& temp)//ת��Ϊ2����
{
	int a;
	string temp1="";
	for (int a = n; a; a = a / 2)
		temp1= temp1 + (a % 2 ? '1' : '0');
	reverse(temp1.begin(), temp1.end());
	temp += temp1;
}
void turnMode(char* mode)//��umaskȷ��mode
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
		cout << "��ȡ�������ʱ�����ʴ���ʧ�ܣ�";
		exit(1);
	}
	disk.seekg(512 + 64 * index);
	disk >> temp;
	disk.close();
}
void showSuperBlock()
{
	cout << "���н���ջ(-1��ʾ��ʹ��)��" << endl;
	for (int i = 0; i < 112; i++)
	{
		cout << superblock.freeInode[i]<<" ";
		if (i % 20 == 0)cout << endl;
	}
	cout <<endl<< "�������������" << superblock.freeICount << endl;
	cout << "���н��������" << superblock.surFICount << endl;
	cout << "���п�ջ(-1��ʾ��ʹ��):" << endl;
	for (int i = 0; i < 5; i++)
		cout << superblock.freeBlock[i] << " ";
	cout << endl << "�����̿�������" << superblock.freeBCount << endl;
	cout  << "�����̿������� " << superblock.surFBCount << endl;
}
void writeInode(Inode inode, int index)
{
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "д���������ʱ�����ʴ���ʧ�ܣ�";
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
		cout << "����Ŀ¼ʱ�����ʴ���ʧ�ܣ�";
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
bool find(string path)//�����ļ�·��
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
				cout << "������ļ���ַ���ļ���С���ܳ���14λ" << endl;
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
int requestNode()//���������
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
int requestBlock()//��������
{
	int index = superblock.freeBlock[5 - superblock.surFBCount];
	if (superblock.surFBCount == 1)
	{
		if (index == 0)//���̿��ѷ�����
			return -1;
		superblock.freeBlock[5 - superblock.surFBCount] = -1;
		superblock.surFBCount = 0;
		fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in|ios::out);
		if (!disk)
		{
			cout << "������̿�ʱ�����ʴ���ʧ�ܣ�";
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
void freeInode(int Iindex)//�ͷ��������
{
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "�ͷ��������ʱ�����ʴ���ʧ�ܣ�";
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
		cout << "�ͷ��̿�ʱ�����ʴ���ʧ�ܣ�";
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
		cout << "����Ŀ¼ʧ�ܣ�";
		exit(1);
	}
	dir.seekg(512 * inode.fAddress[0] + 36 * index);
	dir >> direct;
	dir.close();
}
void writeDir(Inode inode, Direct direct, int index)//д��Ŀ¼
{
	fstream dir("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!dir)
	{
		cout << "д��Ŀ¼ʧ�ܣ�";
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
		cout << "ɾ���ļ�ʱ�����ʴ���ʧ�ܣ�";
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
		cout << "����Ŀ¼ʱ�����ʴ���ʧ�ܣ�";
		exit(1);
	}
	if (havePower(inode))//����дȨ��ʱ
	{
		if (inode.filesize / 36 == 14)
			cout << "��ǰĿ¼������" << endl;
		else
		{
			if (isExist(name, inode,i,subindex,subname))
				cout << "��Ŀ¼�Ѵ��ڣ�"<<endl;
			else
			{
				int Iindex = requestNode();
				if (Iindex != -1)
				{
					int Bindex = requestBlock();
					if (Bindex != -1)
						/*�޸ĵ�ǰĿ¼�̿�*/
					{
						disk.seekp(512 * inode.fAddress[0] + inode.filesize);
						disk << setw(14) << name;//����´�����Ŀ¼��
						disk << setw(4) << Iindex;//�½�Ŀ¼����������
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
						disk << newDirect;//���½�Ŀ¼���������д�����
						//disk.close();
						cout << "Ŀ¼�����ɹ�" << endl;
					}
					else
					{
						freeInode(Iindex);
						cout << "���̿ռ䲻�㣬����ʧ��!" << endl;
					}
				}
				else
				{
					cout << "�޿���������㣬����ʧ�ܣ�" << endl;
				}
			}
		}
	}
	else
		cout << "���߱�����Ȩ��" << endl;
	writeInode(inode, road[num - 1]);//���µ�ǰĿ¼���������
	disk.close();
}
void rm(string filename)//ɾ���ļ�
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
					cout << "�ļ���ɾ��" << endl;
				}
				else
					cout << "ɾ��Ŀ¼��ʹ�����Rmdir" << endl;
			}
			else
				cout << "���߱�ɾ��Ȩ�ޣ�" << endl;
		}
		else
			cout << "���ļ������ڣ�" << endl;
	}
	else
		cout << "���߱�ɾ��Ȩ�ޣ�" << endl;
	writeInode(inode, road[num - 1]);
}
void rmdir(char* name,int currentIndex)//ɾ��Ŀ¼
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
								cout << "ɾ��Ŀ¼ʱ�����ʴ���ʧ�ܣ�";
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
						cout << "Ŀ¼ɾ���ɹ�" << endl;
					}
				}
				else
					cout << "ɾ���ļ���ʹ�����rm" << endl;
			}
			else
				cout << "���߱�ɾ��Ȩ�ޣ�" << endl;
		}
		else
			cout << "��Ŀ¼�����ڣ�" << endl;
	}
	else
		cout << "���߱�ɾ��Ȩ�ޣ�" << endl;
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
void cd(string path)//�л�Ŀ¼
{
	if (path == ".")
	{
		cout << "���л�����ǰĿ¼" << endl;
	}
	else if (path == "/")
	{
		currentName = "/";
		road[0] = 0;
		num = 1;
		cout << "���л�����Ŀ¼" << endl;
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
				cout << "�л�Ŀ¼ʱ�����ʴ���ʧ�ܣ�";
				exit(1);
			}
			disk.seekg(inode.fAddress[0] * 512 + 18);
			disk >> name;
			disk.close();
			currentName = name;
			num--;
			cout << "���л�����Ŀ¼" << endl;
		}
	}
	else
	{
		string::size_type id;
		id = path.find("/");
		if (id == string::npos)//�л�����Ŀ¼
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
					cout << "���л�����Ŀ¼" << endl;
				}
				else
					cout << path << "Ϊ�����ļ����޷��л�" << endl;
			}
			else
				cout << "�����ڸ���Ŀ¼" << endl;
		}
		else//����·���л�
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
					cout << "���л�����Ŀ¼" << endl;
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
void ls()//���Ŀ¼
{
	Inode inode,inode1;
	readInode(road[num - 1], inode);
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in|ios::out);
	if (!disk)
	{
		cout << "����Ŀ¼ʱ�����ʴ���ʧ�ܣ�";
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
void chmod(string name,char* args)//�޸��ļ�Ȩ��
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
								cout << "��������Ȩ��λ���Ϊ3λ" << endl;
							else
								inode1.mode[i-1] = args[i];
						}
						break;
					}
					default:
					{
						cout << "������������-h�鿴�����ĵ�" << endl;
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
								cout << "��������Ȩ��λ���Ϊ3λ" << endl;
							else
								inode1.mode[i+2] = args[i];
						}
						break;
					}
					default:
					{
						cout << "������������-h�鿴�����ĵ�" << endl;
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
							cout << "��������Ȩ��λ���Ϊ3λ" << endl;
						else
							inode1.mode[i + 5] = args[i];
					}
					break;
				}
				default:
				{
					cout << "������������-h�鿴�����ĵ�" << endl;
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
							cout << "��������Ȩ��λ���Ϊ3λ" << endl;
						else
							inode1.mode[i-1] = args[i];
					}
				}
				break;
			}
				default:
				{
					cout << "������������-h�鿴�����ĵ�" << endl;
					break;
				}
					
			}
			//writeInode(inode1, index1);
			cout << "Ȩ���޸����" << endl;
		}
		else
			cout << "���߱��޸�Ȩ�ޣ�" << endl;
	}
	else
		cout << "��Ŀ¼���ļ������ڣ�" << endl;
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
					cout << "�޸ĳɹ�" << endl;
				}
				else
					cout << "�������󣬲����ڸ��û�������help Chown�鿴�����ĵ�" << endl;
			}
			else
				cout << "���߱�Ȩ��" << endl;
		}
		else
			cout << "���ļ���Ŀ¼�����ڣ�" << endl;
	}
	else
		cout << "������Ȩ�ޣ�" << endl;
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
					cout << "�޸ĳɹ�" << endl;
				}
				else
					cout << "�������󣬲����ڸ��û�������help Chgrp�鿴�����ĵ�" << endl;
			}
			else
				cout << "���߱�Ȩ�ޣ�" << endl;
		}
		else
			cout << "���ļ���Ŀ¼�����ڣ�" << endl;
	}
	else
		cout << "���߱�Ȩ�ޣ�" << endl;
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
				cout << "�޸ĳɹ�" << endl;
			}
			else
				cout << "���߱�Ȩ�ޣ�" << endl;
		}
		else
			cout << "���ļ���Ŀ¼�����ڣ�" << endl;
	}
	else
		cout << "���߱�Ȩ�ޣ�" << endl;
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
			cout << "��ǰĿ¼���������޷������ļ�" << endl;
		else
		{
			int i, index;
			string temp;
			if (isExist(name, inode, i, index, temp))
				cout << "����ʧ�ܣ����ļ��Ѵ���" << endl;
			else
			{
				int blocknum;
				if (content.length() % 512 != 0)
					blocknum = content.length() / 512 + 1;
				else
					blocknum = content.length() / 512;
			
				//blocknum = 5;
				int block[6];
				int block1[2];//һ����������洢���̿��
				int block2[4];//�����������
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
							cout << "����ʧ�ܣ�ʣ���̿鲻�㣡" << endl;
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
										//cout << "������ַ��" << block[i + 1]<<endl;
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
					cout << "�ļ������ɹ���" << endl;
				}
				else
					cout << "����ʧ�ܣ��޿���������㣡" << endl;
			}
		}
	}
	else
		cout << "���߱�Ȩ�ޣ�" << endl;
	writeInode(inode, road[num - 1]);
}
void cp(string path)//�����ļ�
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
			cout << "������ֻ�����ڸ��������ļ���" << endl;
	}
	else
		cout << "�޷����ݸ�·���ҵ��ļ���" << endl;
	currentName = tempcurrentname;
	num = tempnum;
	for (int i = 0; i < num; i++)
	{
		road[i] = temproad[i];
	}
	if (isFind)
	{
		mk(name, content);
		cout << "�ļ����Ƴɹ�" << endl;
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
			cout << "��ǰĿ¼������������ʧ�ܣ�" << endl;
		else
		{
			if (isExist(link, inode, i, index, temp))
				cout << "�Ѵ���ͬ���ļ�������ʧ�ܣ�" << endl;
			else
			{
				if (find(path))
				{
					readInode(road[num - 1], inode1);
					cout << "num: " << num << endl;
					cout << "road: " << road[num - 1] << endl;
					fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
					disk.seekp(512 * inode.fAddress[0] + inode.filesize);
					disk << setw(14) << link;//������
					disk << setw(4) << road[num - 1];//Ŀ���ļ�����������
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
					cout << "�������ӳɹ���" << endl;
					disk.close();
					num = tempnum;
					currentName = tempcurrentname;
					for (int i = 0; i < num; i++)
						road[i] =temproad[i] ;
				}
				else
					cout << "�Ҳ���Ŀ���ļ�������ʧ�ܣ�" << endl;	
			}
		}	
	}
	else
		cout << "���߱�����Ȩ�ޣ�" << endl;
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
				if (i <= 3)//ֱ������
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
				else//һ����ַ����
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
						else//������ַ����
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
			cout << "��ʾĿ¼�ļ�����ʹ��ls���" << endl;
	}
	else
		cout << "��ǰĿ¼�����ڸ������ļ���" << endl;
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
	cout << "�����޸ĳɹ�" << endl;
	u.close();
}
void help(string args)
{
	if (args == "")
	{
		cout << "ls: �鿴��ǰĿ¼�µ���������" << endl;
		cout << "������ʽ��		Ls" << endl;
		cout << "chmod: u��ʾ�û���g��ʾ������,o��ʾ�����û���+Ϊ����Ȩ�ޣ�-Ϊȡ��Ȩ�ޣ�=Ϊ���¸���Ȩ��" << endl;
		cout << "������ʽ��		Chmod u+r filename " << endl;
		cout << "chown: �ı��ļ�ӵ����" << endl;
		cout << "������ʽ��		Chown filename ower" << endl;
		cout << "chgrp: �ı��ļ�������" << endl;
		cout << "������ʽ��		Chgrp filename group" << endl;
		cout << "pwd: ��ʾ��ǰĿ¼" << endl;
		cout << "������ʽ��		Pwd" << endl;
		cout << "cd: �л�Ŀ¼,pathΪĿ��·��" << endl;
		cout << "������ʽ��		Cd path" << endl;
		cout << "mkdir: ����Ŀ¼" << endl;
		cout << "������ʽ��		Mkdir directname" << endl;
		cout << "rmdir: ɾ��Ŀ¼" << endl;
		cout << "������ʽ��		Rmdir directname" << endl;
		cout << "umask: �ļ�����������" << endl;
		cout << "������ʽ��		Umask -n,�鿴��ǰ�ļ�������" << endl;
		cout << "������ʽ��		Umask umask,�޸��ļ�������" << endl;
		cout << "mv: �޸��ļ���" << endl;
		cout << "������ʽ��		Mv filename newname" << endl;
		cout << "cp: �����ļ�����ǰĿ¼" << endl;
		cout << "������ʽ��		Cp filepath" << endl;
		cout << "rm: ɾ����ǰĿ¼�µ��ļ�" << endl;
		cout << "������ʽ��		Rm  filename" << endl;
		cout << "ln: �����ļ�����,pathΪĿ���ļ���·����linkΪ������" << endl;
		cout << "������ʽ��		Ln path link" << endl;
		cout << "cat: ��ʾ�ļ�����" << endl;
		cout << "������ʽ��		Cat filename" << endl;
		cout << "password: �޸��û�����" << endl;
		cout << "������ʽ��		Password newpass" << endl;
		cout << "exit: �˳��ļ�ϵͳ" << endl;
		cout << "������ʽ��		exit" << endl;
	}
	if (args == "ls")
	{
		cout << "Ls: �鿴��ǰĿ¼�µ���������" << endl;
		cout << "������ʽ��		Ls" << endl;
	}
	if (args == "chmod")
	{
		cout << "Chmod: u��ʾ�û���g��ʾ������,o��ʾ�����û���+Ϊ����Ȩ�ޣ�-Ϊȡ��Ȩ�ޣ�=Ϊ���¸���Ȩ��" << endl;
		cout << "������ʽ��		Chmod u+r filename " << endl;
	}
	if (args == "chown")
	{
		cout << "Chown: �ı��ļ�ӵ����" << endl;
		cout << "������ʽ��		Chown filename ower" << endl;
	}
	if (args == "chgrp")
	{
		cout << "Chgrp: �ı��ļ�������" << endl;
		cout << "������ʽ��		Chgrp filename group" << endl;
	}
	if (args == "pwd")
	{
		cout << "Pwd: ��ʾ��ǰĿ¼" << endl;
		cout << "������ʽ��		Pwd" << endl;
	}
	if (args == "cd")
	{
		cout << "Cd: �л�Ŀ¼,pathΪĿ��·��" << endl;
		cout << "������ʽ��		Cd path" << endl;
	}
	if (args == "mkdir")
	{
		cout << "Mkdir: ����Ŀ¼" << endl;
		cout << "������ʽ��		Mkdir directname" << endl;
	}
	if (args == "rmdir")
	{
		cout << "Rmdir: ɾ��Ŀ¼" << endl;
		cout << "������ʽ��		Rmdir directname" << endl;
	}
	if (args == "umask")
	{
		cout << "Umask: �ļ�����������" << endl;
		cout << "������ʽ��		Umask -n,�鿴��ǰ�ļ�������" << endl;
		cout << "������ʽ��		Umask umask,�޸��ļ�������" << endl;
	}
	if (args == "mv")
	{
		cout << "Mv: �޸��ļ���" << endl;
		cout << "������ʽ��		Mv filename newname" << endl;
	}
	if (args == "cp")
	{
		cout << "Cp: �����ļ�����ǰĿ¼" << endl;
		cout << "������ʽ��		Cp filepath" << endl;
	}
	if (args == "rm")
	{
		cout << "Rm: ɾ����ǰĿ¼�µ��ļ�" << endl;
		cout << "������ʽ��		Rm  filename" << endl;
	}
	if (args == "ln")
	{
		cout << "Ln: �����ļ�����,pathΪĿ���ļ���·����linkΪ������" << endl;
		cout << "������ʽ��		Ln path link" << endl;
	}
	if (args == "cat")
	{
		cout << "Cat: ��ʾ�ļ�����" << endl;
		cout << "������ʽ��		Cat filename" << endl;
	}
	if (args == "password")
	{
		cout << "Password: �޸��û�����" << endl;
		cout << "������ʽ��		Password newpass" << endl;
	}
	if (args == "exit")
	{
		cout << "exit: �˳��ļ�ϵͳ" << endl;
		cout << "������ʽ��		exit" << endl;
	}
}
void init()//��ʼ��
{
	User user1 = { "admin","admin","admin","022" };
	User user2 = { "root","root","root","022" };
	cout << user1;
	cout<< user2;
	userNum = 2;
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	if (!disk)
	{
		cout << "��ʼ������ʧ�ܣ�";
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
	disk << setw(4) << 0 << setw(4) << 0 << setw(4) << 0 << setw(4) << 0 << setw(4) << 0;//0��ʾ��ָ���κ��̿�
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
void login()//��¼
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
			cout << "��¼ʱ����ȡUser��ʧ�ܣ�" << endl;
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
			cout << endl << "-----�û����������������������" << endl;
		else
		{
			system("cls");
			cout << "---------�ɹ������ļ�ϵͳ--------" << endl;
		}			
	}
}
void readSuper()//���������������
{
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in);
	if (!disk)
	{
		cout << "���볬����ʱ�����ʴ���ʧ�ܣ�";
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
		cout << "�����·��ʱ�����ʴ���ʧ�ܣ�";
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
			if (index == road[i + 1])//������һ��Ŀ¼
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
		if (command == "ls")//��ʾ��ǰĿ¼
		{
			isParse = true;
			ls();
		}
		if (command == "chmod")//�ı��ļ�Ȩ��
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
		if (command == "cd")//�л�Ŀ¼
		{
			isParse = true;
			string path;
			cin >> path;
			cd(path);
		}
		if (command == "mkdir")//������Ŀ¼
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
			cout << endl << "�����ļ����ݣ�" << endl;
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

