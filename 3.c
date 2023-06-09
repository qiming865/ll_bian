#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include <math.h>

//定义所需变量

using namespace std;
int lineNumber = 1;      //行号，初始值为1
char allCharacter[2048]; //所有字符,存储输入的所有字符，包括空格、换行等
string ENDsign = "end.";
int i1 = 0; //用来记录str中的个数，易于遍历
int i2 = 0; //用来记录allString中的个数，易于下移
int i3 = 0; //用来记录符号表中的个数，易于语义检查
//定义符号表
struct symbolTable
{
    string type;
    string variable;
    double value;
};
struct symbolTable table[50];
//定义四元式
struct quaternion
{
    string op;
    string arg1;
    string arg2;
    string result;
};
struct quaternion allquaternion[50];
int dd = 0;
string programSegment[5]; //定义程序段数组，用于存储程序段名字
int pro = 0;
int semanticsError = 0; //语义错误个数

char charStr[100] = "\0"; //存储单个字符，易于存储到allString中
string allString[300] = {"\0"};
int grammarError = 0; //语法错误
int r1 = 0;           //记录错误行
//定义保留字
string reservedWord[20] = {"begin", "call",
                           "const", "do", "end", "if",
                           "odd", "procedure",
                           "read", "then",
                           "var", "while", "write"};
//字符串与字符拼接
void connect(char *str, char ch)
{
    int i = 0;
    while (str[i] != '\0')
        i++;
    str[i] = ch;
    str[i + 1] = '\0';
}
//计算字符串的长度
int strLength(char *str)
{
    int length = 0;
    while (str[length] != '\0')
    {
        length++;
    }
    return length;
}
//判断该字符是否字母
int charIsLetter(char c)
{
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        return 1;
    else
        return 0;
}
//判断字符是否为运算符
int charIsOperator(char c)
{
    if (c == '>' || c == '<' || c == '=' || c == '*' || c == '-' || c == '+' || c == '#')
        return 1;
    else
        return 0;
}
//判断字符是否为数字
int charIsNumber(char c)
{
    if ((c >= '0' && c <= '9'))
        return 1;
    else
        return 0;
}

int charIsDelimiter(char ch)
{
    if (ch == ';' || ch == '.' || ch == ',' || ch == '(' || ch == ')')
        return 1;
    else
        return 0;
}
//判断字符串是否为保留字
int stringIsReservedWord(char *str)
{

    for (int i = 0; i < 20; i++)
        if (reservedWord[i].compare(str) == 0)
            return 1;

    return 0;
}
//判断字符串是否合法（语法）
void handleString(char ch)
{
    if (charIsLetter(ch) || charIsNumber(ch))
    {
        connect(charStr, ch);
        i1++;
        handleString(allCharacter[i1]); //处理下一个字符
    }
    else if (ch == ' ' || ch == '\n' || ch == '\0' || charIsOperator(ch) || charIsDelimiter(ch))
    {
        if (stringIsReservedWord(charStr)) //是保留字
        {
            allString[i2++] = charStr; //存储到allString中
            charStr[0] = '\0';
        }
        else //不是保留字
        {
            if (strLength(charStr) >= 8)
                printf("(标识符长度超长,%s,行号:%d)\n", charStr, lineNumber);
            else
                allString[i2++] = charStr; // printf("(标识符,%s)\n",str);
            charStr[0] = '\0';
        }
    }
    else
    {
        connect(charStr, ch);
        printf("(非法字符(串),%s,行号:%d)\n", charStr, lineNumber);
        i1++;
        charStr[0] = '\0';
    }
}
//处理数字串,检查数字是否错误
void handleNumber(char ch)
{
    if (charIsNumber(ch))
    {
        connect(charStr, allCharacter[i1]);
        i1++;
        handleNumber(allCharacter[i1]);
    }
    else if (!charIsLetter(ch))
    {
        if (strLength(charStr) >= 6)
            printf("(无符号整数越界,%s,行号:%d)\n", charStr, lineNumber);
        else
            allString[i2++] = charStr;
        charStr[0] = '\0';
    }
    else
    {
        while (allCharacter[i1] != ' ' && charIsDelimiter(allCharacter[i1]) != 1 && allCharacter[i1] != '\0')
        {
            connect(charStr, allCharacter[i1]);
            i1++;
        }
        printf("(非法字符(串),%s,行号:%d)\n", charStr, lineNumber);
        charStr[0] = '\0';
    }
}
//处理注释，查看注释是否符合语法
void handleNotes()
{
    if (!allString[i2].compare("\n"))
    {
        i2++;
        lineNumber++;
    }
    if (!allString[i2].compare("//"))
    {
        i2++;
        i2++;
        while (!allString[i2].compare("\n"))
        {
            i2++;
            lineNumber++;
        }
        handleNotes();
    }
    else if (!allString[i2].compare("/*"))
    {
        i2++;
        while (allString[i2].compare("*/"))
        {
            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
            else
            {
                i2++;
            }
        }
        i2++;
    }
    while (!allString[i2].compare("\n"))
    {
        i2++;
        lineNumber++;
    }
}

