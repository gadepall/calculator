#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>
#include <math.h>

#define MAX_SIZE 100

int mode = 0;

typedef struct {
  char data[MAX_SIZE];
  int top;
} Stack;

void push(Stack *s, char val) {
  if (s->top < MAX_SIZE - 1) {
    s->data[++(s->top)] = val;
  }
}

char pop(Stack *s) {
  return (s->top >= 0) ? s->data[(s->top)--] : '\0';
}

char peek(Stack *s) {
  return (s->top >= 0) ? s->data[s->top] : '\0';
}

int precedence(char op) {
  switch (op) {
    case '+': case '-': return 1;
    case '*': case '/': return 2;
    case '^': return 3;
    default: return 0;
  }
}

int isFunction(char c) {
  return (c == 's' || c == 'c' || c == 't' || c == 'e' || c == 'l' || c == 'z' || c == 'y' || c == 'x' || c == 'q');
}

// Convert infix to postfix
void shunting_yard(const char *expr, char *output) {
  Stack operators = {.top = -1};
  int j = 0;
  for (int i = 0; expr[i] != '\0'; i++) {
    if (isdigit(expr[i]) || expr[i] == '.') {
      while (isdigit(expr[i]) || expr[i] == '.') {
        output[j++] = expr[i++];
      }
      output[j++] = ' ';
      i--;
    } else if (isFunction(expr[i])) {
      push(&operators, expr[i]);
    } else if (expr[i] == '(') {
      push(&operators, expr[i]);
      unmatched_brackets++;
    } else if (expr[i] == ')') {
      while (peek(&operators) != '(') {
        output[j++] = pop(&operators);
        output[j++] = ' ';
      }
      unmatched_brackets--;
      pop(&operators); // Remove '('
      if (isFunction(peek(&operators))) {
        output[j++] = pop(&operators);
        output[j++] = ' ';
      }
    } else if (strchr("+-*/^!", expr[i])) {
      while (operators.top != -1 && precedence(peek(&operators)) >= precedence(expr[i])) {
        output[j++] = pop(&operators);
        output[j++] = ' ';
      }
      push(&operators, expr[i]);
    }
  }
  while (operators.top != -1) {
    output[j++] = pop(&operators);
    output[j++] = ' ';
  }
  output[j] = '\0';
}

// Evaluate postfix expression
double evaluate_rpn(const char *postfix) {
  double stack[MAX_SIZE];
  int top = -1;
  char token[20];
  int i = 0;
  while (*postfix) {
    if (isdigit(*postfix) || *postfix == '.') {
      sscanf(postfix, "%s", token);
      stack[++top] = atof(token);
      while (*postfix && *postfix != ' ') postfix++;
    } else if (strchr("!", *postfix)){
      double a = stack[--top];
//      stack[++top] = factorial(a);
    } else if (strchr("+-*/^", *postfix)) {
      double b = stack[top--];
      double a = stack[top--];
      switch (*postfix) {
        case '+': stack[++top] = a + b; break;
        case '-': stack[++top] = a - b; break;
        case '*': stack[++top] = a * b; break;
        case '/': stack[++top] = a / b; break;
        case '^': stack[++top] = pow(a, b); break;
      }
    } else if (isFunction(*postfix)) {
      double a = stack[top--];
      switch (*postfix) {
        case 's': stack[++top] = sin(a); break;
        case 'c': stack[++top] = cos(a); break;
        case 't': stack[++top] = tan(a); break;
        case 'e': stack[++top] = exp(a); break;
        case 'l': stack[++top] = ln(a); break;
        case 'z': stack[++top] = arcsin(a); break;
        case 'y': stack[++top] = arccos(a); break;
        case 'x': stack[++top] = arctan(a); break;
      }
    }
    postfix++;
  }
  return stack[top];
}
factorial : simple for loop
c
double factorial(double x){
  double result = 1;
  while (x > 1){
    result *= x;
    x -= 1;
  }
  return result;
