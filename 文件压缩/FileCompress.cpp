#include"FileCompress.h"
#include"HuffmanTree.h"
#include<iostream>
FileCompress::FileCompress() {
	for (int i = 0; i < 256; i++) {
		fileByteInfo[i].ch = i;//i就是这个字符
		
	}
}


//要把作用域加上
bool FileCompress::CompressFile(const string& filePath) {
	//统计元文件中每个字节出现的次数 
	//保存
	FILE* pf = fopen(filePath.c_str(), "rb");
	if (nullptr == pf) {
		cout << "待压缩文件打开失败" << endl;
		return false;
	}
	//文件大小不确定，采用循环方式获取元文件中内容

	uch readBuff[1024];//一次性读取1K的内容进来
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);//以字节方式读取，一次读取一个字节大小的元素
		if (0 == rdsize) {
			break;
		}
		for (size_t i = 0; i < rdsize; i++) {
			//以字符的ASCII码值作为数组的下标进行快速的统计
			fileByteInfo[readBuff[i]].appearCount++;
		}
	}
	//根据统计的结果创建哈夫曼树
	HuffmanTree<ByteInfo> h;
	ByteInfo invalid;
	h.CreateHuffmanTree(fileByteInfo, 256, invalid);

	//借助huffman树获取每个字节的编码
	GenerateHuffmanCode(h.GetRoot());

	//写解压缩时候要用到的信息
	FILE* fOut = fopen("2.hzp", "wb");
	WirteHead(fOut, filePath);

	//使用字节的编码对元文件进行重新改写

	//这里再次读取pf文件时，需要将pf文件指针移动到文件起始位置，
	//因为刚开始已经读取过一次了，文件指针已经走到末尾了，
	rewind(pf);
	uch ch = 0;
	uch bitCount = 0;
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (0 == rdsize) {
			break;
		}
		//改写，结果要放置到压缩结果的文件中，
		for (size_t i = 0; i < rdsize; i++) {
			string& strCode = fileByteInfo[readBuff[i]].strCode;
			//将拿到的字符串格式的二进制编码往字节中存放
			for (size_t j = 0; j < strCode.size(); j++) {
				ch <<= 1;
				if ('1' == strCode[j]) {
					ch |= 1;
				}
				//当ch中的八个比特位填充满之后，要将该字节填充到文件中
				bitCount++;
				if (8 == bitCount) {
					fputc(ch, fOut);
					bitCount = 0;
				}
			
			}
		}
	}
	//最后一个可能不够八个比特位 会导致没有写进去
	if (bitCount > 0 && bitCount < 8) {
		ch <<= (8 - bitCount);
		fputc(ch, fOut);
	}

	fclose(pf);
	fclose(fOut);
	return true;
}
bool FileCompress::UNCompressFile(const string& filePath) {
	//1. 从压缩文件中读取解压缩时需要用到的信息
	FILE* fIn = fopen(filePath.c_str(), "rb");
	if (nullptr == fIn) {
		cout << "打开压缩文件失败" << endl;
		return false;
	}
	//读取源文件后缀
	string postFix;
	GetLine(fIn, postFix);

	//频次信息总行数
	string strContent;
	GetLine(fIn, strContent);
	size_t lineCount = atoi(strContent.c_str());
	//循环读取linecount行 ,获取字节频次信息
	strContent = "";
	for (size_t i = 0; i < lineCount; i++) {
		
		GetLine(fIn, strContent);
		if ("" == strContent) {
			//刚刚读取到的是一个换行
			strContent += "/n";
			GetLine(fIn, strContent);
		}
		fileByteInfo[(uch)strContent[0]].appearCount = atoi(strContent.c_str() + 2);
		strContent = "";	
		//fileByteInfo[strContent[0]].ch = strContent[0];	
	}
	//2.恢复哈夫曼树
	HuffmanTree<ByteInfo> h;
	ByteInfo invalid;
	h.CreateHuffmanTree(fileByteInfo, 256, invalid);

	//3.读取压缩数据，结合哈夫曼树进行解压缩
	string filename("3");
	filename += postFix;
	FILE* fOut = fopen(filename.c_str(), "wb");
	uch readBuff[1024];
	uch bitCount = 0;
	HuffmanTreeNode<ByteInfo>* cur = h.GetRoot();
	const int fileSize = cur->weight.appearCount;
	int compressSize = 0;
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, fIn);
		if (0 == rdsize) {
			break;
		}
		for (size_t i = 0; i < rdsize; i++) {
			//逐字节进行解压缩
			uch ch = readBuff[i];
			bitCount = 0;
			while (bitCount < 8) {
				if (ch & 0x80) {
					cur = cur->right;	
				}
				else {
					cur = cur->left;
				}
				bitCount++;
				if (nullptr == cur->left && nullptr == cur->right) {
					fputc(cur->weight.ch, fOut);
					cur = h.GetRoot();
					compressSize++;
					//每解压缩一个就给解压缩的字节数++
					//如果成功解压缩字节的个数与源文件大小相同时则解压缩结束
					if (compressSize == fileSize) {
						break;
					}
				}
				ch <<= 1;//将高位的比特位移走
			}
		}
	}

	fclose(fIn);
	fclose(fOut);
	return true;
}

void FileCompress::GetLine(FILE* fIn, string& strContent) {
	uch ch;
	while (!feof(fIn)) {
		ch = fgetc(fIn);
		if (ch == '\n') {
			break;
		}
		strContent += ch;
	}
}

void FileCompress::WirteHead(FILE* fOut, const string& filePath) {
	//获取源文件的后缀
	string postFix = filePath.substr(filePath.rfind('.'));
	postFix += '\n';
	fwrite(postFix.c_str(), 1, postFix.size(), fOut);
	//总行数
	string chAppearCount;
	size_t lineCount = 0;
	for (size_t i = 0; i < 256; i++) {
		if (fileByteInfo[i].appearCount > 0) {
			chAppearCount += fileByteInfo[i].ch;
			chAppearCount += ':';
			chAppearCount += to_string(fileByteInfo[i].appearCount);
			chAppearCount += "\n";
			lineCount++;
		}
	}
	//总行数以及频次信息
	string totalLine = to_string(lineCount);
	totalLine += "\n";
	fwrite(totalLine.c_str(), 1, totalLine.size(), fOut);
	fwrite(chAppearCount.c_str(), 1, chAppearCount.size(), fOut);
}

void FileCompress::GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root) {
	if (nullptr == root) {
		return;
	}
	if (nullptr == root->left && nullptr == root->right) {
		HuffmanTreeNode<ByteInfo>* cur = root;
		HuffmanTreeNode<ByteInfo>* parent = cur->parent;
		string& strCode = fileByteInfo[cur->weight.ch].strCode;
		while (parent) {
			if (cur == parent->left) {
				strCode += '0';
			}
			else {
				strCode += '1';
			}
			cur = parent;
			parent = cur->parent;
		}
		reverse(strCode.begin(), strCode.end());
		
	}
	GenerateHuffmanCode(root->left);
	GenerateHuffmanCode(root->right);
}