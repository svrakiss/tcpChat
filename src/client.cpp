#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <thread>
using boost::asio::ip::tcp;
using namespace std;
typedef boost::shared_ptr<tcp::socket> sockPtr;
class Chatter
{
private:
    boost::shared_ptr<tcp::socket> mySock_;

public:
    Chatter(sockPtr socket) : mySock_(socket)
    {
    }
    ~Chatter()
    {
    }
    void read()
    {
        boost::array<char, 256> buf;
        boost::system::error_code error;
        size_t len = boost::asio::read(*mySock_, boost::asio::buffer(buf), error);
        std::cout.write(buf.data(), len);
        if (error == boost::asio::error::eof)
            std::cout << "time to die" << std::endl; // Connection closed cleanly by peer.
        else if (error)
            throw boost::system::system_error(error); // Some other error.
    }
    void write()
    {
        boost::system::error_code error;
        boost::asio::write(*mySock_, boost::asio::buffer("yo\n"), error);
        if (error == boost::asio::error::eof)
            std::cout << "time to die" << std::endl; // Connection closed cleanly by peer.
        else if (error)
            throw boost::system::system_error(error); // Some other error.
    }
};
int main(int argc, char *argv[])
{
    std::string serv_port;
    try
    {
        // the user should specify the server - the 2nd argument
        if (argc != 3)
        {
            if (argc != 2)
            {
                std::cerr << "Usage: Chatter <host> <port>" << std::endl;
                return 1;
            }
            serv_port = "4567";
        }
        else
        {
            serv_port = argv[2];
        }

        // Any program that uses asio need to have at least one io_service object
        boost::asio::io_service io_service;

        // Convert the server name that was specified as a parameter to the application, to a TCP endpoint.
        // To do this, we use an ip::tcp::resolver object.
        tcp::resolver resolver(io_service);

        // A resolver takes a query object and turns it into a list of endpoints.
        // We construct a query using the name of the server, specified in argv[1],
        // and the name of the service, in this case "daytime".
        tcp::resolver::query query(argv[1], serv_port);

        // The list of endpoints is returned using an iterator of type ip::tcp::resolver::iterator.
        // A default constructed ip::tcp::resolver::iterator object can be used as an end iterator.
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Now we create and connect the socket.
        // The list of endpoints obtained above may contain both IPv4 and IPv6 endpoints,
        // so we need to try each of them until we find one that works.
        // This keeps the Chatter program independent of a specific IP version.
        // The boost::asio::connect() function does this for us automatically.
        // tcp::endpoint connectionEndpoint(endpoint_iterator->address(),argv[2])
        sockPtr sharedSocket(new tcp::socket(io_service));
        boost::asio::connect(*(sharedSocket.get()), endpoint_iterator);
        // tcp::iostream yo
        // The connection is open. All we need to do now is read the response from the daytime service.

        // We use a boost::array to hold the received data.
        boost::array<char, 256> buf;
        boost::system::error_code error;
        // while()

        Chatter charlie(sharedSocket), jimmy(sharedSocket);
        thread t1, t2;

        t1 = thread([&jimmy]() {
            cout << "yessss" << endl;
            while(true){
            jimmy.write();}
        });
        t2 = thread([&charlie]() {
            cout << "nooooo" << endl;
            while(true){
            charlie.read();}
        });
        t1.join();
        t2.join();
        // for (;;)
        // {
        // The boost::asio::buffer() function automatically determines
        // the size of the array to help prevent buffer overruns.
        //   size_t len = socket.read_some(boost::asio::buffer(buf), error);
        // std::cout.write(buf.data(), len);
        // charlie.write();
        // jimmy.read();
        // pthread_create()
        // When the server closes the connection,
        // the ip::tcp::socket::read_some() function will exit with the boost::asio::error::eof error,
        // which is how we know to exit the loop.
        // if (error == boost::asio::error::eof)
        //     break; // Connection closed cleanly by peer.
        // else if (error)
        //     throw boost::system::system_error(error); // Some other error.
        // }
    }
    // handle any exceptions that may have been thrown.
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
