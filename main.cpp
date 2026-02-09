#include "local_density_calculator.hpp"
#include <cmath>
#include <cstdio>
#include <cxxopts.hpp>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_set>

auto parse_argument(int argc, char **argv) {
  cxxopts::Options options(
      "cluster-analyze",
      "LAMMPS cluster analyzer");

  // LAMMPSTRJ file
  options.add_options("positional")("filename", "LAMMPS trajectory file (.lammpstrj)", cxxopts::value<std::string>());
  options.parse_positional({"filename"});
  // mode
  options.add_options()("m,mode",
                        "Analysis mode: bubble or droplet",
                        cxxopts::value<std::string>()->default_value("bubble"));

  // mesh size
  options.add_options()("s,mesh-size",
                        "Mesh size for analysis",
                        cxxopts::value<double>()->default_value("2.0"));

  // density threshold
  options.add_options()("t,density-threshold",
                        "Density threshold for phase classification",
                        cxxopts::value<double>()->default_value("0.3"));
  // VTK output
  options.add_options()("v,vtk",
                        "Output results in VTK format",
                        cxxopts::value<bool>()->default_value("false"));

  // help
  options.add_options()("h,help",
                        "Print usage");

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    std::exit(0);
  }

  if (!result.count("filename")) {
    std::cerr << "Error: filename is required.\n\n"
              << options.help({"", "positional"}) << std::endl;
    exit(1);
  }

  return result;
}

int main(int argc, char *argv[]) {
  auto result = parse_argument(argc, argv);
  auto mode = result["mode"].as<std::string>();
  auto filename = result["filename"].as<std::string>();
  auto mesh_size = result["mesh-size"].as<double>();                       // メッシュのサイズ(割り切れない場合は調整される)
  const auto density_threshold = result["density-threshold"].as<double>(); // 密度のしきい値。これ以上を液相とみなす。
  auto write_vtk = result["vtk"].as<bool>();

  std::cout << mode << std::endl;
  std::cout << filename << std::endl;
  std::cout << mesh_size << std::endl;
  std::cout << density_threshold << std::endl;
  std::cout << write_vtk << std::endl;
  if (mode != "bubble" && mode != "droplet") {
    std::cerr << "Error: mode must be either \"droplet\" or \"bubble\"."
              << std::endl;
    return 1;
  }
  auto calculator = LocalDensityCalculator::create(mode, mesh_size, density_threshold, write_vtk, filename);
  if (!calculator) {
    std::cerr << "Failed to create LocalDensityCalculator." << std::endl;
    return 1;
  }
  calculator->calculate();
}
