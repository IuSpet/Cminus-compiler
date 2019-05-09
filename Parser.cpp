#include "Parser.h"

const int BUFFERLENGTH = 4096;

//构造函数指定文法文件和token文件
Parser::Parser()
{
	grammar_file = "D://cminus//grammar.txt";
	token_file = "D://cminus//token.txt";
}

//去除左递归、左公因子、计算FIRST、FOLLOW集合判断LL(1)文法
void Parser::get_LL1_grammar()
{
}

//解析token，构造语法树
void Parser::Parse()
{
}

//打印从文件中读取的文法检查
void Parser::print_grammar0()
{
	std::ofstream outfile("D://cminus//grammar0.txt");
	//outfile.open("D://cminus//grammar0.txt", std::ios::out, 0);		//文件输出流指向保存grammar0的文件
	
	get_grammar();
	for (auto l : grammar)
	{
		bool first = true;
		for (auto t : l)
		{
			if (first)
			{
				outfile << t << " -> ";
				first = false;
			}
			else outfile << t << " | ";
		}
		outfile << std::endl << std::endl;
	}
	outfile.close();
}

//打印去除左递归后的文法检查
void Parser::print_grammar1()
{
	Eliminate_left_recursion();
	std::ofstream outfile("D://cminus//grammar1.txt");

	for (auto l : grammar)
	{
		bool first = true;
		for (auto t : l)
		{
			if (first)
			{
				outfile << t << " -> ";
				first = false;
			}
			else outfile << t << " | ";
		}
		outfile << std::endl << std::endl;
	}
	outfile.close();
}

void Parser::print_grammar2()
{
	get_left_common_factor();
	std::ofstream outfile("D://cminus//grammar2.txt");

	for (auto l : grammar)
	{
		bool first = true;
		for (auto t : l)
		{
			if (first)
			{
				outfile << t << " -> ";
				first = false;
			}
			else outfile << t << " | ";
		}
		outfile << std::endl << std::endl;
	}
	outfile.close();
}

void Parser::print_FIRST()
{
}

void Parser::print_FOLLOW()
{
}

//读取原始文法，保存到内存中
void Parser::get_grammar()						
{
	f = fopen(grammar_file, "r");
	char input_buffer[BUFFERLENGTH];
	while (fgets(input_buffer, BUFFERLENGTH, f))
	{
		std::list<std::string> tmp;
		int len = strlen(input_buffer);
		std::string str;
		int pos = 0;
		for (int i = 0; i < len; i++)
		{
			if (input_buffer[i] == ' ' && str.length() == 0) continue;
			else if (input_buffer[i] == '|')
			{
				while (str.back() == ' ') str.pop_back();
				tmp.push_back(str);
				str.clear();
				pos = 0;
			}
			else if (input_buffer[i] == -95)
			{
				while (str.back() == ' ') str.pop_back();
				tmp.push_back(str);
				str.clear();
				pos = 0;
				i++;
			}
			else str.push_back(input_buffer[i]);
		}
		while (str.back() == ' ' || str.back() == '\n') str.pop_back();
		tmp.push_back(str);
		grammar.push_back(tmp);
	}
}

