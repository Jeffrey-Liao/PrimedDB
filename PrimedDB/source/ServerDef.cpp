#pragma once
#include "Server.h"
#include "UserManager.h"

USESTD;
USELIAOPRIMED;
namespace liao::Net
{
    const string SUCCESS = "success";
    static void CallError(const string& error, const string& message, Infor::ClassInfor infor)
    {
        Util::ErrorManager::Get().set(
            Util::ErrorLevel::Error,
            error, message, infor
        );
    }
    static void CallInfo(const string& error, const string& message)
    {
        Util::ErrorManager::Get().set(
            Util::ErrorLevel::Warning,
            error, message
        );
    }
    static void CallWarning(const string& error, const string& message)
    {
        Util::ErrorManager::Get().set(
            Util::ErrorLevel::Info,
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
        asio::write(*socket, asio::buffer("terminate\n"));
        string message = "use exit or quit command terminated the connection";

        CallInfo("ClientDisconnect", message);
    }
    void Server::authRead(std::shared_ptr<asio::ip::tcp::socket> socket,
        std::shared_ptr<asio::streambuf> buffer,
        const std::error_code& error)
    {
        string message;
        if (!m_interrupt)
        {
            std::istream is(buffer.get());
            std::getline(is, message);
            if (!message.empty() && message.back() == '\r')
                message.pop_back();
            m_interrupt = true;
            m_timerCv.notify_one();
        }
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
        asio::async_read_until(*socket, *buffer, '\n', [this, buffer, socket,id](const std::error_code& error, size_t bytes_transferred)
            {
                std::string errorMessage;
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
                            string name = message.substr(0, pos),
                                password = message.substr(pos + 1);
                            if (UserManager::Get().login(name, password))
                            {
                                errorMessage = "Login Success";
                                if (socket->is_open())
                                {
                                    CallInfo(name, errorMessage);
                                    asio::write(*socket, asio::buffer("success\n"));
                                    asio::write(*socket, asio::buffer("Welcome to use Primed DB :)!\n"));
                                    auto ptr = PrimedDB::UserManager::Get().get(name);
                                    this->m_user_clients[ptr] = socket;
                                    m_unauthorized.erase(id);
                                    doRead(socket);
                                    return;
                                }
                            }
                            else
                                errorMessage = "User name or password incorrect.";
                        }
                        else
                            errorMessage = "The format of authentication information incorrect.";
                    }
                    else
                    {
                        errorMessage = "Nothing received";
                    }
                }
                if (socket->is_open())
                    asio::write(*socket, asio::buffer(errorMessage + "\n"));
                CallInfo("LoginFailed", errorMessage);
                socket->close();
            });
        m_timer->async_wait([socket,this,id](const std::error_code& error)
            {
                if (!error)
                {
                    asio::write(*socket, asio::buffer("Login failed\n"));
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


            broadcast("terminate");
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
    future<bool> Server::sleep(SocketPtr socket, chrono::seconds sec)
    {
        m_interrupt = false;
        return async(std::launch::async, [&]()
            {
                unique_lock<mutex> lock(m_mutex);
                bool was_interrupted = m_timerCv.wait_for(lock, sec, [this]()
                    {
                        return m_interrupt.load();
                    });
                if (!was_interrupted) 
                {
                    // 情况2：超时到达
                    asio::error_code ec;
                    m_interrupt = true;
                    socket->cancel(ec);  // 超时自动取消socket
 
                }
                return true;
            });
    }
    void Server::doRead(std::shared_ptr<asio::ip::tcp::socket> socket)
    {
        auto buffer = std::make_shared<asio::streambuf>();

        asio::async_read_until(*socket, *buffer, '\n',
            [this, socket, buffer](const std::error_code& error, size_t bytes_transferred)
            {
                handle_read(socket, buffer, error);
            });
    }

    void Server::handle_read(std::shared_ptr<asio::ip::tcp::socket> socket,
        std::shared_ptr<asio::streambuf> buffer,
        const std::error_code& error)
    {
        if (!error && !m_stop) {
            string message;
            stream2string(buffer, message);

            if(message[0] != '/')
            //send to compiler
                Compiler::Compiler::Get().compile(getUser(socket),message);
            else
            {
                if (message == "/quit" || message == "/exit")
                {
                    exitAct(socket);
                }
                else
                    doRead(socket);
            }
        }
        else {
            remove(socket);
        }
    }

    void Server::broadcast(const std::string& message)
    {
        if (m_stop) return;

        std::string formatted_message = message + "\n";
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
                CallError("StopError", "Cannot terminate client because it is not connected to the server", Infor::ClassInfor(THISFUNC, THISFILE));
                message = "Cannot terminate client because it is not connected to the server";
            }

        }
        else
        {
            CallError("StopError", "You don't have permission to stop this client.", Infor::ClassInfor(THISFUNC, THISFILE));
            message = "You don't have permission to stop this client.";
        }
        if (m_user_clients.contains(executor) && m_user_clients[executor]->is_open())
        {
            asio::write(*m_user_clients[executor], asio::buffer(message+"\n"));
        }
        return success;
    }
}
