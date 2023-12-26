#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>//测试字符是否属于特定的字符类别

#define MAX_STACK_SIZE 100

typedef struct {
    char data[MAX_STACK_SIZE];
    int top;
} Stack;//表示栈的结构体

void initStack(Stack *stack) {
    stack->top = -1;
}//这个函数接受一个指向Stack结构体的指针作为参数，通过指针访问top成员变量，并将其设置为-1。这样，当栈被创建时，栈顶指针的初始值就是-1，表示栈为空。

void push(Stack *stack, char c) {
    if (stack->top < MAX_STACK_SIZE - 1) {
        stack->data[++stack->top] = c;
    }
}/*接受一个指向Stack结构体的指针和一个字符作为参数。
检查栈是否已满，如果栈未满，则将栈顶指针加1，并将元素c存储到栈顶位置。元素就被压入栈中。如果栈已满，则不进行任何操作。*/ 

char pop(Stack *stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top--];
    }
    return '\0';
}/*接受一个指向Stack结构体的指针作为参数。它会检查栈是否为空，栈顶指针是否大于等于0。
如果栈非空，则返回栈顶元素并将栈顶指针减1，表示栈顶元素已被移除。这样，元素就从栈中弹出并被返回。如果栈为空，则返回空字符('\0')表示栈为空，没有元素可弹出。*/

char peek(Stack *stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top];
    }
    return '\0';
}/*这个函数接受一个指向Stack结构体的指针作为参数。
它会检查栈是否为空，即栈顶指针是否大于等于0。如果栈非空，则返回栈顶元素，但不修改栈的状态。查看栈顶元素的值而不影响栈的结构。如果栈为空，则返回空字符('\0')表示栈为空。*/

void displayIndentedJSON(FILE *file, int compressed);
void displayIndentedJSON_file(FILE *file, int compressed, FILE *outputFile);
int checkJSONSyntax(FILE *file);

int main() {
    char filename[100];
    char outputFilename[100];
    int choice;

    printf("请输入JSON文件名称: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("无法打开文件.\n");
        return 1;
    }

    printf("1.显示到屏幕   2.压缩到文件  (请输入1或2)\n");
    scanf("%d", &choice);

    int syntaxCheckResult = checkJSONSyntax(file);

    if (syntaxCheckResult == 0) {
        printf("JSON格式正确\n");
        rewind(file);
        if(choice == 1){
            displayIndentedJSON(file, 0);
        }else{
            printf("Enter the path for the output file: ");
            scanf("%s", outputFilename);
            FILE *outputFile = fopen(outputFilename, "w");
            if (outputFile == NULL) {
                printf("无法打开文件\n");
                fclose(file);
                return 1;
            }
            displayIndentedJSON_file(file, 0, outputFile);
        }
    } else {
        int line = 1;
        int col = 1;
        fseek(file, 0, SEEK_SET);
        while (--syntaxCheckResult > 0) {
            char ch = fgetc(file);
            if (ch == '\n') {
                line++;
                col = 1;
            } else {
                col++;
            }
        }
        printf("JSON格式错误在 第%d行, 第%d列.\n", line, col);
    }

    fclose(file);

    return 0;
}

int checkJSONSyntax(FILE *file) {
    char c;
    Stack stack;
    initStack(&stack);
    int position = 0;

    while ((c = fgetc(file)) != EOF) {
        position++;  // 记录字符位置

        if (c == '{' || c == '[') {  // 如果字符是 '{' 或者 '['
            push(&stack, c);  // 将字符压入栈中
        } else if (c == '}' || c == ']') {  // 如果字符是 '}' 或者 ']'
            if (stack.top >= 0 && ((c == '}' && peek(&stack) == '{') || (c == ']' && peek(&stack) == '['))) {
                pop(&stack);  // 弹出栈顶元素
            } else {
                return position;  // 返回错误位置
            }
        }
    }

    if (stack.top != -1) {  // 如果栈不为空
        return position;  // 返回错误位置
    }

    return 0;  // 返回0表示语法正确
}

void displayIndentedJSON(FILE *file, int compressed) {
    char c;
    Stack stack;
    initStack(&stack);
    int indentLevel = 0;

    while ((c = fgetc(file)) != EOF) {
        if (c == '{' || c == '[') {  // 如果字符是 '{' 或者 '['
            if (!compressed) {  // 如果不是压缩模式
                printf("%c\n", c);  // 打印字符并换行
                indentLevel++;  // 缩进级别加一
                for (int i = 0; i < indentLevel; i++) {
                    printf("    ");  // 打印缩进空格
                }
            }
        } else if (c == '}' || c == ']') {  // 如果字符是 '}' 或者 ']'
            indentLevel--;  // 缩进级别减一
            if (!compressed) {  // 如果不是压缩模式
                printf("\n");
                for (int i = 0; i < indentLevel; i++) {
                    printf("    ");  // 打印缩进空格
                }
                printf("%c", c);  // 打印字符
            }
        } else if (c == ',') {  // 如果字符是 ','
            if (!compressed) {  // 如果不是压缩模式
                printf(",\n");
                for (int i = 0; i < indentLevel; i++) {
                    printf("    ");  // 打印缩进空格
                }
            }
        } else if (!isspace(c)) {  // 如果字符不是空白字符
            printf("%c", c);  // 打印字符
        }
    }

    printf("\n");
}

void displayIndentedJSON_file(FILE *file, int compressed, FILE *outputFile) {
    char c;
    Stack stack;
    initStack(&stack);
    int indentLevel = 0;

    while ((c = fgetc(file)) != EOF) {
        if (c == '{' || c == '[') {  // 如果字符是 '{' 或者 '['
            if (!compressed) {  // 如果不是压缩模式
                fprintf(outputFile, "%c\n", c);  // 打印字符并换行
                indentLevel++;  // 缩进级别加一
                for (int i = 0; i < indentLevel; i++) {
                    fprintf(outputFile, "    ");  // 打印缩进空格
                }
            }
        } else if (c == '}' || c == ']') {  // 如果字符是 '}' 或者 ']'
            indentLevel--;  // 缩进级别减一
            if (!compressed) {  // 如果不是压缩模式
                fprintf(outputFile, "\n");
                for (int i = 0; i < indentLevel; i++) {
                    fprintf(outputFile, "    ");  // 打印缩进空格
                }
                fprintf(outputFile, "%c", c);  // 打印字符
            }
        } else if (c == ',') {  // 如果字符是 ','
            if (!compressed) {  // 如果不是压缩模式
                fprintf(outputFile, ",\n");
                for (int i = 0; i < indentLevel; i++) {
                    fprintf(outputFile, "    ");  // 打印缩进空格
                }
            }
        } else if (!isspace(c)) {  // 如果字符不是空白字符
            fprintf(outputFile, "%c", c);  // 打印字符
        }
    }

    fprintf(outputFile, "\n");  // 打印换行符，表示显示结束
}
