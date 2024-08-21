#include <core/corpus_analyzer.hpp>
#include <core/package.hpp>
#include <core/feature_map.hpp>

#include <core/attribute.hpp>

#include <utils/exec_utils.hpp>
#include <utils/json_utils.hpp>
#include <utils/logging.hpp>

#include <llvm/Support/CommandLine.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

using namespace llvm;
using namespace Core;
using namespace Utils;

// Command Line Args
cl::opt<std::string> PackagesIndex("p", cl::Required, cl::desc("Path to packages index"), cl::value_desc("path"));
cl::opt<std::string> AnalysisConfig("c", cl::Required, cl::desc("Path to query config"), cl::value_desc("path"));
cl::opt<std::string> OutputFile("o", cl::Required, cl::desc("Path to output file"), cl::value_desc("path"));
//cl::opt<size_t>      Parallel("parallel", cl::desc("use multithreading"), cl::value_desc("number of threads"));
cl::opt<size_t> Timeout("t", cl::Required, cl::desc("How long to try to evaluate a package for"),
                                           cl::value_desc("duration in minutes"));

int main(int argc, char* argv[]) {
  // setup
  cl::ParseCommandLineOptions(argc, argv);
  initializeLogger();

  // get config
  CorpusAnalyzerConfig conf;
  if (!parseJSONArray<std::string>(AnalysisConfig, &conf.query_subset)) {
    errAndExit("unable to parse query config file");
  }
  spdlog::info("query list parsed");

  // get packages
  std::vector<Package> pkgs;
  if (!parseJSONArray<Package>(PackagesIndex, &pkgs)) {
    errAndExit("unable to parse packages file");
  }
  spdlog::info("packages parsed");

  // verify we can write out results
  std::ofstream of(OutputFile);
  if (!of) {
    errAndExit("cannot write to output file");
  }

  // get the queries
  CorpusAnalyzer ca(conf);
  // run queries on the packages
  FeatureMap fm;

  //if (Parallel.getNumOccurrences() == 0) {
  spdlog::info("evaluating packages...");\
  auto timeout = std::chrono::minutes(Timeout);
  ca.evaluate(pkgs, fm, timeout);
  //} else {
  //  spdlog::info("evaluating packages with {:d} threads.", Parallel);
  //  ca.parallelEvaluate(pkgs, fm, Parallel);
  //}
  spdlog::info("done evaluating packages");

  // write out FeatureMap
  of << fm.json().dump(1, '\t');
  of.close();
  spdlog::info("feature map written out");

  return EXIT_SUCCESS;
}