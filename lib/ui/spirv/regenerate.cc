// Copyright 2021 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdint>

#include "flutter/lib/ui/spirv/transpiler.h"

namespace fs = std::filesystem;

int main(int argc, const char* argv[]) {
  auto transpiler = flutter::spirv::Transpiler::create();
  if (argc != 3) {
    std::cerr << "Invalid argument count." << std::endl;
    return -1;
  }

  fs::path path(argv[1]);
  if (!fs::exists(path)) {
    std::cerr << "File does not exist." << std::endl;
    return -1;
  }

  std::fstream input;
  input.open(argv[1]);
  input.seekg(0, std::ios::end);
  std::streampos size = input.tellg();
  input.seekg(0, std::ios::beg);
  std::vector<char> buf(size);
  input.read(buf.data(), size);
  input.close();

  auto result = transpiler->Transpile(buf.data(), size);
  if (result.status != flutter::spirv::Status::kSuccess) {
    std::cerr << result.message << std::endl;
    return -1;
  }

  std::fstream output;
  output.open(argv[2], std::fstream::out | std::fstream::trunc);
  if (!output.is_open()) {
    output.close();
    std::cerr << "failed to open output file" << std::endl;
    return -1;
  }

  std::string sksl = transpiler->GetSkSL();
  output.write(sksl.c_str(), sksl.size());
  output.close();

  return 0;
}
