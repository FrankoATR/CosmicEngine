#include "Logger.hpp"
#include "../WandAllegroEngine/Models/GameObject.hpp"

void Logger::OnNotify(GameObject *obj, const std::string &event)
{
    std::cout << "Logger: " << obj->GetObjectName() << " ha generado el evento: " << event << std::endl;
}