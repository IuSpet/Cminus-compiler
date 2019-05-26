#include "Parser.h"
const int BUFFERLENGTH = 4096;

//构造函数指定文法文件和token文件
Parser::Parser()
{
	grammar_file = "D://cminus//grammar.txt";
	token_file = "D://cminus//token.txt";
	filepos = 0;
	//语法树根
	root.Parent = NULL;
	root.type = "non-terminal";
	root.value = "program";
}

void Parser::get_LL1_grammar()
{
	get_grammar();						//读文法
	print_grammar0();
	Eliminate_left_recursion();			//去左递归
	print_grammar1();
	get_all_Vn();						//标记所有非终结符
	mark_empty();						//标记所有能产生空的产生式
	print_empty();
	get_left_common_factor();			//消除左公因子
	print_grammar2();
	reconsitution();					//重构存储文法的数据结构
	print_final_grammar();
	get_FIRST();						//计算FIRST集合
	print_FIRST();
	get_FOLLOW();						//计算FOLLOW集合
	print_FOLLOW();
	if (!judge_LL1_grammar())
	{
		std::cout << "不是LL(1)文法" << std::endl;
		//system("pause");
	}
	else
	{
		std::cout << "是LL(1)文法" << std::endl;
		//system("pause");
	}
	get_predict_table();				//构造预测分析表
	print_predictive_table();
	
}

