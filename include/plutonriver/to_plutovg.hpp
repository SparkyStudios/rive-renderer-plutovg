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

#ifndef _PLUTONRIVER_TO_PLUTOVG_HPP_
#define _PLUTONRIVER_TO_PLUTOVG_HPP_

#include <rive/math/mat2d.hpp>
#include <rive/math/raw_path.hpp>
#include <rive/math/vec2d.hpp>
#include <rive/renderer.hpp>
#include <rive/shapes/paint/blend_mode.hpp>
#include <rive/shapes/paint/stroke_cap.hpp>
#include <rive/shapes/paint/stroke_join.hpp>


#include <plutovg.h>

namespace rive
{
  class ToPlutoVG
  {
  public:
    static plutovg_matrix_t convert(const rive::Mat2D& m)
    {
      plutovg_matrix_t result;
      plutovg_matrix_init(&result, m[0], m[1], m[2], m[3], m[4], m[5]);
      return result;
    }

    static plutovg_point_t convert(const rive::Vec2D& point)
    {
      plutovg_point_t result;
      result.x = point.x;
      result.y = point.y;
      return result;
    }

    static plutovg_color_t convert(const ColorInt& color)
    {
      plutovg_color_t result;
      plutovg_color_init_rgba8(
        &result,
        color >> 16 & 255,
        color >> 8 & 255,
        color >> 0 & 255,
        color >> 24 & 255);
      return result;
    }

    static plutovg_fill_rule_t convert(FillRule value)
    {
      switch (value)
      {
        case FillRule::evenOdd:
          return plutovg_fill_rule_even_odd;
        default:
        case FillRule::nonZero:
          return plutovg_fill_rule_non_zero;
      }
    }

    static plutovg_line_cap_t convert(rive::StrokeCap cap)
    {
      switch (cap)
      {
        default:
        case StrokeCap::butt:
          return plutovg_line_cap_butt;
        case StrokeCap::round:
          return plutovg_line_cap_round;
        case StrokeCap::square:
          return plutovg_line_cap_square;
      }
    }

    static plutovg_line_join_t convert(StrokeJoin join)
    {
      switch (join)
      {
        case StrokeJoin::bevel:
          return plutovg_line_join_bevel;
        case StrokeJoin::round:
          return plutovg_line_join_round;
        default:
        case StrokeJoin::miter:
          return plutovg_line_join_miter;
      }
    }

    static plutovg_operator_t convert(BlendMode blendMode)
    {
      // switch (blendMode)
      // {
      // case BlendMode::srcOver:
      //     return plutovg_operator_src_over;
      // case BlendMode::screen:
      //     return plutovg_operator_s;
      // case BlendMode::overlay:
      //     return SkBlendMode::kOverlay;
      // case BlendMode::darken:
      //     return SkBlendMode::kDarken;
      // case BlendMode::lighten:
      //     return SkBlendMode::kLighten;
      // case BlendMode::colorDodge:
      //     return SkBlendMode::kColorDodge;
      // case BlendMode::colorBurn:
      //     return SkBlendMode::kColorBurn;
      // case BlendMode::hardLight:
      //     return SkBlendMode::kHardLight;
      // case BlendMode::softLight:
      //     return SkBlendMode::kSoftLight;
      // case BlendMode::difference:
      //     return plutovg_operator_dst_out;
      // case BlendMode::exclusion:
      //     return SkBlendMode::kExclusion;
      // case BlendMode::multiply:
      //     return SkBlendMode::kMultiply;
      // case BlendMode::hue:
      //     return SkBlendMode::kHue;
      // case BlendMode::saturation:
      //     return SkBlendMode::kSaturation;
      // case BlendMode::color:
      //     return plutovg_operator_src;
      // case BlendMode::luminosity:
      //     return SkBlendMode::kLuminosity;
      // }
      // assert(false);
      // TODO: Find how to deal with this
      return plutovg_operator_src_over;
    }

    static plutovg_path_t* convert(const RawPath& rp)
    {
      plutovg_path_t* result = plutovg_path_create();

      RawPath::Iter iter(rp);
      while (auto rec = iter.next())
      {
        switch (rec.verb)
        {
          case PathVerb::move:
            plutovg_path_move_to(result, rec.pts[0].x, rec.pts[0].y);
            break;
          case PathVerb::line:
            plutovg_path_line_to(result, rec.pts[0].x, rec.pts[0].y);
            break;
          case PathVerb::quad:
            plutovg_path_quad_to(result, rec.pts[0].x, rec.pts[0].y, rec.pts[1].x, rec.pts[1].y);
            break;
          case PathVerb::cubic:
            plutovg_path_cubic_to(result, rec.pts[0].x, rec.pts[0].y, rec.pts[1].x, rec.pts[1].y, rec.pts[2].x, rec.pts[2].y);
            break;
          case PathVerb::close:
            plutovg_path_close(result);
            break;
        }
      }

      return result;
    }
  };
} // namespace rive

#endif
