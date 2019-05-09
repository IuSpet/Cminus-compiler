//作者：IuSpet
//作用：将一般文法转变为LL(1)文法并构建语法分析树

#ifndef Parser_h
#define Parser_h
#include"utlib.h"

//extern const int BUFFERLENGTH;

class Parser
{
public:
	Parser();
	void get_LL1_grammar();						//得到LL（1）文法
	void Parse();
	void print_grammar0();						//打印初始文法
	void print_grammar1();
	void print_grammar2();						//打印提取左公因子后的文法
	void print_FIRST();
	void print_FOLLOW();

private:
	std::list<std::list<std::string>> grammar;		//文法
	const char *grammar_file;
	const char *token_file;

	FILE* f;

	void get_grammar();								//获取初始文法
	void Eliminate_left_recursion();				//消除左递归
	void get_FIRST();								//获取FIRST集合
	void get_FOLLOW();								//获取FOLLOW集合
	bool judge_LL1_grammar();						//判断是不是LL(1)文法
	void get_predict_table();						//计算预测分析表
	void get_left_common_factor();					//提取左公因子
	std::string get_next_token();					//不断获得下一个token
};
#endif // !Parser_h
#pragma once
