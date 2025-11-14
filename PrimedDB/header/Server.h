#pragma once
#include <asio.hpp>
#include "ErrorManager.h"
#include "Compiler.h"
#include <future>
#include <condition_variable>
#include <atomic>
namespace liao::Net
{
    class Server
    {
        using acceptor = asio::ip::tcp::acceptor;
        using Socket = asio::ip::tcp::socket;
        using tcp = asio::ip::tcp;
        using MessageHandler = std::function<void(const std::string&, std::shared_ptr<Socket>)>;
        using ConnectionHandler = std::function<void(std::shared_ptr<Socket>)>;
        using ErrorHandler = std::function<void(const std::error_code&, std::shared_ptr<Socket>)>;
        using UserPtr = PrimedDB::UserPtr;
        using SocketPtr = std::shared_ptr<Socket>;



        acceptor m_acceptor;
        std::unordered_map<UserPtr, SocketPtr> m_user_clients;
        std::unordered_map<std::string, std::pair<SocketPtr, std::shared_ptr<asio::streambuf>>> m_unauthorized;
        std::atomic<bool> m_stop;
        bool m_working;
        asio::io_context& m_io;
        std::shared_ptr<asio::steady_timer> m_timer;
        std::mutex m_mutex;
        std::shared_mutex m_hashMutex;

        void doAccept();
        void doRead(SocketPtr socket);
        void handle_read(SocketPtr socket,
            std::shared_ptr<asio::streambuf> buffer,
            const std::error_code& error);
        auto getip(const std::string&);
        void stream2string(std::shared_ptr<asio::streambuf> buffer, std::string& message);
        void exitAct(SocketPtr);
        bool validateUser(SocketPtr socket,const std::string& userName,const std::string& password, std::string& reply);
        void authenticate(const std::string& id);

        void sendStructure(std::shared_ptr<asio::ip::tcp::socket> socket);
        void writeMessage(std::shared_ptr<asio::ip::tcp::socket> socket,std::string& message);
    public:
        Server(asio::io_context& io,const std::string& ip, unsigned short port);
        ~Server();

        //Start server on given ip address
        void start();

        //Terminate current server
        void terminate();

        void setStop(bool value);

        bool isStop();

        UserPtr getUser(SocketPtr);

        void remove(SocketPtr);

        //Send message to all clients
        void broadcast(const std::string& message);

        //Send message to specific clients or send message to all clients except specific clients(set except to true)
        void broadcastTo(std::vector<UserPtr>&, const std::string&, bool except = false);

        //Send message to a specific client
        void sendToClient(const UserPtr&, const std::string&);

        //Check if a client exists
        bool exist(const UserPtr&);

        //Stop a client(Executor, User)
        bool stop(const UserPtr&, const UserPtr&);
        int size();


    };
}

