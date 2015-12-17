#include "files.h"

std::map<std::string, std::string> loadSettings(const std::string &filename)
{
	// Loading default settings	
	std::map<std::string, std::string> settings;
	settings.insert({"CROP_MARGIN", "10"});
	settings.insert({"IGNORE_TILE_ISSUES", std::to_string(true)});
	settings.insert({"ERASE_PREV_IMAGES", std::to_string(true)});
	settings.insert({"CONVERT_TO_JPEG", std::to_string(true)});
	settings.insert({"MINIMUM_IMG_COUNT_PER_LABEL", std::to_string(15)});
	settings.insert({"BLACK_CROP", std::to_string(true)});
	settings.insert({"LABELS_FILE", "settings/labels.txt"});
	settings.insert({"IMAGES_FILE", "settings/images.txt"});
	settings.insert({"DESTINATION_FOLDER", "1776example"});
	settings.insert({"DEBUG", std::to_string(false)});
	settings.insert({"CAFFE_PATH", "/home/team76/caffe"});	
	settings.insert({"IMAGE_RESIZE", "256"});
	settings.insert({"TRAIN_VAL", "settings/train_val.prototxt"});
	settings.insert({"SOLVER", "settings/solver.prototxt"});
	// We now open the file and loads all the settings we can
	std::ifstream settingsFile;
	settingsFile.open(filename);
	assert(settingsFile.is_open());
	
	std::string line;
	while(std::getline(settingsFile,line) )
	{
		int commaPos= line.find(',');
		settings[line.substr(0, commaPos)] = line.substr(commaPos+1);
	}
	settingsFile.close();
	return settings;
}

std::map<std::string, unsigned int> loadLabels(const std::string &filename)
{
	std::ifstream labelsFile;
	labelsFile.open(filename);
	assert(labelsFile.is_open());
	
	std::map<std::string, unsigned int> labels;
	std::string line;
	while(std::getline(labelsFile,line) )
	{
		int commaPos= line.find(',');
		labels[line.substr(commaPos+1)] = std::stoul(line.substr(0, commaPos));	
	}
	labelsFile.close();
	return labels;
}

std::vector<std::string> loadImageNumbers(const std::string &filename)
{
	std::ifstream imageNumbersFile;
	imageNumbersFile.open(filename);
	assert(imageNumbersFile.is_open());
	
	std::vector<std::string> imageNumbers;
	std::string line;
	while(std::getline(imageNumbersFile, line) )
		imageNumbers.push_back(line);
	return imageNumbers;
}



void generateErrorsFile(std::vector<std::string> &errors, const std::string &filename)
{
	std::cout << "Generating errors report" << std::endl;
	std::ofstream errorsFile;
	errorsFile.open(filename);
	assert(errorsFile.is_open());
	for(auto &err:errors)
	{
		errorsFile << err << std::endl;
	}
}

std::string sanitizePath(const std::string &path)
{
	assert(path.size() > 0);
	return boost::filesystem::system_complete(path).string();
}
