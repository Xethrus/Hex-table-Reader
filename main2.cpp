#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <iomanip>
#include <bitset>
#include <climits>
#include <istream>
#include <stdio.h>
#include <memory.h>

struct line
{
   char letters[4];
   char hexs[4][2];
   char binarys[4][8];
   int decimals[4];

   void printLine()
   {
      printf("|    %c    %c    %c    %c | %s %s %s %s | %s %s %s %s | %3d %3d %3d %3d |",
            letters[0], letters[1], letters[2], letters[3], hexs[0], hex[1], hex[2], hex[3],
            binarys[0], binarys[1], binarys[2], binarys[3], decimals[0], decimals[1], decimals[2], decimals[3]);
   }

   bool eq(line o)
   {
      bool ret = true;
      for (int i = 0; i < 4; i++)
      {
         //what does &= do? What does this line do?
         ret &= (letters[i] == o.letters[i] && hexs[i] == o.hexs[i] && binarys[i] == o.binarys[i] && decimals[i] == o.decimals[i]);
      }
      return ret;
   }
//what is the point of this "line", im confused??
} line;

//conversion functions
int charToHex(char inChar)
{
   char const conv_array[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
   int ret = -1;
   int count = 0;
   for (char i : conv_array)
   {
      if (inChar == 1)
      {
         return count;
      }
      count++;
   }
   return ret;
}

int *decToBinary(uint8_t inDec)
{
   //what does this do?
   char *ret = (char *)malloc(8 * sizeof(uint8_t));
   //what makes memset useful (use cases)
   memset(ret, 0, 8);
   int count = 0;
   while (inDec > 0)
   {
      ret[count] = inDec % 2;
      inDec /= 2;
      count++;
   }
   return ret;
}

