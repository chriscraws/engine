// Copyright 2020 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/lib/ui/painting/fragment_shader.h"

#include "flutter/lib/ui/dart_wrapper.h"
#include "flutter/lib/ui/ui_dart_state.h"
#include "flutter/lib/ui/spirv/transpiler.h"
#include "third_party/skia/include/core/SkString.h"
#include "third_party/tonic/converter/dart_converter.h"
#include "third_party/tonic/dart_args.h"
#include "third_party/tonic/dart_binding_macros.h"
#include "third_party/tonic/dart_library_natives.h"
#include "third_party/tonic/typed_data/typed_list.h"

using tonic::ToDart;

namespace flutter {

static void FragmentShader_constructor(Dart_NativeArguments args) {
  DartCallConstructor(&FragmentShader::Create, args);
}

IMPLEMENT_WRAPPERTYPEINFO(ui, FragmentShader);

#define FOR_EACH_BINDING(V) \
  V(FragmentShader, initWithSPIRV) \
  V(FragmentShader, setFloatUniform) \
  V(FragmentShader, refresh)

FOR_EACH_BINDING(DART_NATIVE_CALLBACK)

sk_sp<SkShader> FragmentShader::shader(SkFilterQuality q) {
	return shader_;
}

void FragmentShader::initWithSPIRV(
		const tonic::Uint8List& data, Dart_Handle children) {
  std::vector<Shader*> shaders =
      tonic::DartConverter<std::vector<Shader*>>::FromDart(children);
  for (auto& shader : shaders) {
    children_.push_back(shader->shader(
			SkFilterQuality::kHigh_SkFilterQuality));
  }
  auto transpiler = spirv::Transpiler::create();
  auto result = transpiler->Transpile(
    reinterpret_cast<const char*>(data.data()),
    data.num_elements());
  if (result.status != spirv::kSuccess) {
    FML_DLOG(ERROR) << "Invalid SPIR-V: " << result.message;
    return;
  }


  auto sksl = transpiler->GetSkSL();
  uniforms_.resize(transpiler->GetUniformBufferSize());
  initEffect(SkString(sksl.c_str()));
  refresh();
}

void FragmentShader::setFloatUniform(size_t i, float value) {
  if (i >= uniforms_.size()) {
    Dart_ThrowException(tonic::ToDart("Float uniform index $i is out of bounds."));
  }
  uniforms_[i] = value;
}

void FragmentShader::refresh() {
	shader_ = runtime_effect_->makeShader(
      uniforms_.size() == 0 ? SkData::MakeEmpty() : SkData::MakeWithCopy(uniforms_.data(), uniforms_.size() * sizeof(float)),
      children_.data(),
      children_.size(),
      nullptr,
      false);
}

void FragmentShader::initEffect(SkString sksl) {
  SkString err;
  SkRuntimeEffect::Result result = SkRuntimeEffect::Make(sksl);
  runtime_effect_ = result.effect;
  if (!runtime_effect_) {
    FML_DLOG(ERROR) << "Invalid SKSL:\n" << sksl.c_str() << "\nSKSL Error:\n" << result.errorText.c_str();
  } else {
    FML_DLOG(ERROR) << "Valid SKSL:\n" << sksl.c_str();
  }
}

void FragmentShader::RegisterNatives(tonic::DartLibraryNatives* natives) {
  natives->Register(
      {{"FragmentShader_constructor", FragmentShader_constructor, 1, true},
       FOR_EACH_BINDING(DART_REGISTER_NATIVE)});
}

fml::RefPtr<FragmentShader> FragmentShader::Create() {
  return fml::MakeRefCounted<FragmentShader>();
}

FragmentShader::FragmentShader() {}

FragmentShader::~FragmentShader() = default;

}  // namespace flutter

