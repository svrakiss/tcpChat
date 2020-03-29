typedef boost::shared_ptr<tcp::socket> sockPtr;

class Chatter {
    public:
    Chatter(sockPtr,std::string);
    Chatter(sockPtr);
    ~Chatter();
    void read();
    void write();
};