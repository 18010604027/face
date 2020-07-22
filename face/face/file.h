#pragma once
#include "pch.h"
#include <iostream> 
#include <fstream> 
using namespace std;

struct data
{
	bool m;//记录是文件型还是文件夹型（真为文件，假为文件夹）
	CString file;//文件地址
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
	void ClearFile(File_Node p);//清理节点
	void ClearData(struct data* p);//清理文件

	File_Node ReadFile(ifstream* infile);//读取节点
	void WriteFile(ofstream* outfile, File_Node p);//读取节点

	File_Node CreateNode(int num, File_Node _p = NULL);//创建节点

	File_Node GetStr(int* file, int dep);//地址获取节点地址
public:
	File();//创建头节点
	~File();//清理创建的控件
	bool Read();//读记录
	bool Write();//写记录
	bool Create_File(int* file, int dep, int num, CString name);//创建文件夹（位置，深度，节点数，文件夹名）
	bool CreateDocument(int* file, int dep, CString this_system_path);//创建文件（位置，深度，系统绝对路径）
	bool EnlargeFile2(int* file, int dep, int num);//扩大文件夹（位置是扩大那个节点的位置，深度，节点数）
	bool EnlargeFile(int* file, int dep, int num);//扩大文件夹（位置，深度，节点数）
	bool LessenFile(int* file, int dep, int num);//减小文件夹（位置是减小那个节点的位置，深度，节点数）
	bool Moving(int* file, int dep);//使一个文件（夹）处于移动状态
	bool GetMoving();//获取是否有文件（夹）处于移动
	CString GetMovingDocument();//获取正在移动文件的地址，若没有，则返回NULL
	CString GetMovingFileDocument(int num);//获取正在移动文件夹中文件的地址，若没有，则返回NULL
	int GetMovingFileState(int num);//获取正在移动文件夹中该地址信息（不存在返回-1，为空返回0，文件返回1，文件夹返回2）
	bool CancelMoving();//取消文（夹）移动状态
	bool DeleteMoving();//删除处于移动状态的文件（夹）
	bool MoveTo(int* file, int dep);//使处于移动的文件（夹）插入
	bool AddDocument(int* file, int dep, CString this_system_path);//插入文件（位置，深度，系统绝对路径）
	bool Delete(int* file, int dep);//删除文件（夹）
	int GetState(int* file, int dep);//获取该地址信息（不存在返回-1，为空返回0，文件返回1，文件夹返回2）
	bool MovingMerge(int* file, int dep, int num, CString name);//使处于移动的文件与一文件合并成文件夹（位置，深度，节点数，文件夹名）
	CString GetFile(int* file, int dep); //文件名
	int GetNum(int* file, int dep);//获取文件夹里节点数
	int GetFileNum(int* file, int dep);//获取文件夹文件（夹）里个数
	bool ToDocument(int* file, int dep);//文件夹转文件
	bool Balance(int* file, int dep, int num, bool less);//平衡文件夹里的节点数，使之成为num的倍数（如果less为真优先采用减少格子的方法）
};