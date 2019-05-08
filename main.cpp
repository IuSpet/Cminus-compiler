#include"scaner.h"
#include"Parser.h"

int main()
{
	//freopen("D://source//in.c", "r", stdin);
	//freopen("D://cminus//token.txt", "w", stdout);
	const char *source_file = "D://cminus//in.c";
	//Scaner s1(source_file);
	//s1.GetToken();
	Parser p1;
	p1.print_grammar0();
	p1.print_grammar1();
	//system("pause");
	return 0;
}