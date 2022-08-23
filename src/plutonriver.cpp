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

#include <plutovg.h>

#include <rive/math/vec2d.hpp>
#include <utils/factory_utils.hpp>

#include <plutonriver/factory.hpp>
#include <plutonriver/renderer.hpp>
#include <plutonriver/to_plutovg.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include <stb_image.h>

#include <stb_image_write.h>

using namespace rive;

class PlutoVG_RenderPath : public RenderPath
{
public:
  PlutoVG_RenderPath()
    : m_path(plutovg_path_create())
  {
  }
  PlutoVG_RenderPath(plutovg_path_t* path)
    : m_path(path)
  {
  }

  ~PlutoVG_RenderPath() override
  {
    plutovg_path_destroy(m_path);
  }

  const plutovg_path_t* path() const { return m_path; }

  void reset() override;
  void addRenderPath(RenderPath* path, const Mat2D& transform) override;
  void fillRule(FillRule value) override;
  void moveTo(float x, float y) override;
  void lineTo(float x, float y) override;
  void cubicTo(float ox, float oy, float ix, float iy, float x, float y) override;
  void close() override;

private:
  friend class rive::PlutoVG_Renderer;

  plutovg_path_t* m_path{nullptr};
  plutovg_fill_rule_t m_fillRule{plutovg_fill_rule_non_zero};
};

class PlutoVG_RenderPaint : public RenderPaint
{
public:
  PlutoVG_RenderPaint() {}

  void style(RenderPaintStyle style) override;
  void color(unsigned int value) override;
  void thickness(float value) override;
  void join(StrokeJoin value) override;
  void cap(StrokeCap value) override;
  void blendMode(BlendMode value) override;
  void shader(rcp<RenderShader> shader) override;

private:
  friend class rive::PlutoVG_Renderer;

  RenderPaintStyle m_style{RenderPaintStyle::fill};
  unsigned int m_color{0};
  float m_thickness{1.0f};
  StrokeCap m_cap{StrokeCap::round};
  StrokeJoin m_join{StrokeJoin::round};
  BlendMode m_blendMode{BlendMode::srcOver};
  rcp<RenderShader> m_shader{nullptr};
};

class PlutoVG_RenderImage : public RenderImage
{
public:
  PlutoVG_RenderImage(plutovg_surface_t* surface);
  PlutoVG_RenderImage(plutovg_texture_t* texture);

  ~PlutoVG_RenderImage() override
  {
    plutovg_texture_destroy(m_texture);

    free(plutovg_surface_get_data(m_surface));
    plutovg_surface_destroy(m_surface);
  }

  const plutovg_texture_t* texture() const { return m_texture; }

private:
  friend class rive::PlutoVG_Renderer;

  plutovg_texture_t* m_texture{nullptr};
  plutovg_surface_t* m_surface{nullptr};
};

class PlutoVG_RenderShader : public RenderShader
{
public:
  PlutoVG_RenderShader() {}
  PlutoVG_RenderShader(plutovg_gradient_t* gradient)
    : m_gradient(gradient)
  {
  }

  ~PlutoVG_RenderShader() override
  {
    plutovg_gradient_destroy(m_gradient);
  }

  const plutovg_gradient_t* gradient() const { return m_gradient; }

private:
  friend class rive::PlutoVG_Renderer;

  plutovg_gradient_t* m_gradient{nullptr};
};

void PlutoVG_RenderPath::reset()
{
  plutovg_path_clear(m_path);
}

void PlutoVG_RenderPath::addRenderPath(RenderPath* path, const Mat2D& transform)
{
  const auto& matrix = ToPlutoVG::convert(transform);
  plutovg_path_add_path(m_path, reinterpret_cast<PlutoVG_RenderPath*>(path)->m_path, &matrix);
}

void PlutoVG_RenderPath::fillRule(FillRule fill)
{
  m_fillRule = ToPlutoVG::convert(fill);
}

