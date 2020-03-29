#include <iostream>
#include <thread>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "chatter.h"
#define PORT 4567
#define BUF_SIZE 256
#define CLADDR_LEN 100
using boost::asio::ip::tcp;
using namespace std;
typedef boost::shared_ptr<tcp::socket> sockPtr;
 typedef boost::shared_ptr<Chatter> pointer;

pointer Chatter::create(boost::asio::io_service& io_service, std::string userName){
    return pointer (new Chatter(boost::shared_ptr<tcp::socket>(new tcp::socket(io_service)), userName));
}
pointer Chatter::create(boost::asio::io_service& io_service){
    create(io_service,"Bob");
}

Chatter::Chatter(sockPtr socket) : mySock_(socket), userName("Bob")
{
}
Chatter::Chatter(sockPtr socket, string name) : mySock_(socket), userName(name) {}
Chatter::~Chatter()
{
    mySock_.reset();
}
sockPtr Chatter::socket(){
    return mySock_;
}
void Chatter::read()
{
    boost::array<char, 256> buf;
    boost::system::error_code error;
    size_t len = boost::asio::read(*mySock_, boost::asio::buffer(buf), error);
    std::cout.write(buf.data(), len);
    if (error == boost::asio::error::eof){
        std::cout << "time to die" << std::endl; // Connection closed cleanly by peer.
        delete this;
        }
    else if (error)
        throw boost::system::system_error(error); // Some other error.
}
void Chatter::write()
{
    string buf;
    cout<< userName << "> ";
    getline(cin, buf);
    boost::system::error_code error;
    boost::asio::write(*mySock_, boost::asio::buffer(buf), error);
    if (error == boost::asio::error::eof){
        std::cout << "time to die" << std::endl; // Connection closed cleanly by peer.
        delete this;
        }
    else if (error)
        throw boost::system::system_error(error); // Some other error.
}

void Chatter::run(){
    thread t1,t2;
    t1=thread([this](){
        while(true){
            read();
        }
    });
       t2=thread([this](){
        while(true){
            write();
        }
    });
}