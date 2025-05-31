#include "environment.hpp"
#include "object.hpp"

__Ptr<EvalObject> Environment::get(const std::string &name) const
{
    auto it = repo_.find(name);
    if (it == repo_.end() && outer_)
    {
        return outer_->get(name);
    }
    else if (it != repo_.end())
    {
        return it->second;
    }
    return nullptr;
}

void Environment::set(const std::string &name, __Ptr<EvalObject> val)
{
    repo_[name] = val;
}