void PlutoVG_RenderPath::moveTo(float x, float y)
{
  plutovg_path_move_to(m_path, x, y);
}

void PlutoVG_RenderPath::lineTo(float x, float y)
{
  plutovg_path_line_to(m_path, x, y);
}

void PlutoVG_RenderPath::cubicTo(float ox, float oy, float ix, float iy, float x, float y)
{
  plutovg_path_cubic_to(m_path, ox, oy, ix, iy, x, y);
}

void PlutoVG_RenderPath::close()
{
  plutovg_path_close(m_path);
}

void PlutoVG_RenderPaint::style(RenderPaintStyle style)
{
  m_style = style;
}

void PlutoVG_RenderPaint::color(unsigned int color)
{
  m_color = color;
}

void PlutoVG_RenderPaint::thickness(float thickness)
{
  m_thickness = thickness;
}

void PlutoVG_RenderPaint::join(StrokeJoin join)
{
  m_join = join;
}

void PlutoVG_RenderPaint::cap(StrokeCap cap)
{
  m_cap = cap;
}

void PlutoVG_RenderPaint::blendMode(BlendMode blendMode)
{
  m_blendMode = blendMode;
}

void PlutoVG_RenderPaint::shader(rcp<RenderShader> shader)
{
  m_shader = shader;
}

PlutoVG_RenderImage::PlutoVG_RenderImage(plutovg_surface_t* surface)
  : m_texture(plutovg_texture_create(surface))
  , m_surface(surface)
{
  m_Width = plutovg_surface_get_width(m_surface);
  m_Height = plutovg_surface_get_height(m_surface);
}

PlutoVG_RenderImage::PlutoVG_RenderImage(plutovg_texture_t* texture)
  : m_texture(texture)
{
  m_surface = plutovg_texture_get_surface(texture);
  m_Width = plutovg_surface_get_width(m_surface);
  m_Height = plutovg_surface_get_height(m_surface);
}

PlutoVG_Renderer::~PlutoVG_Renderer()
{
  plutovg_destroy(m_context);
  plutovg_surface_destroy(m_surface);
}

void PlutoVG_Renderer::save()
{
  if (m_context == nullptr)
    return;

  plutovg_save(m_context);
}

void PlutoVG_Renderer::restore()
{
  if (m_context == nullptr)
    return;

  plutovg_restore(m_context);
}

void PlutoVG_Renderer::transform(const Mat2D& transform)
{
  if (m_context == nullptr)
    return;

  const auto& matrix = ToPlutoVG::convert(transform);
  plutovg_transform(m_context, &matrix);
}

void PlutoVG_Renderer::clipPath(RenderPath* path)
{
  if (m_context == nullptr)
    return;

  plutovg_add_path(m_context, reinterpret_cast<PlutoVG_RenderPath*>(path)->path());
  plutovg_clip(m_context);
}

void PlutoVG_Renderer::drawPath(RenderPath* path, RenderPaint* paint)
{
  if (m_context == nullptr)
    return;

  const auto* pathData = reinterpret_cast<PlutoVG_RenderPath*>(path);
  const auto* paintData = reinterpret_cast<PlutoVG_RenderPaint*>(paint);

  plutovg_add_path(m_context, pathData->path());

  if (paintData->m_shader != nullptr)
  {
    const auto* shaderData = reinterpret_cast<PlutoVG_RenderShader*>(paintData->m_shader.get());
    plutovg_set_source_gradient(m_context, shaderData->m_gradient);
  }
  else
  {
    const plutovg_color_t& color = ToPlutoVG::convert(paintData->m_color);
    plutovg_set_source_color(m_context, &color);
  }

  switch (paintData->m_style)
  {
    case RenderPaintStyle::fill:
      plutovg_set_fill_rule(m_context, pathData->m_fillRule);
      plutovg_fill(m_context);
      break;

    case RenderPaintStyle::stroke:
      plutovg_set_line_width(m_context, paintData->m_thickness);
      plutovg_set_line_cap(m_context, ToPlutoVG::convert(paintData->m_cap));
      plutovg_set_line_join(m_context, ToPlutoVG::convert(paintData->m_join));
      plutovg_stroke(m_context);
      break;
  }
}

