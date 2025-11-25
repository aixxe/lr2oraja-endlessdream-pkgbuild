# Maintainer: aixxe <me@aixxe.net>
source "$(dirname "${BASH_SOURCE[0]}")/PKGBUILD.inc"
pkgname="${_pkgbase}-git"
pkgver=pre0.3.2.g52fb7cf7
provides=("${_pkgbase}")
conflicts=("${_pkgbase}")
source=("git+https://github.com/seraxis/${_pkgbase}.git"
        "https://patch-diff.githubusercontent.com/raw/seraxis/lr2oraja-endlessdream/pull/165.patch"
        "${_common_sources[@]}")
sha256sums=('SKIP'
            '292fbffaea21c4f8deff5c0d55c19710890b484d697b9a954276286d19475aa4'
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
