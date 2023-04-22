message(STATUS "~~~~~~ chillerbot rust module ~~~~~~")

set_glob(RUST_CHILLERBOT_RS GLOB_RECURSE "rs;toml;h;cpp" src/chillerbot-rs
  Cargo.toml
  build.rs
  lib.rs
  rust.rs
  rust_version.rs
)

set_src(RUST_BRIDGE_CHILLERBOT GLOB_RECURSE src/rust-bridge-chillerbot
  rust_version.cpp
  rust_version.h
)

add_library(rust-bridge-chillerbot EXCLUDE_FROM_ALL OBJECT ${RUST_BRIDGE_CHILLERBOT})
list(APPEND TARGETS_OWN rust-bridge-chillerbot)

#####################################################################
# overwrite variables if current content matches expected
#
# could probably also use list(APPEND ..) but lets me strict for now
# to be alerted if somehing changes before weird build errors occur

if(RUST_TARGETS STREQUAL "engine_shared")
    message(STATUS " * patching RUST_TARGETS ...")
    set(RUST_TARGETS engine_shared chillerbot_rs)
else()
    message(SEND_ERROR " failed to patch RUST_TARGETS unexpected content '${RUST_TARGETS}'")
endif()

set(CHILLERBOT_EXPECTED_RUST_SRC
      ${RUST_BASE}
      ${RUST_ENGINE_INTERFACE}
      ${RUST_ENGINE_SHARED}
      Cargo.toml
      Cargo.lock
)

if(RUST_SRC STREQUAL CHILLERBOT_EXPECTED_RUST_SRC)
    message(STATUS " * patching RUST_SRC ...")
    set(RUST_SRC
        ${RUST_BASE}
        ${RUST_ENGINE_INTERFACE}
        ${RUST_ENGINE_SHARED}
        ${RUST_CHILLERBOT_RS}
        Cargo.toml
        Cargo.lock
    )
else()
    message(SEND_ERROR " failed to patch RUST_SRC unexpected content '${RUST_SRC}'")
endif()
