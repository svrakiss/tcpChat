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
class tcpServer
{
public:
std::string userName;
    tcpServer(boost::asio::io_service &io_service, unsigned short portno) : acceptor_(io_service, tcp::endpoint(tcp::v4(), portno)) { spin(); }
    tcpServer(boost::asio::io_service &io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), 4567)) { spin(); }

private:
    // run until stopped
    void spin()
    {
        // initiates an asynchronous accept operation
        // to wait for a new connection.

        Chatter::pointer spongeBoyMeBob = Chatter::create(acceptor_.get_io_service(),userName);
        acceptor_.async_accept(*spongeBoyMeBob->socket(), boost::bind(&tcpServer::handleAccept, this, spongeBoyMeBob, boost::asio::placeholders::error));
    }
    void handleAccept(Chatter::pointer chatter, boost::system::error_code& error)
    {
        if(!error){
            chatter->run();
        }
        // goes to accept next connection
        spin();
    }
    tcp::acceptor acceptor_;
};
int main()
{
    boost::asio::io_service io_service;
    tcpServer tcpServer(io_service);
    io_service.run();
    return 0;
}