#pragma once
#include "Server.h"
#include "UserManager.h"

USESTD;
USELIAOPRIMED;
namespace liao::Net
{
    const string SUCCESS = "success\n";
    const char TERMINAL = '\n';
    const string TERMINATE = "terminate\n";
    const string FINISH = "finish\n";
    //for long text
    //there will be additional line contains 'finish'
    static void CallError(const string& error, const string& message, Infor::ClassInfor infor)
    {
        Util::ErrorManager::Get().set(
            Util::ErrorLevel::Error,
            error, message, infor
        );
    }
    static void CallInfo(const string& error, const string& message)
    {
        StaticFunc::WriteInfo(error, message);
    }
    static void CallWarning(const string& error, const string& message)
    {
        Util::ErrorManager::Get().set(
            Util::ErrorLevel::Warning,
            error, message
        );
    }
    auto Server::getip(const std::string& ip)
    {
        std::string finalIp = ip;
        if (ip.empty())
            finalIp = "0.0.0.0";
        else if(ip == "localhost")
            finalIp = "127.0.0.1";
        else
            finalIp = ip;
        return asio::ip::address_v4::from_string(finalIp);
    }
    void Server::exitAct(SocketPtr socket)
    {
        asio::write(*socket, asio::buffer(TERMINATE.size()+"\n"+TERMINATE));
        string message = "use exit or quit command terminated the connection";
        CallInfo("ClientDisconnect", message);
    }
    bool Server::validateUser(SocketPtr socket, const std::string& userName, const std::string& password, std::string& reply)
    {
        auto& userManager = UserManager::Get();
        if (userManager.exist(userName))
        {
            auto ptr = PrimedDB::UserManager::Get().get(userName);
            if (m_user_clients.contains(ptr))
            {
                asio::write(*socket, asio::buffer(TERMINATE));
            }
            if (UserManager::Get().login(userName, password))
            {
                reply = "Login Success";
                if (socket->is_open())
                {
                    CallInfo(userName, reply);
                    asio::write(*socket, asio::buffer(SUCCESS));
                    auto ptr = PrimedDB::UserManager::Get().get(userName);
                    this->m_user_clients[ptr] = socket;
                    doRead(socket);
                    return true;
                }
            }
            else
                reply = "Password incorrect.";
        }
        else
            reply = "User not exist.";
        return false;
    }
    void Server::authenticate(const string& id)
    {
        chrono::seconds timeout = chrono::seconds(3);
        auto buffer = m_unauthorized.at(id).second;
        std::string message;
        auto socket = m_unauthorized.at(id).first;
        m_timer = make_shared<asio::steady_timer>(m_io, timeout);
        CallInfo("AuthenticationStart", format("Checking authentication information of connection {} ", id));
        //future<bool> timer = sleep(socket,timeout);
        asio::async_read_until(*socket, *buffer, TERMINAL, [this, buffer, socket,id](const std::error_code& error, size_t bytes_transferred)
            {
                std::string reply;
                if (!error)
                {
                    m_timer->cancel();
                    std::string message;
                    stream2string(buffer, message);
                    if (!message.empty())
                    {
                        auto pos = message.find(":");
                        if (pos != message.npos)
                        {
                            string userName = message.substr(0, pos),
                                password = message.substr(pos + 1);
                            WriteLock lock(m_hashMutex);
                            m_unauthorized.erase(id);
                            if (validateUser(socket, userName, password, reply))
                                return;
                        }
                        else
                            reply = "The format of authentication information incorrect.";
                    }
                    else
                    {
                        reply = "Nothing received";
                    }
                }
                if (socket->is_open())
                    asio::write(*socket, asio::buffer(reply + "\n"));
                CallInfo("LoginFailed", reply);
                socket->close();
            });
        m_timer->async_wait([socket,this,id](const std::error_code& error)
            {
                if (!error)
                {
                    asio::write(*socket, asio::buffer("Login Timeout\n"));
                    CallWarning("TimeOutWarning", "Sever wait for 5 seconds but receive nothing of authentication data");
                    socket->cancel();
                    m_unauthorized.erase(id);
                }

            });
    }
    Server::Server(asio::io_context& io,const std::string& ip, unsigned short port)
        :m_stop(false),m_working(false),m_acceptor(io, asio::ip::tcp::endpoint(getip(ip), port)),m_io(io)
    {
    }

