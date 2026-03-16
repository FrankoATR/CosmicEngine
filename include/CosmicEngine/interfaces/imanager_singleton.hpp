#ifndef COSMIC_IMANAGER_SINGLEON_HPP
#define COSMIC_IMANAGER_SINGLEON_HPP

#include "imanager.hpp"

namespace CosmicEngine
{
    template <typename T>
    class SingletonManager : public IManager {
    public:
        static T& GetInstance() {
            static T instance;
            return instance;
        }
    protected:
        SingletonManager() = default;
        ~SingletonManager() = default;
        SingletonManager(const SingletonManager&) = delete;
        SingletonManager& operator=(const SingletonManager&) = delete;
    };

    template <typename T>
    inline T& Manager() {
        return T::GetInstance();
    }

}

#endif //COSMIC_IMANAGER_SINGLEON_HPP