void PlutoVG_Renderer::drawImage(const RenderImage* image, BlendMode blendMode, float opacity)
{
  if (m_context == nullptr)
    return;

  const auto* imageData = reinterpret_cast<const PlutoVG_RenderImage*>(image);

  plutovg_rect(m_context, 0, 0, imageData->m_Width, imageData->m_Height);
  plutovg_set_source_texture(m_context, imageData->m_texture);
  plutovg_set_opacity(m_context, opacity);
  plutovg_set_operator(m_context, ToPlutoVG::convert(blendMode));
  plutovg_fill(m_context);
}

void PlutoVG_Renderer::drawImageMesh(const RenderImage* image,
  rcp<RenderBuffer> vertices,
  rcp<RenderBuffer> uvCoords,
  rcp<RenderBuffer> indices,
  BlendMode blendMode,
  float opacity)
{
  // if (m_context == nullptr)
  //   return;

  // // need our vertices and uvs to agree
  // assert(vertices->count() == uvCoords->count());
  // // vertices and uvs are arrays of floats, so we need their counts to be
  // // even, since we treat them as arrays of points
  // assert((vertices->count() & 1) == 0);

  // // const int vertexCount = vertices->count() >> 1;

  // plutovg_matrix_t matrix;
  // plutovg_matrix_init_identity(&matrix);

  // // auto uvs = (const plutovg_point_t *)DataRenderBuffer::Cast(uvCoords.get())->vecs();

  // const auto* imageData = reinterpret_cast<const PlutoVG_RenderImage*>(image);

  // plutovg_set_source_texture(m_context, imageData->m_texture);
  // plutovg_set_opacity(m_context, opacity);
  // plutovg_set_operator(m_context, ToPlutoVG::convert(blendMode));
  // plutovg_set_matrix(m_context, &matrix);
  // plutovg_paint(m_context);
}

int PlutoVG_Renderer::width() const
{
  if (m_surface == nullptr)
    return 0;

  return plutovg_surface_get_width(m_surface);
}

int PlutoVG_Renderer::height() const
{
  if (m_surface == nullptr)
    return 0;

  return plutovg_surface_get_height(m_surface);
}

int PlutoVG_Renderer::stride() const
{
  if (m_surface == nullptr)
    return 0;

  return plutovg_surface_get_stride(m_surface);
}

uint8_t* PlutoVG_Renderer::data() const
{
  if (m_surface == nullptr)
    return nullptr;

  return plutovg_surface_get_data(m_surface);
}

void PlutoVG_Renderer::writePNG(const char* filename) const
{
  if (m_surface == nullptr)
    return;

  const uint8_t* data = this->data();
  const int width = this->width();
  const int height = this->height();
  const int stride = this->stride();

  auto* image = static_cast<uint8_t*>(calloc(1, static_cast<size_t>(stride) * height));

  for (int y = 0; y < height; y++)
  {
    const auto* src = reinterpret_cast<const uint32_t*>(data + stride * y);
    auto* dst = reinterpret_cast<uint32_t*>(image + stride * y);
    for (int x = 0; x < width; x++)
    {
      const uint32_t a = src[x] >> 24;
      const uint32_t r = src[x] >> 16 & 255;
      const uint32_t g = src[x] >> 8 & 255;
      const uint32_t b = src[x] >> 0 & 255;

      dst[x] = (a << 24) | (b << 16) | (g << 8) | r;
    }
  }

  stbi_write_png(filename, width, height, 4, image, stride);
  free(image);
}

