// Copyright 2021 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// @dart = 2.6
import 'dart:async';
import 'dart:io';
import 'dart:typed_data';
import 'dart:ui';

import 'package:path/path.dart' as path;
import 'package:test/test.dart';

void main() {
  test('throws exception for invalid shader', () {
    final invalidBytes = Uint8List.fromList([1, 2, 3, 4, 5]);
    expect(() => FragmentShader(invalidBytes, []), throws);
  });

  test('renders simple shader', () async {
    final shaderBytes = await (File(path.join(
      'flutter',
      'testing',
      'resources',
      'fragment_shader_simple.spv',
    ))).readAsBytes();
    final shader = FragmentShader(shaderBytes, []);

    final recorder = PictureRecorder();
    final canvas = Canvas(recorder);
    final paint = Paint()..shader = shader;
    canvas.drawPaint(paint);
    final picture = recorder.endRecording();
    final image = await picture.toImage(100, 100);
    final renderedBytes = await image.toByteData();

    final toFloat = (int v) => v.toDouble() / 255.0;
    final epsilon = 0.5 / 255.0;

    expect(toFloat(renderedBytes.getUint8(0)), closeTo(0.0, epsilon));
    expect(toFloat(renderedBytes.getUint8(1)), closeTo(0.25, epsilon));
    expect(toFloat(renderedBytes.getUint8(2)), closeTo(0.75, epsilon));
    expect(toFloat(renderedBytes.getUint8(3)), closeTo(1.0, epsilon));
  });
}
