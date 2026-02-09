#include <cassert>
#include <lammpstrj/lammpstrj.hpp>
#include <map>
#include <numeric>

class LocalDensityCalculator {

private:
  std::string filename_;
  std::string mode_;
  const double mesh_size_;
  const double density_threshold_;
  const bool write_vtk_;
  int frame_;
  lammpstrj::SystemInfo system_info_;
  LocalDensityCalculator(const std::string mode, const double mesh_size, const double density_threshold, const bool write_vtk, const std::string &filename, const lammpstrj::SystemInfo &si)
      : mode_(mode), system_info_(si), mesh_size_(mesh_size), density_threshold_(density_threshold), write_vtk_(write_vtk) {
    filename_ = filename;
    frame_ = 0;
    std::cerr << "mode = " << mode_ << std::endl;
    std::cerr << "mesh_size = " << mesh_size_ << std::endl;
    std::cerr << "density_threshold = " << density_threshold_ << std::endl;
  }

public:
  static std::unique_ptr<LocalDensityCalculator> create(const std::string mode, const double mesh_size, const double density_threshold, const bool write_vtk, std::string &filename) {
    auto si = lammpstrj::read_info(filename);
    if (!si) {
      std::cerr << "Error: Could not read file: " << filename << std::endl;
      return nullptr;
    }
    return std::unique_ptr<LocalDensityCalculator>(new LocalDensityCalculator(mode, mesh_size, density_threshold, write_vtk, filename, *si));
  }

  void calc_temperature(const std::unique_ptr<lammpstrj::SystemInfo> &si,
                        std::vector<lammpstrj::Atom> &atoms) {
    static int index = 0;
    double e = 0.0;
    for (auto &a : atoms) {
      e += a.vx * a.vx;
      e += a.vy * a.vy;
      e += a.vz * a.vz;
    }
    e /= static_cast<double>(si->atoms);
    e /= 3.0;
    printf("%d %f\n", index * 100, e);

    index++;
  }

  void write_vtk(const std::string &filename, int nx, int ny, int nz, const std::vector<double> &data) {
    if (data.size() != static_cast<size_t>(nx * ny * nz)) {
      std::cerr << "Error: data size does not match grid dimensions." << std::endl;
      return;
    }

    std::ofstream ofs(filename);
    if (!ofs) {
      std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
      return;
    }

    ofs << "# vtk DataFile Version 1.0\n";
    ofs << "test\n";
    ofs << "ASCII\n";
    ofs << "DATASET STRUCTURED_POINTS\n";
    ofs << "DIMENSIONS " << nx << " " << ny << " " << nz << "\n";
    ofs << "ORIGIN 0.0 0.0 0.0\n";
    ofs << "SPACING 1.0 1.0 1.0\n";
    ofs << "\n";
    ofs << "POINT_DATA " << nx * ny * nz << "\n";
    ofs << "\n";
    ofs << "SCALARS intensity float\n";
    ofs << "LOOKUP_TABLE default\n";

    for (double v : data) {
      ofs << v << "\n";
    }

    ofs.close();
  }

  int pos2index(int ix, int iy, int iz, int nx, int ny, int nz) {
    if (ix >= nx) ix -= nx;
    if (iy >= ny) iy -= ny;
    if (iz >= nz) iz -= nz;
    return ix + iy * nx + iz * nx * ny;
  }

  int find(int index, const std::vector<int> &cluster) {
    while (index != cluster[index]) {
      index = cluster[index];
    }
    return index;
  }

  void unite(int i1, int i2, const std::vector<double> &density, std::vector<int> &cluster) {
    if (mode_ == "bubble") {
      if (density[i1] > density_threshold_) return;
      if (density[i2] > density_threshold_) return;
    } else {
      if (density[i1] < density_threshold_) return;
      if (density[i2] < density_threshold_) return;
    }

    i1 = find(i1, cluster);
    i2 = find(i2, cluster);
    if (i1 < i2) {
      cluster[i2] = i1;
    } else {
      cluster[i1] = i2;
    }
  }

