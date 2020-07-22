#include "pch.h"
#include "file.h"



File::File()
{
	head = new struct file_node;
	head->next = NULL;
	head->data = NULL;

	moving_p = NULL;
	data_moving = NULL;
}

File::~File()
{
	if (data_moving)
	{
		ClearData(data_moving);
	}
	ClearFile(head);
}

/*˽��*/

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

//�����ڵ�
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

//��ַ��ȡ�ڵ��ַ
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

//�����ļ��У�λ�ã���ȣ��ڵ������ļ�����������һ�����ڴ���ҳ��ڵ㣨���Ϊ0ʱΪͷ��
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

//�����ļ���λ�ã���ȣ�ϵͳ����·�����������ܲ�����
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

//�����ļ��У�λ�ã���ȣ��ڵ�����������Ҫ��������ҳ��
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

//�����ļ��У�λ���������Ǹ��ڵ��λ�ã���ȣ��ڵ�����������Ҫ��������ҳ��
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

//��С�ļ��У�λ���Ǽ�С�Ǹ��ڵ��λ�ã���ȣ��ڵ�������������ȥ�Ľڵ������ļ����޷�ɾ��
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

//ʹһ���ļ����У������ƶ�״̬
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

//��ȡ�Ƿ����ļ����У������ƶ�
bool File::GetMoving()
{
	if (!moving_p)
	{
		return false;
	}
	return true;
}

//��ȡ�����ƶ��ļ��ĵ�ַ����û�У��򷵻�NULL
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

//��ȡ�����ƶ��ļ������ļ��ĵ�ַ����û�У��򷵻�NULL
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

//��ȡ�����ƶ��ļ����иõ�ַ��Ϣ�������ڷ���-1��Ϊ�շ���0���ļ�����1���ļ��з���2��
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

//ȡ���ģ��У��ƶ�״̬
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

//ɾ�������ƶ�״̬���ļ����У�
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

//ʹ�����ƶ����ļ����У�����
bool File::MoveTo(int* file, int dep)
{
	File_Node p = GetStr(file, dep), p2;
	if (!p)
	{
		return false;
	}
	//���
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
		//��ǰ
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

//�����ļ���λ�ã���ȣ�ϵͳ����·����
bool File::AddDocument(int* file, int dep, CString this_system_path)
{
	File_Node p = GetStr(file, dep), p2;
	if (!p)
	{
		return false;
	}
	//���
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
		//��ǰ
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
	p->data->m = true;
	p->data->child = NULL;
	p->data->file = this_system_path;
	return true;
}

//ɾ���ļ����У�
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

//��ȡ�õ�ַ��Ϣ�������ڷ���-1��Ϊ�շ���0���ļ�����1���ļ��з���2��
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

//ʹ�����ƶ����ļ���һ�ļ��ϲ����ļ��У�λ�ã���ȣ��ڵ������ļ�������
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
	p->data->file = name;
	p->data->m = false;
	p = p->data->child;
	p->data = data;
	p->next->data = data_moving;
	moving_p = NULL;
	data_moving = NULL;
	return true;
}

//��ȡ�ļ���
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

//��ȡ�ļ�����ڵ���
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

//ƽ���ļ�����Ľڵ�����ʹ֮��Ϊnum�ı��������lessΪ�����Ȳ��ü��ٸ��ӵķ�����
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

//����¼
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

//д��¼
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

//��ȡ�ڵ�
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
			delete[]name;
			if (!m_)
			{
				p2->data->child = ReadFile(infile);
			}
		}
		p = p2;
	}
}

//��ȡ�ڵ�
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

//��ȡ�ļ����ļ����У������
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
	int i;
	p = p->data->child;
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

//�ļ���ת�ļ�
bool File::ToDocument(int* file, int dep)
{
	File_Node p = GetStr(file, dep), p2;
	int i = GetFileNum(file, dep);
	if (i != 1)
	{
		return false;
	}
	p2 = p->data->child;
	while (p2)
	{
		if (p2->data)
		{
			break;
		}
		p2 = p2->next;
	}
	struct data* data;
	data = p2->data;
	p2->data = NULL;
	ClearData(p->data);
	p->data = data;
	return true;
}