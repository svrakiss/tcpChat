#include <boost/asio.hpp>
using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> sockPtr;

class Chatter 
{

public:
    typedef boost::shared_ptr<Chatter> pointer;
    static pointer create(boost::asio::io_service&);

    static pointer create(boost::asio::io_service&, std::string);

private:
    sockPtr mySock_;
    std::string userName;

public:
    Chatter(sockPtr, std::string);
    Chatter(sockPtr);
    ~Chatter();
    sockPtr socket();
    void read();
    void write();
    void run();
};