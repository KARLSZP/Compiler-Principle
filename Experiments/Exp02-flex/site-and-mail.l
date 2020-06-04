
%{
int sites = 0; 
int emails = 0;
int verbose = 0;
%} 

%% 
http[^ \t\n\r\f\v]+  {printf(">>> [url]: %s is a valid url.\n", yytext); sites++; }

[0-9]+@[a-zA-Z0-9]+\.[a-zA-Z]+  {printf(">>> [Email]: %s is an email address.\n", yytext); emails++; }

[a-zA-Z]+ {if(verbose) printf(">>> [Word]: '%s' is ignored.\n", yytext);}

[ \t\r\f\v] {if(verbose) printf(">>> [Blank]: '%s' is ignored.\n", yytext);}

. {if(verbose) printf(">>> [UNK]: %s is ignored.\n", yytext);}

\n {return 0; } 
%%


int yywrap() {}

int main() {

printf("\n### Find site(s) and email(s) ###\n");
printf(">>> verbose(1/0):");
scanf("%d", &verbose);
getchar();

printf(">>> Enter source text:\n");

yylex();

printf("\nConclusion:\n");
printf("Number of url(s) in the given input - %d\n", sites); 
printf("Number of email(s) in the given input - %d\n", emails); 

return 0; 

}
