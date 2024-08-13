#include <core/attribute.hpp>
#include <core/bin_attr_map.hpp>
#include <core/feature_query.hpp>
#include <core/feature_data.hpp>
#include <core/queries/call_graph_size_query.hpp>
#include <core/queries/utils/query_utils.hpp>
#include <core/query_registry.hpp>

#include <phasar.h>

#include <iostream>
#include <vector>

REGISTER_QUERY(CallGraphSizeQuery)

namespace Core::Queries {

static std::vector<std::string> getEntryPoints(const llvm::Module &mod) {
  if (Utils::isExecutable(mod)) { // executable
    return std::vector<std::string> {"main"};
  } else { // library, we use all functions as entrypoints
    std::vector<std::string> ret;
    for (const auto &F : mod) {
      ret.emplace_back(F.getName());
    }
    return ret;
  }
}

void CallGraphSizeQuery::runOn(Package const * const pkg, Query::Result * const res) const {
  const FeatureID fid_node(*static_cast<Query const *>(this), Type::NODE, Attribute::Type::U_INT, FeatureData::Type::BINMAP);
  BinAttrMap num_nodes_map(Attribute::Type::U_INT);

  const FeatureID fid_edge(*static_cast<Query const *>(this), Type::EDGE, Attribute::Type::U_INT, FeatureData::Type::BINMAP);
  BinAttrMap num_edges_map(Attribute::Type::U_INT);

  size_t num_bins = pkg->bins().size();

  for (size_t i = 0; i < num_bins; i++) {
    auto &bin = pkg->bins()[i];
    auto entrypoints = getEntryPoints(bin->getModuleRef());

    auto mod = bin->getModuleCopy();
    psr::HelperAnalyses HA(std::move(mod), entrypoints);

    const auto &cg = HA.getICFG().getCallGraph();

    // this is what's in the call graph:

    // llvm::DenseMap<N, InstructionVertexTy *> CalleesAt{};
    // llvm::DenseMap<F, FunctionVertexTy *> CallersOf{};

    // setting num nodes
    size_t num_nodes = cg.size();
    num_nodes_map.insert(bin->getID(), Attribute(num_nodes));

    // finding out num edges
    size_t num_edges = 0;
    for (const auto &site : cg.getAllVertexCallSites()) {
      const auto it = cg.getCalleesOfCallAt(site);
      num_edges += std::distance(it.begin(), it.end());
    }

    num_edges_map.insert(bin->getID(), Attribute(num_edges));
  }

  res->emplace(fid_node, FeatureData(num_nodes_map));
  res->emplace(fid_edge, FeatureData(num_edges_map));
}

}