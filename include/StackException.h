#include <execinfo.h>
#include <string>
#include <vector>

#define MAX_STACK_DEPTH 500
class StackException {
  public:
    StackException(std::string msg) : message(msg) {
         void * array[MAX_STACK_DEPTH];
         int nSize = backtrace(array, MAX_STACK_DEPTH);
         symbols.resize(nSize);
         char ** syms = backtrace_symbols(array, nSize);
                
         for (int i = 0; i < nSize; i++)
         {
             symbols[i] = syms[i];
         }

         free(syms);
     }
   
     std::vector<std::string> symbols;
     std::string message;
};
