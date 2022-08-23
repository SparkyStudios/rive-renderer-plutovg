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

#include <rive/file.hpp>
#include <rive/math/aabb.hpp>

#include <plutovg.h>

#include <plutonriver/factory.hpp>
#include <plutonriver/renderer.hpp>

#include <cstdio>
#include <stdio.h>
#include <string>

std::string getFileName(char* path)
{
  std::string str(path);

  const size_t from = str.find_last_of("\\/");
  const size_t to = str.find_last_of(".");
  return str.substr(from + 1, to - from - 1);
}

int main(int argc, char* argv[])
{
  rive::PlutonRiver_Factory factory;

  if (argc < 2)
  {
    fprintf(stderr, "must pass source file");
    return 1;
  }
  FILE* fp;
  fopen_s(&fp, argv[1], "rb");

  const char* outPath;
  std::string filename;
  std::string fullName;
  if (argc > 2)
  {
    outPath = argv[2];
  }
  else
  {
    filename = getFileName(argv[1]);
    fullName = filename + ".png";
    outPath = fullName.c_str();
  }

  if (fp == nullptr)
  {
    fprintf(stderr, "Failed to open rive file.\n");
    return 1;
  }
  fseek(fp, 0, SEEK_END);
  auto length = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  std::vector<uint8_t> bytes(length);
  if (fread(bytes.data(), 1, length, fp) != length)
  {
    fprintf(stderr, "Failed to read rive file.\n");
    fclose(fp);
    return 1;
  }
  fclose(fp);

  auto file = rive::File::import(rive::toSpan(bytes), &factory);
  if (!file)
  {
    fprintf(stderr, "Failed to read rive file.\n");
    return 1;
  }
  auto artboard = file->artboardDefault();
  artboard->advance(0.0f);

  int width = 1024, height = 1024;

  plutovg_surface_t* surface = plutovg_surface_create(width, height);

  rive::PlutoVG_Renderer renderer(surface);
  renderer.save();
  renderer.align(rive::Fit::cover, rive::Alignment::center,
    rive::AABB(0, 0, width, height), artboard->bounds());
  artboard->draw(&renderer);
  renderer.restore();

  renderer.writePNG(outPath);
  plutovg_surface_destroy(surface);

  return 0;
}
