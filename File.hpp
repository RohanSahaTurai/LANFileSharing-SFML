#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <string.h>
#include "FIFO.hpp"

extern FIFO fifo;

class InputFile
{
    private:

      FILE* fp;             // File pointer
      char filepath[263];   // Path to the file
      char filename[128];   // File name
      std::size_t filesize; // File size

      //read file and put in the TxFIFO (thread)
      void ReadChar ();

    public:

      //default constructor
      InputFile();

      //initializes the input file
      bool OpenFile();

      // void get file name and size
      void GetNameSize (char** name, size_t& size);

      // static function to start the thread
      static void* Start_ReadCharThread (void* arg);

      //destructor
      ~InputFile();
};

class OutputFile
{
    private:

      FILE* fp;               // File pointer
      std::string filename;   // File name
      std::size_t filesize;   // File size

      //get data from RxFIFO and write in the file(thread)
      void WriteChar ();

    public:

      //default constructor
      OutputFile(const char* name, const size_t size);

      // static function to start the thread
      static void* Start_WriteCharThread (void* arg);

      //destructor
      ~OutputFile();
};


#endif // FILE_H
