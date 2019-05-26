//作者：IuSpet
//作用：将一般文法转变为LL(1)文法并构建语法分析树

#ifndef Parser_h
#define Parser_h
#include"utlib.h"

//语法树的节点
struct node
{
	node *Parent;
	std::string type;
	std::string value;
	std::list<node*> sons;
};
class Parser
{
public:
	Parser();
	
	void get_LL1_grammar();						//得到LL（1）文法
	void Parse();
	void test();
	
private:
	std::list<std::list<std::string>> grammar;		//文法	
	std::vector<std::vector<std::vector<std::string>>> final_grammar;	//处理后的LL(1)文法
	std::map<std::string, bool> can_produce_empty;
	std::map<std::string, bool> is_Vn;
	std::map<std::string, std::set<std::string>> FIRST, FOLLOW;
	//预测分析表,结构为		predictive_table[非终结符号，终结符号] = 产生式
	std::map<std::pair<std::string, std::string>, std::vector<std::string>> predictive_table;		
	const char *grammar_file;
	const char *token_file;
	FILE* f;
	int filepos;
	node root;										//语法树根


	void get_grammar();								//获取初始文法
	void Eliminate_left_recursion();				//消除左递归
	void get_FIRST();								//获取FIRST集合
	void get_FOLLOW();								//获取FOLLOW集合
	bool judge_LL1_grammar();						//判断是不是LL(1)文法
	bool cmp_set(const std::set<std::string> s1, const std::set<std::string> s2);	//判断两个set是否有交集
	void get_predict_table();						//计算预测分析表
	void get_left_common_factor();					//提取左公因子
	void get_all_Vn();								//找出所有非终结符
	void mark_empty();								//标记能产生empty的文法
	void reconsitution();							//换个方便的数据结构。。。
	std::string get_next_token();					//不断获得下一个token
	void print_grammar0();						//打印初始文法
	void print_grammar1();						//打印去除左递归后的文法
	void print_grammar2();						//打印提取左公因子后的文法
	void print_final_grammar();					//打印重构后的文法，测试
	void print_empty();							//打印各个非终结符能否产生empty
	std::set<std::string> cal_first(std::string Vn);
	void print_FIRST();
	void print_FOLLOW();
	void print_predictive_table();
	void string_to_vector(std::string &s, std::vector<std::string> &v);
	void vector_to_string(std::string &s, std::vector<std::string> &v);
	std::string common_prefix(std::string &gm1, std::string &gm2);
	bool has_common_prefix(std::vector<std::string> &gm1, std::vector<std::string> &gm2);
	std::set<std::string> get_left(std::vector<std::string> & tmp);		//返回左端，即first集，用于查看是否有左公因子
	bool enlarge(std::list<std::vector<std::string>> &l);	//扩展该文法中的一个非终结符，成果扩展返回true
	void get_token_value(std::string &token, std::string &value, std::string &type);
};
#endif // !Parser_h
#pragma once