void handleConstant(int location);
void grammarInspect(int type);
void handerVariable(int location);
void sentence();
void condition();
void expression();
void term();
void factor();
void subroutine();

//检查字符串是否是都是数字
bool allStringIsNumber(string str)
{
    for (int j = 0; j < str.size(); j++)
    {
        int tmp = (int)str[j];
        if (tmp >= 48 && tmp <= 57)
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}

//语义错误检查
void semanticsInspect(int type)
{
    if (type == 0) // 检查常量标识符是否重复
    {

        for (int j = 0; j < i3; j++)
        {
            if (allString[i2].compare(table[j].variable) == 0) //重复定义变量导致语义错误
            {
                if (r1 != lineNumber)
                {
                    printf("(语义错误,行号:%d)\n", lineNumber);
                    semanticsError++;
                    r1 = lineNumber;
                }
            }
        }
    }
    else if (type == 1) //检查变量名是否重复
    {
        for (int j = 0; j < i3; j++)
        {
            if (allString[i2 - 1].compare(table[j].variable) == 0)
            {
                if (r1 != lineNumber)
                {
                    printf("(语义错误,行号:%d)\n", lineNumber);
                    semanticsError++;
                    r1 = lineNumber;
                }
            }
        }
    }
    else if (type == 2) //检查 if < a 的a是否存在,若存在则正确，若不存在，则说明他未定义就使用变量a，造成语义错误
    {

        int r2 = 0;
        for (int j = 0; j < i3; j++)
        {
            if (allString[i2 - 1].compare(table[j].variable) == 0)
            {
                r2++;
            }
        }
        if (r1 != lineNumber && r2 == 0)
        {
            printf("(语义错误,行号:%d)\n", lineNumber);
            semanticsError++;
            r1 = lineNumber;
        }
    }
    else if (type == 3) //检查call p 中的 p是否以及定义
    {
        int r2 = 0;
        for (int j = 0; j < pro; j++)
        {
            if (allString[i2 - 1].compare(programSegment[j]) == 0)
            {
                r2++;
            }
        }
        if (r1 != lineNumber && r2 == 0)
        {
            printf("(语义错误,行号:%d)\n", lineNumber);
            semanticsError++;
            r1 = lineNumber;
        }
    }
    else
    {
        int r2 = 0;
        for (int j = 0; j < pro; j++)
        {
            if (allString[i2 - 1].compare(programSegment[j]) == 0)
            {
                r2++;
            }
        }
        for (int j = 0; j < i3; j++)
        {
            if (allString[i2 - 1].compare(table[j].variable) == 0)
            {
                r2++;
            }
        }
        if (r1 != lineNumber && r2 == 0)
        {
            printf("(语义错误,行号:%d)\n", lineNumber);
            semanticsError++;
            r1 = lineNumber;
        }
    }
}

//检查数字是否正确，字符串转为double,加入到符号表
void numberInspect(int location)
{
    int t2 = 0;
    for (t2 = 0; t2 < allString[i2].length(); t2++)
        charStr[t2] = allString[i2][t2];
    charStr[t2] = '\0';
    t2 = 0;
    double number = 0;
    double t3 = allString[i2].length() - 1; //数字长度
    while (charStr[t2] != '\0')
    {

        number = (charStr[t2] - 48) * pow(10, t3) + number;
        t2++;
        t3--;
    }
    table[location].value = number;
    i2++;
}

//处理常量的标识符
void handerIdentifier(int location)
{
    semanticsInspect(0);                      //检查标识符的名字
    table[location].variable = allString[i2]; // 没问题即可添加到符号表
    i2++;
}

//处理变量名
int handerVariableName()
{
    int t2 = 0;
    for (t2 = 0; t2 < allString[i2].length(); t2++)
        charStr[t2] = allString[i2][t2];
    charStr[t2] = '\0';
    if (!stringIsReservedWord(charStr) && charStr[0] != '.')
    {
        i2++;
    }
    else
        grammarInspect(1);
}

//检查语法错误
void grammarInspect(int type)
{
    if (type == 1) //定义了保留字或无效名.
    {
        if (r1 != lineNumber)
        {
            r1 = lineNumber;
            printf("(语法错误,行号:%d)\n", lineNumber, grammarError);
            grammarError++;
        }
    }
    else
    {
        while (allString[i2].compare("\n") != 0)
            i2++;
        if (r1 != lineNumber)
        {
            r1 = lineNumber;
            printf("(语法错误,行号:%d)\n", lineNumber, grammarError);
            grammarError++;
        }
    }
}

//添加四元式
void addQuaternion(string qop, string qarg1, string qarg2, string qresult, int location)
{
    if (allquaternion[location].op.compare("j") != 0)
        allquaternion[location].op = qop;
    else
        allquaternion[location].op = "j" + qop;
    if (qarg1.compare("") != 0)
        allquaternion[location].arg1 = qarg1;
    if (qarg2.compare("") != 0)
        allquaternion[location].arg2 = qarg2;
    if (qresult.compare("") != 0)
        allquaternion[location].result = qresult;
}
//主程序
void program()
{
    addQuaternion("syss", "_", "_", "_", dd++);
    subroutine();
    if (!allString[i2].compare("."))
    {
        addQuaternion("syse", "_", "_", "_", dd++);
    }
    else
    {
        grammarInspect(0);
        return;
    }
}

//子程序
void subroutine()
{
    if (!allString[i2].compare("const"))
    {
        i2++;
        handleConstant(i3++);

        while (!allString[i2].compare(","))
        {
            i2++;
            handleConstant(i3++);
        }
        if (!allString[i2].compare(";"))
        {
            i2++;

            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
                handleNotes();
            }
            subroutine();
        }
        else
        {
            grammarInspect(0);
            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
                handleNotes();
            }
            subroutine();
        }
        if (!allString[i2].compare("\n"))
        {
            i2++;
            lineNumber++;
            handleNotes();
        }
    }
    else if (!allString[i2].compare("var"))
    {
        i2++;
        handerVariable(i3++);
        while (!allString[i2].compare(","))
        {
            i2++;
            handerVariable(i3++);
        }
        if (!allString[i2].compare(";"))
        {
            i2++;
            while (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
            handleNotes();
            subroutine();
        }
        else
        {
            grammarInspect(0);
            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
                handleNotes();
            }
            subroutine();
        }
    }
    else if (!allString[i2].compare("procedure"))
    {

        i2++;
        handerVariableName();
        addQuaternion("procedure", allString[i2 - 1], "_", "_", dd++);
        semanticsInspect(1);
        programSegment[pro++] = allString[i2 - 1];
        if (!allString[i2].compare(";"))
        {
            i2++;
            subroutine();
            if (!allString[i2].compare(";"))
            {
                handleNotes();
                while (!allString[i2].compare("\n"))
                {
                    i2++;
                    lineNumber++;
                    handleNotes();
                }
                i2++;
            }
            else
            {
                grammarInspect(1);
            }
        }
        else
        {
            grammarInspect(0);
            subroutine();
        }
        while (!allString[i2].compare("procedure"))
        {
            i2++;
            handerVariableName();
            programSegment[pro++] = allString[i2 - 1];
            if (!allString[i2].compare(";"))
            {
                i2++;
                subroutine();
                if (!allString[i2].compare(";"))
                    i2++;
                else
                    grammarInspect(1);
            }
            else
            {
                grammarInspect(0);
                subroutine();
            }
        }
    }
    if (!allString[i2].compare("\n"))
    {
        i2++;
        lineNumber++;
    }
    sentence(); //语句分析
}

