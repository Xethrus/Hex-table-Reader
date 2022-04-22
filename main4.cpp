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
// check: good
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
// check: good
class ByteValueReader
{
public:
    virtual unsigned char readChar(istream &inFile) = 0;
};
// check: good
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
// check: good
int binaryToInt(char c)
{
    if (c >= '0' && c <= '1')
    {
        return c - '0';
    }
    throw HexTableError("Invalid Binary Conversion! Char is not binary!");
}
// check: good
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

// check: good
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
        if (buf[0] == ' ' && buf[1] == ' ')
        {
            return 0;
        }
        return hexToInt(buf[0]) * 16 + hexToInt(buf[1]);
    }
};

// check: good
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
        if (buf[0] == ' ')
        {
            return 0;
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

// check: ~good
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
        if (buf[0] == ' ' && buf[1] == ' ' && buf[2] == ' ')
        {
            return 0;
        }
        if (buf[0] == 0 && buf[1] == ' ' && buf[2] == 1)
        {
            throw HexTableError("Bad/Invalid value ex:\"0 1\" or \"001\"");
        }
        else if (buf[0] == 0 && buf[1] == 0 && buf[2] == 1)
        {
            throw HexTableError("Bad/Invalid value ex:\"0 1\" or \"001\"");
        }
        return (
            decimalToInt(buf[0]) * 100 +
            decimalToInt(buf[1]) * 10 +
            decimalToInt(buf[2]));
    }
};

// check: good
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
            if (buf[3] == ' ')
            {
                return 0;
            }
            return buf[3];
        }
        else if (buf[0] == ' ' && buf[1] == '\'' && buf[2] == ' ' && buf[3] == '\'')
        {
            return ' ';
        }
        throw HexTableError("Byte Value Not Valid Printable Character!");
    }
};

// check: good
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
// check: good
vector<vector<unsigned char>> readTable(istream &inFile)
{
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
            if(inFile.eof()) {
                break;
            }
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
            //cout << count << endl;
            throw HexTableError("Line Lacking \"|\"!");
        }
    }
    return chars;
}
void printOutput(ofstream &outFile, char c1, char c2, char c3, char c4)
{
    if(c1 == 0) {
        return;
    }
    if(c1 == c2 && c1 == c3 && c1 == c4) {
        outFile << c1;
    } else if(c1 != c2 && c1 != c3) {
        outFile << c1;
    } else if(c2 != c3 && c2 != c4) {
        outFile << c2;
    } else if(c3 != c4 && c3 != c1) {
        outFile << c3;
    } else if(c4 != c1 && c4 != c2) {
        outFile << c4;
    }
}
int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        throw HexTableError("Usage: ./a.exe [inFile] [outFile]");
    }
    ifstream inFile(argv[1]);
    ofstream outFile(argv[2]);

    vector<vector<unsigned char>> chars = readTable(inFile);

    int count = 0;
    for (unsigned char c : chars[0])
    {
        printOutput(outFile, chars[0][count], chars[1][count], chars[2][count], chars[3][count]);
        count++;
    }
    return 0;
}
