#ifndef DELPHICPP_PACKAGE_HPP_
#define DELPHICPP_PACKAGE_HPP_

#include <core/binary.hpp>

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

namespace Core {
class Package {
  public:
    Package(std::string * const _name, std::string * const _version) :
                           name(_name),            version(_version) {  }

    // attempt to reify this specific package, return whether successful
    bool reifySelf();

    const std::shared_ptr<std::string> name,
                                       version;
    std::vector<Binary> bins;

    std::unordered_map<std::string, std::string> settings,
                                                 options;
    std::vector<std::string> requires;

  private:
    bool _reified = false;
};
}

#endif  // DELPHICPP_PACKAGE_HPP_