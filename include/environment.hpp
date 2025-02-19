#ifndef ENVIRONMENT_HPP_INCLUDED
#define ENVIRONMENT_HPP_INCLUDED

#include <map>
#include <string>
#include <memory>

struct EvalObject;

class Environment
{
public:
    std::shared_ptr<EvalObject> get(const std::string &name) const;
    void set(const std::string &name, std::shared_ptr<EvalObject> val);

private:
    std::map<std::string, std::shared_ptr<EvalObject>> repo_;
};

#endif