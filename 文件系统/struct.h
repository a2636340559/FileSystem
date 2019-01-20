//disk,70K,每块512字节，共150块，每块可包含8个I结点，第一块（0#）作为超级块，（1#）到（14#）存储I结点，剩余135块存储文件内容，
#include<iostream>
#include<fstream>
#include<iomanip>
using namespace std;
struct User
{
	char name[6];//
	char password[6];//
	char group[6];//
	char umask[4];//
};
struct Inode//64字节
{
	int filesize;//4字节
	int fbnum;//4字节
	int fAddress[6] = { 0,0,0,0,0,0 };//4*6=24字节
	char owner[6];//6字节
	char group[6];//6字节
	char mode[11];//11字节
	char modifyTime[9] = { " " };//9字节
};
struct Direct//36B
{
	char filename[14];//14B
	int index;//4B
	char fathername[14];//14B
	int findex;//4B
};
struct SuperBlock
{
	int freeInode[112];//
	int freeICount;//
	int surFICount;//
	int freeBlock[5];//
	int freeBCount;//
	int surFBCount;//
};
int num1 = 0;//写入的用户数量
int num2=0;
istream &operator >> (istream&disk, Inode &inode)
{

	char time[9];
	disk >> inode.filesize >> inode.fbnum;
	for (int i = 0; i < 6; i++)
		disk >> inode.fAddress[i];
	disk >> inode.owner >> inode.group >> inode.mode;
	for(int j=0;j<8;j++)
		disk>> inode.modifyTime[j];
	return disk;
}
istream &operator >> (istream&dir, Direct & direct)
{
	dir >> direct.filename >> direct.index >> direct.fathername >> direct.findex;
	return dir;
}
ostream &operator<< (ostream&dir, Direct & direct)
{
	dir << setw(14) << direct.filename << setw(4) << direct.index << setw(14) << direct.fathername << setw(4) << direct.findex;
	return dir;
}
ostream &operator<<(ostream&disk, Inode &inode)
{
	//num2++;
	//cout<<endl << "输入次数：" << num2<<"  inode.owner:"<<inode.owner<<"  inode.group:"<< inode.group <<endl;
	disk <<setw(4)<< inode.filesize <<setw(4)<<inode.fbnum;
	for (int i = 0; i < 6; i++)
		disk << setw(4)<<inode.fAddress[i];
	disk <<setw(6)<< inode.owner <<setw(6)<< inode.group<<setw(11)<<inode.mode << setw(9)<<inode.modifyTime;
	return disk;
}
ostream &operator<<(ostream&cout, SuperBlock&super)
{
	fstream disk("C:\\Users\\lenovo\\Desktop\\Disk.txt", ios::in | ios::out);
	int i = 0;
	for (; i < 112; i++)
	{
		disk << setw(4) << super.freeInode[i];
	}
	disk << setw(4) << super.freeICount;
	disk << setw(4) << super.surFICount;
	for (i = 0; i < 5; i++)
	{
		disk << setw(4) << super.freeBlock[i];
	}
	disk << setw(4) << super.freeBCount;
	disk << setw(4) << super.surFBCount;
	disk.close();
	return cout;
}
ostream &operator <<(ostream &output, User& user)
{
	num1++;
	fstream u("C:\\Users\\lenovo\\Desktop\\User.txt", ios::in | ios::out);
	if (!u)
	{
		cout << "读取User表失败！";
		exit(1);
	}
	u.seekp(0);
	u << num1;
	u.seekp(22 * (num1 - 1)+1);
	u << setw(6) << user.name<< setw(6)<<user.password<< setw(6)<<user.group<<setw(4)<<user.umask;
	u.close();
	return output;
}


