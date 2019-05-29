//作者：IuSpet
//作用：将c-源代码转化为token输出

#ifndef Scanner_h
#define Scanner_h
#include"utlib.h"

class Scanner
{
private:
	//char buffer[4096];		//读入源程序的缓冲区
	std::string buffer;
	int pos;						//缓冲区位置
	int syn;						//token类别
	int state;						//DFA中的状态
	std::string sourcename;
	int filepos;
	//int tsss;
	const int BUFFERLENGTH = 4096;
public:
	Scanner(const char* s)
	{
		//if (source = fopen(s, "r"));
		//else exit(1);
		sourcename = s;
		pos = 0;
		syn = -1;
		state = 0;
		filepos = 0;
		//fgets(buffer, BUFFERLENGTH, source);
		
	}
	void GetToken();				//在DFA上转移，识别token
	bool IsNum(const char c);
	bool IsLetter(const char c);
	char GetNext();					//获取下一个字符
	void Back();					//向前看完后回溯
};


#endif // !Scaner_h
#pragma once
