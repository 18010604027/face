#pragma once
#include "pch.h"
#include <iostream> 
#include <fstream> 
using namespace std;

struct data
{
	bool m;//��¼���ļ��ͻ����ļ����ͣ���Ϊ�ļ�����Ϊ�ļ��У�
	CString file;//�ļ���ַ
	struct file_node* child;
};

typedef struct file_node
{
	struct file_node* next;
	struct file_node* last;
	struct data* data;
}*File_Node;


class File
{
private:
	File_Node head;

	File_Node moving_p;
	struct data* data_moving;
private:
	void ClearFile(File_Node p);//����ڵ�
	void ClearData(struct data* p);//�����ļ�

	File_Node ReadFile(ifstream* infile);//��ȡ�ڵ�
	void WriteFile(ofstream* outfile, File_Node p);//��ȡ�ڵ�

	File_Node CreateNode(int num, File_Node _p = NULL);//�����ڵ�

	File_Node GetStr(int* file, int dep);//��ַ��ȡ�ڵ��ַ
public:
	File();//����ͷ�ڵ�
	~File();//�������Ŀؼ�
	bool Read();//����¼
	bool Write();//д��¼
	bool Create_File(int* file, int dep, int num, CString name);//�����ļ��У�λ�ã���ȣ��ڵ������ļ�������
	bool CreateDocument(int* file, int dep, CString this_system_path);//�����ļ���λ�ã���ȣ�ϵͳ����·����
	bool EnlargeFile2(int* file, int dep, int num);//�����ļ��У�λ���������Ǹ��ڵ��λ�ã���ȣ��ڵ�����
	bool EnlargeFile(int* file, int dep, int num);//�����ļ��У�λ�ã���ȣ��ڵ�����
	bool LessenFile(int* file, int dep, int num);//��С�ļ��У�λ���Ǽ�С�Ǹ��ڵ��λ�ã���ȣ��ڵ�����
	bool Moving(int* file, int dep);//ʹһ���ļ����У������ƶ�״̬
	bool GetMoving();//��ȡ�Ƿ����ļ����У������ƶ�
	CString GetMovingDocument();//��ȡ�����ƶ��ļ��ĵ�ַ����û�У��򷵻�NULL
	CString GetMovingFileDocument(int num);//��ȡ�����ƶ��ļ������ļ��ĵ�ַ����û�У��򷵻�NULL
	int GetMovingFileState(int num);//��ȡ�����ƶ��ļ����иõ�ַ��Ϣ�������ڷ���-1��Ϊ�շ���0���ļ�����1���ļ��з���2��
	bool CancelMoving();//ȡ���ģ��У��ƶ�״̬
	bool DeleteMoving();//ɾ�������ƶ�״̬���ļ����У�
	bool MoveTo(int* file, int dep);//ʹ�����ƶ����ļ����У�����
	bool AddDocument(int* file, int dep, CString this_system_path);//�����ļ���λ�ã���ȣ�ϵͳ����·����
	bool Delete(int* file, int dep);//ɾ���ļ����У�
	int GetState(int* file, int dep);//��ȡ�õ�ַ��Ϣ�������ڷ���-1��Ϊ�շ���0���ļ�����1���ļ��з���2��
	bool MovingMerge(int* file, int dep, int num, CString name);//ʹ�����ƶ����ļ���һ�ļ��ϲ����ļ��У�λ�ã���ȣ��ڵ������ļ�������
	CString GetFile(int* file, int dep); //�ļ���
	int GetNum(int* file, int dep);//��ȡ�ļ�����ڵ���
	int GetFileNum(int* file, int dep);//��ȡ�ļ����ļ����У������
	bool ToDocument(int* file, int dep);//�ļ���ת�ļ�
	bool Balance(int* file, int dep, int num, bool less);//ƽ���ļ�����Ľڵ�����ʹ֮��Ϊnum�ı��������lessΪ�����Ȳ��ü��ٸ��ӵķ�����
};