//常量定义
void handleConstant(int location)
{
    table[location].type = "const";
    handerIdentifier(location);
    addQuaternion("const", allString[i2 - 1], "_", "_", dd++); //生成四元式
    if (!allString[i2].compare("="))
    {
        i2++;
        numberInspect(location);
        addQuaternion("=", allString[i2 - 1], "_", allString[i2 - 3], dd++);
    }
    else
    {
        grammarInspect(0);
    }
}

//变量定义
void handerVariable(int location)
{
    table[location].type = "var";
    handerVariableName();
    addQuaternion("var", allString[i2 - 1], "_", "_", dd++);
    semanticsInspect(1);
    table[location].variable = allString[i2 - 1];
}

//语句分析
void sentence()
{
    if (!allString[i2].compare("if"))
    {
        i2++;
        addQuaternion("j", "", "", "", dd++);
        condition(); //条件分析

        if (!allString[i2].compare("then"))
        {
            i2++;
            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
            sentence();
        }
        else
        {
            grammarInspect(0);
            sentence();
        }
        if (!allString[i2].compare("\n"))
        {
            i2++;
            lineNumber++;
        }
    }
    else if (!allString[i2].compare("while"))
    {
        i2++;
        addQuaternion("j", "", "", "", dd++);
        condition();
        if (!allString[i2].compare("do"))
        {
            i2++;
            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
            sentence();
        }
        else
        {
            grammarInspect(1);
            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
            sentence();
        }
    }
    else if (!allString[i2].compare("call"))
    {
        i2++;
        addQuaternion("call", allString[i2], "_", "_", dd++);
        handerVariableName();
        semanticsInspect(3);
    }
    else if (!allString[i2].compare("read"))
    {
        i2++;

        if (!allString[i2].compare("("))
        {
            i2++;
            addQuaternion("read", allString[i2], "_", "_", dd++);
            handerVariableName();
            semanticsInspect(2);
            while (!allString[i2].compare(","))
            {
                i2++;
                handerVariableName();
                semanticsInspect(2);
            }
            if (!allString[i2].compare(")"))
                i2++;
            else
                grammarInspect(1);
        }
        else if (!allString[i2].compare("\n"))
        {
            i2++;
            lineNumber++;
        }
        else
            grammarInspect(0);
    }
    else if (!allString[i2].compare("write"))
    {
        i2++;

        if (!allString[i2].compare("("))
        {
            i2++;
            expression();
            addQuaternion(allString[i2 - 2], allString[i2 - 3], allString[i2 - 1], "T1", dd++);
            semanticsInspect(2);
            while (!allString[i2].compare(","))
            {
                i2++;
                expression();
                semanticsInspect(2);
            }
            if (!allString[i2].compare(")"))
            {
                i2++;
                addQuaternion("write", "T1", "_", "_", dd++);
            }
            else if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
            else
                grammarInspect(0);
        }
        else
        {
            grammarInspect(1);
            expression();
            while (!allString[i2].compare(","))
            {
                i2++;
                expression();
            }
            if (!allString[i2].compare(")"))
                i2++;
            else if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
            else
                grammarInspect(0);
        }
    }
    else if (!allString[i2].compare("begin"))
    {
        i2++;
        if (!allString[i2].compare("\n"))
        {
            i2++;
            lineNumber++;
        }
        sentence();
        if (!allString[i2].compare(";"))
        {
            i2++;
            handleNotes();
            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
            sentence();
        }
        else
        {
            grammarInspect(0);
            sentence();
        }
        while (!allString[i2].compare(";"))
        {
            i2++;
            handleNotes();
            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
            sentence();
        }
        if (!allString[i2].compare("end"))
        {
            i2++;
            if (!allString[i2].compare("\n"))
            {
                i2++;
                lineNumber++;
            }
        }
        else
        {
            grammarInspect(0);
            return;
        }
        if (!allString[i2].compare("\n"))
        {
            i2++;
            lineNumber++;
        }
    }
    else if (!allString[i2].compare("\n"))
    {
        i2++;
        lineNumber++;
    }
    else if (!allString[i2].compare("//") || !allString[i2].compare("/*"))
    {
        handleNotes();
        subroutine();
    }
    else
    {
        int tt = 0;
        for (tt = 0; tt < allString[i2].length(); tt++)
            charStr[tt] = allString[i2][tt];
        charStr[tt] = '\0';
        if (!stringIsReservedWord(charStr) && charStr[0] != '.')
        {
            handerVariableName();
            if (!allString[i2].compare(":="))
                i2++;
            expression();
            addQuaternion(allString[i2 - 2], allString[i2 - 3], allString[i2 - 1], allString[i2 - 5], dd++);
            addQuaternion("ret", "_", "_", "_", dd++);
        }
    }
}