//解析token，构造语法树
void Parser::Parse()
{
	std::ofstream outfile("D://cminus//matching_process.txt");
	outfile << std::setiosflags(std::ios::left);
	outfile << std::setw(30) << "Stack" << std::setw(20) << "Input" << "Action" << std::endl;
	std::stack<node*> match;
	//std::stack<std::string> match;

	node end;
	end.Parent = NULL;
	end.type = "";
	end.value = "$";

	match.push(&end);
	match.push(&root);
	//match.push("$");
	//match.push(root.value);

	std::string type;
	std::string value;
	std::string out;
	std::string token = get_next_token();
	get_token_value(token, value, type);

	while (true)
	{
		node top = *match.top();
		match.pop();
		//token读取完了
		if (token == "$")
		{
			//匹配成功
			if (match.size() == 1)
			{
				outfile << std::setw(30) << "$" << std::setw(20) << "$" << "accept" << std::endl;
				match.pop();
				break;
			}
			else
			{
				//栈顶是非终结符，符号或保留字，匹配值存在value中
				bool choose = true;
				if (top.type == "RESERVED WORD" || top.type == "SYMBOL" || top.type == "non-terminal")
				{
					out = top.value + " ... " + "$";
					outfile << std::setw(30) << out << std::setw(20) << "$" << "output" << top.value;
				}
				//栈顶是标识符或运算符，匹配值存在type中
				else
				{
					out = top.type + " ... " + "$";
					outfile << std::setw(30) << out << std::setw(20) << "$" << "output" << top.value;
					choose = false;
				}

				//查表获取操作
				std::vector<std::string> &pro = choose ? predictive_table[std::make_pair(top.value, value)] : predictive_table[std::make_pair(top.type, value)];
				out.clear();
				vector_to_string(out, pro);
				outfile << " -> " << out << std::endl;

				//空的情况
				if (out == "empty")
				{
					continue;
				}

				//往栈中压入新值,并连接节点建树
				for (int i = pro.size() - 1; i >= 1; i--)
				{
					node *son;
					son = new node;
					//新节点是原栈顶的子节点
					top.sons.push_back(son);
					son->Parent = &top;

					//根据新压入符号不同构造节点
					if (is_Vn[pro[i]])
					{
						son->type = "non-terminal";
						son->value = pro[i];
					}
					else
					{
						if (pro[i] == "ID" || pro[i] == "NUM")
						{
							son->type = pro[i];
							son->value = "";
						}
						else
						{
							if (pro[i][0] >= 'a' && pro[i][0] <= 'z')
							{
								son->type = "RESERVED WORD";
								son->value = pro[i];
							}
							else
							{
								son->type = "SYMBOL";
								son->value = pro[i];
							}
						}
					}
					match.push(son);
				}
			}
		}		
		else
		{
			bool choose = true;
			if (top.type == "RESERVED WORD" || top.type == "SYMBOL" || top.type == "non-terminal")
			{
				out = top.value + " ... " + "$";
				outfile << std::setw(30) << out;
			}
			//栈顶是标识符或运算符，匹配值存在type中
			else
			{
				out = top.type + " ... " + "$";
				outfile << std::setw(30) << out;
				choose = false;
			}

			//token是关键字或符号
			if (type == "RESERVED WORD" || type == "SYMBOL")
			{
				out = value + " ... " + "$";
				outfile << std::setw(30) << out;
				//匹配，读取下一个token
				if (top.value == value)
				{
					outfile << "match" << std::endl;
					token.clear();
					token = get_next_token();
					get_token_value(token, value, type);
					continue;
				}
				//不匹配，查表
				else
				{
					std::vector<std::string> &pro = choose ? predictive_table[std::make_pair(top.value, value)] : predictive_table[std::make_pair(top.type, value)];
					if (choose)
					{
						outfile << "output: " << top.value << " -> ";
					}
					else
					{
						outfile << "output: " << top.type << " -> ";
					}
					out.clear();
					vector_to_string(out, pro);
					outfile << out << std::endl;

					//空的情况
					if (out == "empty")
					{
						continue;
					}

					//往栈中压入新值,并连接节点建树
					for (int i = pro.size() - 1; i >= 0; i--)
					{
						node *son;
						son = new node;
						//新节点是原栈顶的子节点
						top.sons.push_back(son);
						son->Parent = &top;

						//根据新压入符号不同构造节点
						if (is_Vn[pro[i]])
						{
							son->type = "non-terminal";
							son->value = pro[i];
						}
						else
						{
							if (pro[i] == "ID" || pro[i] == "NUM")
							{
								son->type = pro[i];
								son->value = "";
							}
							else
							{
								//保留字
								if (pro[i][0] >= 'a' && pro[i][0] <= 'z')
								{
									son->type = "RESERVED WORD";
									son->value = pro[i];
								}
								//运算符
								else
								{
									son->type = "SYMBOL";
									son->value = pro[i];
								}
							}
						}
						match.push(son);
					}
				}
			}
			//token是标识符或数字
			else
			{
				out = type + " ... " + "$";
				outfile << std::setw(30) << out;
				//匹配，节点加入值，读取下一个token
				if (top.type == type)
				{
					outfile << "match" << std::endl;
					top.value = value;
					token.clear();
					token = get_next_token();
					get_token_value(token, value, type);
					continue;
				}
				else
				{
					std::vector<std::string> &pro = choose ? predictive_table[std::make_pair(top.value, type)] : predictive_table[std::make_pair(top.type, type)];

					if (choose)
					{
						outfile << "output: " << top.value << " -> ";
					}
					else
					{
						outfile << "output: " << top.type << " -> ";
					}

					out.clear();
					vector_to_string(out, pro);
					outfile << out << std::endl;

					//空的情况
					if (out == "empty")
					{
						continue;
					}

					//往栈中压入新值,并连接节点建树
					for (int i = pro.size() - 1; i >= 0; i--)
					{
						node *son;
						son = new node;
						//新节点是原栈顶的子节点
						top.sons.push_back(son);
						son->Parent = &top;

						//根据新压入符号不同构造节点
						if (is_Vn[pro[i]])
						{
							son->type = "non-terminal";
							son->value = pro[i];
						}
						//终结符，判断是否匹配
						else
						{
							if (pro[i] == "ID" || pro[i] == "NUM")
							{
								son->type = pro[i];
								son->value = "";
							}
							else
							{
								//保留字
								if (pro[i][0] >= 'a' && pro[i][0] <= 'z')
								{
									son->type = "RESERVED WORD";
									son->value = pro[i];
								}
								//运算符
								else
								{
									son->type = "SYMBOL";
									son->value = pro[i];
								}
							}
						}
						match.push(son);
					}
				}
			}
		}
	}
	outfile.close();
}

void Parser::test()
{
	std::string s;
	while (s != "$") s = get_next_token();
	system("pause");
}

