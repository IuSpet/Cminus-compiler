#include "Parser.h"

const int BUFFERLENGTH = 4096;

Parser::Parser()
{
	grammar_file = "D://cminus//grammar.txt";
	token_file = "D://cminus//token.txt";
}

void Parser::get_LL1_grammar()
{
}

void Parser::Parse()
{
}


void Parser::print_grammar0()
{
	std::ofstream outfile("D://cminus//grammar0.txt");
	//outfile.open("D://cminus//grammar0.txt", std::ios::out, 0);		//文件输出流指向保存grammar0的文件
	
	get_grammar();
	for (auto l : grammar0)
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

void Parser::print_grammar1()
{
	Eliminate_left_recursion();
	std::ofstream outfile("D://cminus//grammar1.txt");

	for (auto l : grammar0)
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
		grammar0.push_back(tmp);
	}
}

//消除左递归
void Parser::Eliminate_left_recursion()
{
	//TO DO:消除grammar0里的左递归  
	for (auto p = grammar0.begin(); p != grammar0.end(); p++)
	{
		//展开当前文法
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
				std::string item(production);
				std::string tmp;					//提取production中的非终结符
				int subbg = 0, sublength = 0;			//记录tmp出现的位置保留前缀后缀
				for (char c:production)
				{
					if (c != ' ')
					{
						tmp.push_back(c);
						sublength++;
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
								item.replace(subbg, sublength, *pB);
								pA = A.insert(pA, item);		//加入替换后的产生式
							}
							pA = A.begin(); 
							break;
						}
						//std::cout << tmp << std::endl;
						subbg += sublength;
						subbg++;
						sublength = 0;
						tmp.clear();
					}
				}
				//最后一个产生式
				if (Vn == tmp && pA != A.begin())				//该非终结符是前面的开始符号
				{
					//std::string prefix = production.substr(0,subbg);
					//std::string suffix = production.substr(subend);						
					pA = A.erase(pA);		//删除原产生式
					auto pB = B.begin(); pB++;
					for (; pB != B.end(); pB++)		//将production替换为B中的产生式
					{
						std::string newprd(item);
						item.replace(subbg, sublength, *pB);
						pA = A.insert(pA, item);		//加入替换后的产生式
					}
					pA = A.begin();
					continue;
				}
			}
		}
		//去除直接左递归

	}
}

std::string Parser::get_next_token()
{
	return std::string();
}
