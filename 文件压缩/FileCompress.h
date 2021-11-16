#pragma once

#include<string>
#include"HuffmanTree.h"
using namespace std;

typedef unsigned char uch;

//ÿ���ӽڵ���Ϣ
struct ByteInfo {
	uch ch;
	int appearCount;//ch�ֽڳ��ֵĴ���
	string strCode;//ch�ֽڶ�Ӧ�ı���

	ByteInfo(int count = 0)
		:appearCount(count)
	{}
	
	ByteInfo operator+(const ByteInfo& b)const {
		ByteInfo temp;
		temp.appearCount = appearCount + b.appearCount;
		return temp;
	}
	bool operator>(const ByteInfo& b)const {
		return appearCount > b.appearCount;
	}
	bool operator!=(const ByteInfo& b)const {
		return appearCount != b.appearCount;
	}
	bool operator==(const ByteInfo& b)const {
		return appearCount == b.appearCount;
	}

};

class FileCompress {
public:
	FileCompress();//����Ĺ���

	bool CompressFile(const string& filePath);
	bool UNCompressFile(const string& filePath);

	////////////////////////////////////////////
	//�ļ������ڴ����������ֽڷ�ʽ�洢�ģ��ֽڹ���256��״̬
	//ֻ���һ������256��ByteInfo���͵�������ܱ����ֽڳ��ֵ�
	//����Ƶ����Ϣ
private:
	void GetLine(FILE* fIn,string& strContent);
	void WirteHead(FILE* fOut,const string& filePath);
	void GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root);
	ByteInfo fileByteInfo[256];

};
