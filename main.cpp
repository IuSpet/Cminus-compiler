#include"scaner.h"

int main()
{
	//freopen("D://source//in.c", "r", stdin);
	freopen("D://cminus//token.txt", "w", stdout);
	const char *source_file = "D://cminus//in.c";
	const char *grammar_file = "D://cminus//grammar.txt";
	const char *token_file = "D://cminus//token.txt";
	Scaner s1(source_file);
	s1.GetToken();
	return 0;
}