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
    // cout<<"Hello from read"<<endl;
    std::cout.write(getBuf().data(), bytes_transferred);
    std::cout << "\n";
    if (error == boost::asio::error::eof)
    {
        std::cout << "[reader] dying" << std::endl; // Connection closed cleanly by peer.
        die();
    }
    else if (error)
    {
        die();
        throw boost::system::system_error(error); // Some other error.
    }
    boost::asio::async_read(*socket().get(), boost::asio::buffer(getBuf()), boost::bind(&Chatter::read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
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
        std::cout << "[writer] dying" << std::endl; // Connection closed cleanly by peer.
        die();
    }
    else if (error)
    {
        die();
        throw boost::system::system_error(error); // Some other error.
    }

    std::string buffer;
    std::cout << userName << ">";
    std::getline(std::cin, buffer);
    boost::asio::async_write(*socket().get(), boost::asio::buffer(userName + "> " + buffer),
                             boost::bind(&Chatter::write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
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
            boost::asio::async_read(*p1->socket().get(), boost::asio::buffer(p1->getBuf()),
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
            boost::asio::async_write(*p2->socket().get(), boost::asio::buffer(p2->userName + "> " + buffer),
                                     boost::bind(&Chatter::write, p2, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    });
    t1.join();
    t2.join();
}