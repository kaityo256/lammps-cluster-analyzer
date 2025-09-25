# LAMMPS Cluster Analyzer

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

`lammps-cluster-analyzer` は、LAMMPS のトラジェクトリファイル (`lammpstrj`) を解析し、Type 1 の原子について局所密度の解析とクラスタリングを行うスタンドアロンの C++ コードです。

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

## 設定ファイル

カレントディレクトリに`cluster-analyze.cfg`というファイルを作成することでパラメータを設定します。このファイルがないと実行時にエラーになります。実行可能ファイル`cluster-analyze`と同じ場所ではなく、実行場所に必要であることに注意。

ファイルには以下のような内容を設定します。

```cfg
mode = bubble
mesh_size = 2.0
density_threshold = 0.3
```

それぞれの意味は以下の通り。

| パラメータ名        | 型       | 説明                                                                 | 省略時のデフォルト値 |
|---------------------|----------|----------------------------------------------------------------------|----------------------|
| mode                | 文字列   | `bubble` または `droplet` を指定する。                               | （必須）             |
| mesh_size           | 実数     | メッシュサイズ。                                                     | 2.0                  |
| density_threshold   | 実数     | 各セルの密度のしきい値。これより高いセルを液相、低いセルを気相とみなす。 | 0.3                  |


### 処理内容：

* LAMMPS の lammpstrj 形式のファイルを読み込み
* 各フレームについて:
    * Type == 1 の原子の局所密度を計算
    * 密度がしきい値以上のセルを液相とみなす
    * 液相セルを隣接性に基づいてクラスタリング（サイトパーコレーション）
    * クラスター数を標準出力に表示

### ライセンス

このリポジトリはMIT Licenseのもとで公開されています。詳細は[LICENSE](LICENSE)ファイルを参照してください。

