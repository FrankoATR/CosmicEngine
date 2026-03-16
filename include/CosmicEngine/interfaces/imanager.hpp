#ifndef COSMIC_IMANAGER_HPP
#define COSMIC_IMANAGER_HPP

#include <string>

namespace CosmicEngine
{

    struct ManagerInitParams
    {
        virtual ~ManagerInitParams() = default;
    };

    class IManager
    {
    protected:
        std::string name = "-No name manager especified-";

    public:
        IManager() = default;
        virtual ~IManager() = default;

        virtual void init(const ManagerInitParams& params) = 0;
        virtual void shutdown() = 0;
    };

} // namespace CosmicEngine

#endif // COSMIC_IMANAGER_HPP