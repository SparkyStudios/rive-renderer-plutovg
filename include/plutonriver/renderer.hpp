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

#ifndef _PLUTONRIVER_RENDERER_HPP_
#define _PLUTONRIVER_RENDERER_HPP_

#include <rive/renderer.hpp>

#include <plutovg.h>

namespace rive
{
  class PlutoVG_Renderer : public Renderer
  {
  protected:
    plutovg_t* m_context;

  public:
    PlutoVG_Renderer(plutovg_surface_t* surface)
      : m_context(plutovg_create(surface))
    {
    }
    ~PlutoVG_Renderer() override;

    void save() override;
    void restore() override;
    void transform(const Mat2D& transform) override;
    void clipPath(RenderPath* path) override;
    void drawPath(RenderPath* path, RenderPaint* paint) override;
    void drawImage(const RenderImage*, BlendMode, float opacity) override;
    void drawImageMesh(const RenderImage*,
      rcp<RenderBuffer> vertices_f32,
      rcp<RenderBuffer> uvCoords_f32,
      rcp<RenderBuffer> indices_u16,
      BlendMode,
      float opacity) override;
  };
} // namespace rive

#endif /* _PLUTONRIVER_RENDERER_HPP_ */