//条件分析
void condition()
{
    expression();

    if (!allString[i2].compare("=") || !allString[i2].compare("#") || !allString[i2].compare("<") || !allString[i2].compare("<=") || !allString[i2].compare(">") || !allString[i2].compare(">="))
    {
        if (!allString[i2].compare("<="))
            addQuaternion(allString[i2], allString[i2 - 1], allString[i2 + 1], "$8", dd - 1);
        else
        {
            addQuaternion(allString[i2], allString[i2 - 1], allString[i2 + 1], "$13", dd - 1);
            addQuaternion("j=", allString[i2 - 1], allString[i2 + 1], "$17", dd++);
        }
        i2++;
        expression();
    }
    else if (!allString[i2].compare("odd"))
    {
        i2++;
        expression();
    }
    else
        grammarInspect(0);
}
//表达式
void expression()
{
    term();
    if (!allString[i2].compare("+") || !allString[i2].compare("-"))
    {
        i2++;
        term();
        while (!allString[i2].compare("+") || !allString[i2].compare("-"))
        {
            i2++;
            term();
        }
    }
}
//项
void term()
{
    factor();
    while (!allString[i2].compare("*") || !allString[i2].compare("/"))
    {
        i2++;
        factor();
    }
}

void factor() //因子
{
    if (!allString[i2].compare("("))
    {
        i2++;
        expression();
        if (!allString[i2].compare(")"))
        {
            i2++;
        }
        else
            grammarInspect(0);
    }
    else if (!allString[i2].compare("\n"))
    {
        i2++;
        lineNumber++;
    }
    else
    {
        i2++;
        if (!allStringIsNumber(allString[i2 - 1]))
            semanticsInspect(2);
    }
}

