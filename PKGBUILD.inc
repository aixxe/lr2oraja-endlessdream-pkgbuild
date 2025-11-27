_pkgbase=lr2oraja-endlessdream
pkgrel=1
pkgdesc="A featureful fork of beatoraja"
arch=('x86_64')
url="https://github.com/seraxis/lr2oraja-endlessdream"
license=('GPL-3.0-only')
depends=('bash' 'java-runtime>=17')
makedepends=('gradle' 'java-environment>=17' 'portaudio' 'git' 'cmake' 'gcc')
optdepends=('portaudio: enables low-latency audio output options')
_javafx_version="21.0.8"
_portaudio_java_commit="2ec5cc47d6f8abe85ddb09c34e69342bfe72c60b"
_common_sources=("https://github.com/philburk/portaudio-java/archive/${_portaudio_java_commit}.tar.gz"
                 "${_pkgbase}.sh"
                 "${_pkgbase}.desktop"
                 "${_pkgbase}-icon.png"
                 "MainLoaderEntrypoint.java")
_common_sha256sums=('7a70d90d449fc9d91026c54e1d08303242749475ad20b30b5bfa45fc93f18043'
                    '872b9cb25857622aec70a8a1cd694a6c8b2c4f86ad4716c72862c290ec069cd8'
                    'bfa611672d6926830edc33d7845ce1430b415cf42325f565e609dc6997dcb3f9'
                    'fdbd37ff43aa6af20f9eb643bf271a77ef579014970a7a3dcecf78e65123d83d'
                    '95c603669e6a4864060fc976be349e345a8f320606286f7a830021243ababedb')

_prepare() {
  cd "${srcdir}/${_pkgbase}"
  git submodule update --init --recursive

  # no strict toolchain language version
  sed -i 's@languageVersion\.set(JavaLanguageVersion\.of(17))@// &@' core/build.gradle.kts

  # fix for build error on gradle 9.x
  sed -i 's/id("org.gradle.kotlin.kotlin-dsl") version "4.2.1"/id("org.gradle.kotlin.kotlin-dsl") version "6.2.0"/' buildSrc/build.gradle.kts

  # embed javafx into jar - not ideal, but i'd rather not rely on other AUR packages
  sed -i "/kotlin = \"1.9.20\"/a javafx = \"${_javafx_version}\"" gradle/libs.versions.toml
  sed -i '/id("application")/a \    id("org.openjfx.javafxplugin") version "0.1.0"' core/build.gradle.kts
  sed -i '/^dependencies {/a \
    javafx { \
        version = libs.versions.javafx.get() \
        modules = listOf("javafx.base", "javafx.graphics", "javafx.controls", "javafx.fxml", "javafx.web") \
    }' core/build.gradle.kts

  # copy javafx wrapper class for main loader
  cp "${srcdir}/MainLoaderEntrypoint.java" \
     "${srcdir}/${_pkgbase}/core/src/bms/player/beatoraja/"
}

_build() {
  cd "${srcdir}/${_pkgbase}"

  # build native dependencies
  JAVA_HOME=${JAVA_HOME:-/usr/lib/jvm/default} \
  cmake \
    -S "${srcdir}/portaudio-java-${_portaudio_java_commit}" \
    -B "${srcdir}/portaudio-java-build" \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
  cmake --build "${srcdir}/portaudio-java-build"

  # build game with gradle
  gradle --no-daemon -Dplatform=linux core:shadowJar
}

_package() {
  cd "${srcdir}/${_pkgbase}"

  # launcher script
  install -Dm755 "${srcdir}/${_pkgbase}.sh" \
    "${pkgdir}/usr/bin/${_pkgbase}"

  # main game executable
  install -Dm644 dist/lr2oraja-*-endlessdream-linux-*.jar \
    "${pkgdir}/usr/share/java/${_pkgbase}.jar"

  # default game assets
  install -dm755 "${pkgdir}/usr/share/${_pkgbase}"
  cp -r assets/* "${pkgdir}/usr/share/${_pkgbase}/"
  rm -f "${pkgdir}/usr/share/${_pkgbase}"/beatoraja-config*

  # portaudio-java native library
  install -Dm644 "${srcdir}/portaudio-java-build/libjportaudio_0_1_0.so" \
    "${pkgdir}/usr/share/${_pkgbase}/natives/libjportaudio.so"

  # desktop application icon
  install -Dm644 "${srcdir}/${_pkgbase}-icon.png" \
    "${pkgdir}/usr/share/pixmaps/${_pkgbase}-icon.png"

  # desktop application entry
  install -Dm644 "${srcdir}/${_pkgbase}.desktop" \
    "${pkgdir}/usr/share/applications/${_pkgbase}.desktop"
  sed -i "s/__PKGVER__/${pkgver}/" \
    "${pkgdir}/usr/share/applications/${_pkgbase}.desktop"
}
