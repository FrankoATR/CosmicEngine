#include "NetworkManager.hpp"

namespace WandEngine
{

    NetworkManager &NetworkManager::GetInstance()
    {
        static NetworkManager instance;
        return instance;
    }

    NetworkManager::NetworkManager() 
    {
        std::cout << "Network manager created" << std::endl;
    }

    NetworkManager::~NetworkManager()
    {
        std::cout << "Network manager destroyed" << std::endl;
    }

    

    bool NetworkManager::Init()
    {
        workGuard.emplace(asio::make_work_guard(io));

        std::cout << "Network manager initialized" << std::endl;
        return true;
    }
    

    void NetworkManager::Shutdown()
    {
        io.stop();
    
        if (ioThread.joinable()) {
            ioThread.join();
        }
    
        std::lock_guard<std::mutex> lock(clientsMutex);
        clients.clear();
        acceptor_.reset();

        if (workGuard) {
            workGuard->reset();
            workGuard.reset();
        }
    }
    


    void NetworkManager::StartServer(unsigned short port)
    {
        if (acceptor_) return;
    
        acceptor_ = std::make_unique<tcp::acceptor>(io, tcp::endpoint(tcp::v4(), port));
        doAccept();
    
        ioThread = std::thread([&]() {
            try {
                std::cout << "[Server] Escuchando en puerto " << port << "...\n";
                io.run();
            } catch (const std::exception& e) {
                std::cerr << "[Server] Excepción en io.run(): " << e.what() << "\n";
            }
        });
    }

    

    void NetworkManager::doAccept()
    {
        auto newSocket = std::make_shared<tcp::socket>(io);
        acceptor_->async_accept(*newSocket,
            [this, newSocket](std::error_code ec) {
                if (!ec) {
                    static int id = 1;
                    auto session = std::make_shared<Session>(std::move(*newSocket), "Cliente" + std::to_string(id++));
                    {
                        std::lock_guard<std::mutex> lock(clientsMutex);
                        clients.push_back(session);
                    }
                    session->start();
                }
                doAccept();
            });
    }
    

    void NetworkManager::broadcast(const std::string &message)
    {

    }



}
