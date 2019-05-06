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
		for (auto t : l)
		{
			std::cout << t << "  ";
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
		char str[256] = {};
		int pos = 0;
		for (int i = 0; i < len; i++)
		{
			if (input_buffer[i] == ' ') continue;
			else if (input_buffer[i] == '|')
			{
				tmp.push_back(str);
				memset(str, 0, sizeof(str));
				pos = 0;
			}
			else if (input_buffer[i] == -95)
			{
				tmp.push_back(str);
				memset(str, 0, sizeof(str));
				pos = 0;
				i++;
			}
			else str[pos++] = input_buffer[i];
		}
		tmp.push_back(str);
		grammar0.push_back(tmp);
	}
}

void Parser::Eliminate_left_recursion()
{
	//TO DO:Ïû³ýgrammar0ÀïµÄ×óµÝ¹é
}

std::string Parser::get_next_token()
{
	return std::string();
}
