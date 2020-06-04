/**
* 17341137 宋震鹏
*   This .l file help generate a scanner with flex.
*   Target file name: inputfile.c
* 
* Run in terminal:
*   make
* 
* With verbose:
*   = 1: show log of other pattern found.
*   = 0: show log of intergers and float numbers only.
*/



%{
int intergers = 0; 
int floats = 0;
int comments = 0;
int var_funcs = 0;
int verbose = 0;
%} 

%% 
[/]{2}.* {if (verbose) printf(">>> [Comment]: \"%s\" is a comment.\n", yytext); comments++;}
[/][*][^*]*[*]+([^*/][^*]*[*]+)*[/] {if (verbose) printf(">>> [Multiline-Comment]: \"%s\" is a multilined-comment.\n", yytext); comments++;}

-?[1-9][0-9]*\.[0-9]+ {printf(">>> [Float]: \"%s\" is a float number.\n", yytext); floats++;}
[a-zA-Z_]+-?[0-9]+ {if (verbose) printf(">>> [Var/Func]: \"%s\" is a variable or a function name.\n", yytext); var_funcs++;}
-?[0-9]+ {printf(">>> [Interger]: \"%s\" is an interger.\n", yytext); intergers++;}

\n {}
. {}
<<EOF>> {printf("File End.\n"); return 0;} 
%%


int yywrap() {}

int main() {

    printf("\n### Find Interger(s) and Float number(s) ###\n");
    printf(">>> verbose(1/0):");
    scanf("%d", &verbose);
    getchar();


    freopen("./inputfile.c", "r", stdin);
    yylex();

    printf("\n### Conclusion ###\n");
    printf("Number of Interger(s) in the code - %d\n", intergers); 
    printf("Number of Float number(s) in the code - %d\n", floats);
    printf("Number of Comment(s) in the code - %d\n", comments);
    printf("Number of Variable(s)/Function(s) in the code - %d\n", var_funcs);

    return 0; 

}
