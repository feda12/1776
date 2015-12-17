#ifndef CAFFE_HELPER_H
#define CAFFE_HELPER_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>

void saveFile(const std::string &filename, const std::string &content);

void saveDoItScript(const std::string &destinationFolder);
void saveSettingsFile(const std::map<std::string, std::string> &settings, const std::string &destinationFolder);
void saveCreateDataScript(const std::string &destinationFolder, const std::string &caffePath, const int resize);
void saveMakeDataMeanScript(const std::string &destinationFolder, const std::string &caffePath);
void saveResumeTrainingScript(const std::string &destinationFolder, const std::string &caffePath);
void saveTrainNetScript(const std::string &destinationFolder, const std::string &caffePath);

void setupCaffeDirectory(const std::string &destinationFolder);

void saveSynset(const std::map<std::string, unsigned int> &labels, const std::string &filename, const std::string &destinationFolder);

void saveTrainValSet(const std::map<std::string, unsigned int> &set, const std::string &filename, const std::string &destinationFolder);

void moveTrainValImages(std::map<std::string, unsigned int> &set, const std::string &imgFolder, const std::string &destinationFolder);
#endif
