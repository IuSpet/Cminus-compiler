#include"scaner.h"
#include"Parser.h"

int main()
{
	const char *source_file = "D://cminus//in.c";
	//Scaner s1(source_file);
	//s1.GetToken();
	Parser p1;
	p1.get_LL1_grammar();
	p1.print_grammar0();
	p1.print_grammar1();
	p1.print_grammar2();
	p1.print_empty();
	//system("pause");
	return 0;
}