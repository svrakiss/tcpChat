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
Chatter::Chatter(sockPtr socket, std::string name) : mySock_(socket), userName(name), writeQueue(std::deque<ChatMessage>()) {}
Chatter::~Chatter() {}
void Chatter::die()
{
    std::cout << "REALLY?" << std::endl;
    mySock_->close();
    // this_thread.interrupt();
    boost::this_thread::yield();
}

sockPtr Chatter::socket()
{
    return mySock_;
}
boost::array<char, 256> &Chatter::getBuf()
{
    return buf;
}
boost::array<std::size_t, 3> &Chatter::getHeadBuf()
{
    return headbuf;
}
void Chatter::readHeader(const boost::system::error_code &error)
{
    std::cout << "decoding header";
    if (!error)
    {
        size_t msg_length = getHeadBuf().c_array()[0];
        if (msg_length != 0) // time to read the full message
        {
            boost::asio::async_read(*socket(), boost::asio::buffer(getBuf(), msg_length),
                                    boost::bind(&Chatter::read, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
        else //empty message-->>go get a header
        {
            boost::asio::async_read(*socket(), boost::asio::buffer(getHeadBuf(), sizeof(boost::array<std::size_t, 3>)),
                                    boost::bind(&Chatter::readHeader, getMe(), boost::asio::placeholders::error));
        }
    }
    else
    {
        die();
    }
}
void Chatter::read(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    std::cout << "Hello from read" << std::endl;
    // std::cout<<std::flush;

    // getOut()<<"\n";
    if (error)
    {
        std::cout << "[reader] dying" << std::endl; // Connection closed cleanly by peer.
        die();
    }
    else
    {
        std::cout.write(getBuf().data(), bytes_transferred);
        boost::asio::async_read(*socket(), boost::asio::buffer(getHeadBuf(), sizeof(boost::array<std::size_t, 3>)),
                                boost::bind(&Chatter::readHeader, getMe(), boost::asio::placeholders::error));
    }
}
void Chatter::addMessage(ChatMessage &chatmessage)
{
    std::cout << "Hello from adddmsg" << std::endl;

    socket()->get_io_service().dispatch(boost::bind(&Chatter::handleWrite, getMe(), chatmessage));
}
void Chatter::handleWrite(ChatMessage &msg)
{
    std::cout << "Hello from handlewrite1" << std::endl;

    bool busy = !writeQueue.empty();
    std::cout << "Hello from handlewrite2" << std::endl;

    writeQueue.push_back(msg);
    std::cout << "Hello from handlewrite3" << std::endl;

    if (!busy)
    {
        std::vector<boost::asio::const_buffer> buffy = {boost::asio::buffer(writeQueue.front().getHeader()), boost::asio::buffer(writeQueue.front().getData())};
        boost::asio::async_write(*socket(),
                                 buffy,
                                 boost::bind(&Chatter::write, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
}
void Chatter::write(const boost::system::error_code &error, size_t bytes_transferred)
{
    std::cout << "Hello from write" << std::endl;
    // string buf;
    // cout<< userName << "> ";
    // getline(cin, buf);
    // boost::asio::write(*mySock_, boost::asio::buffer(buf), error);
    if (error)
    {
        std::cout << "[writer] dying" << std::endl; // Connection closed cleanly by peer.
        die();
    }
    else

    {
        // std::vector<boost::asio::buffer> bv;
        writeQueue.pop_front();
        if (!writeQueue.empty())
        { //const buffers are for sending, mutable buffers are for receiving
            std::vector<boost::asio::const_buffer> buffy = {boost::asio::buffer(writeQueue.front().getHeader()), boost::asio::buffer(writeQueue.front().getData())};
            boost::asio::async_write(*socket(),
                                     buffy,
                                     boost::bind(&Chatter::write, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    }
}

void Chatter::run()
{
    boost::shared_ptr<Chatter> p1 = getMe();
    boost::shared_ptr<Chatter> p2 = getMe();
    // boost::thread  t1, t2;
    // cout<<"i'm about to do some runnin"<<endl;
    boost::thread t1([p1]() {
        // while (true)
        std::cout << boost::this_thread::get_id() << " " + p1->userName << "\n";
        // cout <<"thread 1"<<endl;
        boost::asio::async_read(*p1->socket(), boost::asio::buffer(p1->getBuf(), sizeof(boost::array<std::size_t, 3>)),
                                boost::bind(&Chatter::readHeader, p1->shared_from_this(), boost::asio::placeholders::error));
    });
    boost::thread t2([p2]() {
        std::string buffer;
        ;
        while (true)
        {
            // std::cout << boost::this_thread::get_id() << "\n";
            std::cout << p2->userName << ">";
            std::getline(std::cin, buffer);
            ChatMessage chat(buffer, p2->userName);
            chat.getHeader();
            p2->addMessage(chat);
            p2->socket()->get_io_service().poll();
            //    cout << "thread 2"<<endl;
            // p2->socket()->async_write_some(boost::asio::buffer(p2->userName + "> " + buffer + "\n"),
            //                                boost::bind(&Chatter::write, p2, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    });
    t1.join();
    t2.join();
}