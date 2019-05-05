#include "Parser.h"

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
}

void Parser::get_grammar()
{
	f = fopen(grammar_file, "r");
	char input_buffer[BUFFERLENGTH];
	while (fgets(input_buffer, BUFFERLENGTH, f))
	{
		std::list<std::string> tmp;
		
	}
}

void Parser::Eliminate_left_recursion()
{
}

std::string Parser::get_next_token()
{
	return std::string();
}
