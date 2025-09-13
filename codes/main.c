#define F_CPU 16000000UL  // 16 MHz clock
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <math.h>
#define MAX_SIZE 100
// LCD Pin Assignments
#define LCD_RS PD2
#define LCD_EN PD3
#define LCD_D4 PD4
#define LCD_D5 PD5
#define LCD_D6 PD6
#define LCD_D7 PD7

// Keypad Pin Assignments
#define ROW1 PB0
#define ROW2 PB1
#define ROW3 PB2
#define ROW4 PB3
#define ROW5 PC3
#define COL1 PB4
#define COL2 PB5
#define COL3 PC0
#define COL4 PC1
#define COL5 PC2
#define PI 3.14159265358979323846
#define H 0.01
#define EULER_CONST 2.718281828459045
uint8_t unmatched_brackets = 0;
double factorial(double x){
  double result = 1;
  while (x > 1){
    result *= x;
    x -= 1;
  }
  return result;
}

double fast_inv_sqrt(double x){
    x = (float) x;
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = x * 0.5F;
	y  = x;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );

	return (double) y;
}

double arctan(double x) {
    double x0 = 0.0, y = 0.0;
    int steps = x >= 0 ? (int) (x / H): (int) (-x / H); 
    double step_dir = (x >= 0) ? 1 : -1;
    
    for (int i = 0; i < steps; i++) {
        double k1 = H / (1 + x0*x0);
        double k2 = H / (1 + (x0 + H/2)*(x0 + H/2));
        double k3 = H / (1 + (x0 + H/2)*(x0 + H/2));
        double k4 = H / (1 + (x0 + H)*(x0 + H));
        
        y += step_dir * (k1 + 2*k2 + 2*k3 + k4) / 6;
        x0 += step_dir * H;
    }
    
    return y;
}
double arcsin(double x) {
    if (x < -1 || x > 1) return 0;

    double x0 = 0.0, y = 0.0;
    int steps = x >= 0 ? (int) (x / H): (int) (-x / H); 
    double step_dir = (x >= 0) ? 1 : -1;

    for (int i = 0; i < steps; i++) {
        double k1 = H * fast_inv_sqrt(1 - x0*x0);
        double k2 = H * fast_inv_sqrt(1 - (x0 + step_dir*H/2)*(x0 + step_dir*H/2));
        double k3 = H * fast_inv_sqrt(1 - (x0 + step_dir*H/2)*(x0 + step_dir*H/2));
        double k4 = H * fast_inv_sqrt(1 - (x0 + step_dir*H)*(x0 + step_dir*H));

        y += step_dir * (k1 + 2*k2 + 2*k3 + k4) / 6;
        x0 += step_dir * H;
    }

    return y;
}

double arccos(double x){
    return ((PI/2) - arcsin(x));
}
/*
double normalize_angle(double angle) {
  while (angle >= 2*PI) {
    angle -= 2*PI;
  }
  while (angle < 0) {
    angle += 2*PI;
  }
  return angle;
}
*/
double sin(double x_target) {
  double x, y, z;
  double k1, k2, k3, k4, l1, l2, l3, l4;
  double angle = x_target;
  int k = (int)(angle / (2*PI));
    angle -= k * (2*PI);
    if (angle < 0) angle += (2*PI);
  x_target = angle;

  x = 0.0;    
  y = 0.0;    // y(0) = 0
  z = 1.0;    // y'(0) = 1

  while(x < x_target) {
    if (x + H > x_target) {
      double last_h = x_target - x;

      k1 = last_h * z;
      l1 = -last_h * y;

      k2 = last_h * z + l1/2;
      l2 = - (last_h * (y + k1/2));

      k3 = last_h * z + l2/2;
      l3 = - (last_h * (y + k2/2));

      k4 = last_h * z + l3;
      l4 = -(last_h * (y + k3));

      y = y + (k1 + 2*k2 + 2*k3 + k4)/6;
      z = z + (l1 + 2*l2 + 2*l3 + l4)/6;

      x = x_target;
    } else {
      k1 = H *  z;
      l1 = -H * y;

      k2 = H * (z + l1/2);
      l2 = -(H * (y + k1/2));

      k3 = H * (z + l2/2);
      l3 = - H * (y + k2/2);

      k4 = H * (z + l3);
      l4 =  -H * (y + k3);

      y = y + (k1 + 2*k2 + 2*k3 + k4)/6;
      z = z + (l1 + 2*l2 + 2*l3 + l4)/6;

      x = x + H;
    }
  }

  return y;
}

double cos(double x){
  return sin(PI/2 - x);
}