rcp<RenderBuffer> PlutonRiver_Factory::makeBufferU16(Span<const uint16_t> data)
{
  return DataRenderBuffer::Make(data);
}

rcp<RenderBuffer> PlutonRiver_Factory::makeBufferU32(Span<const uint32_t> data)
{
  return DataRenderBuffer::Make(data);
}

rcp<RenderBuffer> PlutonRiver_Factory::makeBufferF32(Span<const float> data)
{
  return DataRenderBuffer::Make(data);
}

rcp<RenderShader> PlutonRiver_Factory::makeLinearGradient(float sx,
  float sy,
  float ex,
  float ey,
  const ColorInt colors[], // [count]
  const float stops[],     // [count]
  size_t count)
{
  plutovg_gradient_t* gradient = plutovg_gradient_create_linear(sx, sy, ex, ey);

  for (size_t i = 0; i < count; ++i)
  {
    plutovg_gradient_stop_t stop;
    stop.offset = stops[i];
    stop.color = ToPlutoVG::convert(colors[i]);

    plutovg_gradient_add_stop(gradient, &stop);
  }

  return rcp<RenderShader>(new PlutoVG_RenderShader(gradient));
}

rcp<RenderShader> PlutonRiver_Factory::makeRadialGradient(float cx,
  float cy,
  float radius,
  const ColorInt colors[], // [count]
  const float stops[],     // [count]
  size_t count)
{
  plutovg_gradient_t* gradient = plutovg_gradient_create_radial(cx, cy, radius, cx, cy, 0);

  for (size_t i = 0; i < count; ++i)
  {
    plutovg_gradient_stop_t stop;
    stop.offset = stops[i];
    stop.color = ToPlutoVG::convert(colors[i]);

    plutovg_gradient_add_stop(gradient, &stop);
  }

  return rcp<RenderShader>(new PlutoVG_RenderShader(gradient));
}

std::unique_ptr<RenderPath>
PlutonRiver_Factory::makeRenderPath(Span<const Vec2D> points, Span<const PathVerb> verbs, FillRule fillRule)
{
  RawPath rawPath(points.data(), points.size(), verbs.data(), verbs.size());

  plutovg_path_t* path = ToPlutoVG::convert(rawPath);

  PlutoVG_RenderPath renderPath(path);
  renderPath.fillRule(fillRule);

  return std::make_unique<PlutoVG_RenderPath>(renderPath);
}

std::unique_ptr<RenderPath> PlutonRiver_Factory::makeEmptyRenderPath()
{
  return std::make_unique<PlutoVG_RenderPath>();
}

std::unique_ptr<RenderPaint> PlutonRiver_Factory::makeRenderPaint()
{
  return std::make_unique<PlutoVG_RenderPaint>();
}

std::unique_ptr<RenderImage> PlutonRiver_Factory::decodeImage(Span<const uint8_t> raw)
{
  int width, height, n;
  stbi_uc* data = stbi_load_from_memory(raw.data(), raw.size(), &width, &height, &n, 4);
  if (data == nullptr)
    return nullptr;

  const int stride = width * n;
  auto* image = static_cast<unsigned char*>(calloc(1, static_cast<size_t>(stride) * height));

  for (int y = 0; y < height; y++)
  {
    const auto* src = reinterpret_cast<uint32_t*>(data + stride * y);
    auto* dst = reinterpret_cast<uint32_t*>(image + stride * y);

    for (int x = 0; x < width; x++)
    {
      const uint32_t r = src[x] >> 0 & 255;
      const uint32_t g = src[x] >> 8 & 255;
      const uint32_t b = src[x] >> 16 & 255;
      const uint32_t a = src[x] >> 24;

      dst[x] = (a << 24) | (r << 16) | (g << 8) | b;
    }
  }

  plutovg_surface_t* surface = plutovg_surface_create_for_data(image, width, height, stride);

  return std::make_unique<PlutoVG_RenderImage>(surface);
}
