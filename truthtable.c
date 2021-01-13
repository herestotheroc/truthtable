 //take in some formula and return a truth table
 //ex:
 // truthtable "A v B"
 //
 // A B  A v B
 // T T    T
 // T F    T
 // F T    T 
 // F F    F
 //


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "truthtable.h"
#include "parser.c"


bool alphaToBool(char a) { // self explanatory. 
   if(a == 'T') { return true; }
   return false;
}

char boolBinToChar(char a) {
  if (a == '1') { return 'T'; } 
  return 'F';

}

char boolToAlpha(bool b) { // this was for debugging but it's useful now
   if (b) { return 'T'; }
   return 'F';
}

bool isIn(char* arr, char a) { // finds out whether a is in arr
   int i;
   for (i=0; i<strlen(arr); i++) {
      if (arr[i] == a) { return true; }
   }
   return false;
}

int countOccurences(char* str, char target) { // finds out how many times target is in str
   int count = 0;
   for (int i = 0; i < strlen(str); i++) {
      if (str[i] == target) {
         count++;
      }
   }
   return count;
}

int countElems(char* string, char* key) { //the one above this is worse but w/e ig
  int i, j, count;
  count = 0;
  for(i = 0; i < strlen(string); i++) {
     for(j=0; j < strlen(key); j++) {
         if (string[i] == key[j]) {
           count++;
         }
      }
   }
  return count;
}

int largestPowerOfTwo(int i) { // largest power of two lower than i. for the binary thingy
   int j = trunc(log(i)/log(2));
   return log(trunc(pow(2, j)))/log(2);

}

int binLen(int i) { // length of base 10 number if it were to be in base 2.
   return trunc(log(i)/log(2))+1;
}


void toBin(int i, char* ans, int len) { // change number from base 10 to base 2. fills the rest of the string with zeros so the other part works. yoinked this algorith from some website but i cant remember which one q.q
   int d = i;
   int count = 0;
   int p = largestPowerOfTwo(i);
   if (i == 0) {
      memset(ans, '0', len*sizeof(char));
   }
   if(binLen(i)<len) {
      while (count<(len-binLen(i))) {
         ans[count] = '0';
         count++;
      }
   }
   while (!(p<0)) {
      if (1 << p <= d) {
         *(ans+count) = '1';
         d -= 1 << p;
      } else {
         *(ans+count) = '0';
      }
      count++;
      p--;
   }
}

char eval(char op, char first, char second) { // evaluates a single binary operator and it's two operands
   char firstbool = alphaToBool(first);
   char secondbool = alphaToBool(second);
   switch (op) {
      case '^':
         return boolToAlpha(firstbool && secondbool);
         break;
      case 'v':
         return boolToAlpha(firstbool || secondbool);
         break;
      case '>':
         return boolToAlpha(!firstbool || secondbool);
         break;
      case '=':
         return boolToAlpha((!firstbool || secondbool) && (!secondbool || firstbool));
         break;
      }
   return 0;
}

char evalRPN(char* str) { // evaluates a whole expression in rpn
   char first, second, ans, tmp;
   struct Stack* stack = createStack(strlen(str));
   for (int i = 0; i < strlen(str); i++) {
      if (isIn(alphas, str[i])) { push(stack, str[i]); }
      else if (str[i] == '~') {
         //i++;
         tmp = peek(stack);
         pop(stack);
         switch (tmp) {
            case 'T' :
               push(stack, 'F');
               break;
            case 'F':
               push(stack, 'T');
               break;
         }
      }
      else { 
         second = peek(stack);
         pop(stack);
         first = peek(stack);
         pop(stack);
         push(stack, eval(str[i], first, second));
      }
   }
   ans = peek(stack);
   destroyStack(stack);
   return ans;
}

void strReplc(char* string, char target, char replace) { // replaces
   int len = strlen(string);
  
   for (int i = 0; i < len; i++) {
      if (*(string+i) == target) { *(string+i) = replace; }
   }
}

