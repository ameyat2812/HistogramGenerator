// Intent : This tools reads a PNG file and outputs RGB histogram to a text file
// Auther: Ameya Tambekar

#include <fstream>
#include <iostream>
#include "Image.h"

/*
    Simple print usage and exit function
*/
void printHelpAndExit() {
  std::cerr
      << "usage: HistogramGenerator [-h] [-i INPUT_FILE] [-o OUTPUT_FILE]\n";
  exit(1);
}

/*
    This function parses input arguments to read input and output file names
*/
void getOpts(int argc, char *argv[], std::string &input_filename,
             std::string &output_filename) {
  bool flag_i = false;
  bool flag_o = false;
  int opt = -1;
  // Retrieve the options:
  while ((opt = getopt(argc, argv, "hi:o:")) != -1) {  // for each option...
    switch (opt) {
      case 'i':
        flag_i = true;
        input_filename = std::string(optarg);
        break;
      case 'o':
        flag_o = true;
        output_filename = std::string(optarg);
        break;
      case 'h':
        printHelpAndExit();
      case '?':  // unknown option..
        printHelpAndExit();
        break;
    }
  }

  if (flag_i == false || flag_o == false) printHelpAndExit();
}

/*
	This function to write histogram is a specific format to an output file
*/
void writeToOutputFile(std::string &file_name,
                       std::array<std::array<uint32_t, 256>, 3> &histograms) {
  std::ofstream ofs(file_name, std::ofstream::out);

  for (auto histogram = histograms.begin(); histogram != histograms.end();
       histogram++) {
    for (auto it = histogram->begin(); it != histogram->end(); it++) {
      ofs << *it;
	  // Don't write , after the last element
      if (it + 1 != histogram->end()) ofs << ",";
    }

	// Don't put a new line after the last histogram
    if (histogram + 1 != histograms.end()) ofs << std::endl;
  }
}

int main(int argc, char *argv[]) {
  std::string input_file_name, output_file_name;
  getOpts(argc, argv, input_file_name, output_file_name);

  try {
    PNGReader reader(input_file_name);

    RGBImage image = reader.AsRGBImage();

    auto histograms = image.GenerateRGBHistogram8Bit();

    writeToOutputFile(output_file_name, histograms);

  } catch (std::runtime_error err) {
    std::cerr << "Exiting! because: " << err.what();
    exit(1);
  }
  return 0;
}
