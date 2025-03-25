#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "Loader.h"
#include "Memory.h"

#define ADDRBEGIN 2
#define ADDREND 4
#define DATABEGIN 7
#define COMMENT 28
/*
 * Loader
 * opens up the file named in argv[0] and loads the
 * contents into memory. If the file is able to be loaded,
 * then loaded is set to true.
 */
// This method is complete and does not need to be modified.
Loader::Loader(int argc, char *argv[])
{
   std::ifstream inf; // input file stream for reading from file
   int lineNumber = 1;
   lastAddress = -1;
   loaded = false;

   // if no file name given or filename badly formed, return without loading
   if (argc < 2 || badFile(argv[1]))
      return;
   inf.open(argv[1]);

   // if file can't be opened, return without loading
   if (!inf.is_open())
      return;

   std::string line;
   while (getline(inf, line))
   {
      if (hasErrors(line))
      {
         std::cout << "Error on line " << std::dec << lineNumber
                   << ": " << line << std::endl;
         return;
      }
      if (hasAddress(line) && hasData(line))
         loadLine(line);
      lineNumber++;
   }
   loaded = true;
}

/*
 * hasAddress
 * returns true if the line passed in has an address on it.
 * A line that has an address has a '0' in column 0.
 * It is assumed that the address has already been checked to
 * make sure it is properly formed.
 *
 * @param line - a string containing a line of valid input from
 *               a .yo file
 * @return true, if the line has an address on it
 *         false, otherwise
 */
bool Loader::hasAddress(std::string line)
{
   // if (line[0] == 0)
   // {
   //    return true;
   // }
   // else
   //    return false;

   return (!line.empty() && line[0] == '0');
}

/*
 * hasData
 * returns true if the line passed in has data on it.
 * A line that has data does not contain a space
 * at index DATABEGIN.
 * It is assumed that the data has already been checked to
 * make sure it is properly formed.
 *
 * @param line - a string containing a line of valid input from
 *               a .yo file
 * @return true, if the line has data in it
 *         false, otherwise
 */
bool Loader::hasData(std::string line)
{
   // if (line[7] != ' ')
   // {
   //    return true;
   // }
   // else
   //    return false;
   return (line.size() > DATABEGIN && line[DATABEGIN] != ' ');
}

/*
 * hasComment
 * returns true if line is at least COMMENT in length and
 * line has a | at index COMMENT.
 *
 * @param line - a string containing a line from a .yo file
 * @return true, if the line is long enough and has a | in index COMMENT
 *         false, otherwise
 */
bool Loader::hasComment(std::string line)
{
   return (line.size() >= COMMENT && line[COMMENT] == '|');
}

/*s
 * loadLine
 * The line that is passed in contains an address and data.
 * This method loads that data into memory byte by byte
 * using the Memory::getInstance->putByte method.
 *
 * @param line - a string containing a line of valid input from
 *               a .yo file. The line contains an address and
 *               a variable number of bytes of data (at least one)
 */
void Loader::loadLine(std::string line)
{
   // if (hasAddress(line))
   // {
   //    std::string addressStr = line.substr(0, 5);   // slices off the address space
   //    uint64_t address = convert(addressStr, 0, 5); // converts it into a hex digit

   //    if (hasData(line))
   //    {
   //       // build dataLen variable by scanning for a ' '
   //       int dataLen = 0;
   //       while (line[DATABEGIN + dataLen] != ' ')
   //       {
   //          dataLen++;
   //       }

   //       std::string dataStr = line.substr(7, dataLen); // slices off the data
   //       uint64_t data = convert(dataStr, 0, 16);       // converts it to hex

   //       bool error = false; // imem_variable originally built in the memory class
   //       int j = 0;          // address space iterator

   //       for (int i = 0; i < dataStr.length(); i += 2)
   //       { // works through the data byte by byte
   //          if (i + 2 <= dataStr.length())
   //          {

   //             std::string byteStr = dataStr.substr(i, 2); // one byte of data
   //             uint8_t byte = convert(byteStr, 0, 2);      // convert to hex

   //             Memory::getInstance()->putByte(byte, address + j, error); // adds data one byte at a time to each mem[address] location in the class
   //             if (error)
   //             {
   //                break;
   //             }
   //             j++; // go to next address
   //          }
   //       }
   //    }
   // }

   // 1) Find colon that separates address from data
   size_t colonPos = line.find(':');
   if (colonPos == std::string::npos)
   {
      // No colon => not valid for machine code (or it’s an error line).
      return;
   }

   // 2) Parse the address (allow 0x or just hex)
   std::string addrStr = line.substr(0, colonPos);
   int32_t address = (int32_t)strtol(addrStr.c_str(), nullptr, 16);

   // 3) Find | which indicates comment start (or end of data)
   size_t barPos = line.find('|');
   if (barPos == std::string::npos)
   {
      // If no |, read data until end of line
      barPos = line.size();
   }

   // 4) Extract data portion: from just after colon to barPos
   std::string dataSection = line.substr(colonPos + 1, barPos - (colonPos + 1));

   // 5) Remove all whitespace
   std::string dataHex;
   for (char c : dataSection)
   {
      if (!isspace((unsigned char)c))
      {
         dataHex.push_back(c);
      }
   }

   // 6) Parse dataHex in pairs of hex digits and write to memory
   Memory *mem = Memory::getInstance();
   bool mem_error = false;
   int offset = 0;

   for (size_t i = 0; i + 1 < dataHex.size(); i += 2)
   {
      std::string byteStr = dataHex.substr(i, 2);
      uint8_t byteVal = (uint8_t)strtol(byteStr.c_str(), nullptr, 16);

      mem->putByte(byteVal, address + offset, mem_error);
      if (mem_error)
      {
         std::cerr << "Memory error writing byte at address 0x"
                   << std::hex << (address + offset) << std::endl;
         break;
      }
      offset++;
   }

   // Hints:
   // Use the convert method to convert the characters
   // that represent the address into a number.
   // Also, use the convert method for each byte of data.
}