    Server::~Server()
    {
        terminate();
    }
    void Server::stream2string(std::shared_ptr<asio::streambuf> buffer, std::string& message)
    {
        std::istream is(buffer.get());
        std::getline(is, message);
    }
    void Server::start()
    {
        if (!m_working)
        {
            m_working = true;
            if (!m_stop) {
                doAccept();
                auto endpoint = m_acceptor.local_endpoint();
                CallInfo("ServerStart", format("Server started successfully [{}:{}]!", endpoint.address().to_string(), endpoint.port()));
            }
        }

    }

    void Server::terminate()
    {
        if (!m_stop) {

            broadcast("Server shutdown.");
            broadcast(TERMINATE);
            m_stop = true;
            for (auto& client : m_user_clients) {
                if (client.second->is_open()) {
                    client.second->close();
                }
            }
            m_user_clients.clear();

            if (m_acceptor.is_open()) {
                m_acceptor.close();
            }
            CallInfo("ServerStop", "Server terminated by administrator");
        }
    }

    void Server::setStop(bool value)
    {
        m_stop = value;
    }

    bool Server::isStop()
    {
        return m_stop;
    }
    UserPtr Server::getUser(SocketPtr socket)
    {
        auto iter = std::find_if(m_user_clients.begin(), m_user_clients.end(), [&](const auto& pair)
            {
                auto user = pair.first;
                auto m_s = pair.second;
                return socket == m_s;
            });
        return iter != m_user_clients.end() ? iter->first : nullptr;
    }

    void Server::remove(SocketPtr socket)
    {
        if (socket->is_open())
        {
            socket->close();
            std::erase_if(m_user_clients, [&](const auto& pair)
                {
                    auto user = pair.first;
                    auto m_s = pair.second;
                    return socket == m_s;
                });
        }

    }

    void Server::doAccept()
    {
        auto socket = std::make_shared<asio::ip::tcp::socket>(m_io);
        m_acceptor.async_accept(*socket,
            [this, socket](const std::error_code& error)
            {
                if (error || m_stop)
                {
                    if (!m_stop)
                    {
                        CallError("AcceptFailed", "An error happened when accept a client", Infor::ClassInfor(THISFUNC, THISFILE));
                    }
                    return;
                }
                string success = format("Accept a new client [{}:{}] success!", socket->remote_endpoint().address().to_string(), socket->remote_endpoint().port());
                CallInfo("AcceptSuccess", success);
                string id = StaticFunc::GetUniqueId();
                m_unauthorized[id] = make_pair(socket,make_shared<asio::streambuf>());
                authenticate(id);
                    // 继续接受新连接
                doAccept();
            });
    }
   
    void Server::doRead(std::shared_ptr<asio::ip::tcp::socket> socket)
    {
        auto buffer = std::make_shared<asio::streambuf>();

        asio::async_read_until(*socket, *buffer, TERMINAL,
            [this, socket, buffer](const std::error_code& error, size_t bytes_transferred)
            {
                handle_read(socket, buffer, error);
            });
    }
    void Server::sendStructure(std::shared_ptr<asio::ip::tcp::socket> socket)
    {
        string structure = std::move(TableManager::Get().format());
        if (structure.empty())
            structure = "No table exits\n";
        writeMessage(socket,structure);
    }
    void Server::writeMessage(std::shared_ptr<asio::ip::tcp::socket> socket,std::string& message)
    {
        int size = message.size();
        message = to_string(size) + "\n" + message;
        shared_ptr<string> messageBody = make_shared<string>(std::move(message));
        if (messageBody->back() != '\n')
            messageBody->push_back('\n');
        asio::async_write(*socket, asio::buffer(*messageBody), [messageBody, socket,size](const std::error_code& error, size_t /*bytes_transferred*/)
            {
                if (error)
                {
                    CallError("ReturnError", "An Error happened when trying to get the table structures", Infor::ClassInfor(THISFUNC, THISFILE));
                }
                else
                {
                    CallInfo("Return", std::format("Total message bytes : {}", size));
                    CallInfo("Return", std::format("InformationBody : \n[\n{}]", *messageBody));
                }
            });
    }
    void Server::handle_read(std::shared_ptr<asio::ip::tcp::socket> socket,
        std::shared_ptr<asio::streambuf> buffer,
        const std::error_code& error)
    {
        if (!error && !m_stop) {
            string message;
            stream2string(buffer, message);
            CallInfo("MessageReceived", message);
            if (message[0] != '/')
            {
                auto result = Compiler::Compiler::Get().compile(getUser(socket), std::move(message));
                message.clear();
                auto text = result.get();

                writeMessage(socket, text.m_message);
            }
            else
            {
                if (message == "/quit" || message == "/exit")
                {
                    exitAct(socket);
                    return;
                }
                if (message == "/structure")
                {
                    sendStructure(socket);
                }
            }
            doRead(socket);
        }
        else {
            remove(socket);
        }
    }

