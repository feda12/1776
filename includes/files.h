#ifndef FILE_HELPER_H
#define FILE_HELPER_H

#define CROP_MARGIN 10
#define IGNORE_TILE_ISSUES true
#define ERASE_PREV_IMAGES true
#define CONVERT_TO_JPEG true
#define MINIMUM_IMG_COUNT_PER_LABEL 15

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <boost/filesystem.hpp>

std::map<std::string, std::string> loadSettings(const std::string &filename = "settings/config.txt");

std::map<std::string, unsigned int> loadLabels(const std::string &filename="settings/labels.txt");

std::vector<std::string> loadImageNumbers(const std::string &filename="settings/images.txt");

void generateErrorsFile(std::vector<std::string> &errors, const std::string &filename="errors.txt");

std::string sanitizePath(const std::string &path);

#endif
