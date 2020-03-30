#include <iostream>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "chatter.hpp"
#define PORT 4567
#define BUF_SIZE 256
#define CLADDR_LEN 100
using boost::asio::ip::tcp;
// using namespace std;
using boost::thread;
typedef boost::shared_ptr<tcp::socket> sockPtr;

Chatter::pointer Chatter::create(boost::asio::io_service &io_service, std::string userName)
{
    return pointer(new Chatter(boost::shared_ptr<tcp::socket>(new tcp::socket(io_service)), userName));
}
Chatter::pointer Chatter::create(boost::asio::io_service &io_service)
{
    return create(io_service, "Bob");
}
Chatter::pointer Chatter::create(sockPtr sockPtr, std::string userName)
{
    return pointer(new Chatter(sockPtr, userName));
}
Chatter::Chatter(sockPtr socket) : mySock_(socket), userName("Bob")
{
}
Chatter::Chatter(sockPtr socket, std::string name) : mySock_(socket), userName(name) {}
Chatter::~Chatter() {}
void Chatter::die()
{
    std::cout<<"REALLY?"<<std::endl;
    mySock_->close();
}

sockPtr Chatter::socket()
{
    return mySock_;
}
boost::array<char, 256> &Chatter::getBuf()
{
    return buf;
}
boost::array<char, 256> &Chatter::getSentMsg()
{
    return sentMsg;
}
void Chatter::read(const boost::system::error_code &error, size_t bytes_transferred)
{
    std::cout<<"Hello from read"<<std::endl;
    std::cout.write(getBuf().data(), bytes_transferred);
    std::cout << "\n";
    if (error == boost::asio::error::eof)
    {
        std::cout << "[reader] dying" << std::endl; // Connection closed cleanly by peer.
        die();
    }
    else if (error)
    {
        std::cerr << error.message() << std::endl;

        die();
        throw boost::system::system_error(error); // Some other error.
    }
    socket()->async_read_some(boost::asio::buffer(getBuf()), boost::bind(&Chatter::read, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}
void Chatter::write(const boost::system::error_code &error, size_t bytes_transferred)
{
    // std::cout<<"Hello from write"<<std::endl;
    // string buf;
    // cout<< userName << "> ";
    // getline(cin, buf);
    // boost::asio::write(*mySock_, boost::asio::buffer(buf), error);
    if (error == boost::asio::error::eof)
    {
        std::cout << "[writer] dying" << std::endl; // Connection closed cleanly by peer.
        die();
    }
    else if (error)
    {

        std::cerr << error.message() << std::endl;

        die();
        throw boost::system::system_error(error); // Some other error.
    }

    std::string buffer;
    std::cout << userName << ">";
    std::getline(std::cin, buffer);
    socket()->async_write_some(boost::asio::buffer(userName + "> " + buffer),
                             boost::bind(&Chatter::write, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

void Chatter::run()
{
    boost::shared_ptr<Chatter> p1 = getMe();
    boost::shared_ptr<Chatter> p2 = getMe();
    // boost::thread  t1, t2;
    // cout<<"i'm about to do some runnin"<<endl;
    boost::thread t1([p1]() {
        // while (true)
        {
            // cout <<"thread 1"<<endl;
           p1->socket()->async_read_some(boost::asio::buffer(p1->getBuf()),
                                    boost::bind(&Chatter::read, p1->shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    });
    boost::thread t2([p2]() {
        std::string buffer;
        // while (true)
        {
            std::cout << p2->userName << ">";
            std::getline(std::cin, buffer);

            //    cout << "thread 2"<<endl;
            p2->socket()->async_write_some(boost::asio::buffer(p2->userName + "> " + buffer),
                                     boost::bind(&Chatter::write, p2, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    });
    // std::cout << "STOP" << std::endl;
    t1.join();
    // std::cout << "STOP" << std::endl;

    t2.join();
    // std::cout << "STOP" << std::endl;
}