double tan(double x){
  return sin(x) / cos(x);
}

double pow(double x, double w) {
  if (x == 0) return 0;  
  if (w == 0) return 1;  

  double x0 = 1.0;
  double y = 1.0;  // y(1) = 1

  int steps = (int)((x - x0) / H);
  if (x < 1.0) {
    steps = (int)((x0 - x) / H);
    steps = -steps;  // Negative steps for x < 1
  }

  for (int i = 0; i < steps; i++) {
    double k1 = H * (w * y / x0);
    double k2 = H * (w * (y + 0.5 * k1) / (x0 + 0.5 * H));
    double k3 = H * (w * (y + 0.5 * k2) / (x0 + 0.5 * H));
    double k4 = H * (w * (y + k3) / (x0 + H));

    y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    x0 += H;
  }

  double remaining = x - x0;
  if (remaining > 0) {
    double k1 = remaining * (w * y / x0);
    double k2 = remaining * (w * (y + 0.5 * k1) / (x0 + 0.5 * remaining));
    double k3 = remaining * (w * (y + 0.5 * k2) / (x0 + 0.5 * remaining));
    double k4 = remaining * (w * (y + k3) / (x0 + remaining));

    y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
  }

  return y;
}
double ln(double x) {
  if (x <= 0) {
    return 0;
  }
  if (x <= 1){
    return -ln((double) 1/x);
  }

  double x0 = 1.0;
  double y = 0.0;  // ln(1) = 0

  int steps = (int)((x - x0) / H);
  if (x < 1.0) {
    steps = (int)((x0 - x) / H);
    steps = -steps;  // Negative steps for x < 1
  }

  for (int i = 0; i < steps; i++) {
    double k1 = H * (1.0 / x0);
    double k2 = H * (1.0 / (x0 + 0.5 * H));
    double k3 = H * (1.0 / (x0 + 0.5 * H));
    double k4 = H * (1.0 / (x0 + H));

    y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
    x0 += H;
  }

  double remaining = x - x0;
  if (remaining > 0) {
    double k1 = remaining * (1.0 / x0);
    double k2 = remaining * (1.0 / (x0 + 0.5 * remaining));
    double k3 = remaining * (1.0 / (x0 + 0.5 * remaining));
    double k4 = remaining * (1.0 / (x0 + remaining));

    y += (k1 + 2 * k2 + 2 * k3 + k4) / 6;
  }

  return y;
}


double exp(double x){
  return pow(EULER_CONST, x);
}

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
void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_init();
void lcd_setCursor(uint8_t pos);
void lcd_clear();
char read_key(uint8_t row);

