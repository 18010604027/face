#include "pch.h"
#include "file.h"



File::File()
{
	head = new struct file_node;
	head->next = NULL;
	head->data = NULL;

	moving_p = NULL;
	data_moving = NULL;

	total = NULL;
}

File::~File()
{
	if (data_moving)
	{
		ClearData(data_moving);
	}
	ClearFile(head);
}

/*私有*/

void File::ClearFile(File_Node p)
{
	if (p == NULL)
	{
		return;
	}
	ClearFile(p->next);
	ClearData(p->data);
	delete p;
}
void File::ClearData(struct data* p)
{
	if (p == NULL)
	{
		return;
	}
	if (!p->m)
	{
		ClearFile(p->child);
	}
	delete p;
}

//创建节点
File_Node File::CreateNode(int num, File_Node _p)
{
	if (num <= 0)
	{
		return NULL;
	}
	File_Node head, p, p2;
	head = p = new struct file_node;
	head->last = NULL;
	head->data = NULL;
	while (--num)
	{
		p2 = new struct file_node;
		p->next = p2;
		p2->last = p;
		p2->data = NULL;
		p = p2;
	}
	p->next = _p;
	if (_p)
	{
		_p->last = p;
	}
	return head;
}

//地址获取节点地址
File_Node File::GetStr(int* file, int dep)
{
	File_Node p = head;
	for (int i = 0; i < dep; i++)
	{
		if (p == NULL)
		{
			return NULL;
		}
		if (p->data == NULL)
		{
			return NULL;
		}
		p = p->data->child;
		for (int j = 0; j < file[i]; j++)
		{
			if (p == NULL)
			{
				return NULL;
			}
			p = p->last;
		}
	}
	return p;
}

//创建文件夹（位置，深度，节点数，文件夹名）——一般用于创建页面节点（深度为0时为头）
bool File::Create_File(int* file, int dep, int num, CString name)
{
	File_Node p = GetStr(file, dep);
	if (p == NULL)
	{
		return false;
	}
	if (p->data != NULL)
	{
		return false;
	}
	p->data->child = CreateNode(num);
	p->data->m = false;
	p->data->file = name;
	return true;
}

//创建文件（位置，深度，系统绝对路径）——可能不常用
bool File::CreateDocument(int* file, int dep, CString this_system_path)
{
	File_Node p = GetStr(file, dep);
	if (p == NULL)
	{
		return false;
	}
	if (p->data != NULL)
	{
		return false;
	}
	p->data->file = this_system_path;
	p->data->m = true;
	p->data->child = NULL;
	return true;
}

//扩大文件夹（位置，深度，节点数）——主要用于增大页面
bool File::EnlargeFile(int* file, int dep, int num)
{
	File_Node p = GetStr(file, dep);
	if (p == NULL || p->data == NULL || p->data->m == true)
	{
		return false;
	}
	p = p->data->child;
	while (p->next)
	{
		p = p->next;
	}
	File_Node p2 = CreateNode(num);
	if (p2 == NULL)
	{
		return false;
	}
	return true;
}

//扩大文件夹（位置是扩大那个节点的位置，深度，节点数）——主要用于增大页面
bool File::EnlargeFile2(int* file, int dep, int num)
{
	File_Node p = GetStr(file, dep);
	File_Node p2 = CreateNode(num, p->next);
	if (p == NULL || p2 == NULL)
	{
		return false;
	}
	p->next = p2;
	p2->last = p;
	return true;
}

//减小文件夹（位置是减小那个节点的位置，深度，节点数）——当减去的节点里有文件将无法删除
bool File::LessenFile(int* file, int dep, int num)
{
	File_Node p = GetStr(file, dep), p2;
	if (p == NULL)
	{
		return false;
	}
	p2 = p;
	for (int i = 0; i < num; i++)
	{
		if (p2->data != NULL)
		{
			return false;
		}
		p2 = p2->next;
	}
	File_Node last = p->last, next = p2;
	for (int i = 0; i < num; i++)
	{
		p2 = p->next;
		ClearData(p->data);
		delete p;
		p = p2;
	}
	if (last)
	{
		last->next = next;
	}
	if (next)
	{
		next->last = last;
	}
	return true;
}

//使一个文件（夹）处于移动状态
bool File::Moving(int* file, int dep)
{
	if (moving_p)
	{
		return false;
	}
	moving_p = GetStr(file, dep);
	if (moving_p)
	{
		return false;
	}
	data_moving = moving_p->data;
	moving_p->data = NULL;
	return true;
}

//获取是否有文件（夹）处于移动
bool File::GetMoving()
{
	if (!moving_p)
	{
		return false;
	}
	return true;
}

//获取正在移动文件的地址，若没有，则返回NULL
CString File::GetMovingDocument()
{
	if (!moving_p)
	{
		return NULL;
	}
	if (!moving_p->data->m)
	{
		return NULL;
	}
	return moving_p->data->file;
}

