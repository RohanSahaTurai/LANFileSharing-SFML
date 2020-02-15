#ifndef FIFO_H
#define FIFO_H

#include <inttypes.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include "SFML/Network.hpp"

#define EOS -1

#define FIFO_SIZE 20
#define MAX_PACKET_SIZE 50000000 //50 MB

class FIFO
{
    private:

      uint16_t      GetIndex;                 // Index of where to get the data from
      uint16_t      PutIndex;                 // Index of where of next empty space
      sem_t         NbBytesFree_Semaphore;    // Semaphore counting spaces free in buffer
      sem_t         NbBytesFilled_Semaphore;  // Semaphore counting spaces filled in buffer
      sf::Packet*   Packet;                   // Packet Buffer of the FIFO
      bool          LoadingComplete;          // Variable to indicate the end of loading

    public:

      // default constructor
      FIFO();

      // Function to get char data from the current packet
      bool FIFO_GetChar(int& data);

      // Function to put char data into the current packet
      bool FIFO_PutChar(const int data);

      // Function to get the current packet from the FIFO
      bool FIFO_GetPacket(sf::Packet& packet);

      // Function to put a packet from the FIFO
      bool FIFO_PutPacket(const sf::Packet packet);

      // Function to get the number of packets loaded
      int FIFO_GetNBPacketsLoaded();

      // Function to check if the loading is complete
      bool FIFO_GetLoadingStatus();

      // destructor
      ~FIFO();
};

#endif // FIFO_H
