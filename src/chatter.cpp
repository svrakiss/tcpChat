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
}

sockPtr Chatter::socket()
{
    return mySock_;
}
boost::array<char, 256> &Chatter::getBuf()
{
    return buf;
}
// boost::array<char, 256> &Chatter::getSentMsg()
// {
//     return sentMsg;
// }
// std::ostream &Chatter::getOut()
// {
//     return mine;
// }
void Chatter::read(const boost::system::error_code &error, size_t bytes_transferred)
{
    // std::cout<<"Hello from read"<<std::endl;
    // std::cout<<std::flush;
    write(getBuf().data(), bytes_transferred);
    // getOut()<<"\n";
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
void Chatter::addMessage(ChatMessage &chatmessage)
{
    socket()->get_io_service().post(boost::bind(&Chatter::handleWrite, getMe(), chatmessage));
}
void Chatter::handleWrite(ChatMessage &msg)
{
    bool busy = !writeQueue.empty();
    writeQueue.push_back(msg);
    if (!busy)
    {
        boost::asio::async_write(*socket(), boost::asio::buffer(writeQueue.front().getHeader() + writeQueue.front().getData()),
                                 boost::bind(&Chatter::write, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
}
void Chatter::write(const boost::system::error_code &error, size_t bytes_transferred)
{
    // std::cout<<"Hello from write"<<std::endl;
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
        writeQueue.pop_front();
        if (!writeQueue.empty())
            boost::asio::async_write(*socket(), boost::asio::buffer(writeQueue.front().getHeader() + writeQueue.front().getData()),
                                     boost::bind(&Chatter::write, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
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
        p1->socket()->async_read_some(boost::asio::buffer(p1->getBuf()),
                                      boost::bind(&Chatter::read, p1->shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    });
    boost::thread t2([p2]() {
        std::string buffer;
        while (true)
        {
            // std::cout << boost::this_thread::get_id() << "\n";
            std::cout << p2->userName << ">";
            std::getline(std::cin, buffer);
            ChatMessage chat(buffer, p2->userName);
            p2->addMessage(chat);
            //    cout << "thread 2"<<endl;
            // p2->socket()->async_write_some(boost::asio::buffer(p2->userName + "> " + buffer + "\n"),
            //                                boost::bind(&Chatter::write, p2, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    });
    t1.join();
    t2.join();
}