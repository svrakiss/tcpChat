#include <iostream>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "chatter.hpp"
#include <ncurses.h>

using boost::thread;
using boost::asio::ip::tcp;
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
    if (!isendwin()) // the first handler gets here
    {
        endwin(); // exit ncurses
        if (mySock_->is_open())
        {
            mySock_->shutdown(mySock_->shutdown_both);
            mySock_->close();
            // if (mySock_->is_open())
        }
        // pthread_exit()
        // boost::
        // mySock_->shutdown(tcp::socket::shutdown_both);
        // this_thread.interrupt();
        // boost::this_thread::yield();

        std::cout << "someone left" << std::endl;
        //    throw_error(boost::asio::placeholders::error, "read");
    }
    throw new std::exception;

    // boost::this_thread::interruption_requested();
}

sockPtr Chatter::socket()
{
    return mySock_;
}
boost::array<char, BUF_SIZE> &Chatter::getBuf()
{
    return buf;
}
boost::array<char, 5> &Chatter::getHeadBuf()
{
    return headbuf;
}
void Chatter::readHeader(const boost::system::error_code &error)
{
    // std::cout << "decoding header";
    if (!error)
    {
        size_t msg_length = ChatMessage::readHeader(getHeadBuf().c_array());
        sizenow = msg_length;
        // std::cout << "message length" << msg_length << '\n';
        if (msg_length != 0) // time to read the full message
        {
            boost::system::error_code read_error;

            std::size_t bytes = boost::asio::read(*socket(), boost::asio::buffer(getBuf(), msg_length), read_error);
            read(read_error, bytes);
            // boost::asio::async_read(*socket(), boost::asio::buffer(getBuf(), msg_length),
            // boost::bind(&Chatter::read, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
            // socket()->get_io_service().poll();
        }
        else //empty message-->>go get a header
        {
            boost::asio::async_read(*socket(), boost::asio::buffer(getHeadBuf(), ChatMessage::headerlength),
                                    boost::bind(&Chatter::readHeader, getMe(), boost::asio::placeholders::error));
        }
    }
    else
    {
        die();
    }
}
bool shouldIDie(char *data)
{
    return std::strstr(data, KILL_WORD) != NULL;
}
void Chatter::read(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    // std::cout << "Hello from read" << std::endl;
    // if (isendwin())
    // {
    //     endwin();
    // }
    // std::cout << " bytes transferred" << bytes_transferred << '\n';
    // std::cout << "sizenow is "<<sizenow<<'\n';
    // refresh();
    if (error)
    {
        std::cout << "[reader] dying" << std::endl; // Connection closed cleanly by peer.
        die();
    }
    else
    {
        ;
        auto jimmy = boost::array<char, BUF_SIZE>();
        char currentState[BUF_SIZE] = ""; // technically incorrect, but good enough
        strncpy(jimmy.data(), getBuf().data(), sizenow);
        if (shouldIDie(jimmy.data()))
            die();
        int y, x, xx;                  // to store where you are
        getyx(stdscr, y, x);           // save current pos
        move(y, 0);                    // move to begining of line
        instr(currentState);           // current text in terminal
        clrtoeol();                    // clear line
        printw(jimmy.data(), sizenow); // this copies the entire contents of the array, regardless of the amount entered
        addch('\n');
        getyx(stdscr, y, xx); // save current pos
        printw(currentState); // put it back
        move(y, x);           // move back to where you were (this moves output, not the cursor)
        // mvcur(y, xx, y, x);
        refresh(); //this makes it show up immediately
        boost::asio::async_read(*socket(), boost::asio::buffer(getHeadBuf(), ChatMessage::headerlength),
                                boost::bind(&Chatter::readHeader, getMe(), boost::asio::placeholders::error));
    }
}
void Chatter::addMessage(ChatMessage &chatmessage)
{
    // std::cout << "Hello from adddmsg" << std::endl;

    socket()->get_io_service().dispatch(boost::bind(&Chatter::handleWrite, getMe(), chatmessage));
}
void Chatter::handleWrite(ChatMessage &msg)
{
    // std::cout << "Hello from handlewrite1" << std::endl;

    bool busy = !writeQueue.empty();
    // std::cout << "Hello from handlewrite2" << std::endl;

    writeQueue.push_back(msg);
    // std::cout << "Hello from handlewrite3" << std::endl;

    if (!busy)
    {
        // std::cout << "Hello from handlewrite4" << std::endl;

        // std::cout << "sending size" << sizeof(buffy) << '\n';
        // std::cout << "header says" << writeQueue.front().getHeader().c_array()[0] << '\n';

        boost::asio::async_write(*socket(),
                                 boost::asio::buffer(writeQueue.front().getData(), writeQueue.front().getlength()),
                                 boost::bind(&Chatter::write, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
}
void Chatter::write(const boost::system::error_code &error, size_t bytes_transferred)
{
    // std::cout << "Hello from write" << std::endl;
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
        {
            boost::asio::async_write(*socket(),
                                     boost::asio::buffer(writeQueue.front().getData(), writeQueue.front().getlength()),
                                     boost::bind(&Chatter::write, getMe(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
    }
}
void Chatter::sayHello()
{
    socket()->send(boost::asio::buffer(userName, sizeof(userName)));
    std::size_t bytes = socket()->receive(boost::asio::buffer(getBuf()));

    std::cout << "Connected to " << socket()->remote_endpoint().address().to_string() << "  (";
    std::cout.write(getBuf().data(), bytes);
    std::cout << ")\n";
}
auto setupNcurses()
{
    auto heya = initscr();
    cbreak(); // one char at a time
    scrollok(stdscr, false);
    leaveok(heya, true);
    echo();
    keypad(stdscr, true);
    refresh(); // opens new window
    return heya;
}

void Chatter::run()
{
    try
    {
        tcp::socket::broadcast option(true);
        // boost::asio::ip::multicast:: option2(true);
        mySock_->set_option(option);
        // mySock_->set_option();

        // sayHello();
        boost::shared_ptr<Chatter> p1 = getMe();
        boost::shared_ptr<Chatter> p2 = getMe();
        window = setupNcurses(); //essentially a global variable
        ;
        boost::thread t1([p1]() {
            // while (true)
            // std::cout << boost::this_thread::get_id() << " " + p1->userName << "\n";
            // cout <<"thread 1"<<endl;
            boost::asio::async_read(*p1->socket(), boost::asio::buffer(p1->getHeadBuf(), ChatMessage::headerlength),
                                    boost::bind(&Chatter::readHeader, p1->shared_from_this(), boost::asio::placeholders::error));
            p1->socket()->get_io_service().run(); // this is so that reading is happening concurrently with writing to the terminal, at least
        });
        ;
        boost::thread t2([p2]() {
            std::string buffer2 = p2->userName + "> ";
            char msg[BUF_SIZE] = "";
            while (true)
            {
                printw(buffer2.c_str());
                refresh();   // might get something that moves the cursor during this time
                getstr(msg); // currently this triggers received messages to display
                // (std::cin, buffer);
                // if(isendwin()){
                //     endwin();
                // }
                // // std::cout<<"read: "<<header<<'\n';
                // refresh();
                if (shouldIDie(msg))
                {
                    p2->die();
                    break;
                }
                ChatMessage chat(msg, p2->userName);

                p2->addMessage(chat);
                p2->socket()->get_io_service().poll();
                refresh();
            }
        });
        t1.join();
        t2.join();
    }
    catch (std::exception &error)

    {
        std::cout << "IM OUT" << std::endl;
        exit;
    }
}