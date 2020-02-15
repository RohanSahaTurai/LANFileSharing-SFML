#include "LAN.hpp"

static size_t ReceivedFileSize;
static size_t SendFileSize;

LAN::LAN()
:port(5555)
{
  myIPAddress = sf::IpAddress::getLocalAddress();
}

bool LAN::StartServer()
{
  std::cout<<"Running Server Mode..."<<std::endl;
  std::cout<<"Server IPv4: "<<myIPAddress<<std::endl;
  std::cout<<"Waiting for client to connect..."<<std::endl;

  sf::TcpListener listener;
  sf::TcpListener::Status status;

  status = listener.listen(port);

  if (status != sf::Socket::Done)
  {
    std::cout<<"TCP Listener error!"<<std::endl;
    return false;
  }

  status = listener.accept(socket);

  if (status != sf::Socket::Done)
  {
    std::cout<<"Error binding socket to client!"<<std::endl;
    return false;
  }

  std::cout<<"Client Connected! IPv4: "<<socket.getRemoteAddress()<<std::endl;

  return true;
}

bool LAN::ConnectToServer()
{
  std::string IPstring;

  std::cout<<"Running Client Mode..."<<std::endl;
  std::cout<<"Enter the IPv4 of the server> ";
  std::cin>>IPstring;

  sf::IpAddress serverIP(IPstring);

  if (socket.connect(serverIP, port) == sf::Socket::Done)
  {
    std::cout<<"Connected to server!"<<std::endl;
    return true;
  }

  else
  {
    std::cout<<"Error connecting to server!"<<std::endl;
    return false;
  }
}

bool LAN::SendNameSize(const char* name, const size_t size)
{
  sf::Packet packet;

  packet<<name;
  packet<<size;

  SendFileSize = size;

  if (socket.send(packet) != sf::Socket::Done)
    return false;

  return true;
}

bool LAN::ReceiveNameSize(std::string& name, size_t& size)
{
  sf::Packet packet;

  if (socket.receive(packet) != sf::Socket::Done)
    return false;

  packet>>name;
  packet>>size;

  std::cout<<"Filename: "<<name<<std::endl;
  std::cout<<"Size: "<<size<<"bytes"<<std::endl;

  ReceivedFileSize = size;

  return true;
}

void LAN::SendPacket()
{
  sf::Packet packet;
  packet.clear();

  while (fifo.FIFO_GetLoadingStatus() != true ||
         fifo.FIFO_GetNBPacketsLoaded() != 0)
  {
    ::fifo.FIFO_GetPacket(packet);

    if (socket.send(packet) != sf::Socket::Done)
    {
       std::cout<<"Error sending file!\n";
       return;
    }

    else
      std::cout<<"Packet Sent"<<std::endl;
  }

  std::cout<<"File Sent"<<std::endl;

}

void LAN::ReceivePacket()
{
  sf::Packet packet;
  packet.clear();

  //set the socket in non-blocking mode
  socket.setBlocking(false);

  // one extra byte to be sent for EOF
  while (fifo.FIFO_GetLoadingStatus() != true)
  {
    if (socket.receive(packet) == sf::Socket::Done)
    {
      ::fifo.FIFO_PutPacket(packet);

      std::cout<<"Packet Received"<<std::endl;
    }
  }

  std::cout<<"File Received"<<std::endl;
}

void* LAN::StartSendThread(void* arg)
{
  static_cast<LAN*>(arg)->SendPacket();
  return NULL;
}

void* LAN::StartReceiveThread(void* arg)
{
  static_cast<LAN*>(arg)->ReceivePacket();
  return NULL;
}

LAN::~LAN()
{

}