//用gets（）来输入，自动舍弃\n，输入一个字符串，结尾使用\0.

int main()
{

    allCharacter[0] = '\0';
    scanf("%[^\.]", allCharacter);
    allCharacter[strLength(allCharacter)] = '.';

    while (allCharacter[i1] != '\0') //每一行的每个字符    //词法分析
    {

        if (allCharacter[i1] == '\0')
            break;
        if (allCharacter[i1] == 9)
        {
            i1++;
            continue;
        }
        if (allCharacter[i1] == ' ')
        {
            i1++;
            continue;
        }
        if (allCharacter[i1] == '\n')
        {
            allString[i2++] = "\n";
            i1++;
            lineNumber++;
        }
        else if (charIsLetter(allCharacter[i1]))
        {
            handleString(allCharacter[i1]);
            continue;
        }
        else if (charIsNumber(allCharacter[i1]))
        {
            handleNumber(allCharacter[i1]);
            continue;
        }
        else if (allCharacter[i1] == '/')
        {
            if (allCharacter[i1 + 1] == '/')
            {
                allString[i2++] = "//";
                i1 = i1 + 2;
                charStr[0] = '\0';
                while (allCharacter[i1] != '\n')
                    connect(charStr, allCharacter[i1++]);
                allString[i2++] = charStr;
                allString[i2++] = "\n";
                i1++;
                continue;
            }
            else if (allCharacter[i1 + 1] == '*')
            {
                allString[i2++] = "/*";
                i1 = i1 + 2;
                while (!(allCharacter[i1] == '*' && allCharacter[i1 + 1] == '/'))
                {
                    if (allCharacter[i1] == '\n')
                    {
                        allString[i2++] = "\n";
                        i1++;
                        continue;
                    }
                    charStr[0] = '\0';
                    while (allCharacter[i1] != '\n')
                        connect(charStr, allCharacter[i1++]);
                    allString[i2++] = charStr;
                    allString[i2++] = "\n";
                    i1++;
                }
                allString[i2++] = "*/";
                i1 = i1 + 2;
                charStr[0] = '\0';
                continue;
            }
            else
            {
                while (allCharacter[i1] != ' ' && charIsDelimiter(allCharacter[i1]) != 1 && allCharacter[i1] != '\0')
                {
                    connect(charStr, allCharacter[i1]);
                    i1++;
                }
                printf("(非法字符(串),%s,行号:%d)\n", charStr, lineNumber);
                charStr[0] = '\0';
                break;
            }
        }
        else if (allCharacter[i1] == '<' || allCharacter[i1] == '>')
        {
            i1++;
            if (allCharacter[i1] == '=')
            {
                if (allCharacter[i1 - 1] == '<')
                    allString[i2] = "<=";

                else
                    allString[i2] = ">=";
                i2++;
                i1++;
                continue;
            }
            else
            {
                if (allCharacter[i1 - 1] == '<')
                    allString[i2] = "<";
                else
                    allString[i2] = ">";
                i2++;

                continue;
            }
        }
        else if (allCharacter[i1] == ':')
        {
            i1++;
            if (allCharacter[i1] == '=')
            {
                allString[i2++] = ":=";

                i1++;
                continue;
            }
            else
            {
                printf(":后面不是=号\n");
                continue;
            }
        }
        else if (charIsOperator(allCharacter[i1]))
        {
            allString[i2++] = allCharacter[i1];
            i1++;
            continue;
        }
        else if (charIsDelimiter(allCharacter[i1]))
        {
            allString[i2++] = allCharacter[i1];
            i1++;
            continue;
        }
        else
        {
            printf("(非法字符(串),%c,行号:%d)\n", allCharacter[i1], lineNumber);
            i1++;
        }
    }

    //语法分析
    i1 = 0;
    i2 = 0;
    lineNumber = 1;

    program();

    //输出所有符号表
    if (semanticsError == 0)
    {
        printf("语义正确\n中间代码:\n");
        //......此处应填上中间代码
        for (int p = 0; p < dd; p++)
        {
            printf("(%d)(%s,%s,%s,%s)\n", p + 1, allquaternion[p].op.c_str(), allquaternion[p].arg1.c_str(), allquaternion[p].arg2.c_str(), allquaternion[p].result.c_str());
        }
        printf("符号表:\n");
        for (int j = 0; j < i3; j++)
        {
            printf("%s %s %.0f\n", table[j].type.c_str(), table[j].variable.c_str(), table[j].value);
        }
        for (int j = 0; j < pro; j++)
        {
            printf("procedure %s\n", programSegment[j].c_str());
        }
    }

    return 0;
}
