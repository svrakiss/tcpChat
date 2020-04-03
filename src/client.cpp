#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "chatter.hpp"
using boost::asio::ip::tcp;
// using namespace std;
typedef boost::shared_ptr<tcp::socket> sockPtr;

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
        std::string userName;
        bool flag = false;
        do
        {
            std::cout << "Please enter a user name:";
            std::getline(std::cin, userName);
            if (std::strstr(userName.c_str(), KILL_WORD) != NULL || userName.empty())
            {
                std::cout << "contains reserved word " << KILL_WORD << " or is empty\n";

                flag = true;
            }
            else
                break;
        } while (flag);
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
        boost::shared_ptr<tcp::socket> sharedSocket(new tcp::socket(io_service));
        boost::asio::connect(*(sharedSocket.get()), endpoint_iterator);
        // The connection is open. All we need to do now is read the response.
        // tcp::socket
        // boost::asio::
        // io_service.
        Chatter::pointer chtPtr = Chatter::create(sharedSocket, userName);
        chtPtr->run();
        // VERY IMPORTANTvvvv
        boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));
        t.join();
    }
    // handle any exceptions that may have been thrown.
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
