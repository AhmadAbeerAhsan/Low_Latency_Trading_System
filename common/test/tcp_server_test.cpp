#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <tcp_async_server.hpp>


int main()
{
  try
  {
    boost::asio::io_context io_context;
    tcp_async_server server(io_context, 13);
    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}