  void clustering(int nx, int ny, int nz, const std::vector<double> &density) {
    const int total_cells = nx * ny * nz;
    std::vector<int> cluster(total_cells);
    std::iota(cluster.begin(), cluster.end(), 0);
    for (int iz = 0; iz < nz; iz++) {
      for (int iy = 0; iy < ny; iy++) {
        for (int ix = 0; ix < nx; ix++) {
          int i1 = pos2index(ix, iy, iz, nx, ny, nz);
          int i2 = pos2index(ix + 1, iy, iz, nx, ny, nz);
          unite(i1, i2, density, cluster);
          i2 = pos2index(ix, iy + 1, iz, nx, ny, nz);
          unite(i1, i2, density, cluster);
          i2 = pos2index(ix, iy, iz + 1, nx, ny, nz);
          unite(i1, i2, density, cluster);
        }
      }
    }
    // クラスター数の確認
    for (int i = 0; i < total_cells; i++) {
      cluster[i] = find(i, cluster);
    }
    // サイズ分布の計算
    std::map<int, int> root_size;
    for (int i = 0; i < total_cells; i++) {
      if (mode_ == "bubble" && density[i] < density_threshold_) {
        root_size[cluster[i]]++;
      } else if (mode_ == "droplet" && density[i] > density_threshold_) {
        root_size[cluster[i]]++;
      }
    }
    // --- サイズ分布を作る（size -> 個数） ---
    std::map<int, int> size_distribution;
    for (auto it = root_size.begin(); it != root_size.end(); ++it) {
      if (it->second == 1) continue; // サイズ1のクラスターは無視
      size_distribution[it->second]++;
    }

    std::ostringstream oss;
    oss << "clustersize." << std::setfill('0') << std::setw(4) << frame_ << ".dat";
    std::string filename = oss.str();
    std::ofstream ofs(filename);
    // --- 小さい順に出力 ---
    for (auto it = size_distribution.begin(); it != size_distribution.end(); ++it) {
      ofs << it->first << " " << it->second << std::endl;
    }

    int num_cluster = 0;
    for (const auto &kv : size_distribution) {
      num_cluster += kv.second;
    }
    std::cerr << frame_ << std::endl;
    std::cout << frame_ << " " << num_cluster << std::endl;
  }

  void calc_density(const std::unique_ptr<lammpstrj::SystemInfo> &si,
                    std::vector<lammpstrj::Atom> &atoms) {
    // セル数（切り上げ）
    const int nx = static_cast<int>(std::ceil(si->LX / mesh_size_));
    const int ny = static_cast<int>(std::ceil(si->LY / mesh_size_));
    const int nz = static_cast<int>(std::ceil(si->LZ / mesh_size_));
    const int total_cells = nx * ny * nz;

    // 実際のセルサイズ
    const double mx = si->LX / static_cast<double>(nx);
    const double my = si->LY / static_cast<double>(ny);
    const double mz = si->LZ / static_cast<double>(nz);
    const double mx_inv = 1.0 / mx;
    const double my_inv = 1.0 / my;
    const double mz_inv = 1.0 / mz;
    // 初期化
    std::vector<double> density;
    density.assign(total_cells, 0.0);

    // 各原子を対応するセルに割り当ててカウント
    for (const auto &atom : atoms) {
      double x = atom.x;
      double y = atom.y;
      double z = atom.z;
      if (x < 0.0) x += si->LX;
      if (y < 0.0) y += si->LY;
      if (z < 0.0) z += si->LZ;
      if (x >= si->LX) x -= si->LX;
      if (y >= si->LY) y -= si->LY;
      if (z >= si->LZ) z -= si->LZ;

      int ix = static_cast<int>(x * mx_inv);
      int iy = static_cast<int>(y * my_inv);
      int iz = static_cast<int>(z * mz_inv);

      int index = ix + nx * (iy + ny * iz);
      assert(index >= 0);
      assert(index < total_cells);
      if (atom.type == 1) {
        density[index] += 1.0;
      }
    }

    // 密度に変換（個数密度: 個数 / セル体積）
    const double cell_volume = mx * my * mz;
    for (auto &d : density) {
      d /= cell_volume;
    }
    std::ostringstream oss;
    oss << "density." << std::setfill('0') << std::setw(4) << frame_ << ".vtk";
    std::string filename = oss.str();
    if (write_vtk_) {
      write_vtk(filename, nx, ny, nz, density);
    }
    clustering(nx, ny, nz, density);
    frame_++;
  }

  void calculate() {
    lammpstrj::for_each_frame(filename_,
                              [this](const std::unique_ptr<lammpstrj::SystemInfo> &si, std::vector<lammpstrj::Atom> &atoms) {
                                this->calc_density(si, atoms);
                              });
  }
};