/*
 * convert
 * takes "len" characters from the line starting at character "start"
 * and converts them to a number, assuming they represent hex characters.
 * For example, if len is 2 and line[start] is '1' and
 * line[start + 1] is 'a' then this function returns 26.
 * This function assumes that the line is long enough to hold the desired
 * characters and that the characters represent hex values.
 *
 * @param line - string of characters
 * @param start - starting index in line
 * @param len - represents the number of characters to retrieve
 */
int32_t Loader::convert(std::string line, int32_t start, int32_t len)
{
   // return strtol(line.c_str(), nullptr, 16);

   std::string sub = line.substr(start, len);
   return strtol(sub.c_str(), nullptr, 16);
   // Hint: you need something to convert a string to an int such as strtol
}

/*
 * hasErrors
 * Returns true if the line file has errors in it and false
 * otherwise.
 *
 * @param line - a string that contains a line from a .yo file
 * @return true, if the line has errors
 *         false, otherwise
 */
bool Loader::hasErrors(std::string line)
{
   /*
    */
   // checking for errors in a particular order can significantly
   // simplify your code

   // 1) line is at least COMMENT characters long and contains a | in
   //    column COMMENT. If not, return true
   //    Hint: use hasComment
   if (!hasComment(line))
      return true;

   // 2) check whether line has an address.  If it doesn't,
   //    return result of isSpaces (line must be all spaces up
   //    to the | character)
   //    Hint: use hasAddress and isSpaces
   if (!hasAddress(line))
   {
      if (!isSpaces(line, 0, COMMENT))
         return true;
      return false;
   }

   // 3) return true if the address is invalid
   //    Hint: use errorAddress
   if (errorAddr(line))
      return true;

   // 4) check whether the line has data. If it doesn't
   //    return result of isSpaces (line must be all spaces from
   //    after the address up to the | character)
   //    Hint: use hasData and isSpaces
   size_t colonPos = line.find(':');
   if (colonPos == std::string::npos)
   {
      return true;
   }

   if (!hasData(line))
   {
      if (!isSpaces(line, colonPos + 1, COMMENT))
         return true;
      return false;
   }

   // 5) if you get past 4), line has an address and data. Check to
   //    make sure the data is valid using errorData
   //    Hint: use errorData
   int32_t numDBytes = 0;
   if (errorData(line, numDBytes))
      return true;

   // 6) if you get past 5), line has a valid address and valid data.
   //    Make sure that the address on this line is > the last address
   //    stored to (lastAddress is a private data member)
   //    Hint: use convert to convert address to a number and compare
   //    to lastAddress
   int32_t addr = convert(line, ADDRBEGIN, colonPos - ADDRBEGIN);
   if (addr <= lastAddress)
      return true;

   // 7) Make sure that the last address of the data to be stored
   //    by this line doesn't exceed the memory size
   //    Hint: use numDBytes as set by errorData, MEMSIZE in Memory.h,
   //          and addr returned by convert
   if (addr + numDBytes > MEMSIZE)
      return true;

   // 8)Consecutive Coluns error
   int i = colonPos + 2; // skip : 

   // Skip any leading spaces in the data field
   while (i < (int)line.size() && isspace((unsigned char)line[i]))
   {
      i++;
   }

   // Read a continuous block of hex digits
   while (i < (int)line.size() && isxdigit((unsigned char)line[i]))
   {
      i++;
   }

   while (i < (int)line.size() && line[i] != '|')
   {
      if (!isspace((unsigned char)line[i]))
      {
         return true;
      }
      i++;
   }

   
   // 9)Space after Colon error
   if (colonPos + 1 >= line.size() || line[colonPos + 1] != ' ')
   {
      return true;
   }

   // if control reaches here, no errors found
   lastAddress = addr + numDBytes - 1;
   return false;
}

