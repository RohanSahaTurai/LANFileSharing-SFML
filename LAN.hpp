#ifndef LAN_H
#define LAN_H

#include "SFML/Network.hpp"
#include <iostream>
#include "FIFO.hpp"

extern FIFO fifo;

class LAN
{
    private:

      const uint16_t  port;
      sf::TcpSocket   socket;
      sf::IpAddress   myIPAddress;

    public:

      //default constructor
      LAN();

      //creates a server and listens for client
      bool StartServer();

      //connects to a server if available
      bool ConnectToServer();

      //sends the file size and name
      bool SendNameSize (const char* name, const size_t size);

      //receives the file size and name
      bool ReceiveNameSize (std::string& name, size_t& size);

      //sends data from the FIFO
      void SendPacket();

      //receives data and puts into the FIFO
      void ReceivePacket();

      // Function to start a send thread
      static void* StartSendThread(void* arg);

      // Function to start a receive thread
      static void* StartReceiveThread(void* arg);

      //destructor
      ~LAN();
};

#endif // LAN_H
