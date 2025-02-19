#include "environment.hpp"
#include "object.hpp"

std::shared_ptr<EvalObject>
Environment::get(const std::string &name) const
{
    auto it = repo_.find(name);
    if (it != repo_.end())
    {
        return it->second;
    }
    return nullptr;
}

void Environment::set(const std::string &name, std::shared_ptr<EvalObject> val)
{
    repo_[name] = val;
}