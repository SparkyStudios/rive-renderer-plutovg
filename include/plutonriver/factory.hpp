// Copyright (c) 2021-present Sparky Studios. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifndef _PLUTONRIVER_FACTORY_HPP_
#define _PLUTONRIVER_FACTORY_HPP_

#include <rive/factory.hpp>
#include <vector>

namespace rive
{
  class PlutonRiver_Factory : public Factory
  {
  public:
    rcp<RenderBuffer> makeBufferU16(Span<const uint16_t>) override;
    rcp<RenderBuffer> makeBufferU32(Span<const uint32_t>) override;
    rcp<RenderBuffer> makeBufferF32(Span<const float>) override;

    rcp<RenderShader> makeLinearGradient(float sx,
      float sy,
      float ex,
      float ey,
      const ColorInt colors[], // [count]
      const float stops[],     // [count]
      size_t count) override;

    rcp<RenderShader> makeRadialGradient(float cx,
      float cy,
      float radius,
      const ColorInt colors[], // [count]
      const float stops[],     // [count]
      size_t count) override;

    std::unique_ptr<RenderPath>
    makeRenderPath(Span<const Vec2D> points, Span<const PathVerb> verbs, FillRule) override;

    std::unique_ptr<RenderPath> makeEmptyRenderPath() override;

    std::unique_ptr<RenderPaint> makeRenderPaint() override;

    std::unique_ptr<RenderImage> decodeImage(Span<const uint8_t>) override;
  };
} // namespace rive

#endif
