
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/format.hpp>

#include <map>
#include <vector>
using boost::asio::ip::udp;
using namespace std;


map <string,int> lines;
vector<std::string> screen;

class udp_receiver
{
public:
  udp_receiver(boost::asio::io_service& io_service)
    : socket(io_service, udp::endpoint(udp::v4(), 5555))
  {
    start_receive();
  }

private:
  void start_receive()
  {
    socket.async_receive_from(
			       boost::asio::buffer(recv_buffer,1000), remote_endpoint,
			       boost::bind(&udp_receiver::handle_receive, this,
					   boost::asio::placeholders::error,
					   boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive(const boost::system::error_code& error,
      std::size_t bytes_transferred)
  {
    if (!error || error == boost::asio::error::message_size)
    {

    string hash=boost::str(boost::format("%s:%i") % remote_endpoint.address().to_string() %remote_endpoint.port());
   
    if (lines.find(hash)==lines.end())
      {
	lines[hash]=screen.size();
	screen.push_back("");
	}    
    unsigned int line=lines[hash];
    recv_buffer[bytes_transferred]=0;
    string s=string(recv_buffer);
    if (s.length()>60) s="..."+s.substr(s.length()-57,57);
    if (hash.length()>9)
      screen[line]=hash.substr(hash.length()-9,9)+" "+s;
    else
      screen[line]=hash+" "+s;
    cout<<"\033[2J";
    for (unsigned int i=0;i<screen.size();i++)
      {
	int x=i %35;
	int y=78*(i/35);
      if (i==line)
	cout<<"\033["<<x<<";"<<y<<"H\033[1;32m"<<i<<" "<<screen[i]<<"\033[0;37m";
      else
	cout<<"\033["<<x<<";"<<y<<"H"<<i<<" "<<screen[i];
}
    start_receive();
    }
  }
  udp::socket socket;
  udp::endpoint remote_endpoint;
 char recv_buffer[1000];
};

int main()
{
  try
  {
    boost::asio::io_service io_service;
    udp_receiver server(io_service);
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
