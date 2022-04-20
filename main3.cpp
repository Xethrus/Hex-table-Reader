#include <iostream>
#include <fstream>
#include <string>
const char *EXPECTED_FIRST_ROW  = "| Printable           | Hexadecimal | Binary    | Decimal         |";
const char *EXPECTED_SECOND_ROW = "|---------------------+-------------+-----------+-----------------|";
struct {
   char letters[4];
   char hexs[4][2];
   char binarys[4][8];
   int decimals[4];

} line;


int main(int argc, char *argv[])
{
   if(argc < 3)
   {
      std::cerr << "Invalid Argument Count: " << argc << " is < 3" << std::endl;
      return 1;
   }
   std::ifstream inFile;
   std::ofstream outFile;

   inFile.open(argv[1], std::ios::in | std::ios::binary);
   outFile.open(argv[2]);

   if (!inFile)
   {
      std::cerr << "Unable to load/open " << argv[1] << std::endl;
      return 1;
   }
   if (!outFile)
   {
      std::cerr << "Unable to load/open " << argv[2] << std::endl;
      return 1;
   }

   std::string row;
   while(std::getline(inFile, row))
   {
      std::cout << row << std:: endl;
   }

   inFile.close();
   outFile.close();


}