//消除左递归
void Parser::Eliminate_left_recursion()
{
	//TO DO:消除grammar里的左递归  
	for (auto p = grammar.begin(); p != grammar.end(); p++)
	{
		std::list<std::string> &A = *p;			//文法A
		//展开当前文法
		for (auto j = grammar.begin(); j != p ; j++)
		{
			//替换A产生式中的Vn			
			std::list<std::string> &B = *j;			//文法B
			std::string &Vn = B.front();			//文法B的开头
			auto pA = A.begin(); pA++;
			//auto pB = B.begin(); pB++;
			for (; pA != A.end(); pA++)				
			{
				std::string &production = *pA;		//文法A中的产生式
				std::string item(production);
				std::string tmp;					//提取production中的非终结符
				for (char c:production)
				{
					if (c != ' ')
					{
						tmp.push_back(c);
					}
					else							//提取出了一个完整的非终结符
					{
						if (Vn == tmp)				//该非终结符是前面的开始符号
						{
							//std::string prefix = production.substr(0,subbg);
							//std::string suffix = production.substr(subend);						
							pA = A.erase(pA);		//删除原产生式
							auto pB = B.begin(); pB++;
							for (; pB != B.end(); pB++)		//将production替换为B中的产生式
							{
								std::string newprd(item);
								newprd.replace(0,Vn.length(), *pB);
								A.insert(pA, newprd);		//加入替换后的产生式
							}
							pA = A.begin(); 
							break;
						}
						//std::cout << tmp << std::endl;
						break;
					}
				}
			}
		}
		//去除直接左递归
		do
		{
			std::list<std::string>::iterator pA = A.begin();
			auto Vn = *pA;
			pA++;
			/*
			A -> Aα| β
			vs1存α，vs2存β
			*/
			std::vector<std::string> vs1, vs2;
			for (; pA != A.end(); pA++)
			{
				std::string &production = *pA;
				std::string first;				//每个产生式的第一个字母判断分给α还是β
				bool flag = true;
				for (char c : production)
				{
					if (c == ' ')
					{
						if (first == Vn)		//是Aα
						{
							vs1.push_back(production.substr(Vn.length() + 1));
						}
						else					//是β
						{
							vs2.push_back(production);
						}
						flag = false;			//已经处理过当前产生式
						break;
					}
					else
					{
						first.push_back(c);
					}
				}
				if (flag) vs2.push_back(production);
			}
			if (vs1.empty()) continue;			//α是空的，即无左递归
			pA = A.begin(); pA++;
			/*
			将A -> Aα| β改为
			A -> βA1
			A1 -> αA1
			*/
			while (pA != A.end()) pA = A.erase(pA);		//清空A的产生式
			//如果β只有empty，直接改为 A -> αA | empty
			if (vs2[0] == "empty")
			{
				for (std::string s : vs1)
				{
					A.push_back(s + ' ' + Vn);
				}
				A.push_back("empty");
				continue;
			}
			std::string newprdt(Vn + '1');				//附加产生式
			//βA1
			for (std::string s : vs2)
			{
				A.push_back(s + ' ' + newprdt);
			}
			//A1 -> αA1
			std::list<std::string> newgrammar;
			newgrammar.push_back(newprdt);				//加入A1
			for (std::string s : vs1)
			{
				newgrammar.push_back(s + ' ' + newprdt);
			}
			newgrammar.push_back("empty");
			p++;
			p = grammar.insert(p, newgrammar);
		} while (false);
		
	}
}

//获取FIRST集合
void Parser::get_FIRST()
{
}

//获取FOLLOW集合
void Parser::get_FOLLOW()
{
}

//判断是不是LL(1)文法
bool Parser::judge_LL1_grammar()
{
	return false;
}

//计算预测分析表
void Parser::get_predict_table()
{
}

//提取左公因子
void Parser::get_left_common_factor()
{
	//遍历所有文法规则
	for (auto p = grammar.begin(); p != grammar.end(); p++)
	{	
		auto &A = *p;
		if (A.size() == 2) continue;	//只有一个产生式
		auto it = A.begin(); it++;
		std::string common_factor(*it);
		//遍历从第二个开始的每个产生式
		for (; it != A.end(); it++)
		{
			const auto &production = *it;
			if (production == common_factor) continue;
			bool flag = false;			//有无公因子
			for (int i = production.length() - 1; i >= 0; i--)
			{
				//到空格，比较前面的产生式和已提取的公因子
				if (production[i] == ' ')
				{
					std::string prefix_production = production.substr(0, i);
					//将公因子截取为和当前子串长度相同
					common_factor = common_factor.substr(0, i);
					//前缀和已提取的公因子相同，比较下一个产生式
					if (common_factor == prefix_production)
					{
						flag = true;
						break;
					}
					else continue;
				}
			}
			//没有公因子
			if (!flag)
			{
				common_factor.clear();
				break;
			}
		}
		/*
		有公因子
		将A -> αB | αC 转化为
		A -> αA_2
		A_2 -> B | C
		*/
		if (common_factor.length())
		{
			//提取公因子外的其他产生式
			std::vector<std::string> left_factor;
			it = A.begin(); it++;
			for (; it != A.end(); it++)
			{
				if (it->length() == common_factor.length())
				{
					left_factor.push_back("empty");
				}
				else
				{
					std::string left_production = it->substr(common_factor.length() + 1);		//+1吞空格
					left_factor.push_back(left_production);
				}
				
			}			
			std::list<std::string> newgrammar;
			std::string Vn = *A.begin();
			std::string newVn(Vn + "_2");
			newgrammar.push_back(Vn);
			newgrammar.push_back(common_factor + ' ' + newVn);
			p = grammar.erase(p);
			//将A -> αA_2插入文法列表
			grammar.insert(p, newgrammar);
			newgrammar.clear();
			newgrammar.push_back(newVn);
			for (auto factor : left_factor)
			{
				newgrammar.push_back(factor);
			}
			//将A_2 -> B | C插入文法列表
			p = grammar.insert(p, newgrammar);
		}
	}
}

//不断获取下一个token建立语法树
std::string Parser::get_next_token()
{
	return std::string();
}
