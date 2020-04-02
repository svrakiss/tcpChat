#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <deque>
using boost::asio::ip::tcp;
typedef boost::shared_ptr<tcp::socket> sockPtr;
//In order to be able to consistently read a single message from the socket at a time, you need a header of set length that gives you info about the message.
// Like, how many more bytes to read.
class ChatMessage
{
public:
    std::string sentMsg;
    std::string name;
    std::string bb;
    char header[4];
    enum
    {
        headerlength = 4
    };
    std::string getData()
    {
        return header+bb;
    }
    std::string getbb() const
    {

        return bb;
    }
    std::size_t length()
    {
        std::string bb = name + ">" + sentMsg;
        return bb.size();
    }
    std::size_t getlength()
    {
        return headerlength + bb.size();
    }

    ChatMessage(std::string buf, std::string uname) : sentMsg(buf), name(uname)
    {

        std::sprintf(header, "%4d", length());
        std::cout << "length is " << length() << '\n';
        bb = name + ">" + sentMsg;
    }
    char *getHeader()
    {

        return header;
    }
    static int readHeader(char *data)
    {
        char header[5] = "";
        //the last byte is the null character
        std::strncat(header, data, 4);
        //read string as int
        int msg_length = std::atoi(header);
        std::cout << " Message header says length is " << msg_length << '\n';
        if (msg_length > 256)
        {
            // might get nonsense. ok probably will get nonsense
            msg_length = 0;
        }
        return msg_length;
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
    boost::array<char, 5> headbuf = {0};
    std::size_t sizenow = 0;

public:
    Chatter(sockPtr, std::string);
    Chatter(sockPtr);
    ~Chatter();
    sockPtr socket();
    void read(const boost::system::error_code &, std::size_t);
    void write(const boost::system::error_code &, std::size_t);
    void handleWrite(ChatMessage &);
    void addMessage(ChatMessage &);
    void readHeader(const boost::system::error_code &);
    boost::array<char, 5> &getHeadBuf();
    void run();
    void die();
    boost::array<char, 256> &getBuf();
    boost::array<char, 256> &getSentMsg();
    // std::ostream& getOut();
};
