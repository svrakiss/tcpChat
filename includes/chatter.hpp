#include <boost/asio.hpp>
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

public:
    Chatter(sockPtr, std::string);
    Chatter(sockPtr);
    ~Chatter();
    sockPtr socket();
    void read();
    void write(const boost::system::error_code &, std::size_t);
    void run();
    boost::shared_ptr<Chatter> getMe();
};