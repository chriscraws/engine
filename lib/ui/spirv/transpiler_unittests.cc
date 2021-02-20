#include "flutter/lib/ui/spirv/transpiler.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "third_party/googletest/googletest/include/gtest/gtest.h"

namespace fs = std::filesystem;

namespace flutter {
namespace testing {

namespace {

constexpr char kGeneratedDirectory[] = "gen/flutter/lib/ui/spirv";
constexpr char kSourceDirectory[] = "obj/flutter/lib/ui/spirv";

}  // namespace

TEST(TranspilerTest, ConstructsTranspiler) {
  auto transpiler = spirv::Transpiler::create();
  EXPECT_TRUE(transpiler);
}

TEST(TranspilerTest, CreatesSkSLMatchingGoldens) {
  fs::path p = fs::current_path();

  fs::path generated_dir = p / kGeneratedDirectory;

  auto it = fs::directory_iterator(generated_dir);
  for (const fs::directory_entry& file : it) {
    auto spv = fs::path(kSourceDirectory) / file.path().stem();
    spv.replace_extension(".spv");
    ASSERT_TRUE(fs::exists(spv));

    std::fstream input;
    input.open(spv);
    input.seekg(0, std::ios::end);
    std::streampos size = input.tellg();
    input.seekg(0, std::ios::beg);
    std::vector<char> buf(size);
    input.read(buf.data(), size);
    input.close();

    auto transpiler = spirv::Transpiler::create();
    auto result = transpiler->Transpile(buf.data(), size);

    EXPECT_EQ(result.status, spirv::Status::kSuccess);

    std::fstream golden_file(file.path());
    std::stringstream golden;
    golden << golden_file.rdbuf();

    EXPECT_EQ(transpiler->GetSkSL(), golden.str());
  }
}

}  // namespace testing
}  // namespace flutter
