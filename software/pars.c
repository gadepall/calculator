#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 256

extern double m_abs(double x);
extern double m_sqrt(double n);
extern double m_sin(double x);
extern double m_cos(double x);
extern double m_tan(double x);
extern double m_ln(double x);
extern double m_pow(double x, double w);
extern double m_fact(double n);

void error(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
}

typedef struct { char data[MAX][32]; int top; } S_Str;
typedef struct { double data[MAX]; int top; } S_Dbl;

void push_s(S_Str *s, char *v) {
    if (s->top >= MAX - 1) { error("Operator Stack Overflow"); return; }
    strcpy(s->data[++s->top], v);
}

char* pop_s(S_Str *s) {
    if (s->top < 0) { error("Operator Stack Underflow"); return ""; }
    return s->data[s->top--];
}

char* peek_s(S_Str *s) {
    if (s->top < 0) return ""; 
    return s->data[s->top];
}

void push_d(S_Dbl *s, double v) {
    if (s->top >= MAX - 1) { error("Value Stack Overflow"); return; }
    s->data[++s->top] = v;
}

double pop_d(S_Dbl *s) {
    if (s->top < 0) { error("Value Stack Underflow"); return 0.0; }
    return s->data[s->top--];
}

int prec(char *op) {
    if (!strcmp(op, "!")) return 5; // Factorial
    if (!strcmp(op, "^")) return 4;
    if (!strcmp(op, "*") || !strcmp(op, "/")) return 3;
    if (!strcmp(op, "+") || !strcmp(op, "-")) return 2;
    return 0;
}

int is_func(char *t) {
    return !strcmp(t,"sin") || !strcmp(t,"cos") || !strcmp(t,"tan") || 
           !strcmp(t,"ln")  || !strcmp(t,"sqrt") || !strcmp(t,"fact");
}

int is_op(char *t) {
    return !strcmp(t,"+") || !strcmp(t,"-") || !strcmp(t,"*") || 
           !strcmp(t,"/") || !strcmp(t,"^") || !strcmp(t,"!");
}

void tokenize_buffer(const char *input, char *output) {
    int j = 0;
    for (int i = 0; input[i] != '\0' && j < MAX*10; i++) {
        char c = input[i];
        if (strchr("()+-*/^!", c)) {
            output[j++] = ' ';
            output[j++] = c;
            output[j++] = ' ';
        } else {
            output[j++] = c;
        }
    }
    output[j] = '\0';
}

void shunt(char *input_expr, char output[MAX][10], int *out_len) {
    S_Str ops = { .top = -1 };
    char processed_expr[MAX * 10]; // Buffer for space-padded string
    tokenize_buffer(input_expr, processed_expr);

    char *token = strtok(processed_expr, " \t\n"); 
    *out_len = 0;

    while (token) {
        // Number or Variable
        if (isdigit(token[0]) || (token[0]=='-' && isdigit(token[1])) || !strcmp(token, "x") || !strcmp(token, "t")) {
            if (*out_len >= MAX) { error("Output Queue Overflow"); return; }
            strcpy(output[(*out_len)++], token);
        } 
        // Function or Left Paren
        else if (is_func(token) || !strcmp(token, "(")) {
            push_s(&ops, token);
        } 
        // Right Paren
        else if (!strcmp(token, ")")) {
            while (ops.top != -1 && strcmp(peek_s(&ops), "(")) {
                strcpy(output[(*out_len)++], pop_s(&ops));
            }
            if (ops.top != -1) pop_s(&ops); // Pop '('
            if (ops.top != -1 && is_func(peek_s(&ops))) 
                strcpy(output[(*out_len)++], pop_s(&ops)); 
        } 
        // Operator
        else { 
            while (ops.top != -1 && prec(peek_s(&ops)) >= prec(token))
                strcpy(output[(*out_len)++], pop_s(&ops));
            push_s(&ops, token);
        }
        token = strtok(NULL, " \t\n");
    }
    while (ops.top != -1) {
        if (!strcmp(peek_s(&ops), "(")) { error("Mismatched Parentheses"); return; }
        strcpy(output[(*out_len)++], pop_s(&ops));
    }
}

double eval_rpn(char rpn[MAX][10], int n, double var_val) {
    S_Dbl vals = { .top = -1 };

    for (int i = 0; i < n; i++) {
        char *t = rpn[i];

        if (isdigit(t[0]) || (t[0] == '-' && isdigit(t[1]))) {
            push_d(&vals, atof(t));
        } else if (!strcmp(t, "x") || !strcmp(t, "t")) {
            push_d(&vals, var_val);
        } else if (is_op(t)) {
            if (!strcmp(t, "!")) {
                double a = pop_d(&vals);
                push_d(&vals, m_fact(a));
            } 

            else {
                double b = pop_d(&vals);
                double a = pop_d(&vals);
                if (!strcmp(t, "+")) push_d(&vals, a + b);
                else if (!strcmp(t, "-")) push_d(&vals, a - b);
                else if (!strcmp(t, "*")) push_d(&vals, a * b);
                else if (!strcmp(t, "/")) push_d(&vals, b != 0 ? a / b : 0); // Safety check
                else if (!strcmp(t, "^")) push_d(&vals, m_pow(a, b));
            }
        } else if (is_func(t)) {
            double a = pop_d(&vals);
            if (!strcmp(t, "sin")) push_d(&vals, m_sin(a));
            else if (!strcmp(t, "cos")) push_d(&vals, m_cos(a));
            else if (!strcmp(t, "tan")) push_d(&vals, m_tan(a));
            lse if (!strcmp(t, "ln")) push_d(&vals, m_ln(a));
            else if (!strcmp(t, "sqrt")) push_d(&vals, m_sqrt(a));
            else if (!strcmp(t, "fact")) push_d(&vals, m_fact(a));
        }
    }
    return pop_d(&vals);
}

// If step <= 0 or start==end, we evaluate once. Otherwise we generate a CSV list.
void compute(const char* input, double start, double end, double step, char* output) {
    char rpn[MAX][10];
    int n = 0;
    
    // 1. Parse
    shunt((char*)input, rpn, &n);
    
    output[0] = '\0'; // Initialize buffer
    
    // Mode A: Single scalar (Calculator)
    if (step <= 0 || (m_abs(end - start) < 1e-9)) {
        double res = eval_rpn(rpn, n, start);
        sprintf(output, "%.6f", res);
    } 
    // Mode B: Function Mapping (Plotter)
    else {
        char line[64];
        double t = start;
        int safety = 0; 
        while (t <= end + 1e-9 && safety++ < 2000) {
            double res = eval_rpn(rpn, n, t);
            sprintf(line, "%.4f,%.4f\n", t, res);
            strcat(output, line);
            t += step;
        }
    }
}
