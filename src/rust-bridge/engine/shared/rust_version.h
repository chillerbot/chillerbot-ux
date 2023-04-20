#pragma once
#include "base/rust.h"
#include "engine/console.h"

void RustVersionPrint(const ::IConsole &console) noexcept;

void RustVersionRegister(::IConsole &console) noexcept;

int ChillerRustGaming(const char *text, size_t size) noexcept;