#include <core/feature_query.hpp>

#include <nlohmann/json.hpp>
using jsonf = nlohmann::json;

#include <memory>

namespace Core {

Location::Location(const nlohmann::json &jloc) {
  bin_name = std::make_shared<std::string>(jloc["bin_name"]);
  bin_path = std::make_shared<std::string>(jloc["bin_path"]);
}

jsonf Location::json() const {
  jsonf ret = jsonf::object();
  ret["bin_name"] = *bin_name;
  ret["bin_path"] = *bin_path;
  return ret;
}

FeatureID::FeatureID(const nlohmann::json &jfid) {
  _query_name = jfid["name"];
  _query_type = jfid["type"];
}

jsonf FeatureID::json() const {
  jsonf ret = jsonf::object();
  ret["name"] = _query_name;
  ret["type"] = _query_type;
  return ret;
}

Feature::Feature(const nlohmann::json &jftr) :
  fid(FeatureID(jftr["ftr_id"])),
  num_locs(jftr["num_locations"]),
  locs(jftr["locations"].begin(), jftr["locations"].end())
{ }

jsonf Feature::json() const {
  jsonf ret = jsonf::object();

  ret["ftr_id"] = fid.json();

  ret["num_locations"] = num_locs;

  jsonf jlocs = jsonf::array();
  for (const auto &loc: locs) {
    jlocs.push_back(loc.json());
  }
  ret["locations"] = jlocs;

  return ret;
}

}