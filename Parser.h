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
	//去除左递归、左公因子、计算FIRST、FOLLOW集合判断LL(1)文法
	void get_LL1_grammar();						//得到LL（1）文法
	void Parse();
	void print_grammar0();						//打印初始文法
	void print_grammar1();						//打印去除左递归后的文法
	void print_grammar2();						//打印提取左公因子后的文法
	void print_empty();							//打印各个非终结符能否产生empty
	void print_FIRST();
	void print_FOLLOW();
private:
	std::list<std::list<std::string>> grammar;		//文法	
	std::list<std::list<std::vector<std::string>>> final_grammar;	//处理后的LL(1)文法
	std::map<std::string, bool> can_produce_empty;
	std::map<std::string, bool> is_Vn;
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
	void get_all_Vn();								//找出所有非终结符
	void mark_empty();								//标记能产生empty的文法
	void reconsitution();							//换个方便的数据结构。。。
	std::string get_next_token();					//不断获得下一个token
};
#endif // !Parser_h
#pragma once
