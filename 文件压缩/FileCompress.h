#pragma once

#include<string>
#include"HuffmanTree.h"
using namespace std;

typedef unsigned char uch;

//每个子节的信息
struct ByteInfo {
	uch ch;
	int appearCount;//ch字节出现的次数
	string strCode;//ch字节对应的编码

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
	FileCompress();//数组的构造

	bool CompressFile(const string& filePath);
	bool UNCompressFile(const string& filePath);

	////////////////////////////////////////////
	//文件最终在磁盘上是以字节方式存储的，字节共有256种状态
	//只需给一个包含256个ByteInfo类型的数组就能保存字节出现的
	//所有频次信息
private:
	void GetLine(FILE* fIn,string& strContent);
	void WirteHead(FILE* fOut,const string& filePath);
	void GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root);
	ByteInfo fileByteInfo[256];

};
