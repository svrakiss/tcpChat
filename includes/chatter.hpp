#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> sockPtr;
//In order to be able to consistently read a single message from the socket at a time, you need a header of set length that gives you info about the message.
// Like, how many more bytes to read.
class ChatMessage
{
public:
    ChatMessage(std::string buf, std::string uname) : sentMsg(buf), name(uname)
    {
        // std::memcpy(header);
    }
    std::string sentMsg;
    std::string name;
    char header[9] = {0};
    std::string getData()
    {
        return name + ">" + sentMsg;
    }
    std::size_t length()
    {
        return sizeof(name + ">") + sizeof(sentMsg);
    }
    char* getHeader(){
        
        std::memcpy(header, (char *)((int) length()),sizeof((int) length()));
        return header;

    }
};
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
    std::deque<ChatMessage> writeQueue;

public:
    Chatter(sockPtr, std::string);
    Chatter(sockPtr);
    ~Chatter();
    sockPtr socket();
    void read(const boost::system::error_code &,std::size_t);
    void write(const boost::system::error_code &, std::size_t);
    void handleWrite(ChatMessage &);
    void addMessage(ChatMessage &);
    void readHeader(const boost::system::error_code&);
    void run();
    void die();
    boost::array<char, 256> &getBuf();
    boost::array<char, 256> &getSentMsg();
    // std::ostream& getOut();
};