/*
 * errorData
 * Called when the line contains data. It returns true if the data
 * in the line is invalid.
 *
 * Valid data consists of characters in the range
 * '0' .. '9','a' ... 'f', and 'A' .. 'F' (valid hex digits).
 * The data digits start at index DATABEGIN.
 * The hex digits come in pairs, thus there must be an even number of them.
 * In addition, the characters after the last hex digit up to the
 * '|' character at index COMMENT must be spaces.
 * If these conditions are met, errorData returns false, else errorData
 * returns true.
 *
 * @param line - input line from the .yo file
 * @return numDBytes is set to the number of data bytes on the line
 */
bool Loader::errorData(std::string line, int32_t &numDBytes)
{
   // Hint: use isxdigit and isSpaces
   if (line.size() < COMMENT)
      return true;
   std::string dataStr = line.substr(DATABEGIN, COMMENT - DATABEGIN);
   // Trim trailing spaces.
   size_t endPos = dataStr.find_last_not_of(' ');
   if (endPos != std::string::npos)
      dataStr = dataStr.substr(0, endPos + 1);

   int countHex = 0;
   for (size_t i = 0; i < dataStr.size(); i++)
   {
      if (isspace((unsigned char)dataStr[i]))
         continue;
      if (!isxdigit((unsigned char)dataStr[i]))
         return true;
      countHex++;
   }
   if (countHex == 0 || (countHex % 2 != 0))
      return true;

   numDBytes = countHex / 2;
   return false;
}

/*
 * errorAddr
 * This function is called when the line contains an address in order
 * to check whether the address is properly formed.  An address must be of
 * this format: 0xHHH: where HHH are valid hex digits.
 *
 * @param line - input line from a .yo input file
 * @return true if the address is not properly formed and false otherwise
 */
bool Loader::errorAddr(std::string line)
{
   // Hint: use isxdigit
   if (line.size() < 4)
      return true;
   if (line[0] != '0' || line[1] != 'x')
      return true;
   size_t colonPos = line.find(':');
   if (colonPos == std::string::npos)
      return true;
   for (size_t i = 2; i < colonPos; i++)
   {
      if (!isxdigit((unsigned char)line[i]))
         return true;
   }
   return false;
}

/*
 * isSpaces
 * This function checks that characters in the line starting at
 * index start and ending at index end are all spaces.
 * This can be used to check for errors
 *
 * @param line - string containing a line from a .yo file
 * @param start - starting index
 * @param end - ending index
 * @return true, if the characters in index from start to end are spaces
 *         false, otherwise
 */
bool Loader::isSpaces(std::string line, int32_t start, int32_t end)
{
   // for (int i = start; i < end; i++)
   // {
   //    if (!line[i] != (' '))
   //    {
   //       return false;
   //    }
   // }
   // return true;

   if (start < 0)
      start = 0;
   if (end > (int)line.size())
      end = line.size();
   for (int i = start; i < end; i++)
   {
      if (line[i] != ' ')
         return false;
   }
   return true;
}

/*
 * isLoaded
 * getter for the private loaded data member
 */
bool Loader::isLoaded()
{
   return loaded;
}

/*
 * badFile
 * returns true if the name of the file passed in is an improperly
 * formed .yo filename. A properly formed .yo file name is at least
 * four characters in length and ends with a .yo extension.
 *
 * @return true - if the filename is improperly formed
 *         false - otherwise
 */
bool Loader::badFile(std::string filename)
{
   // Hint: use std::string length method and C strcmp (or std::string find
   //       or std::string at or ...)
   if (filename.size() < 4)
      return true;
   if (filename.substr(filename.size() - 3) != ".yo")
      return true;
   return false;
}
