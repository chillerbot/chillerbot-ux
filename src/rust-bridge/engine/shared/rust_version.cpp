#include "base/rust.h"
#include "engine/console.h"

extern "C" {
void cxxbridge1$RustVersionPrint(const ::IConsole &console) noexcept;
int cxxbridge1$ChillerRustGaming(const char *text) noexcept;

void cxxbridge1$RustVersionRegister(::IConsole &console) noexcept;
} // extern "C"

void RustVersionPrint(const ::IConsole &console) noexcept {
  cxxbridge1$RustVersionPrint(console);
}

int ChillerRustGaming(const char *text) noexcept {
  return cxxbridge1$ChillerRustGaming(text);
}

void RustVersionRegister(::IConsole &console) noexcept {
  cxxbridge1$RustVersionRegister(console);
}