char** boolVals(int n) { // returns an array that counts from 0 to n in binary. conveniently represents all the possible true-false values for n different variables.
   char** ans = malloc((1 << n)*sizeof(char*));
   int i;
   for (i = 0; i < 1 << n; i++) {
      *(ans+i) = malloc(n*sizeof(char));
   }
   for (i = 0; i < 1 << n; i++) {
      toBin(i, *(ans+i), n);
   }
   return ans;
}

char* intersection(char* string, char* key) { // A n B
   char* ans = malloc((strlen(string)<strlen(key)) ? strlen(string) : strlen(key));
   int count = 0;
   for (int i = 0; i < strlen(string);  i++) {
      if (isIn(key, string[i]) && !isIn(ans, string[i])) {
         ans[count] = string[i];
         count++;
         
      }
   }
   return ans;
}

char* propsToBools(char* wff, char* bools) { // changes the propositions in wff to T/F, as specified by the bools array. 1 = true, 0 = false.
   char* props = intersection(wff, alphas);
   char* ans = malloc(sizeof(wff));
   memcpy(ans, wff, strlen(wff)*sizeof(char));
   for (int i = 0; i < strlen(props); i++) {
      strReplc(ans, props[i], boolBinToChar(bools[i]));
   }
   return ans;
}

char** createTruthTable(char* rpn) { // creates a truth table from the wff
   char* propositions = intersection(rpn, alphas);
   int n = strlen(propositions);
   int temp;
   char** bools = boolVals(n);
   char** lines = malloc(((1 << n)+2)*sizeof(char*));
   lines[0] = malloc(n*2 + strlen(input));
   lines[1] = malloc(sizeof(char));
   lines[1][0] = ' ';
   for (int i = 2; i < (1 << n)+2; i++) {
      lines[i] = malloc((n*2 + strlen(input))*sizeof(char));
   }
   for (int i = 0; i < n; i++) {
      lines[0][2*i] = propositions[i];
      lines[0][2*i+1] = ' ';
   }
   strcpy((lines[0]+n*2), input);
   memset(lines[1], ' ', strlen(lines[1]));
   for (int i = 2; i < (1 << n)+2; i++) {
      for (int j = 0; j < n; j++) {
         lines[i][2*j] = boolBinToChar(bools[i-2][j]);
         lines[i][2*j+1] = ' ';
      }
      memset((lines[i]+n*2), ' ', (strlen(input)-1)/2);
      lines[i][n*2+((strlen(input)-1)/2)] = evalRPN(propsToBools(rpn, bools[i-2]));
   }
   for (int i = 0; i < 1 << n; i++) {
      free(bools[i]);
   }
   free(bools);
   return lines;
}

bool checkValidity(char* wff) { // useless
   for (int i = 0; i < strlen(wff); i++) {
      if (!(isIn(alphas, wff[i]) || isIn(connectives, wff[i]))) { 
         if (!(wff[i] == '(' || wff[i] == ')')) { 
            printf("%c\n", wff[i]);
            return false; 
         }
      }  
   }
   return true;
}

int main(int argc, char *argv[]) { // the MAIN FUNCTION !!!
   struct Stack* stack = createStack(strlen(argv[1])*sizeof(char));
   struct Stack* queue = createStack(strlen(argv[1])*sizeof(char));
   input = malloc(sizeof(argv[1]));
   strcpy(input, argv[1]);
   toRPN(queue, stack);
   if (errored) { return 1; }
   char* rpn = malloc((queue->top+1)*sizeof(char));
   for (int i = queue->top; i > -1; i--) {
      //printf("%d\n", i);
      rpn[i] = peek(queue);
      pop(queue);
   }
   
   int n = strlen(intersection(argv[1], alphas));
   char** table = createTruthTable(rpn);
   for (int i = 0; i < (1 << n)+2; i++) {
      printf("%s\n", table[i]);
      free(table[i]);
   }
   free(table);
   destroyStack(stack); // lol
   destroyStack(queue); // lol
   return 1;
}


