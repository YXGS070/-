#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>//�����ַ��Ƿ������ض����ַ����

#define MAX_STACK_SIZE 100

typedef struct {
    char data[MAX_STACK_SIZE];
    int top;
} Stack;//��ʾջ�Ľṹ��

void initStack(Stack *stack) {
    stack->top = -1;
}//�����������һ��ָ��Stack�ṹ���ָ����Ϊ������ͨ��ָ�����top��Ա����������������Ϊ-1����������ջ������ʱ��ջ��ָ��ĳ�ʼֵ����-1����ʾջΪ�ա�

void push(Stack *stack, char c) {
    if (stack->top < MAX_STACK_SIZE - 1) {
        stack->data[++stack->top] = c;
    }
}/*����һ��ָ��Stack�ṹ���ָ���һ���ַ���Ϊ������
���ջ�Ƿ����������ջδ������ջ��ָ���1������Ԫ��c�洢��ջ��λ�á�Ԫ�ؾͱ�ѹ��ջ�С����ջ�������򲻽����κβ�����*/ 

char pop(Stack *stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top--];
    }
    return '\0';
}/*����һ��ָ��Stack�ṹ���ָ����Ϊ������������ջ�Ƿ�Ϊ�գ�ջ��ָ���Ƿ���ڵ���0��
���ջ�ǿգ��򷵻�ջ��Ԫ�ز���ջ��ָ���1����ʾջ��Ԫ���ѱ��Ƴ���������Ԫ�ؾʹ�ջ�е����������ء����ջΪ�գ��򷵻ؿ��ַ�('\0')��ʾջΪ�գ�û��Ԫ�ؿɵ�����*/

char peek(Stack *stack) {
    if (stack->top >= 0) {
        return stack->data[stack->top];
    }
    return '\0';
}/*�����������һ��ָ��Stack�ṹ���ָ����Ϊ������
������ջ�Ƿ�Ϊ�գ���ջ��ָ���Ƿ���ڵ���0�����ջ�ǿգ��򷵻�ջ��Ԫ�أ������޸�ջ��״̬���鿴ջ��Ԫ�ص�ֵ����Ӱ��ջ�Ľṹ�����ջΪ�գ��򷵻ؿ��ַ�('\0')��ʾջΪ�ա�*/

void displayIndentedJSON(FILE *file, int compressed);
void displayIndentedJSON_file(FILE *file, int compressed, FILE *outputFile);
int checkJSONSyntax(FILE *file);

int main() {
    char filename[100];
    char outputFilename[100];
    int choice;

    printf("������JSON�ļ�����: ");
    scanf("%s", filename);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("�޷����ļ�.\n");
        return 1;
    }

    printf("1.��ʾ����Ļ   2.ѹ�����ļ�  (������1��2)\n");
    scanf("%d", &choice);

    int syntaxCheckResult = checkJSONSyntax(file);

    if (syntaxCheckResult == 0) {
        printf("JSON��ʽ��ȷ\n");
        rewind(file);
        if(choice == 1){
            displayIndentedJSON(file, 0);
        }else{
            printf("Enter the path for the output file: ");
            scanf("%s", outputFilename);
            FILE *outputFile = fopen(outputFilename, "w");
            if (outputFile == NULL) {
                printf("�޷����ļ�\n");
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
        printf("JSON��ʽ������ ��%d��, ��%d��.\n", line, col);
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
        position++;  // ��¼�ַ�λ��

        if (c == '{' || c == '[') {  // ����ַ��� '{' ���� '['
            push(&stack, c);  // ���ַ�ѹ��ջ��
        } else if (c == '}' || c == ']') {  // ����ַ��� '}' ���� ']'
            if (stack.top >= 0 && ((c == '}' && peek(&stack) == '{') || (c == ']' && peek(&stack) == '['))) {
                pop(&stack);  // ����ջ��Ԫ��
            } else {
                return position;  // ���ش���λ��
            }
        }
    }

    if (stack.top != -1) {  // ���ջ��Ϊ��
        return position;  // ���ش���λ��
    }

    return 0;  // ����0��ʾ�﷨��ȷ
}

void displayIndentedJSON(FILE *file, int compressed) {
    char c;
    Stack stack;
    initStack(&stack);
    int indentLevel = 0;

    while ((c = fgetc(file)) != EOF) {
        if (c == '{' || c == '[') {  // ����ַ��� '{' ���� '['
            if (!compressed) {  // �������ѹ��ģʽ
                printf("%c\n", c);  // ��ӡ�ַ�������
                indentLevel++;  // ���������һ
                for (int i = 0; i < indentLevel; i++) {
                    printf("    ");  // ��ӡ�����ո�
                }
            }
        } else if (c == '}' || c == ']') {  // ����ַ��� '}' ���� ']'
            indentLevel--;  // ���������һ
            if (!compressed) {  // �������ѹ��ģʽ
                printf("\n");
                for (int i = 0; i < indentLevel; i++) {
                    printf("    ");  // ��ӡ�����ո�
                }
                printf("%c", c);  // ��ӡ�ַ�
            }
        } else if (c == ',') {  // ����ַ��� ','
            if (!compressed) {  // �������ѹ��ģʽ
                printf(",\n");
                for (int i = 0; i < indentLevel; i++) {
                    printf("    ");  // ��ӡ�����ո�
                }
            }
        } else if (!isspace(c)) {  // ����ַ����ǿհ��ַ�
            printf("%c", c);  // ��ӡ�ַ�
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
        if (c == '{' || c == '[') {  // ����ַ��� '{' ���� '['
            if (!compressed) {  // �������ѹ��ģʽ
                fprintf(outputFile, "%c\n", c);  // ��ӡ�ַ�������
                indentLevel++;  // ���������һ
                for (int i = 0; i < indentLevel; i++) {
                    fprintf(outputFile, "    ");  // ��ӡ�����ո�
                }
            }
        } else if (c == '}' || c == ']') {  // ����ַ��� '}' ���� ']'
            indentLevel--;  // ���������һ
            if (!compressed) {  // �������ѹ��ģʽ
                fprintf(outputFile, "\n");
                for (int i = 0; i < indentLevel; i++) {
                    fprintf(outputFile, "    ");  // ��ӡ�����ո�
                }
                fprintf(outputFile, "%c", c);  // ��ӡ�ַ�
            }
        } else if (c == ',') {  // ����ַ��� ','
            if (!compressed) {  // �������ѹ��ģʽ
                fprintf(outputFile, ",\n");
                for (int i = 0; i < indentLevel; i++) {
                    fprintf(outputFile, "    ");  // ��ӡ�����ո�
                }
            }
        } else if (!isspace(c)) {  // ����ַ����ǿհ��ַ�
            fprintf(outputFile, "%c", c);  // ��ӡ�ַ�
        }
    }

    fprintf(outputFile, "\n");  // ��ӡ���з�����ʾ��ʾ����
}
