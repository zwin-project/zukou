#pragma once

#include <zukou.h>

#include <bits/types/FILE.h>
#include <stdio.h>
//
#include <jpeglib.h>

class JpegTexture final : public zukou::GlTexture
{
 public:
  DISABLE_MOVE_AND_COPY(JpegTexture);
  JpegTexture(zukou::System *system);
  ~JpegTexture();

  bool Init();

  bool Load(const char *filename);

 private:
  bool loaded_ = false;

  int fd_ = 0;
  zukou::ShmPool pool_;
  zukou::Buffer texture_buffer_;
};
