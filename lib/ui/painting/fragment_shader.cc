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
  V(FragmentShader, initWithSource) \
  V(FragmentShader, initWithSPIRV) \
  V(FragmentShader, setTime) \
  V(FragmentShader, setImage) \
  V(FragmentShader, setFloatUniform) \
  V(FragmentShader, refresh)

FOR_EACH_BINDING(DART_NATIVE_CALLBACK)

void FragmentShader::initWithSource(const std::string& source) {
  initEffect(SkString(source.c_str()));
  setShader();
}

void FreeFinalizer(void* isolate_callback_data,
                   Dart_WeakPersistentHandle handle,
                   void* peer) {
  free(peer);
}

void FragmentShader::initWithSPIRV(const tonic::Uint8List& data) {
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
	// uniform_data_.Set(
	// 		UIDartState::Current(),
	// 		Dart_NewExternalTypedDataWithFinalizer(
	// 			Dart_TypedData_kFloat32,  // type
	// 			dst,
	// 			length,
	// 			dst,  // peer
	// 			size,  // external_allocation_size
	// 			FreeFinalizer));

	//uniform_data_.get()->value().Set(tonic::DartState::Current(), typed_data);
  // auto uniform_data = Dart_NewExternalTypedData(
	// 		Dart_TypedData_kFloat32,
	// 		uniform_data_buffer_.data(),
	// 		uniform_data_buffer_.size());
	// uniform_data_handle_ = std::make_shared<tonic::DartPersistentValue>(
	// 		tonic::DartState::Current(),
	// 		uniform_data);
  initWithSource(sksl);
}

void FragmentShader::setFloatUniform(size_t i, float value) {
	if (i >= uniforms_.size()) {
    Dart_ThrowException(tonic::ToDart("Float uniform index $i is out of bounds."));
	}
	uniforms_[i] = value;
}

void FragmentShader::setTime(float time) {
  t_ = time;
  setShader();
}

void FragmentShader::setImage(CanvasImage* image,
                              SkTileMode tmx,
                              SkTileMode tmy,
                              const tonic::Float64List& matrix4) {
  SkMatrix sk_matrix = ToSkMatrix(matrix4);
  input_ = image->image()->makeShader(tmx, tmy, &sk_matrix);
  setShader();
}

void FragmentShader::refresh() {
  set_shader(UIDartState::CreateGPUObject(
				runtime_effect_->makeShader(
					SkData::MakeWithCopy(
						static_cast<void*>(uniforms_.data()),
						uniforms_.size() * sizeof(float)),  // uniforms
					nullptr,  // children
					0,  // childCount
					nullptr,  // localMatrix
					false /* isOpaque */)));
}

void FragmentShader::initEffect(SkString sksl) {
  SkString err;
  std::tie(runtime_effect_, err) = SkRuntimeEffect::Make(sksl);
  if (!runtime_effect_) {
    FML_DLOG(ERROR) << "Invalid SKSL:\n" << sksl.c_str() << "\nSKSL Error:\n" << err.c_str();
  }// else {
   // FML_DLOG(ERROR) << "Valid SKSL:\n" << sksl.c_str();
   //}
}

// Creates a builder and sets time uniform and image child if
// they are valid and are defined in the SKSL program.
//
// After any uniforms/children are set on the builser, the shader is 
// created and set.
void FragmentShader::setShader() {
  //set_shader(UIDartState::CreateGPUObject(builder_->makeShader(nullptr, false)));
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

FragmentShader::~FragmentShader() {}

}  // namespace flutter

