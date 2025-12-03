# Maintainer: aixxe <me@aixxe.net>
source "$(dirname "${BASH_SOURCE[0]}")/PKGBUILD.inc"
pkgname="${_pkgbase}-git"
pkgver=pre0.3.2.g43d7471c
provides=("${_pkgbase}")
conflicts=("${_pkgbase}")
source=("git+https://github.com/seraxis/${_pkgbase}.git"
        "https://patch-diff.githubusercontent.com/raw/seraxis/lr2oraja-endlessdream/pull/165.patch"
        "${_common_sources[@]}")
sha256sums=('SKIP'
            '20c95c8f6eaa0637d450a36c5e0da047ccac393dc56c11db7a76bd28bc0422f8'
            "${_common_sha256sums[@]}")

pkgver() {
  cd "${srcdir}/${_pkgbase}"
  local version=$(grep '^endlessdream = ' gradle/libs.versions.toml | sed 's/.*= "\(.*\)"/\1/')
  local commit=$(git rev-parse --short HEAD)
  echo "${version}.g${commit}"
}

prepare() {
  _prepare

  # fixes another issue when building w/newer gradle
  cd "${srcdir}/${_pkgbase}"
  patch -Np1 -i "${srcdir}/165.patch"
}

build() {
  _build
}

package() {
  _package
}
