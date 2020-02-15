#include "File.hpp"

static size_t ExpectedReceivedSize;

// FIFO for storing the file buffer
FIFO fifo;

/////////////////////////////////////////////////////////
// Input file class
/////////////////////////////////////////////////////////
InputFile::InputFile()
{

}

bool InputFile::OpenFile()
{
  // Common variables
  int i, j;

  /*Get the file path*/
  std::cout<<"Enter the absolute file path: ";
  gets(filepath);

  bool containsQuotes = false;

  /*Remove quotes (if any) and extract the filename*/
  for (i = 0, j = 0; filepath[i] != '\0'; i++, j++)
  {
    if (filepath[0] == '"')
    {
      containsQuotes = true;
      strcpy(&filepath[0], &filepath[1]);
    }
    if (filepath[i] == '\\')
    {
       j = 0;  i++;
    }
    filename[j] = filepath[i];
  }

  // Remove the last quote from the path (if any)
  if (containsQuotes)
  {
    filepath[--i] = '\0';
    filename[--j] = '\0'; // Null Terminate the filename
  }
  else
    filename[j] = '\0';

  /*Open the file*/
  fp = fopen(filepath, "rb");

  if (!fp)
  {
    fputs("Error Opening File!\n", stderr);
    return false;
  }

  std::cout<<"File Name: "<<filename<<std::endl;

  // Get the file size
  fseek(fp, 0, SEEK_END);
  filesize = ftell(fp);
  rewind(fp);

  std::cout<<"File Size: "<<filesize*sizeof(char)<<" bytes"<<std::endl;

  return true;;
}

void InputFile::GetNameSize(char** name, size_t& size)
{
  *name = filename;
  size = filesize;
}

void InputFile::ReadChar()
{
  int buffer = 0;

  //load the file in the FIFO
  while (buffer != EOS)
  {
    buffer = fgetc(fp);

    if (ferror(fp))
      fputs("Error reading file!", stderr);

    if (feof(fp))
       buffer = EOS;

    ::fifo.FIFO_PutChar(buffer);
  }

  std::cout<<"File Read Complete"<<std::endl;
}

void* InputFile::Start_ReadCharThread (void* arg)
{
  static_cast<InputFile*>(arg)->ReadChar();
  return NULL;
}

InputFile::~InputFile()
{
  if (!fp)
    fclose(fp);
}

/////////////////////////////////////////////////////////
// Output file class
/////////////////////////////////////////////////////////
OutputFile::OutputFile(const char* name, const size_t expectedSize)
: filename(name)
{
  fp = fopen(filename.c_str(), "wb");

  if (!fp)
    fputs("Error opening output file!\n", stderr);

  ExpectedReceivedSize = expectedSize;
}

void OutputFile::WriteChar()
{
  int buffer = 0;

  while (1)
  {
    ::fifo.FIFO_GetChar(buffer);

    if (buffer == EOS)
      break;

    fputc(buffer, fp);
  }

  // get the size and check
  fseek(fp, 0, SEEK_END);
  filesize = ftell(fp);
  rewind(fp);

  if (filesize == ExpectedReceivedSize)
    std::cout<<"File Write Complete"<<std::endl;

  else
    std::cout<<"Expected Size and Read Size doesn't match"<<std::endl;
}

void* OutputFile::Start_WriteCharThread (void* arg)
{
  static_cast<OutputFile*>(arg)->WriteChar();
  return NULL;
}

OutputFile::~OutputFile()
{
  fclose(fp);
}

