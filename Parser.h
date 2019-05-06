//作者：IuSpet
//作用：将一般文法转变为LL(1)文法并构建语法分析树

#ifndef Parser_h
#define Parser_h
#include"utlib.h"

//extern const int BUFFERLENGTH;

class Parser
{
public:
	Parser(const char *gf, const char *tf);
	void get_LL1_grammar();						//得到LL（1）文法
	void Parse();
	void print_grammar0();						//打印初始文法

private:
	std::list<std::list<std::string>> grammar0;		//初始文法
	std::list<std::list<std::string>> grammar1;		//消除左递归后的文法
	std::list<std::list<std::string>> grammar2;		//去除左公因子后的文法
	const char *grammar_file;
	const char *token_file;

	FILE* f;

	void get_grammar();								//获取初始文法
	void Eliminate_left_recursion();				//消除左递归
	std::string get_next_token();					//不断获得下一个token
};
#endif // !Parser_h
#pragma once
