# LAMMPS Cluster Analyzer

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

`lammps-cluster-analyzer` は、LAMMPS のトラジェクトリファイル (`lammpstrj`) を解析し、Type 1 の原子について局所密度の解析とクラスタリングを行うスタンドアロンの C++ コードです。

## Dependencies

All dependencies are header-only libraries included via `external/`:

| Library | Purpose | License |
|----------|----------|----------|
| [lammpstrj-parser](https://github.com/wtnb-appi/lammpstrj-parser) | LAMMPS trajectory reader | MIT |
| [cxxopts](https://github.com/jarro2783/cxxopts) | Command-line parser | MIT |


## ビルド方法

本リポジトリには GNU Makefile が同梱されています。以下のコマンドでビルドできます：

```bash
git clone --recursive https://github.com/kaityo256/lammps-cluster-analyzer.git
cd lammps-cluster-analyzer
make
```

ビルドが完了すると、実行ファイル cluster-analyze が生成されます。

外部ライブラリ`lammpstrj-parser`は Git Submoduleとして管理されています。クローン時に `--recursive` をつけるか、以下で初期化してください：

```sh
git submodule update --init --recursive
```

## 使い方

```sh
./cluster-analyze input.lammpstrj > output.dat
```

## コマンドラインオプション

本プログラムは、LAMMPS のトラジェクトリファイル（`.lammpstrj`）を入力として、クラスタ解析を行います。  
基本的な使い方は以下の通りです。

```bash
cluster-analyze [options] filename
```

### 位置引数

| 引数名 | 説明 |
|---|---|
| `filename` | 解析対象となる LAMMPS トラジェクトリファイル（`.lammpstrj`） |

---

### オプション一覧

| 短い形式 | 長い形式 | 型 | デフォルト | 説明 |
|---|---|---|---|---|
| `-m` | `--mode` | string | `bubble` | 解析モードを指定します。`bubble` または `droplet` を指定できます。 |
| `-s` | `--mesh-size` | double | `2.0` | 解析に用いるメッシュサイズを指定します。 |
| `-t` | `--density-threshold` | double | `0.3` | 各セルの密度のしきい値。この値より高いセルを液相、低いセルを気相として分類します。 |
| `-v` | `--vtk` | bool | `false` | 指定すると、解析結果を VTK 形式で出力します。 |
| `-h` | `--help` | – | – | 使用方法およびオプション一覧を表示します。 |

---

### 処理内容：

* LAMMPS の lammpstrj 形式のファイルを読み込み
* 各フレームについて:
    * Type == 1 の原子の局所密度を計算
    * 密度がしきい値以上のセルを液相とみなす
    * 液相セルを隣接性に基づいてクラスタリング（サイトパーコレーション）
    * クラスター数を標準出力に表示

### ライセンス

このリポジトリはMIT Licenseのもとで公開されています。詳細は[LICENSE](LICENSE)ファイルを参照してください。