int main() {
  // Set LCD Pins as Output
  char displayarr[16] = {'\0'};
  char actualdisplay[16] = {'\0'};
  DDRD |= (1 << LCD_RS) | (1 << LCD_EN) | (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);

  // Set Keypad Row Pins as Output (Start HIGH)
  DDRB |= (1 << ROW1) | (1 << ROW2) | (1 << ROW3) | (1 << ROW4);
  PORTB |= (1 << ROW1) | (1 << ROW2) | (1 << ROW3) | (1 << ROW4);

  // Set Keypad Column Pins as Input with Pull-ups
  DDRB &= ~((1 << COL1) | (1 << COL2));
  PORTB |= (1 << COL1) | (1 << COL2);

  DDRC |= (1 << ROW5);
  DDRC &= ~((1 << COL3) | (1 << COL4) | (1 << COL5));
  PORTC |= (1 << ROW5);
  PORTC |= (1 << COL3) | (1 << COL4) | (1 << COL5);

  lcd_init();
  lcd_clear();
  uint8_t encodedPosition = 0;
  uint8_t cursorPosition = 0;

  while (1) {
    for (uint8_t row = 0; row < 5; row++) {
      if (row == 4){
        PORTC &= ~(1 << ROW5);
      } else {
        PORTB &= ~(1 << (ROW1 + row)); 
      } // Set one row LOW (active scanning)
      _delay_ms(5);

      char key = read_key(row);
      if (key) {
        if (key == 'C') { // Clear Button
          lcd_clear();
          cursorPosition = 0;
          encodedPosition = 0;
          unmatched_brackets = 0;
          for (int i = 0; i < 16; i++){
            actualdisplay[i] = '\0';
            displayarr[i] = '\0';
          }
        } else if (key == 'b'){
          if (displayarr[encodedPosition - 2] == 's' || displayarr[encodedPosition- 2] == 'c' || displayarr[encodedPosition - 2] == 't'|| displayarr[encodedPosition-2] == 'e'){
            actualdisplay[cursorPosition-1] = '\0';
            actualdisplay[cursorPosition-2] = '\0';
            actualdisplay[cursorPosition-3] = '\0';
            actualdisplay[cursorPosition-4] = '\0';
            displayarr[encodedPosition-1] = '\0';
            displayarr[encodedPosition-2] = '\0';
            encodedPosition -= 2;
            cursorPosition -= 4;
          } else if (displayarr[encodedPosition - 2] == 'z' || displayarr[encodedPosition - 2] == 'x' || displayarr[encodedPosition - 2] == 'y'){
            actualdisplay[cursorPosition-1] = '\0';
            actualdisplay[cursorPosition-2] = '\0';
            actualdisplay[cursorPosition-3] = '\0';
            actualdisplay[cursorPosition-4] = '\0';
            actualdisplay[cursorPosition-5] = '\0';
            displayarr[encodedPosition-1] = '\0';
            displayarr[encodedPosition-2] = '\0';
            encodedPosition -= 2;
            cursorPosition -= 4;
          } else {
            actualdisplay[cursorPosition - 1] = '\0';
            displayarr[encodedPosition - 1] = '\0';
            --encodedPosition;
            cursorPosition -= 1;
          }
          lcd_clear();
          cursorPosition = 0;
          for (int i = 0; i < 16; i++){
            if (displayarr[i] == '\0') break;
            else {
              switch (displayarr[i]){
                case 's':
                  lcd_data('s');
                  actualdisplay[cursorPosition++] = 's';
                  lcd_data('i');
                  actualdisplay[cursorPosition++] = 'i';
                  lcd_data('n');
                  actualdisplay[cursorPosition++] = 'n';
                  lcd_data('(');
                  actualdisplay[cursorPosition++] = '(';
                  i++;
                  break;
                case 'c':
                  lcd_data('c');
                  actualdisplay[cursorPosition++] = 'c';
                  lcd_data('o');
                  actualdisplay[cursorPosition++] = 'o';
                  lcd_data('s');
                  actualdisplay[cursorPosition++] = 's';
                  lcd_data('(');
                  actualdisplay[cursorPosition++] = '(';
                  i++;
                  break;
                case 't':
                  lcd_data('t');
                  actualdisplay[cursorPosition++] = 't';
                  lcd_data('a');
                  actualdisplay[cursorPosition++] = 'a';
                  lcd_data('n');
                  actualdisplay[cursorPosition++] = 'n';
                  lcd_data('(');
                  actualdisplay[cursorPosition++] = '(';
                  i++;
                  break;
                case 'e':
                  lcd_data('e');
                  actualdisplay[cursorPosition++] = 'e';
                  lcd_data('x');
                  actualdisplay[cursorPosition++] = 'x';
                  lcd_data('p');
                  actualdisplay[cursorPosition++] = 'p';
                  lcd_data('(');
                  actualdisplay[cursorPosition++] = '(';
                  i++;
                  break;
                case 'z':
                  lcd_data('a');
                  actualdisplay[cursorPosition++] = 'a';
                  lcd_data('s');
                  actualdisplay[cursorPosition++] = 's';
                  lcd_data('i');
                  actualdisplay[cursorPosition++] = 'i';
                  lcd_data('n');
                  actualdisplay[cursorPosition++] = 'n';
                  lcd_data('(');
                  actualdisplay[cursorPosition++] = '(';
                  i++;
                  break;
                case 'y':
                  lcd_data('a');
                  actualdisplay[cursorPosition++] = 'a';
                  lcd_data('c');
                  actualdisplay[cursorPosition++] = 'c';
                  lcd_data('o');
                  actualdisplay[cursorPosition++] = 'o';
                  lcd_data('s');
                  actualdisplay[cursorPosition++] = 's';
                  lcd_data('(');
                  actualdisplay[cursorPosition++] = '(';
                  i++;
                  break;
                case 'x':
                  lcd_data('a');
                  actualdisplay[cursorPosition++] = 'a';
                  lcd_data('t');
                  actualdisplay[cursorPosition++] = 't';
                  lcd_data('a');
                  actualdisplay[cursorPosition++] = 'a';
                  lcd_data('n');
                  actualdisplay[cursorPosition++] = 'n';
                  lcd_data('(');
                  actualdisplay[cursorPosition++] = '(';
                  i++;
                  break;
                default :
                  lcd_data(displayarr[i]);
                  cursorPosition++;
              }
            }
          }

        } else if (key == '='){
          
          char reduced_polish[16] = {'\0'};
          shunting_yard(displayarr, reduced_polish);
          for (int i = 0; i < unmatched_brackets; i++){
            lcd_data(')');
            actualdisplay[cursorPosition++] = ')';
            displayarr[encodedPosition++] = ')';
          }
          lcd_command(0xC0);
          double result_val = evaluate_rpn(reduced_polish);
          char result[16];
          //snprintf(result, sizeof(result), "%.2f", result_val);
          dtostrf(result_val, 16, 5, result);
          int index = 0;
          for (int i = 0; i < 16; i++){
            if (result[i] != '\0'){
              lcd_setCursor(0xC0 - 0x80 + index);
              lcd_data(result[i]);
              index++;
            } else {
              break;
            }
          }
        } else if (key == 'm'){
          if (mode) mode = 0; 
          else mode = 1;
        } else if (key == 's'){
          displayarr[encodedPosition++] = key;
          displayarr[encodedPosition++] = '(';
          lcd_setCursor(cursorPosition++);
          lcd_data('s');
          actualdisplay[cursorPosition - 1] = 's';
          lcd_setCursor(cursorPosition++);
          lcd_data('i');
          actualdisplay[cursorPosition - 1] = 'i';
          lcd_setCursor(cursorPosition++);
          lcd_data('n');
          actualdisplay[cursorPosition - 1] = 'n';
          lcd_setCursor(cursorPosition++);
          lcd_data('(');
          actualdisplay[cursorPosition - 1] = '(';

          if (cursorPosition >= 16) cursorPosition = 0;
        } else if (key == 'c'){
          displayarr[encodedPosition++] = key;
          displayarr[encodedPosition++] = '(';
          lcd_setCursor(cursorPosition++);
          lcd_data('c');
          actualdisplay[cursorPosition - 1] = 'c';
          lcd_setCursor(cursorPosition++);
          lcd_data('o');
          actualdisplay[cursorPosition - 1] = 'o';
          lcd_setCursor(cursorPosition++);
          lcd_data('s');
          actualdisplay[cursorPosition - 1] = 's';
          lcd_setCursor(cursorPosition++);
          lcd_data('(');
          actualdisplay[cursorPosition - 1] = '(';
          if (cursorPosition >= 16) cursorPosition = 0;
        } else if (key == 't'){
          displayarr[encodedPosition++] = key;
          displayarr[encodedPosition++] = '(';
          lcd_setCursor(cursorPosition++);
          lcd_data('t');
          actualdisplay[cursorPosition - 1] = 't';
          lcd_setCursor(cursorPosition++);
          lcd_data('a');
          actualdisplay[cursorPosition - 1] = 'a';
          lcd_setCursor(cursorPosition++);
          lcd_data('n');
          actualdisplay[cursorPosition - 1] = 'n';
          lcd_setCursor(cursorPosition++);
          lcd_data('(');
          actualdisplay[cursorPosition - 1] = '(';

          if (cursorPosition >= 16) cursorPosition = 0;
        }else if (key == 'e'){
          displayarr[encodedPosition++] = key;
          displayarr[encodedPosition++] = '(';
          lcd_setCursor(cursorPosition++);
          lcd_data('t');
          actualdisplay[cursorPosition - 1] = 'e';
          lcd_setCursor(cursorPosition++);
          lcd_data('a');
          actualdisplay[cursorPosition - 1] = 'x';
          lcd_setCursor(cursorPosition++);
          lcd_data('n');
          actualdisplay[cursorPosition - 1] = 'p';
          lcd_setCursor(cursorPosition++);
          lcd_data('(');
          actualdisplay[cursorPosition - 1] = '(';

          if (cursorPosition >= 16) cursorPosition = 0;
        }else if (key == 'z'){
          displayarr[encodedPosition++] = key;
          displayarr[encodedPosition++] = '(';
          lcd_setCursor(cursorPosition++);
          lcd_data('a');
          actualdisplay[cursorPosition - 1] = 'a';

          lcd_setCursor(cursorPosition++);
          lcd_data('s');
          actualdisplay[cursorPosition - 1] = 's';
          lcd_setCursor(cursorPosition++);
          lcd_data('i');
          actualdisplay[cursorPosition - 1] = 'i';
          lcd_setCursor(cursorPosition++);
          lcd_data('n');
          actualdisplay[cursorPosition - 1] = 'n';
          lcd_setCursor(cursorPosition++);
          lcd_data('(');
          actualdisplay[cursorPosition - 1] = '(';

          if (cursorPosition >= 16) cursorPosition = 0;
        }else if (key == 'y'){
          displayarr[encodedPosition++] = key;
          displayarr[encodedPosition++] = '(';
          lcd_setCursor(cursorPosition++);
          lcd_data('a');
          actualdisplay[cursorPosition - 1] = 'a';

          lcd_setCursor(cursorPosition++);
          lcd_data('c');
          actualdisplay[cursorPosition - 1] = 'c';
          lcd_setCursor(cursorPosition++);
          lcd_data('o');
          actualdisplay[cursorPosition - 1] = 'o';
          lcd_setCursor(cursorPosition++);
          lcd_data('s');
          actualdisplay[cursorPosition - 1] = 's';
          lcd_setCursor(cursorPosition++);
          lcd_data('(');
          actualdisplay[cursorPosition - 1] = '(';

          if (cursorPosition >= 16) cursorPosition = 0;
        }else if (key == 'x'){
          displayarr[encodedPosition++] = key;
          displayarr[encodedPosition++] = '(';
          lcd_setCursor(cursorPosition++);
          lcd_data('a');
          actualdisplay[cursorPosition - 1] = 'a';

          lcd_setCursor(cursorPosition++);
          lcd_data('t');
          actualdisplay[cursorPosition - 1] = 't';
          lcd_setCursor(cursorPosition++);
          lcd_data('a');
          actualdisplay[cursorPosition - 1] = 'a';
          lcd_setCursor(cursorPosition++);
          lcd_data('n');
          actualdisplay[cursorPosition - 1] = 'n';
          lcd_setCursor(cursorPosition++);
          lcd_data('(');
          actualdisplay[cursorPosition - 1] = '(';

          if (cursorPosition >= 16) cursorPosition = 0;
        }



        else {
          lcd_setCursor(cursorPosition++);
          lcd_data(key);
          actualdisplay[cursorPosition-1] = key;
          encodedPosition++;
          displayarr[encodedPosition-1] = key;
          _delay_ms(50);
          if (cursorPosition >= 16) cursorPosition = 0;
        }
      }
      if (row == 4){
        PORTC |= (1 << ROW5);
      } else {
        PORTB |= (1 << (ROW1 + row));  // Reset row HIGH
      }
      _delay_ms(5);
    }
  }
}

