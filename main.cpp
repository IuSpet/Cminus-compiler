#include"scanner.h"
#include"Parser.h"

int main()
{
	const char *source_file = "D://cminus//in.c";
	Scanner s1(source_file);
	s1.GetToken();
	Parser p1;
	//p1.get_LL1_grammar();
	//p1.Parse();
	return 0;
}