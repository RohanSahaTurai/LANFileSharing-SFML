#include "FIFO.hpp"

static pthread_mutex_t MutextLock;

// default constructor
FIFO::FIFO()
: GetIndex(0), PutIndex(0), LoadingComplete(false)
{
  Packet = new (std::nothrow) sf::Packet[FIFO_SIZE];

  if (!Packet)
  {
    printf("Error allocating memory\n");
    exit(-1);
  }

  sem_init(&NbBytesFilled_Semaphore, 0, 0);
  sem_init(&NbBytesFree_Semaphore, 0, FIFO_SIZE);
  pthread_mutex_init(&MutextLock, NULL);
}

bool FIFO::FIFO_GetPacket(sf::Packet& packet)
{
  // Wait if FIFO is empty
  if (sem_wait(&NbBytesFilled_Semaphore))
  {
    fputs("Error waiting on NbBytesFilled Semaphore in FIFO GET!\n", stderr);
    return false;
  }

  packet = Packet[GetIndex];

  printf("Packet Get from FIFO\n");

  if (GetIndex + 1 == FIFO_SIZE)
    GetIndex = 0;
  else
    GetIndex++;

  // Increase the spaces available
  if (sem_post(&NbBytesFree_Semaphore))
  {
    fputs("Error signaling on NbBytesFree Semaphore in FIFO GET!\n", stderr);
    return false;
  }

  return true;
}

bool FIFO::FIFO_PutPacket(const sf::Packet packet)
{
  // Wait if FIFO is full
  if (sem_wait(&NbBytesFree_Semaphore))
  {
    fputs("Error waiting on NbBytesFree Semaphore in FIFO PUT!\n", stderr);
    return false;
  }

  Packet[PutIndex] = packet;

  printf("Packet Put in FIFO\n");

  if (PutIndex + 1 == FIFO_SIZE)
    PutIndex = 0;
  else
    PutIndex++;

  // Increase the number of spaces available
  if (sem_post(&NbBytesFilled_Semaphore))
  {
    fputs("Error signaling on NbBytesFilled Semaphore in FIFO PUT!\n", stderr);
    return false;
  }

  return true;
}

bool FIFO::FIFO_GetChar(int& data)
{
  static sf::Packet tempPacket;

  // Only update the get index if the Packet has been emptied
  if (tempPacket.endOfPacket() == true)
  {
    tempPacket.clear();

    FIFO_GetPacket(tempPacket);
  }

  tempPacket>>data;

  if (data == EOS)
  {
    pthread_mutex_lock(&MutextLock);
    LoadingComplete = true;
    pthread_mutex_unlock(&MutextLock);
  }

  return true;
}

bool FIFO::FIFO_PutChar(const int data)
{
  static sf::Packet tempPacket;

  tempPacket<<data;

  if (tempPacket.getDataSize() >= MAX_PACKET_SIZE || data == EOS)
  // Only update the get index if the Packet has been emptied
  {
    FIFO_PutPacket(tempPacket);

    tempPacket.clear();

    if (data == EOS)
    {
      pthread_mutex_lock(&MutextLock);
      LoadingComplete = true;
      pthread_mutex_unlock(&MutextLock);
    }
  }

  return true;
}

int FIFO::FIFO_GetNBPacketsLoaded()
{
  int fifoSize;

  if (!sem_getvalue(&NbBytesFilled_Semaphore, &fifoSize))
    return fifoSize;

  else
    return -1;
}

bool FIFO::FIFO_GetLoadingStatus()
{
  bool status;

  pthread_mutex_lock(&MutextLock);
  status = LoadingComplete;
  pthread_mutex_unlock(&MutextLock);

  return status;
}

FIFO::~FIFO()
{
  delete[] Packet;
  sem_destroy(&NbBytesFilled_Semaphore);
  sem_destroy(&NbBytesFree_Semaphore);
  pthread_mutex_destroy(&MutextLock);
}
