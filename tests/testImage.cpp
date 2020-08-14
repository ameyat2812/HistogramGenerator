#include <array>
#include <vector>
#include "../Image.h"
#include "gtest/gtest.h"
const std::vector<std::vector<RGBPixel>> g_pixels = {
    {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}},
    {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}},
    {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}},
    {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}},
    {{1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}, {1, 2, 3}},
};
TEST(GenerateRGBHistogram8Bit, w5_h5_r1_g2_b3) {
  RGBImage image(5, 5);
  

  image.SetPixels(g_pixels);
  std::array<std::array<uint32_t, 256>, 3> hist{0};

  hist[0][1] = 25;
  hist[1][2] = 25;
  hist[2][3] = 25;

  EXPECT_EQ(image.GenerateRGBHistogram8Bit(), hist);
}

TEST(GetPixels, w5_h5_r1_g2_b3) {
  RGBImage image(5, 5);
  image.SetPixels(g_pixels);
  EXPECT_EQ(image.GetPixels(), g_pixels);
}

TEST(PNGReader, throwsFileNotFoundError) {
  EXPECT_THROW(PNGReader("non-existing-file.png"), FileNotFoundError);
}

TEST(PNGReader, throwsNotAPNGFileError) {
  EXPECT_THROW(PNGReader("tests/sample.jpg"), NotAPNGFileError);
}

TEST(PNGReader, DoesNotthrowException) {
  EXPECT_NO_THROW(PNGReader("tests/sample.png"));
}

TEST(PNGReader, throwsPNGLibraryOperationError) {
  auto png_reader = PNGReader("tests/sample_rgba.png");
  EXPECT_THROW(png_reader.AsRGBImage(), PNGLibraryOperationError);
}

TEST(PNGReader, AsRGBImageVarifyImage) {
  auto png_reader = PNGReader("tests/sample.png");
  auto image = png_reader.AsRGBImage();
  EXPECT_EQ(image.GetPixels(), g_pixels);
}