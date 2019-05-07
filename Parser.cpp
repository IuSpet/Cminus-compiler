#include "Parser.h"

const int BUFFERLENGTH = 4096;

Parser::Parser(const char * gf, const char * tf)
{
	grammar_file = gf;
	token_file = tf;
}

void Parser::get_LL1_grammar()
{
}

void Parser::Parse()
{
}

void Parser::print_grammar0()
{
	get_grammar();
	for (auto l : grammar0)
	{
		bool first = true;
		for (auto t : l)
		{
			if (first)
			{
				std::cout << t << " -> ";
				first = false;
			}
			else std::cout << t << " | ";
		}
		std::cout << std::endl << std::endl;
	}
}

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
		grammar0.push_back(tmp);
	}
}

void Parser::Eliminate_left_recursion()
{
	//TO DO:消除grammar0里的左递归
	for (auto p = grammar0.begin(); p != grammar0.end(); p++)
	{
		for (auto j = grammar0.begin(); j != p ; j++)
		{
			//替换A产生式中的Vn
			std::list<std::string> &A = *p;			//文法A
			std::list<std::string> &B = *j;			//文法B
			std::string &Vn = B.front();			//文法B的开头
			auto pA = A.begin(); pA++;
			//auto pB = B.begin(); pB++;
			for (; pA != A.end(); pA++)				
			{
				std::string &production = *pA;		//文法A中的产生式
				std::string tmp;					//提取production中的非终结符
				int subbg = 0, subend = 0;			//记录tmp出现的位置保留前缀后缀
				for (char c : production)
				{
					if (c != ' ') tmp.push_back(c);
					else							//提取出了一个完整的非终结符
					{
						if (Vn == tmp)				//该非终结符是前面的开始符号
						{
							std::string prefix = production.substr();
							std::string suffix = production.substr();
							auto pB = B.begin(); pB++;
							for (; pB != B.end(); pB++)		//将production替换为B中的产生式
							{
								  
							}
						}
					}
				}
			}
		}
	}
}

std::string Parser::get_next_token()
{
	return std::string();
}
