#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>
#include <thread>
#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include <iostream>
#include <array>
#include <optional>

using asio::ip::tcp;

namespace WandEngine {

    struct Session : std::enable_shared_from_this<Session> {
        tcp::socket socket_;
        std::array<char, 1024> buffer_;
        std::string clientId;
    
        Session(tcp::socket socket, std::string id)
            : socket_(std::move(socket)), clientId(std::move(id)) {}
    
        void start() {
            doRead();
        }
    
        void doRead() {
            auto self = shared_from_this();
            socket_.async_read_some(asio::buffer(buffer_),
                [this, self](std::error_code ec, std::size_t length) {
                    if (!ec) {
                        std::string msg(buffer_.data(), length);
                        std::cout << "[" << clientId << "] dijo: " << msg << "\n";
                        doRead();
                    } else {
                        std::cerr << "[" << clientId << "] desconectado.\n";
                    }
                });
        }
    
        void send(const std::string& msg) {
            auto self = shared_from_this();
            asio::async_write(socket_, asio::buffer(msg),
                [this, self](std::error_code ec, std::size_t /*length*/) {
                    if (ec) {
                        std::cerr << "[" << clientId << "] Error al enviar: " << ec.message() << "\n";
                    }
                });
        }
    };
    

    class NetworkManager {
    private:
        NetworkManager();
        ~NetworkManager();
        NetworkManager(const NetworkManager &) = delete;
        NetworkManager &operator=(const NetworkManager &) = delete;

        asio::io_context io;
        std::mutex clientsMutex;
        std::unique_ptr<tcp::acceptor> acceptor_;
        std::vector<std::shared_ptr<Session>> clients;
        std::thread ioThread;
        std::optional<asio::executor_work_guard<asio::io_context::executor_type>> workGuard;

        void doAccept();
        void doRead(asio::ip::tcp::socket* socket);


    public:
        static NetworkManager& GetInstance();

        bool Init();
        void Shutdown();
        
        void StartServer(unsigned short port);
        void broadcast(const std::string& message);
    };

}

#endif // NETWORKMANAGER_HPP