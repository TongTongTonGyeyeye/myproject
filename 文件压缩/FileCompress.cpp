#include"FileCompress.h"
#include"HuffmanTree.h"
#include<iostream>
FileCompress::FileCompress() {
	for (int i = 0; i < 256; i++) {
		fileByteInfo[i].ch = i;//i��������ַ�
		
	}
}


//Ҫ�����������
bool FileCompress::CompressFile(const string& filePath) {
	//ͳ��Ԫ�ļ���ÿ���ֽڳ��ֵĴ��� 
	//����
	FILE* pf = fopen(filePath.c_str(), "rb");
	if (nullptr == pf) {
		cout << "��ѹ���ļ���ʧ��" << endl;
		return false;
	}
	//�ļ���С��ȷ��������ѭ����ʽ��ȡԪ�ļ�������

	uch readBuff[1024];//һ���Զ�ȡ1K�����ݽ���
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);//���ֽڷ�ʽ��ȡ��һ�ζ�ȡһ���ֽڴ�С��Ԫ��
		if (0 == rdsize) {
			break;
		}
		for (size_t i = 0; i < rdsize; i++) {
			//���ַ���ASCII��ֵ��Ϊ������±���п��ٵ�ͳ��
			fileByteInfo[readBuff[i]].appearCount++;
		}
	}
	//����ͳ�ƵĽ��������������
	HuffmanTree<ByteInfo> h;
	ByteInfo invalid;
	h.CreateHuffmanTree(fileByteInfo, 256, invalid);

	//����huffman����ȡÿ���ֽڵı���
	GenerateHuffmanCode(h.GetRoot());

	//д��ѹ��ʱ��Ҫ�õ�����Ϣ
	FILE* fOut = fopen("2.hzp", "wb");
	WirteHead(fOut, filePath);

	//ʹ���ֽڵı����Ԫ�ļ��������¸�д

	//�����ٴζ�ȡpf�ļ�ʱ����Ҫ��pf�ļ�ָ���ƶ����ļ���ʼλ�ã�
	//��Ϊ�տ�ʼ�Ѿ���ȡ��һ���ˣ��ļ�ָ���Ѿ��ߵ�ĩβ�ˣ�
	rewind(pf);
	uch ch = 0;
	uch bitCount = 0;
	while (true) {
		size_t rdsize = fread(readBuff, 1, 1024, pf);
		if (0 == rdsize) {
			break;
		}
		//��д�����Ҫ���õ�ѹ��������ļ��У�
		for (size_t i = 0; i < rdsize; i++) {
			string& strCode = fileByteInfo[readBuff[i]].strCode;
			//���õ����ַ�����ʽ�Ķ����Ʊ������ֽ��д��
			for (size_t j = 0; j < strCode.size(); j++) {
				ch <<= 1;
				if ('1' == strCode[j]) {
					ch |= 1;
				}
				//��ch�еİ˸�����λ�����֮��Ҫ�����ֽ���䵽�ļ���
				bitCount++;
				if (8 == bitCount) {
					fputc(ch, fOut);
					bitCount = 0;
				}
			
			}
		}
	}
	//���һ�����ܲ����˸�����λ �ᵼ��û��д��ȥ
	if (bitCount > 0 && bitCount < 8) {
		ch <<= (8 - bitCount);
		fputc(ch, fOut);
	}

	fclose(pf);
	fclose(fOut);
	return true;
}
bool FileCompress::UNCompressFile(const string& filePath) {
	//1. ��ѹ���ļ��ж�ȡ��ѹ��ʱ��Ҫ�õ�����Ϣ
	FILE* fIn = fopen(filePath.c_str(), "rb");
	if (nullptr == fIn) {
		cout << "��ѹ���ļ�ʧ��" << endl;
		return false;
	}
	//��ȡԴ�ļ���׺
	string postFix;
	GetLine(fIn, postFix);

	//Ƶ����Ϣ������
	string strContent;
	GetLine(fIn, strContent);
	size_t lineCount = atoi(strContent.c_str());
	//ѭ����ȡlinecount�� ,��ȡ�ֽ�Ƶ����Ϣ
	strContent = "";
	for (size_t i = 0; i < lineCount; i++) {
		
		GetLine(fIn, strContent);
		if ("" == strContent) {
			//�ոն�ȡ������һ������
			strContent += "/n";
			GetLine(fIn, strContent);
		}
		fileByteInfo[(uch)strContent[0]].appearCount = atoi(strContent.c_str() + 2);
		strContent = "";	
		//fileByteInfo[strContent[0]].ch = strContent[0];	
	}
	//2.�ָ���������
	HuffmanTree<ByteInfo> h;
	ByteInfo invalid;
	h.CreateHuffmanTree(fileByteInfo, 256, invalid);

	//3.��ȡѹ�����ݣ���Ϲ����������н�ѹ��
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
			//���ֽڽ��н�ѹ��
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
					//ÿ��ѹ��һ���͸���ѹ�����ֽ���++
					//����ɹ���ѹ���ֽڵĸ�����Դ�ļ���С��ͬʱ���ѹ������
					if (compressSize == fileSize) {
						break;
					}
				}
				ch <<= 1;//����λ�ı���λ����
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
	//��ȡԴ�ļ��ĺ�׺
	string postFix = filePath.substr(filePath.rfind('.'));
	postFix += '\n';
	fwrite(postFix.c_str(), 1, postFix.size(), fOut);
	//������
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
	//�������Լ�Ƶ����Ϣ
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