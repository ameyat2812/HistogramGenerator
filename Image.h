#include <png.h>
#include <stdint.h>
#include <unistd.h>

#include <array>
#include <vector>

class Error : public std::runtime_error {
 public:
  Error(const std::string& message) : runtime_error(message) {}
};

class FileNotFoundError : public Error {
 public:
  FileNotFoundError(void) : Error("File Not Found") {}
};

class NotAPNGFileError : public Error {
 public:
  NotAPNGFileError(void) : Error("Not a PNG file") {}
};

class CreatePNGStructError : public Error {
 public:
  CreatePNGStructError(void) : Error("Failed to create PNG struct") {}
};

class CreateInfoStructError : public Error {
 public:
  CreateInfoStructError(void) : Error("Failed to create Info struct") {}
};

class PNGLibraryOperationError : public Error {
 public:
  PNGLibraryOperationError(const std::string& message) : Error(message) {}
};

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RGBPixel;

class RGBImage {
 public:
  RGBImage(uint32_t width, uint32_t height) : width_(width), height_(height) {}

  std::vector<std::vector<RGBPixel>> GetPixels(void) const { return pixels_; }
  void SetPixels(std::vector<std::vector<RGBPixel>> pixels) {
    pixels_ = pixels;
  }
  std::array<std::array<uint32_t, 256>, 3> GenerateRGBHistogram8Bit(void) {
    std::array<std::array<uint32_t, 256>, 3> hist {0};
    for (auto& row : pixels_) {
      for (auto& pixel : row) {
        hist[0][pixel.r]++;
        hist[1][pixel.g]++;
        hist[2][pixel.b]++;
      }
    }

    return hist;
  }

 private:
  const uint32_t width_;
  const uint32_t height_;
  std::vector<std::vector<RGBPixel>> pixels_;
};

class PNGReader {
 public:
  PNGReader(std::string file_name) {
    unsigned char header[8];
    file_ptr_ = fopen(file_name.c_str(), "rb");
    if (!file_ptr_) {
      throw FileNotFoundError();
    }
    size_t read = fread(header, 1, 8, file_ptr_);
    if (!read || png_sig_cmp(header, 0, 8)) {
      throw NotAPNGFileError();
    }

    ReadBuffer();
  }

  ~PNGReader() {
    /* cleanup heap allocation */
    if (row_pointers_) {
      for (int y = 0; y < height_; y++)
        if (row_pointers_[y]) free(row_pointers_[y]);
      free(row_pointers_);
    }

    if (file_ptr_) fclose(file_ptr_);
  }

  RGBImage AsRGBImage() {
    if (color_type_ != PNG_COLOR_TYPE_RGB) {
      throw PNGLibraryOperationError("Incorrect color type");
    }
    RGBImage rgb_image = {width_, height_};

    std::vector<std::vector<RGBPixel>> pixels;
    for (int y = 0; y < height_; y++) {
      std::vector<RGBPixel> pixel_row;
      png_byte* row = row_pointers_[y];
      for (int x = 0; x < width_; x++) {
        png_byte* ptr = &(row[x * 3]);
        RGBPixel pixel = {ptr[0], ptr[1], ptr[2]};
        pixel_row.push_back(pixel);
      }
      pixels.push_back(pixel_row);
    }

    rgb_image.SetPixels(pixels);

    return rgb_image;
  }

 private:
  void ReadBuffer() {
    png_structp png_ptr =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
      throw CreatePNGStructError();
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      throw CreateInfoStructError();
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      throw PNGLibraryOperationError("Failed to set a callback");
    }

    png_init_io(png_ptr, file_ptr_);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    width_ = png_get_image_width(png_ptr, info_ptr);
    height_ = png_get_image_height(png_ptr, info_ptr);
    color_type_ = png_get_color_type(png_ptr, info_ptr);
    bit_depth_ = png_get_bit_depth(png_ptr, info_ptr);

    number_of_passes_ = png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    row_pointers_ = (png_bytep*)malloc(sizeof(png_bytep) * height_);
    for (int y = 0; y < height_; y++)
      row_pointers_[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

    png_read_image(png_ptr, row_pointers_);
  }

  FILE* file_ptr_ = nullptr;
  uint32_t width_ = 0;
  uint32_t height_ = 0;
  uint32_t number_of_passes_ = 0;
  png_byte color_type_;
  png_byte bit_depth_;
  png_bytep* row_pointers_ = nullptr;
};