//获取正在移动文件夹中文件的地址，若没有，则返回NULL
CString File::GetMovingFileDocument(int num)
{
	if (!moving_p)
	{
		return NULL;
	}
	if (moving_p->data->m)
	{
		return NULL;
	}
	File_Node p = moving_p->data->child;
	while (--num)
	{
		if (p == NULL)
		{
			return NULL;
		}
		p = p->next;
	}
	if (p == NULL)
	{
		return NULL;
	}
	if (!p->data)
	{
		return NULL;
	}
	if (!p->data->m)
	{
		return NULL;
	}
	return p->data->file;
}

//获取正在移动文件夹中该地址信息（不存在返回-1，为空返回0，文件返回1，文件夹返回2）
int File::GetMovingFileState(int num)
{
	if (!moving_p)
	{
		return -1;
	}
	if (moving_p->data->m)
	{
		return -1;
	}
	File_Node p = moving_p->data->child;
	while (--num)
	{
		if (p == NULL)
		{
			return -1;
		}
		p = p->next;
	}
	if (p == NULL)
	{
		return -1;
	}
	if (!p->data)
	{
		return 0;
	}
	if (!p->data->m)
	{
		return 2;
	}
	return 1;
}

//取消文（夹）移动状态
bool File::CancelMoving()
{
	if (!moving_p)
	{
		return false;
	}
	if (!moving_p->data)
	{
		return false;
	}
	moving_p->data = data_moving;
	moving_p = NULL;
	data_moving = NULL;
	return true;
}

//删除处于移动状态的文件（夹）
bool File::DeleteMoving()
{
	if (!moving_p)
	{
		return false;
	}
	moving_p = NULL;
	ClearData(data_moving);
	data_moving = NULL;
	return true;
}

//使处于移动的文件（夹）插入
bool File::MoveTo(int* file, int dep)
{
	File_Node p = GetStr(file, dep), p2;
	if (!p)
	{
		return false;
	}
	//向后
	p2 = p;
	while (p2->data)
	{
		p2 = p2->next;
		if (!p2)
		{
			break;
		}
	}
	if (p2)
	{
		while (p != p2)
		{
			p2->data = p2->last->data;
			p2 = p2->last;
		}
	}
	else
	{
		//向前
		p2 = p;
		while (p2->data)
		{
			p2 = p2->last;
			if (!p2)
			{
				return false;
			}
		}
		while (p != p2)
		{
			p2->data = p2->next->data;
			p2 = p2->next;
		}
	}
	return true;
}

//插入文件（位置，深度，系统绝对路径）
bool File::AddDocument(int* file, int dep, CString this_system_path)
{
	File_Node p = GetStr(file, dep), p2;
	if (!p)
	{
		return false;
	}
	//向后
	p2 = p;
	while (p2->data)
	{
		p2 = p2->next;
		if (!p2)
		{
			break;
		}
	}
	if (p2)
	{
		while (p != p2)
		{
			p2->data = p2->last->data;
			p2 = p2->last;
		}
	}
	else
	{
		//向前
		p2 = p;
		while (p2->data)
		{
			p2 = p2->last;
			if (!p2)
			{
				return false;
			}
		}
		while (p != p2)
		{
			p2->data = p2->next->data;
			p2 = p2->next;
		}
	}
	p->data = new struct data;
	p->data->num = -1;
	p->data->m = true;
	p->data->child = NULL;
	p->data->file = this_system_path;
	return true;
}

//删除文件（夹）
bool File::Delete(int* file, int dep)
{
	File_Node p = GetStr(file, dep);
	if (!p)
	{
		return false;
	}
	if (!p->data)
	{
		return false;
	}
	ClearData(p->data);
	p->data = NULL;
	return true;
}

//获取该地址信息（不存在返回-1，为空返回0，文件返回1，文件夹返回2）
int File::GetState(int* file, int dep)
{
	File_Node p = GetStr(file, dep);
	if (p == NULL)
	{
		return -1;
	}
	if (!p->data)
	{
		return 0;
	}
	if (!p->data->m)
	{
		return 2;
	}
	return 1;
}

//使处于移动的文件与一文件合并成文件夹（位置，深度，节点数，文件夹名）
bool File::MovingMerge(int* file, int dep, int num, CString name)
{
	if (num < 2)
	{
		return false;
	}
	File_Node p = GetStr(file, dep);
	if (!p)
	{
		return false;
	}
	if (!p->data)
	{
		return false;
	}
	struct data* data = p->data;
	p->data = new struct data;
	p->data->num = -1;
	p->data->file = name;
	p->data->m = false;
	p = p->data->child;
	p->data = data;
	p->next->data = data_moving;
	moving_p = NULL;
	data_moving = NULL;
	return true;
}

//获取文件名
CString File::GetFile(int* file, int dep)
{
	File_Node p = GetStr(file, dep);
	if (!p)
	{
		return NULL;
	}
	if (!p->data)
	{
		return NULL;
	}
	return p->data->file;
}

//获取文件夹里节点数
int File::GetNum(int* file, int dep)
{
	File_Node p = GetStr(file, dep);
	if (!p)
	{
		return -1;
	}
	if (!p->data)
	{
		return -1;
	}
	int i = 0;
	p = p->data->child;
	while (p)
	{
		i++;
		p = p->next;
	}
	return i;
}

