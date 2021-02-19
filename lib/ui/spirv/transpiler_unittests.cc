#include "flutter/lib/ui/spirv/transpiler.h"

#include "third_party/googletest/googletest/include/gtest/gtest.h"

namespace flutter {
namespace testing {

TEST(TranspilerTest, ConstructsTranspiler) {
  auto transpiler = spirv::Transpiler::create();
  EXPECT_TRUE(transpiler);
}

}  // namespace testing
}  // namespace flutter
