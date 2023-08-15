%{
#include "parser.tab.h"

void double_asterisk_type_comment(const char* cont);
void double_slash_type_comment(const char* cont);
void command_series_name(const char* cont);
void command_name(const char* cont);
%}

%option yylineno

%%
"/\*"[^*]*"\*"+([^/*][^*]*\*+)*"/"	double_asterisk_type_comment(yytext);	return GLOBALCMT;
"//"+.*								double_slash_type_comment(yytext);		return SGLINECMT;
"\["[a-zA-Z_]*"\]"					command_series_name(yytext);			return FILENAME;
[a-zA-Z_]+[ \t]*.*";"				command_name(yytext);					return CMDNAME;
(\n|\r\n)							/* ignore */;
[ \t]								/* ignore */;
%%

void double_asterisk_type_comment(const char* cont)
{
	if (cont)
	{
		yylval = (YYSTYPE)_strdup(cont);
		//printf("%s", cont);
	}
}

void double_slash_type_comment(const char* cont)
{
	if (cont)
	{
		yylval = (YYSTYPE)_strdup(cont);
		//printf("%s", cont);
	}
}

void command_series_name(const char* cont)
{
	if (cont)
	{
		yylval = (YYSTYPE)_strdup(cont);
		//printf("[Command Series Name : %s]", cont);
	}
}

void command_name(const char* cont)
{
	if (cont)
	{
		yylval = (YYSTYPE)_strdup(cont);
		//printf("Command Name : %s", cont);
	}
}

int yywrap(void)
{
	return 1;
}
