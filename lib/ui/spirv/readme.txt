This directory contains a C++ transpiler and tests for converting SPIR-V into
SkSL. At this time, a subset of SPIR-V is supported. 

More information on the feature set and design can be found at

https://flutter.dev/go/shaders


The main code is contained in transpiler.h and transpiler.cc. The remaining
files are concerned with testing.

Testing is achieved by using the transpiler to create SkSL from the '.spv'
files under the 'golden' directory. The output of the transpiler is compared
to the '.sksl' files in the same directory. The sksl files must be updated
if changes are made to the transpiler. They can be copied from the build
directory. For a host-build, these are located at:

$FLUTTER_ENGINE_BUILDROOT/src/out/host_debug_unopt/gen/flutter/lib/ui/spirv/*.sksl

SPIR-V test files can be modified by hand by converting to SPIR-V assembly with the `spirv-dis`, then re-compiled to binary SPIR-V with `spirv-as`. These tools can be found here https://github.com/KhronosGroup/SPIRV-Tools/blob/master/docs/downloads.md

SPIR-V test files can also be modified by generating new ones using the dart `expr` library, currently hosted at https://github.com/chriscraws/ssir