//打印从文件中读取的文法
void Parser::print_grammar0()
{
	std::ofstream outfile("D://cminus//grammar0.txt");
	for (const auto &l : grammar)
	{
		bool first = true;
		for (const auto &t : l)
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

//打印去除左递归后的文法
void Parser::print_grammar1()
{
	std::ofstream outfile("D://cminus//grammar1.txt");
	for (const auto &l : grammar)
	{
		bool first = true;
		for (const auto &t : l)
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

//打印提取公因子后的文法
void Parser::print_grammar2()
{
	std::ofstream outfile("D://cminus//grammar2.txt");

	for (const auto &l : grammar)
	{
		bool first = true;
		for (const auto &t : l)
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

void Parser::print_final_grammar()
{
	std::ofstream outfile("D://cminus//ll(1)grammar.txt");
	for (auto &gm : final_grammar)
	{
		outfile << gm[0][0] << " -> ";
		for (int i = 1; i < gm.size(); i++)
		{
			for (int j = 0; j < gm[i].size(); j++)
			{
				outfile << gm[i][j] << " ";
			}
			outfile << "| ";
		}
		outfile << std::endl << std::endl;
	}
	outfile.close();
}

//打印能产生empty的文法
void Parser::print_empty()
{
	std::ofstream outfile("D://cminus//empty.txt");
	for (const auto &gm : grammar)
	{
		const std::string &Vn = *gm.begin();
		if (can_produce_empty[Vn]) outfile << Vn << std::endl;
		else continue;
	}
	outfile.close();
}

//打印各个文法的FIRST集合
void Parser::print_FIRST()
{
	std::ofstream outfile("D://cminus//FIRST.txt");
	for (const auto &gm : final_grammar)
	{
		outfile << gm[0][0] << " : { ";
		for (const auto &str : FIRST[gm[0][0]])
		{
			outfile << str << "| ";
		}
		outfile << "}"<<std::endl << std::endl;
	}
	outfile.close();
}

//打印各个文法的FOLLOW集合
void Parser::print_FOLLOW()
{
	std::ofstream outfile("D://cminus//FOLLOW.txt");
	for (const auto &gm : final_grammar)
	{
		const auto &Vn = gm[0][0];
		outfile << Vn << " :{ ";
		for (auto &str : FOLLOW[Vn])
		{
			outfile << str << " | ";
		}
		outfile << "}" << std::endl << std::endl;
	}
	outfile.close();
}

//打印预测分析表
void Parser::print_predictive_table()
{
	std::ofstream outfile("D://cminus//predictive_table.txt");
	for (auto &p : predictive_table)
	{
		outfile << p.first.first << "\t" << p.first.second << "\t";
		std::string pro;
		vector_to_string(pro, p.second);
		outfile << pro << std::endl << std::endl;
	}
	outfile.close();
}

void Parser::string_to_vector(std::string & s, std::vector<std::string>& v)
{
	std::string tmp;
	for (auto &c : s)
	{
		if (c == ' ')
		{
			v.push_back(tmp);
			tmp.clear();
		}
		else
		{
			tmp.push_back(c);
		}
	}
	if (tmp.length()) v.push_back(tmp);
}

void Parser::vector_to_string(std::string & s, std::vector<std::string>& v)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == "") continue;
		s += v[i];
		if (i != v.size() - 1)
		{
			s.push_back(' ');
		}
	}
}

//计算两个文法产生式产生能识别的公共终结符
std::string Parser::common_prefix(std::string & gm1, std::string & gm2)
{
	std::vector<std::string> pro1, pro2;
	string_to_vector(gm1, pro1);
	string_to_vector(gm2, pro2);
	std::string common_fact;

	return std::string();
}

//判断两个产生式有没有左公因子
bool Parser::has_common_prefix(std::vector<std::string>& gm1, std::vector<std::string>& gm2)
{
	std::set<std::string> S, s1, s2;
	s1 = get_left(gm1);
	s2 = get_left(gm2);
	S.insert(s1.begin(), s1.end());
	S.insert(s2.begin(), s2.end());
	return s1.size() + s2.size() != S.size();
}

//返回产生式的first集，查找左公因子用
std::set<std::string> Parser::get_left(std::vector<std::string> & tmp)
{
	std::set<std::string> res;
	if (tmp.size() == 0)
	{
		return res;
	}
	int i = 0;
	do
	{
		if (is_Vn[tmp[i]])
		{
			for (auto &gm : grammar)
			{
				auto p = gm.begin();
				if (tmp[i] == *p)
				{
					for (p++; p != gm.end(); p++)
					{
						std::vector<std::string> vs;
						string_to_vector(*p, vs);
						auto s = get_left(vs);
						res.insert(s.begin(), s.end());
					}
					break;
				}
			}
		}
		else
		{
			res.insert(tmp[i]);
		}
		if (i == tmp.size() - 1 && can_produce_empty[tmp[i]])
		{
			res.insert("empty");
			break;
		}
	} while (can_produce_empty[tmp[i++]]);
	return res;
}

 

//将产生式中的第一个非终结符扩展为终结符，出现 | 往list中加入新的产生式，成功扩展返回true
bool Parser::enlarge(std::list<std::vector<std::string>>& l)
{
	bool res = false;
	std::vector<std::string> &org = *l.begin();
	for (auto &pro : org)
	{

	}
	return res;
}

void Parser::get_token_value(std::string & token, std::string & value, std::string & type)
{
	type.clear();
	value.clear();
	node *res;
	res = new node;
	for (int i = 0; i < token.length(); i++)
	{
		auto c = token[i];
		if (c == '<' && i == 0) continue;
		else if (c == ',')
		{
			type = value;
			value.clear();
			continue;
		}
		else if (c == '>' && i == token.length() - 1)
		{
			break;
		}
		else
		{
			value.push_back(c);
		}
	}
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
			std::string newprdt(Vn + "_1");				//附加产生式
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
	FIRST.clear();
	for (auto &gm : final_grammar)
	{
		std::string &Vn = gm[0][0];
		if (FIRST[Vn].empty())
		{
			FIRST[Vn] = cal_first(Vn);
		}
	}
}

//递归计算当前非终结符的fist集合
std::set<std::string> Parser::cal_first(std::string Vn)
{
	std::set<std::string> res;
	for (auto &gm : final_grammar)
	{
		if (gm[0][0] == Vn)
		{
			//每个产生式的FIRST集合
			for (int j = 1; j < gm.size(); j++)
			{
				auto &pro = gm[j];
				//从每个产生式的第一个非终结符开始计算FISRST，如果里面有empty，再计算下一个，若最后一个也有empty，将empty加入FIRST集合
				for (int i = 0; i < pro.size(); i++)
				{
					//判断是不是非终结符
					if (is_Vn[pro[i]])
					{
						FIRST[pro[i]] = cal_first(pro[i]);
						res.insert(FIRST[pro[i]].begin(), FIRST[pro[i]].end());
						if (FIRST[pro[i]].find("empty") != FIRST[pro[i]].end())
						{
							if (i == pro.size() - 1)
							{
								res.insert("empty");
								break;
							}
							else continue;
						}
						else break;
					}
					else
					{
						if (pro[i] != "empty")
						{
							res.insert(pro[i]);
						}
						//产生式只有一个empty的情况
						else if (i == 0)
						{
							res.insert("empty");
						}
						break;
					}
				}
			}
			break;
		}
	}
	return res;
}

//获取FOLLOW集合
void Parser::get_FOLLOW()
{
	FOLLOW[final_grammar[0][0][0]].insert("$");
	int t = final_grammar.size();
	while (t--)
	{
		for (auto &gm : final_grammar)
		{
			/*
			对于每条文法规则，不断寻找A -> αBβ结构，
			将FIRST(β)中除empty外的所有符号加入FOLLOW(B)
			*/
			auto p = gm.begin();
			for (p++; p != gm.end(); p++)
			{
				auto &pro = *p;
				/*
				标记该符号后面的部分能否产生empty
				从而判断是否应用规则：对于A -> αBβ
				将FOLLOW(A)中的符号加入FOLLOW(B)中
				flag为true时是应用，false时是不应用
				*/
				bool flag = true;
				//从后往前判断每个文法，不能产生empty时将flag改为false
				for (int i = pro.size() - 1; i >= 0; i--)
				{
					//判断是否是非终结符
					if (is_Vn[pro[i]])
					{
						
						if (flag)
						{
							//因为gm[0][0]的follow集合可能不全，故需要反复调用该函数补全各个集合
							FOLLOW[pro[i]].insert(FOLLOW[gm[0][0]].begin(), FOLLOW[gm[0][0]].end());
							if (FIRST[pro[i]].find("empty") == FIRST[pro[i]].end()) flag = false;
						}
						//将后半部分first加入follow
						for (int j = i + 1; j < pro.size(); j++)
						{
							if (is_Vn[pro[j]])
							{
								FOLLOW[pro[i]].insert(FIRST[pro[j]].begin(), FIRST[pro[j]].end());
								if (FIRST[pro[j]].find("empty") == FIRST[pro[j]].end())
								{
									flag = false;
									break;
								}
							}
							else
							{
								FOLLOW[pro[i]].insert(pro[j]);
								break;
							}
						}	
					}
					else
					{
						flag = false;
					}
				}
			}
		}
	}
	//FOLLOW集合没有empty
	for (auto &gm : final_grammar)
	{
		auto &Vn = gm[0][0];
		auto p = FOLLOW[Vn].find("empty");
		if (p != FOLLOW[Vn].end()) FOLLOW[Vn].erase(p);
	}
}

//判断是不是LL(1)文法
bool Parser::judge_LL1_grammar()
{
	bool res = true;
	for (auto &gm : final_grammar)
	{
		auto &Vn = gm[0][0];
		bool flag = false;
		//first(Vn)里有empty，需要比较follow(Vn)与first(A)，A是first集里没有empty的产生式
		if (FIRST[Vn].find("empty") != FIRST[Vn].end())
		{
			flag = true;
		}
		for (int i = 1; i < gm.size(); i++)
		{
			//整个产生式的FIRST集合
			std::set<std::string> s1;
			for (auto &v : gm[i])
			{
				//是非终结符
				if (is_Vn[v])
				{
					s1.insert(FIRST[v].begin(), FIRST[v].end());
					//里面不含empty
					if (FIRST[v].find("empty") == FIRST[v].end()) break;
				}
				else
				{
					s1.insert(v);
					break;
				}
			}
			if (flag && s1.find("empty") == s1.end()) res &= cmp_set(s1, FOLLOW[Vn]);
			for (int j = i + 1; j < gm.size(); j++)
			{
				std::set<std::string> s2;
				for (auto &v : gm[j])
				{
					if (is_Vn[v])
					{
						s2.insert(FIRST[v].begin(), FIRST[v].end());
						if (FIRST[v].find("empty") == FIRST[v].end()) break;
					}
					else
					{
						s2.insert(v);
						break;
					}
				}
				res &= cmp_set(s1, s2);
			}
		}
	}
	return res;
}

bool Parser::cmp_set(const std::set<std::string> s1, const std::set<std::string> s2)
{
	int l1 = s1.size();
	int l2 = s2.size();
	std::set<std::string> s;
	s.insert(s1.begin(), s1.end());
	s.insert(s2.begin(), s2.end());
	int l = s.size();
	if (l1 + l2 != l) {
		std::cout << "error" << std::endl;
	}
	return (l1 + l2) == l;
}

//计算预测分析表
void Parser::get_predict_table()
{
	/*
	对于A -> α,将产生式加入pair(A, a∈FIRST(α))中
	如果empty在FIRST(α)中，将产生式加入pair(A, b∈FOLLOW(A))中
	*/
	for (auto &gm : final_grammar)
	{
		auto &Vn = gm[0][0];
		for (int i = 1; i < gm.size(); i++)
		{
			//gm[i]即为α
			auto &pro = gm[i];
			std::set<std::string> s;		//α的FIRST集合
			bool flag = false;				//FIRST里有没有empty
			for (auto &v : pro)
			{
				//是非终结符
				if (is_Vn[v])
				{
					s.insert(FIRST[v].begin(), FIRST[v].end());
					//里面不含empty
					if (FIRST[v].find("empty") == FIRST[v].end()) break;
				}
				else
				{
					s.insert(v);
					break;
				}
			}
			if (s.find("empty") != s.end())
			{
				flag = true;
			}
			for (auto &Vt : s)
			{
				predictive_table[std::make_pair(Vn, Vt)] = pro;
			}
			if (flag)
			{
				for (auto &Vt : FOLLOW[Vn])
				{
					predictive_table[std::make_pair(Vn, Vt)] = pro;
				}
			}
		}

	}
}

//提取左公因子
void Parser::get_left_common_factor()
{
	for (auto &gm : grammar)
	{
		int sign = 2;
		//reconsitution();
		//get_FIRST();		
	loop:
		auto p = gm.begin();
		p++;
		//两层循环比较该非终结符所有的产生式
		for (; p != gm.end(); p++)
		{
			
			auto p2 = p;
			for ( p2++; p2 != gm.end(); p2++)
			{
				
				//把产生式内每个标识符存到vector里方便访问
				std::vector<std::string> pro1, pro2;
				string_to_vector(*p, pro1);
				string_to_vector(*p2, pro2);
				//判断两个产生式有没有左公因式
				if (!has_common_prefix(pro1, pro2)) continue;
				else
				{	
					//产生式相同，提取出来
					if (pro1[0] == pro2[0])
					{
						std::vector<std::string> common;
						int i = 0;
						while (i < pro1.size() && i < pro2.size() && pro1[i] == pro2[i])
						{
							common.push_back(pro1[i]);
							pro1[i] = "";
							pro2[i] = "";
							i++;
						}
						std::string common_fact;
						vector_to_string(common_fact, common);
						std::string npro1, npro2;
						vector_to_string(npro1, pro1);
						vector_to_string(npro2, pro2);
						std::string nVn(gm.front());
						nVn = nVn + '_' + (char)(sign + 48);
						sign++;
						//将有公因子的产生式删除
						gm.erase(p);
						gm.erase(p2);
						//替换
						gm.push_back(common_fact + ' ' + nVn);
						//新的文法规则
						std::list<std::string> tmp;
						tmp.push_back(nVn);
						if (npro1 != "")
						{
							tmp.push_back(npro1);
						}
						else
						{
							tmp.push_back("empty");
						}
						if (npro2 != "")
						{
							tmp.push_back(npro2);
						}	
						else
						{
							tmp.push_back("empty");
						}
						grammar.push_back(tmp);
						is_Vn[nVn] = true;
					}
					//说明是该产生式的产生式有公因子,先展开
					else
					{
						//展开p指向的产生式
						if (is_Vn[pro1[0]])
						{
							for (auto & g: grammar)
							{
								if (g.front() == pro1[0])
								{
									std::string npro;
									pro1[0] = "";
									vector_to_string(npro, pro1);
									gm.erase(p);
									auto it = g.begin(); it++;
									for (; it != g.end(); it++)
									{
										if (npro != "")
										{
											gm.push_back(*it + ' ' + npro);
										}
										else
										{
											gm.push_back(*it);
										}
									}
									break;
								}
							}
						}
						//展开p2指向的产生式
						if (is_Vn[pro2[0]])
						{
							for (auto & g : grammar)
							{
								if (g.front() == pro2[0])
								{
									std::string npro;
									pro2[0] = "";
									vector_to_string(npro, pro2);
									gm.erase(p2);
									auto it = g.begin(); it++;
									for (; it != g.end(); it++)
									{
										if (npro != "")
										{
											gm.push_back(*it + ' ' + npro);
										}
										else
										{
											gm.push_back(*it);
										}
									}
									break;
								}
							}
						}
					}	
					goto loop;
				}
			}
		}
	}
}

//标记所有非终结符与终结符
void Parser::get_all_Vn()
{
	//往map中加入所有终结符与非终结符
	for (auto gm : grammar)
	{
		for (auto V : gm)
		{
			is_Vn[V] = false;
		}
	}
	//将非终结符标记为true
	for (auto gm : grammar)
	{
		is_Vn[*gm.begin()] = true;
	}
}

//标记能产生empty的文法
void Parser::mark_empty()
{
	for (auto &gm : grammar)
	{
		auto it = gm.begin();
		auto &Vn = *it;
		it++;
		can_produce_empty[Vn] = false;
		for (; it != gm.end(); it++)
		{
			if (*it == "empty")
			{
				can_produce_empty[Vn] = true;
				break;
			}
		}
	}
}

//文法不再改变，换个数据结构，方便后面使用
void Parser::reconsitution()
{
	final_grammar.clear();
	std::vector<std::vector<std::string>> tmp;
	for (auto &gm : grammar)
	{
		tmp.clear();
		std::vector<std::string> vs;
		for (auto &pro : gm)
		{
			vs.clear();
			std::string sub;
			for (auto &c : pro)
			{
				if (c == ' ')
				{
					vs.push_back(sub);
					sub.clear();
				}
				else
				{
					sub.push_back(c);
				}
			}
			vs.push_back(sub);
			tmp.push_back(vs);
		}
		final_grammar.push_back(tmp);
	}
	//grammar.clear();
}


//不断获取下一个token建立语法树
std::string Parser::get_next_token()
{
	std::ifstream infile(token_file);
	infile.seekg(filepos, std::ios::beg);
	std::string str;
	if (std::getline(infile, str))
	{
		filepos += str.length() + 2;
		//std::cout << str << std::endl;
		return str;
	}
	else
	{
		return std::string("$");
	}
	infile.close();
}
