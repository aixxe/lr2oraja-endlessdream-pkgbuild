# Maintainer: aixxe <me@aixxe.net>
source "$(dirname "${BASH_SOURCE[0]}")/PKGBUILD.inc"
pkgname="${_pkgbase}-git"
pkgver=pre0.3.1.gf65e0669
provides=("${_pkgbase}")
conflicts=("${_pkgbase}")
source=("git+https://github.com/seraxis/${_pkgbase}.git"
        "${_common_sources[@]}")
sha256sums=('SKIP'
            "${_common_sha256sums[@]}")

pkgver() {
  cd "${srcdir}/${_pkgbase}"
  local version=$(grep '^endlessdream = ' gradle/libs.versions.toml | sed 's/.*= "\(.*\)"/\1/')
  local commit=$(git rev-parse --short HEAD)
  echo "${version}.g${commit}"
}

prepare() {
  _prepare
}

build() {
  _build
}

package() {
  _package
}
