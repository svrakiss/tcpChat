#include <boost/asio.hpp>
using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> sockPtr;

class Chatter {
    private:
    sockPtr mySock_;
    std::string userName;
    public:
    Chatter(sockPtr,std::string);
    Chatter(sockPtr);
    ~Chatter();
    void read();
    void write();
};