//平衡文件夹里的节点数，使之成为num的倍数（如果less为真优先采用减少格子的方法）
bool File::Balance(int* file, int dep, int num, bool less)
{
	File_Node p = GetStr(file, dep), p2, p3, p4;
	if (!p)
	{
		return false;
	}
	if (!p->data)
	{
		return false;
	}
	if (p->data->m)
	{
		return false;
	}
	p2 = p->data->child;
	int i = 0;
	if (p2)
	{
		i++;
		while (p2->next)
		{
			i++;
			p2 = p2->next;
		}
	}
	if (less && i > num)
	{
		p3 = p2;
		int j;
		for (j = i; j % num; j--)
		{
			if (p3->data)
			{
				break;
			}
			p3 = p3->last;
		}
		if (!(j % num))
		{
			j = i - j;
			p3 = p2;
			while (j--)
			{
				p4 = p3->last;
				delete p3;
				p3 = p4;
			}
			p3->last = NULL;
			return true;
		}
	}

	p3 = CreateNode(num - (i % num));
	p3->last = p2;
	p2->next = p3;
	return true;
}

//读记录
bool File::Read()
{
	ifstream infile;
	infile.open("data\\rem.file", ios::in | ios::binary);
	if (infile)
	{
		return false;
	}
	head = ReadFile(&infile);
	infile.close();
	return true;
}

//写记录
bool File::Write()
{
	ofstream outfile;
	outfile.open("data\\rem.file", ios::out | ios::binary);
	if (outfile)
	{
		return false;
	}
	WriteFile(&outfile, head);
	outfile.close();
	return true;
}

//读取节点
File_Node File::ReadFile(ifstream* infile)
{
	File_Node head = NULL, p = nullptr, p2;
	short m;
	int len;
	bool m_;
	char* name;
	while (1)
	{
		infile->read((char*)&m, sizeof(m));
		if (m == -1)
		{
			return head;
		}
		if (head)
		{
			p2 = new struct file_node;
			p->next = p2;
			p2->last = p;
		}
		else
		{
			head = p2 = new struct file_node;
			p2->last = NULL;
		}
		if (m == 1)
		{
			p2->data = NULL;
		}
		else if (m == 2)
		{
			infile->read((char*)&m_, sizeof(m_));
			infile->read((char*)&len, sizeof(len));
			name = new char[len + 1]();
			infile->read(name, len);
			p2->data->file = name;
			p2->data->num = -1;
			delete[]name;
			if (!m_)
			{
				p2->data->child = ReadFile(infile);
			}
		}
		p = p2;
	}
}

//读取节点
void File::WriteFile(ofstream* outfile, File_Node p)
{
	short m;
	int len;
	while (p)
	{
		if (p->data)
		{
			m = 1;
			outfile->write((char*)&m, short(m));
			outfile->write((char*)&(p->data->m), sizeof(bool));
			len = p->data->file.GetLength();
			outfile->write((char*)&(len), sizeof(len));
			outfile->write((char*)&(p->data->file), len);
			if (!p->data->m)
			{
				WriteFile(outfile, p->data->child);
			}
		}
		else
		{
			m = 0;
			outfile->write((char*)&m, sizeof(m));
		}
		p = p->next;
	}
	m = -1;
	outfile->write((char*)&m, sizeof(m));
}

//获取文件夹里的文件数
int File::GetFileNum(int* file, int dep)
{
	File_Node p = GetStr(file, dep);
	if (!p)
	{
		return -1;
	}
	if (!p->data)
	{
		return -1;
	}
	if (p->data->m)
	{
		return -1;
	}
	p = p->data->child;
	int i;
	for (i = 0; p; )
	{
		if (p->data)
		{
			i++;
		}
		p = p->next;
	}
	return i;
}

//文件夹转文件
bool File::CancelFile(int* file, int dep)
{
	int num = GetFileNum(file, dep);
	if (num > 1 || num == -1)
	{
		return false;
	}
	File_Node p = GetStr(file, dep), p2, p3;
	p3 = p2 = p->data->child;
	while (p2)
	{
		if (p2->data)
		{
			break;
		}
		p2 = p2->next;
	}
	p->data = p2->data;
	p2->data = NULL;
	ClearFile(p3);
	return true;
}

//获取文件（夹）图标序号——没有文件则返回-2，没有分配返回-1
int File::GetIcoNum(int* file, int dep)
{
	File_Node p = GetStr(file, dep);
	if (!p)
	{
		return -2;
	}
	if (!p->data)
	{
		return -2;
	}
	return p->data->num;
}

//设置文件（夹）图标序号——没有文件则返回-2，以分配返回-1
int File::SetIcoNum(int* file, int dep)
{
	File_Node p = GetStr(file, dep);
	if (!p)
	{
		return -2;
	}
	if (!p->data)
	{
		return -2;
	}
	if (p->data->num == -1)
	{
		p->data->num = total++;
		return p->data->num;
	}
	return -1;
}