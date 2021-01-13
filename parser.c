#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

char ops[5] = {'^','v','>','=','~'};
char currToken, nextToken;
int currIndex = -1;
bool errored = false;
char* input;

int handlealpha(); // it was complaining about forward declarations :|
int peeknexttoken();
int handlewordops();
void handleUnexpectedToken();
void handleIllegalNegation();

bool isin(char target, char* arr) { // theres one of these in the other folder but whatever
   for (int i = 0; i < strlen(arr); i++) {
      if (arr[i] == target) { return true; }
   }
   return false;
}

enum prec { // operator precedence 
   parens,
   impl,
   iff,
   or,
   and,
   not
};

enum token { // token enumeration 
   op,
   prop,
   paren,
   eof
};

struct Stack { // stack structure
   int cap;
   int top;
   char* stack;
};

struct Stack* createStack(int capacity) { // Create A New Stack
   struct Stack* newStack = (struct Stack*)malloc(sizeof(struct Stack));
   newStack->top = -1;
   newStack->cap = capacity;
   newStack->stack = malloc(capacity*sizeof(char));
   memset(newStack->stack, ' ', capacity*sizeof(char));

   return newStack;
}

void destroyStack(struct Stack* stack) { // arent you supposed to free memory or something
   free(stack->stack);
   free(stack);
}

bool isempty(struct Stack* stack) { // i cant remember why i wrote this
   if (stack->top == -1) { return true; }
   return false;
}

void push(struct Stack* stack, char data) { // push onto the stack
   (stack->top)++;
   stack->stack[stack->top] = data;
}

void pop(struct Stack* stack) { // pop from the stack
   stack->stack[stack->top] = ' ';
   (stack->top)--;
}

char peek(struct Stack* stack) { // look at the top of the stack
   return stack->stack[stack->top];
}


int getprec(char op) { // get precedence for an operator
   if (op == '>') { return impl; }
   if (op == '=') { return iff; }
   if (op == 'v') { return or; }
   if (op == '^') { return and; }
   if (op == ' ') { return 10; }
   if (op == '(' || op == ')') { return parens; }
   return -1;
}

int gettoken() { // advance one token, sending the current one into the shadow realm
   currIndex++;
   currToken = input[currIndex];
   
   if (isin(currToken, ops)) { return op; }
   if (isin(currToken, alphas)) { return prop; }//handlealpha(); }
   if (currToken == '(' || currToken == ')') { return paren; }
   if (currIndex == strlen(input)) { return eof; }
   return -1;
}
/* 
void eattokens(int n) {
   for (int i = 0; i < n; i++) {
      gettoken();
   }
}

int handlealpha() {
   if (peeknexttoken() == prop) { return handlewordops(); }
   else if (peeknexttoken() == op) { return op; }
}

int handlewordops() {
   if (currToken == 'a') { 
      eattokens(3);
      currToken = '^';
      return op;
   }
}*/
 // ^who need that garbage

int peeknexttoken() { // look at the next token without eating one
   nextToken = input[currIndex+1];
            
   if (isin(nextToken, ops)) { return op; }
   if (isin(nextToken, alphas)) { return prop; }
   if (nextToken == '(' || nextToken == ')') { return paren; }
   if (currIndex+1 == strlen(input)) { return eof; }

   return -1;
}

void toRPN(struct Stack* queue, struct Stack* stack) { // parses whatever you type to reverse polish notation, which is very easy to evaluate. wikipedia/shunting yard algorithm
   int currTokType;
   int loop = 0; // i still dont get why using currIndex for the loop doesn't work but w/e
   char tmpToken; 
   //printf("%d, %lu\n", currIndex, strlen(input));
   while (loop < strlen(input)) {
      //printf("index: %d\n", currIndex+1);
      //printf("stack size: %d\n", stack->top+1);
      currTokType = gettoken(); // get next token
      //printf("token: %c\n", currToken);
      if (currTokType != eof) { // make sure it's not the end of the file. i don't even know if this does anything, but i figured it was a good idea so i put it in
         if (currTokType == prop) { // if it's a proposition
            if (peeknexttoken() == prop) { // there shouldn't be two propositions right next to eachother
               handleUnexpectedToken(1);
            }
            else { push(queue, currToken); } // push onto the output queue
            //printf("Pushing %c to queue.\n", currToken);
         }
         if (currTokType == op) {
            if (currToken == '~') {
               peeknexttoken();
               if (peeknexttoken() == prop) { // handle negations
                  gettoken();
                  push(queue, currToken);
                  push(queue, '~');
               }
               else if (nextToken == '(') { push(stack, '~'); } // handle negating parentheses
               else if (peeknexttoken() == op) {
                  if (nextToken == '~') { // handle double negations. it just deletes them, because that's easy
                     gettoken();
                  }
                  else { handleIllegalNegation(); } // you did something bad
               }
            }
            else {
               if (isempty(stack)) { // push operator onto the stack
                  push(stack, currToken);
                  //printf("Pushing %c to stack.\n", currToken);
               }
               else if (getprec(peek(stack)) > getprec(currToken)) { // compare precedence with the top of the operator stack
                  push(queue, currToken);
                  //printf("Pushing %c to queue.\n", currToken);
               }
               else { 
                  push(stack, currToken);
                  //printf("Pushing %c to stack.\n", currToken);
               }
            }
         }
         if (currTokType == paren) { // handle parentheses
            if (currToken == '(') {
               push(stack, currToken);
               //printf("Pushing ( to stack.\n");
            }
            if (currToken == ')') {
               while (peek(stack) != '(') {
                  push(queue, peek(stack));
                  //printf("Pushing %c to queue.\n", peek(stack));
                  pop(stack);
               }
               pop(stack);
            }
         }
         if (currTokType == -1) { // error
            handleUnexpectedToken(0);
         }
      }
      loop++;
   }
   //printf("stack size: %d\n", stack->top+1);
   while (stack->top > -1) { // put the rest of the operators onto the output queue
       push(queue, peek(stack));
       //printf("Pushing %c to queue.\n", peek(stack));
       pop(stack);
   }
}

void handleUnexpectedToken(int mode) { // incase they try to type in anything bad
   printf("%s\n", input);
   printf("\033[31m%*c\n", currIndex+1+mode, '^'); // wow it prints red isn't that cool!!
   printf("error: unexpected token");
   if (isalpha(currToken) && mode == 0) { printf(", please use capital letters to represent propositions\n"); } 
   else { printf("\n"); }
   printf("\033[0m");
   errored = true;
}

void handleIllegalNegation() { // incase they try to negate something that shouldn't be negated, like a binary operator
   printf("%s\n", input);
   printf("\033[31m%*c\n", currIndex+1, '^');
   printf("error: illegal negation\n");
   printf("\033[0m");
   errored = true;
}

