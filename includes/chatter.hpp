#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <deque>
using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> sockPtr;

class Chatter : public boost::enable_shared_from_this<Chatter>
{

public:
    typedef boost::shared_ptr<Chatter> pointer;
    static pointer create(boost::asio::io_service &);

    static pointer create(boost::asio::io_service &, std::string);
    static pointer create(sockPtr, std::string);
    boost::shared_ptr<Chatter> getMe()
    {
        return shared_from_this();
    };

private:
    sockPtr mySock_;
    std::string userName;
    boost::array<char, 256> buf;

public:
    Chatter(sockPtr, std::string);
    Chatter(sockPtr);
    ~Chatter();
    sockPtr socket();
    void read(const boost::system::error_code &, std::size_t);
    void write(const boost::system::error_code &, std::size_t);
    void run();
    void die();
    boost::array<char, 256> &getBuf();
    boost::array<char, 256> &getSentMsg();
};

class ChatMessage
{
    boost::array<char, 256> sentMsg;
    std::uint16_t length;

    boost::array<char, 256> &getData()
    {
        return sentMsg;
    }
};