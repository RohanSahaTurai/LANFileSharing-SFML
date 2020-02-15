#include <pthread.h>
#include <pthread.h>
#include "File.hpp"
#include "LAN.hpp"

LAN Network;

void StartServerThreads()
{
  pthread_t threadID[2];

  if (!Network.StartServer())
  {
    std::cout<<"Could not connect server!\n";
    return;
  }

  InputFile file;

  if(!file.OpenFile())
    return;

  char* name;
  size_t size;

  file.GetNameSize(&name, size);

  if (!Network.SendNameSize(name, size))
    return;

  if (pthread_create(&threadID[0], NULL, &InputFile::Start_ReadCharThread, &file))
  {
    std::cout<<"Could not create Read Char Thread\n";
    return;
  }

  if (pthread_create(&threadID[1], NULL, &LAN::StartSendThread, &Network))
  {
    std::cout<<"Could not create Send Thread\n";
    return;
  }

  if (pthread_join(threadID[0], NULL))
    std::cout<<"Could not join read thread\n";


  if (pthread_join(threadID[1], NULL))
    std::cout<<"Could not join send thread\n";
}


void StartClientThreads()
{
  pthread_t threadID[2];

  if (!Network.ConnectToServer())
    return;

  std::string name;
  size_t size;

  if (!Network.ReceiveNameSize(name, size))
    return;

  OutputFile file(name.c_str(), size);

  if (pthread_create(&threadID[0], NULL, &LAN::StartReceiveThread, &Network))
  {
    std::cout<<"Could not create Receive Thread\n";
    return;
  }

  if (pthread_create(&threadID[1], NULL, &OutputFile::Start_WriteCharThread, &file))
  {
    std::cout<<"Could not create Write Char Thread\n";
    return;
  }

  if (pthread_join(threadID[0], NULL))
    std::cout<<"Could not join receive thread\n";

  if (pthread_join(threadID[1], NULL))
    std::cout<<"Could not join read thread\n";
}

int main()
{
  char input;

  while (1)
  {
    std::cout<<"Do you want to send (s) or receive (r)?\n> ";
    std::cin>>input;

    while (getchar() != '\n');

    if (input == 's')
      StartServerThreads();

    else if (input == 'r')
      StartClientThreads();

    else
    {
      std::cout<<"Invalid Input!"<<std::endl;
      continue;
    }

    break;
  }

  system("pause");

  return 0;
}
