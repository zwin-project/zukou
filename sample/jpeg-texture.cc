#include "jpeg-texture.h"

#include <sys/mman.h>
#include <unistd.h>

JpegTexture::JpegTexture(zukou::System *system)
    : zukou::GlTexture(system), pool_(system)
{}

JpegTexture::~JpegTexture()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

bool
JpegTexture::Init()
{
  return zukou::GlTexture::Init();
}

bool
JpegTexture::Load(const char *texture_path)
{
  FILE *fp;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr err;
  uint32_t width, height;
  int channel;
  size_t size;
  bool result = false;

  if (loaded_) {
    fprintf(stderr, "JpegTexture::Load -- texture already loaded\n");
    goto err;
  }
  loaded_ = true;

  fp = fopen(texture_path, "rb");
  if (fp == nullptr) {
    fprintf(stderr, "JpegTexture::Load -- failed to open %s\n", texture_path);
    goto err;
  }

  jpeg_create_decompress(&cinfo);

  cinfo.err = jpeg_std_error(&err);

  jpeg_stdio_src(&cinfo, fp);

  jpeg_read_header(&cinfo, true);

  cinfo.out_color_space = JCS_EXT_RGBA;

  if (jpeg_start_decompress(&cinfo) == false) {
    fclose(fp);
    fprintf(stderr, "JpegTexture::Load -- failed to decompress jpeg image\n");
    goto err_destroy_decompress;
  }

  width = cinfo.output_width;
  height = cinfo.output_height;
  channel = cinfo.output_components;

  size = channel * width * height;

  fd_ = zukou::Util::CreateAnonymousFile(size);
  if (fd_ <= 0) goto err_finish_decompress;
  if (!pool_.Init(fd_, size)) goto err_finish_decompress;
  if (!texture_buffer_.Init(&pool_, 0, size)) goto err_finish_decompress;

  {
    JSAMPLE *data = static_cast<JSAMPLE *>(
        mmap(nullptr, size, PROT_WRITE, MAP_SHARED, fd_, 0));

    uint32_t scanned_lines = 0;
    while (scanned_lines < height) {
      JSAMPROW row = data + channel * width * scanned_lines;
      scanned_lines +=
          jpeg_read_scanlines(&cinfo, &row, height - scanned_lines);
    }

    munmap(data, size);
  }

  Image2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &texture_buffer_);

  result = true;

err_finish_decompress:
  jpeg_finish_decompress(&cinfo);

err_destroy_decompress:
  jpeg_destroy_decompress(&cinfo);
  fclose(fp);

err:
  return result;
}
