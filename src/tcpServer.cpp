#include <ctime>
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "chatter.hpp"
using boost::asio::ip::tcp;
#define DEFAULT_PORT 4567
class tcpServer
{
public:
    std::string userName;
    tcpServer(boost::asio::io_service &io_service, int portno) : acceptor_(io_service, tcp::endpoint(tcp::v4(), portno))
    {
        prompt();
        spin();
    }
    tcpServer(boost::asio::io_service &io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), DEFAULT_PORT))
    {
        prompt();
        spin();
    }
    void prompt()
    {
        std::cout << "Please enter a user name:";
        std::getline(std::cin, userName);
        if(std::strpbrk(KILL_WORD,userName.c_str())!=NULL){
            std::cout <<"contains reserved word "<<KILL_WORD<<'\n';
            prompt();
        }

    }

private:
    // run until stopped
    void spin()
    {
        // initiates an asynchronous accept operation
        // to wait for a new connection.
        // std::cout << "I'm spinning" << std::endl;
        Chatter::pointer spongeBoyMeBob = Chatter::create(acceptor_.get_io_service(), userName);
        // std::cout << "I made a chatter (pointer)" << std::endl;
        acceptor_.async_accept(*spongeBoyMeBob->socket(), boost::bind(&tcpServer::handleAccept, this, spongeBoyMeBob, boost::asio::placeholders::error));
        // std::cout << "I just did something with bind and aysnc_accept" << std::endl;
    }
    void handleAccept(Chatter::pointer chatter, const boost::system::error_code &error)
    {
        // std::cout << "I'm in handle accept" << std::endl;
        if (!error)
        {

            // std::cout << "did  something just happen?" << std::endl;

            chatter->run();
        }
        // std::cout << "Onward!" << std::endl;
        // goes to accept next connection
        spin();
    }
    tcp::acceptor acceptor_;
};
int main(int argc, char *argv[])
{
    unsigned short portno=DEFAULT_PORT;
    if (argc != 2)
    {
        std::cout << "usage: <optional Port no.>" << '\n';
        portno = DEFAULT_PORT;
    }
    else
        portno = atoi(argv[1]);
    boost::asio::io_service io_service;
    tcpServer tcpServer(io_service, portno);
    io_service.run();
    return 0;
}