
using namespace std;

#include "FileCompress.h"

int main() {
	FileCompress fc;
	fc.CompressFile("test.txt");
	fc.UNCompressFile("2.hzp");
	return 0;
}