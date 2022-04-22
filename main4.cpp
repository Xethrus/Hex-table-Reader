#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>

using namespace std;

const char *EXPECTED_ROW_ONE = "| Printable           | Hexadecimal | Binary                              | Decimal         |";
const char *EXPECTED_ROW_TWO = "|---------------------+-------------+-------------------------------------+-----------------|";
const int VALUEPERCOL = 4;
const int NUMCOLUMNS = 4;
// good habit Mikel recommends getting into
class HexTableError : public runtime_error
{
public:
   HexTableError(const char *err_msg) : runtime_error(err_msg) {}
};

/*
| Printable           | Hexadecimal | Binary                              | Decimal         |
|---------------------+-------------+-------------------------------------+-----------------|
|    H    e    l    l | 48 65 6c 6c | 01001000 01100101 01101100 01101100 |  72 101 108 108 |
|    o    ,  ' '    W | 6f 2c 20 57 | 01101111 00101100 00100000 01010111 | 111  44  32  87 |
|    o    r    l    d | 6f 72 6c 64 | 01101111 01110010 01101100 01100100 | 111 114 108 100 |
|    !   \n           | 21 0a       | 00100001 00001010                   |  33  10         |
*/

//

void checkHeaders(istream &inFile)
{
   string line;
   getline(inFile, line);
   if (line != EXPECTED_ROW_ONE)
   {
      throw HexTableError("Unmatching Row One");
   }
   getline(inFile, line);
   if (line != EXPECTED_ROW_TWO)
   {
      throw HexTableError("Unmatching Row Two");
   }
}
int hexToInt(char c)
{
   if (c >= '0' && c <= '9')
   {
      return c - '0';
   }
   else if (c >= 'a' && c <= 'f')
   {
      return c - 'a' + 10;
   }
   throw HexTableError("Invalid Hexadecimal Conversion! Char is not a Hex!");
}
int binaryToInt(char c)
{
   if (c >= '0' && c <= '1')
   {
      return c - '0';
   }
   throw HexTableError("Invalid Binary Conversion! Char is not binary!");
}
int decimalToInt(char c)
{
   if (c >= '0' && c <= '9')
   {
      return c - '0';
   }
   else if (c == ' ')
   {
      return 0;
   }
   throw HexTableError("Invalid, Char is not decimal!");
}

//

class ByteValueReader
{
public:
   virtual unsigned char readChar(istream &inFile) = 0;
};

//

class HexReader : public ByteValueReader
{
public:
   unsigned char readChar(istream &inFile) override
   {
      char buf[2];
      inFile.read(buf, sizeof(buf));
      if (inFile.bad())
      {
         throw HexTableError("Char Unreadable!");
      }
      return hexToInt(buf[0]) * 16 + hexToInt(buf[1]);
   }
};

//

class BinaryReader : public ByteValueReader
{
public:
   unsigned char readChar(istream &inFile) override
   {
      char buf[8];
      inFile.read(buf, sizeof(buf));
      if (inFile.bad())
      {
         throw HexTableError("Binary Unreadable!");
      }
      return (
          binaryToInt(buf[0]) * 128 +
          binaryToInt(buf[1]) * 64 +
          binaryToInt(buf[2]) * 32 +
          binaryToInt(buf[3]) * 16 +
          binaryToInt(buf[4]) * 8 +
          binaryToInt(buf[5]) * 4 +
          binaryToInt(buf[6]) * 2 +
          binaryToInt(buf[7]));
   }
};

//

class DecimalReader : public ByteValueReader
{
public:
   unsigned char readChar(istream &inFile) override
   {
      char buf[3];
      inFile.read(buf, sizeof(buf));
      if (inFile.bad())
      {
         throw HexTableError("Binary Unreadable!");
      }
      return (
          decimalToInt(buf[0]) * 100 +
          decimalToInt(buf[1]) * 10 +
          decimalToInt(buf[2]));
   }
};

//

class PrintableReader : public ByteValueReader
{
public:
   unsigned char readChar(istream &inFile) override
   {
      char buf[4];
      inFile.read(buf, sizeof(buf));
      if (inFile.bad())
      {
         throw HexTableError("Char Unreadable!");
      }
      if (buf[0] == '\\' && buf[1] == 'x')
      {
         return hexToInt(buf[2]) * 16 + hexToInt(buf[3]);
      }
      else if (buf[0] == ' ' && buf[1] == ' ' && buf[2] == '\\')
      {
         switch (buf[3])
         {
         case '0':
            return '\0';
         case 't':
            return '\t';
         case 'n':
            return '\n';
         }
      }
      else if (buf[0] == ' ' && buf[1] == ' ' && buf[2] == ' ')
      {
         return buf[3];
      }
      else if (buf[0] == ' ' && buf[1] == '\'' && buf[2] == ' ' && buf[3] == '\'')
      {
         return ' ';
      }
      throw HexTableError("Byte Value Not Valid Printable Character!");
   }
};

//

void readColumn(istream &inFile, vector<unsigned char> &chars, ByteValueReader &reader)
{
   if (inFile.get() != '|' || inFile.get() != ' ')
   {
      throw HexTableError("Row Lacking: \"|\"!");
   }
   for (int i = 0; i < VALUEPERCOL; ++i)
   {
      chars.push_back(reader.readChar(inFile));
      if (inFile.get() != ' ')
      {
         throw HexTableError("Printable Char Missing Following Space!");
      }
   }
}

vector<vector<unsigned char>> readTable(istream& inFile) {
   checkHeaders(inFile);
   //
   vector<vector<unsigned char>> chars(NUMCOLUMNS);
   PrintableReader readPrintableChars;
   HexReader readHexChar;
   BinaryReader readBinaryChar;
   DecimalReader readDecimalChar;
   ByteValueReader *readers[NUMCOLUMNS] = {
         &readPrintableChars, &readHexChar, &readBinaryChar, &readDecimalChar};

   while (inFile.peek() == '|')
   {
      for (int i = 0; i < NUMCOLUMNS; i++)
      {
         readColumn(inFile, chars[i], *readers[i]);
      }
      if (inFile.get() != '|' || inFile.get() != '\n')
      {
         throw HexTableError("Row Missing Valid ending!");
      }
   }
   if (!inFile.eof())
   {
      if (inFile.bad()) 
      {
         throw HexTableError("Error While Reading Input File!");
      }
      else
      {
         throw HexTableError("Line Lacking \"|\"!");
      }
   }
   return chars;
}
int main(int argc, const char *argv[])
{
   ifstream inFile(argv[1]);

   vector<vector<unsigned char>> chars = readTable(inFile);
   

   for (unsigned char c : chars[0])
   {
      cout << "printable: ";
      cout << static_cast<int>(c) << endl;
   }
   for (unsigned c : chars[1])
   {
      cout << "hex: ";
      cout << static_cast<int>(c) << endl;
   }
   for (unsigned c : chars[2])
   {
      cout << "binary: ";
      cout << static_cast<int>(c) << endl;
   }
   for (unsigned c : chars[3])
   {
      cout << "decimal: ";
      cout << static_cast<int>(c) << endl;
   }

   return 0;
}
