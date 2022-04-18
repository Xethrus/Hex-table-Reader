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
// broken: Does not account for empty space in the input token, will go into random memory when nothing exists

struct line
{
    char letters[4];
    char hexs[4][2];
    char binarys[4][8];
    int decimals[4];

    void printLine()
    {
        printf("|    %c    %c    %c    %c | %s %s %s %s | %s %s %s %s | %3d %3d %3d %3d |",
               letters[0], letters[1], letters[2], letters[3], hexs[0], hexs[1], hexs[2], hexs[3],
               binarys[0], binarys[1], binarys[2], binarys[3], decimals[0], decimals[1], decimals[2], decimals[3]);
    }

    bool eq(line o)
    {
        bool ret = true;
        for (int i = 0; i < 4; i++)
        {
            ret &= (letters[i] == o.letters[i] && hexs[i] == o.hexs[i] && binarys[i] == o.binarys[i] && decimals[i] == o.decimals[i]);
        }
        return ret;
    }
} line;

int ctoh(char x)
{
    char const conv_array[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    int ret = -1;
    int count = 0;
    for (char i : conv_array)
    {
        if (x == i)
        {
            return count;
        }
        count++;
    }
    return ret;
}

char *dtob(uint8_t val)
{
    char *ret = (char *)malloc(8 * sizeof(uint8_t));
    memset(ret, 0, 8);
    int count = 0;
    while (val > 0)
    {
        ret[count] = val % 2;
        val /= 2;
        count++;
    }
    return ret;
}

uint8_t btod(char arr[8])
{
    uint8_t ret = 0;
    for (int i = 0; i < 8; i++)
    {
        ret += arr[i];
        ret *= 2;
    }
    return ret;
}

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        std::cerr << "input or output file needed" << std::endl;
        return 1;
    }
    std::ifstream inFile;
    std::ofstream outFile;
    const int BUFFER_SIZE = 100;
    unsigned char buffer[BUFFER_SIZE];
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
    std::string line;
    int counter = 0;
    while (std::getline(inFile, line))
    {
        // skip first two lines lol
        counter++;
        if (counter < 3)
        {
            continue;
        }

        std::stringstream lineStream(line);
        std::string token;
        char letters[16];
        for (int i = 0; i < 16; i++)
        {
            letters[i] = 0;
        }
        int indexCounter = 0;
        int validTokenLength = 0;
        std::cout << "This is the total token length > " << validTokenLength << " <" << std::endl;
        int count = 0;
        int subc = 0;
        struct line l;
        memset(&l, 0, sizeof(struct line));
        while (lineStream >> token)
        {
            if (count == 1 and token != "|")
            {
                l.letters[subc] = token[0];
                subc++;
            }
            if (count == 2 and token != "|")
            {
                l.hexs[subc][0] = ctoh(token[0]);
                l.hexs[subc][1] = ctoh(token[1]);
            }
            if (count == 3 and token != "|")
            {
                l.binarys[subc][0] = token[0];
                l.binarys[subc][1] = token[1];
                l.binarys[subc][2] = token[2];
                l.binarys[subc][3] = token[3];
                l.binarys[subc][4] = token[4];
                l.binarys[subc][5] = token[5];
                l.binarys[subc][6] = token[6];
                l.binarys[subc][7] = token[7];
            }
            if (count == 4 and token != "|")
            {
                int val = 0;
                int i = 0;
                while (token[i] != 0)
                {
                    val += ctoh(token[i]);
                    val *= 10;
                    i++;
                }
                l.decimals[subc] = val;
            }

            if (token == "|")
            {
                count++;
                subc = 0;
            }
        }
        l.printLine();
    }
}