void lcd_init() {
  _delay_ms(50);
  lcd_command(0x33);
  lcd_command(0x32);
  lcd_command(0x28);
  lcd_command(0x0C);
  lcd_command(0x06);
  lcd_command(0x01);
}

void lcd_command(unsigned char cmd) {
  PORTD &= ~(1 << LCD_RS);
  PORTD = (PORTD & 0x0F) | (cmd & 0xF0);
  PORTD |= (1 << LCD_EN);
  _delay_ms(1);
  PORTD &= ~(1 << LCD_EN);

  PORTD = (PORTD & 0x0F) | (cmd << 4);
  PORTD |= (1 << LCD_EN);
  _delay_ms(1);
  PORTD &= ~(1 << LCD_EN);
}

void lcd_data(unsigned char data) {
  PORTD |= (1 << LCD_RS);
  PORTD = (PORTD & 0x0F) | (data & 0xF0);
  PORTD |= (1 << LCD_EN);
  _delay_ms(5);
  PORTD &= ~(1 << LCD_EN);

  PORTD = (PORTD & 0x0F) | (data << 4);
  PORTD |= (1 << LCD_EN);
  _delay_ms(5);
  PORTD &= ~(1 << LCD_EN);
}

void lcd_setCursor(uint8_t pos) {
  lcd_command(0x80 + pos);
}

void lcd_clear() {
  lcd_command(0x01);
  _delay_ms(2);
}

char read_key(uint8_t row) {
  const char keys[2][5][5] = {
    {
      {'0', '1', '2', '3', '4'},
      {'5', '6', '7', '8', '9'},
      {'+', '-', '*', '/', 's'},  // 's' for sin
      {'c', 't', 'e', 'l', 'C'}, 
      {'b', '.', '=', 'm', 'q'}// cos, tan, exp, ln, clr
    }, 
    {
      {'0', '1', '2', '3', '4'},
      {'5', '6', '7', '8', '9'},
      {'(', ')', '^', 'f', 'z'},  //d -> modulo, 
      {'y', 'x', 'd', 'g', 'C'}, 
      {'b', '.', '=', 'm', 'q'}
    }
  };

  if (!(PINB & (1 << COL1))) return keys[mode][row][0];
  if (!(PINB & (1 << COL2))) return keys[mode][row][1];
  if (!(PINC & (1 << COL3))) return keys[mode][row][2];
  if (!(PINC & (1 << COL4))) return keys[mode][row][3];
  if (!(PINC & (1 << COL5))) return keys[mode][row][4];

  return 0; // No key pressed
}
