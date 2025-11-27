#!/usr/bin/env bash
set -e

# read-only directory where default game assets exists
export BEATORAJA_VARIANT="lr2oraja-endlessdream"
export BEATORAJA_INSTALL_DIR="/usr/share/${BEATORAJA_VARIANT}"

# include application-specific native libraries in library load path
export LD_LIBRARY_PATH="${BEATORAJA_INSTALL_DIR}/natives:${LD_LIBRARY_PATH:-}"

# working directory where configuration & player-specific files exists
export BEATORAJA_USER_DIR="${BEATORAJA_USER_DIR:-${XDG_CONFIG_HOME:-$HOME/.config}/${BEATORAJA_VARIANT}}"
mkdir -p "${BEATORAJA_USER_DIR}" && cd "${BEATORAJA_USER_DIR}"

# tests if a flag is supported by the java runtime
test_flag() {
  if java "$1" -version >/dev/null 2>&1; then
    echo "$1"
  else
    echo ""
  fi
}

# symlink default files to user directory
setup_default_assets() {
  local asset_dirs=('defaultsound' 'folder' 'font' 'random')

  for dir in "${asset_dirs[@]}"; do
    local src_dir="${BEATORAJA_INSTALL_DIR}/${dir}"
    local dst_dir="${BEATORAJA_USER_DIR}/${dir}"
    
    if [[ -d "${src_dir}" ]]; then
      mkdir -p "${dst_dir}"

      for file in "${src_dir}"/*; do
        [[ -e "${file}" ]] || continue
        local filename=$(basename "${file}")
        local dst_file="${dst_dir}/${filename}"

        if [[ ! -e "${dst_file}" ]]; then
          ln -s "${file}" "${dst_file}"
        fi
      done
    fi
  done

  # symlink entire default skin directory
  local skin_src="${BEATORAJA_INSTALL_DIR}/skin/default"
  local skin_dst="${BEATORAJA_USER_DIR}/skin/default"

  if [[ -d "${skin_src}" && ! -e "${skin_dst}" ]]; then
    mkdir -p "${BEATORAJA_USER_DIR}/skin"
    ln -s "${skin_src}" "${skin_dst}"
  fi

  # create empty directories if needed
  mkdir -p "${BEATORAJA_USER_DIR}"/{ir,table}
}

# pull in additional java options from JDK_JAVA_OPTIONS
build_java_options() {
  local options=(
    -Dsun.java2d.opengl=true
    -Dawt.useSystemAAFontSettings=on
    -Dswing.aatext=true
    -Dswing.defaultlaf=com.sun.java.swing.plaf.gtk.GTKLookAndFeel
    -Dfile.encoding="UTF-8"
  )

  # prioritize using shenandoah GC...
  GC_OPTION=$(test_flag "-XX:+UseShenandoahGC")

  # ...if it's not available, try zgc instead
  if [ -z "$GC_OPTION" ]; then
    GC_OPTION=$(test_flag "-XX:+UseZGC")
  fi

  if [ -n "$GC_OPTION" ]; then
    options+=("$GC_OPTION")
  fi

  # some extra optimization flags
  OPTIMIZATION_FLAGS=(
    -XX:+UseNUMA
    -XX:+AlwaysPreTouch
    -XX:-UseBiasedLocking
  )

  for flag in "${OPTIMIZATION_FLAGS[@]}"; do
    FLAG_OPTION=$(test_flag "$flag")
    if [ -n "$FLAG_OPTION" ]; then
      options+=("$FLAG_OPTION")
    fi
  done

  if [[ -n "${JDK_JAVA_OPTIONS}" ]]; then
    while IFS= read -r opt; do
      [[ -n "$opt" ]] && options+=("$opt")
    done < <(echo "${JDK_JAVA_OPTIONS}" | tr ' ' '\n')
  fi

  echo "${options[*]}"
}

# ensure required default files exist
setup_default_assets

# launch the game!
JDK_JAVA_OPTIONS=$(build_java_options) \
exec java \
  -Xms"${JAVA_HEAP_MIN:-1g}" \
  -Xmx"${JAVA_HEAP_MAX:-4g}" \
  --enable-native-access=ALL-UNNAMED \
  -cp /usr/share/java/${BEATORAJA_VARIANT}.jar:ir/* \
  bms.player.beatoraja.MainLoaderEntrypoint \
  "$@"
