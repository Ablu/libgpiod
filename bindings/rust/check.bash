SOURCE_DIR="$(realpath $(dirname ${BASH_SOURCE[0]}))"


# fail test if a command in it failed
set -e

# Link libgpiod statically from the intermediate build results
export SYSTEM_DEPS_LIBGPIOD_NO_PKG_CONFIG=1
export SYSTEM_DEPS_LIBGPIOD_SEARCH_NATIVE="$SOURCE_DIR/../../lib/.libs/"
export SYSTEM_DEPS_LIBGPIOD_LIB=gpiod
export SYSTEM_DEPS_LIBGPIOD_INCLUDE="$SOURCE_DIR/../../include/"
export SYSTEM_DEPS_LIBGPIOD_LINK=static

# libgpiod.a is build without PIC by default, so build without PIE here
export RUSTFLAGS="-C relocation-model=dynamic-no-pic"

testCargoBuildOnMSRV() {
  local msrv=$(grep -oP 'rust-version\s*=\s*"(\K[^"]*)' ${SOURCE_DIR}/libgpiod/Cargo.toml)
  cargo +"$msrv" build --tests --examples
}

testCargoTestsOnStable() {
  cargo +stable test
}

testCargoClippyOnStable() {
  cargo +stable clippy
}

. shunit2