    void Server::broadcast(const std::string& message)
    {
        if (m_stop) return;

        std::string formatted_message = message;
        for (auto& client : m_user_clients)
        {
            if (client.second->is_open())
            {
                asio::async_write(*client.second, asio::buffer(formatted_message),
                    [this, client](const std::error_code& error, size_t /*bytes_transferred*/)
                    {
                        if (error) {
                            CallError("SendError", "Remote client might closed connection.", Infor::ClassInfor(THISFUNC, THISFILE));
                            m_user_clients.erase(client.first);
                            if (client.second->is_open()) {
                                client.second->close();
                            }
                        }
                    });
            }
        }
        
    }

    void Server::broadcastTo(std::vector<UserPtr>& users, const std::string& message, bool except)
    {
        vector<SocketPtr> clients;
        if (except)
        {
            for (auto& client : m_user_clients)
                clients.push_back(client.second);
            for (auto& user : users)
                std::remove(clients.begin(), clients.end(), m_user_clients[user]);
        }
        else
        {
            for (auto& user:users)
            {
                if (m_user_clients.contains(user))
                {
                    clients.push_back(m_user_clients[user]);
                }
            }
        }
        for (auto& client:clients)
        {
            asio::write(*client, asio::buffer(message+"\n"));
        }
    }
    void Server::sendToClient(const UserPtr& user, const std::string& message)
    {
        std::string formatted_message = message + "\n";
        auto client = m_user_clients[user];
        asio::async_write(*client, asio::buffer(formatted_message),
            [this, client,user](const std::error_code& error, size_t /*bytes_transferred*/)
            {
                if (error) {
                    CallError("SendError", "Remote client might closed connection.", Infor::ClassInfor(THISFUNC, THISFILE));
                    m_user_clients.erase(user);
                    if (client->is_open()) {
                        client->close();
                    }
                }
            });
    }
    //Check if a client exists
    bool Server::exist(const UserPtr& ptr)
    {
        return m_user_clients.contains(ptr);
    }

    //Stop a client(Executor, User)
    bool Server::stop(const UserPtr& executor, const UserPtr& ptr)
    {
        bool success = false;
        string message;
        if (executor->getLevel() >= UserLevel::Administrator)
        {
            if (m_user_clients.contains(ptr))
            {
                if (m_user_clients[ptr]->is_open())
                    m_user_clients[ptr]->close();
                m_user_clients.erase(ptr);
                message = SUCCESS;
                success =  true;
            }
            else
            {
                message = "Cannot terminate client because it is not connected to the server";
                CallError("StopError", message, Infor::ClassInfor(THISFUNC, THISFILE));
            }
        }
        else
        {
            message = "You don't have permission to stop this client.";
            CallError("StopError", message, Infor::ClassInfor(THISFUNC, THISFILE));
        }
        
        if (m_user_clients.contains(executor) && m_user_clients[executor]->is_open())
        {
            asio::write(*m_user_clients[executor], asio::buffer(message+"\n"));
        }
        return success;
    }
    int Server::size()
    {
        Lock lock(m_mutex);
        return m_user_clients.size();
    }
}
