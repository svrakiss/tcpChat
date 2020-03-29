#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "chatter.h"
using boost::asio::ip::tcp;
// using namespace  boost::asio;
using namespace std;
class tcpServer{
public:
tcpServer(boost::asio::io_service& io_service,unsigned short portno):acceptor_(io_service,tcp::endpoint(tcp::v4(),portno)){spin();}
tcpServer(boost::asio::io_service& io_service):acceptor_(io_service,tcp::endpoint(tcp::v4(),4567)){spin();}

private:
// run until stopped
void spin(){
Chatter spongeBoyMeBob = Chatter(boost::shared_ptr<tcp::socket>(new tcp::socket(acceptor_.get_io_service())));
}
tcp::acceptor acceptor_;
};
int main()
{
    boost::asio::io_service io_service;
    tcpServer tcpServer(io_service);
    return 0;
}