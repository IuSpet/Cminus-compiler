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

//打印去除左递归后的文法检查
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
		std::list<std::string> &A = *p;			//文法A
		//展开当前文法
		for (auto j = grammar0.begin(); j != p ; j++)
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
			p = grammar0.insert(p, newgrammar);
		} while (false);
		
	}
}

std::string Parser::get_next_token()
{
	return std::string();
}
