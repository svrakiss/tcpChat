#include <iostream>
#include <thread>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "chatter.hpp"
#define PORT 4567
#define BUF_SIZE 256
#define CLADDR_LEN 100
using boost::asio::ip::tcp;
using namespace std;
typedef boost::shared_ptr<tcp::socket> sockPtr;

Chatter::pointer Chatter::create(boost::asio::io_service &io_service, std::string userName)
{
    return pointer(new Chatter(boost::shared_ptr<tcp::socket>(new tcp::socket(io_service)), userName));
}
Chatter::pointer Chatter::create(boost::asio::io_service &io_service)
{
    return create(io_service, "Bob");
}
Chatter::pointer Chatter::create(sockPtr sockPtr,string userName){
    return pointer(new Chatter(sockPtr,userName));
}
Chatter::Chatter(sockPtr socket) : mySock_(socket), userName("Bob")
{
}
Chatter::Chatter(sockPtr socket, string name) : mySock_(socket), userName(name) {}

Chatter::~Chatter()
{
    mySock_.reset();
}
sockPtr Chatter::socket()
{
    return mySock_;
}
void Chatter::read()
{
    // cout<<"Hello from read"<<endl;
    boost::array<char, 256> buf;
    boost::system::error_code error;
    size_t len = boost::asio::read(*mySock_, boost::asio::buffer(buf), error);
    std::cout.write(buf.data(), len);
    cout << endl;
    if (error == boost::asio::error::eof)
    {
        std::cout << "time to die" << std::endl; // Connection closed cleanly by peer.
        delete this;
    }
    else if (error)
        throw boost::system::system_error(error); // Some other error.
}
void Chatter::write(const boost::system::error_code &error, size_t bytes_transferred)
{
    // cout<<"Hello from write"<<endl;
    // string buf;
    // cout<< userName << "> ";
    // getline(cin, buf);
    // boost::asio::write(*mySock_, boost::asio::buffer(buf), error);
    if (error == boost::asio::error::eof)
    {
        std::cout << "time to die" << std::endl; // Connection closed cleanly by peer.
        delete this;
    }
    else if (error)
        throw boost::system::system_error(error); // Some other error.
}

void Chatter::run()
{
    thread t1, t2;
    boost::shared_ptr<Chatter> p1=getMe();
    boost::shared_ptr<Chatter> p2=getMe();
    // cout<<"i'm about to do some runnin"<<endl;
    t1 = thread([p1]() {
        while (true)
        {
            // cout <<"thread 1"<<endl;
            p1->read();
        }
    });
    t2 = thread([p2]() {
        std::string buf;
        while (true)
        {
            getline(cin, buf);
            //    cout << "thread 2"<<endl;
            boost::asio::async_write(*p2->socket().get(), boost::asio::buffer(buf),
                                     boost::bind(&Chatter::write, p2, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    });
    t1.join();
    t2.join();
}