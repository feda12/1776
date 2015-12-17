// Ben Le Cam, Fall 2015
//
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <iterator>
#include <algorithm>
#include <map>

#include <boost/filesystem.hpp>

#include "images.h"
#include "files.h"
#include "caffe.h"
#include "kfold.h" 
#include "label.h"
#include "eriolHeader.h"
#include "debug.h"


Tile &loadJustOneTile(const string &tileID, const string &imgName);
vector<PixelLoc> getPixelsFor(int);
void inputAllTilesForImage(const string &);
string getAssignedNameAt(PixelLoc);

bool getInput(std::string &str)
{
	str.clear();
	char tmp[50];
	while(std::cin.get(tmp,1))
	{
		if(std::string(tmp) != "\n")
			str.append(tmp);
		else if(str.empty())
			return false;
	}
	return true;
}

bool label(std::map<std::string, unsigned int> &caffeLabels, const std::map<std::string, unsigned int> &labels, const std::string &imgFilename, const std::string &tileName, std::vector<int> &labelsCounter)
{
	assert(!tileName.empty());
	std::string label = tileName;
	if(tileName.at(0) == '!' && IGNORE_TILE_ISSUES)
		label = tileName.substr(1); 
		
	try {
		int labNum = labels.at(tileName);
		caffeLabels.insert({imgFilename, labNum});
		labelsCounter[labNum] ++;
		return true;
	} catch(std::exception e) {
		return false;
	}
}

void kfoldLabel(const Label &label, const unsigned int &folds, std::map<std::string, unsigned int> &trainSet, std::map<std::string, unsigned int> &valSet)
{
	std::vector<std::string> labelImages = label.getImages();
	Kfold<std::vector<std::string>::const_iterator>kf(folds, labelImages.begin(), labelImages.end());
	std::vector<std::string> train, val;
	std::cout << label.getLabel() << "(" << label.getId() << ")" << " has " << label.getImages().size() << std::endl;
	debug("Number of folds: "+std::to_string(folds));
	for(unsigned int i = 0; i < folds; ++i)
	{
		kf.getFold(folds, back_inserter(train), back_inserter(val));
	}
	
	for(auto t: train)
		trainSet.insert({t, label.getId()});	

	for(auto v: val)
		valSet.insert({v, label.getId()});	
}

void prompt_settings(std::map<std::string, std::string> &settings)
{
	std::cout << "Enter your settings(press enter for default)" << std::endl;
	for(auto kv: settings)
	{
		std::string tmp;
		std::cout << kv.first << " (" << kv.second << "):";
		if(!getInput(tmp))
			std::cout << "Stays the same" << std::endl;
		else 
			kv.second = tmp;
		std::cout << std::endl;
		std::cin.clear();
	}
}

void run_with_settings(std::map<std::string, std::string> &settings)
{
	// We load the labels file and prepare our vector of counts
	debug("Loading labels");
	const std::map<std::string, unsigned int> labels = loadLabels(settings["LABELS_FILE"]);
	std::map<unsigned int, std::string> reversedLabels;
	for(auto kv: labels)
		reversedLabels.insert({kv.second, kv.first});
	std::vector<int> labelsCounter(labels.size(), 0);	
	std::vector<std::string> errors;

	// We load the image numbers and prepare a list of all the image names(both left and right images)
	debug("Loading image numbers");
	std::vector<std::string> imgNumbers = loadImageNumbers(settings["IMAGES_FILE"]);
	std::vector<std::string> images;
	for(auto i:imgNumbers)
	{
		images.push_back(i+"R");
		images.push_back(i+"L");
	}

	// We keep statistics about our run
	unsigned int imgCounter = 1;
	unsigned int totalTiles = 0;
	unsigned int invalidTiles = 0;
	unsigned int failedTiles = 0;
	unsigned int labelFailTiles = 0;
	std::map<std::string, unsigned int> caffeLabels;
	std::vector<Tile> tiles;
	
	// We define useful paths
	debug("Working with boost on path");
	settings["DESTINATION_FOLDER"] = sanitizePath(settings["DESTINATION_FOLDER"]);
	bool dirExist = boost::filesystem::exists(settings["DESTINATION_FOLDER"]);
	if(dirExist)
		std::system(std::string("rm -R "+settings["DESTINATION_FOLDER"]).c_str());

	std::system(std::string("mkdir "+settings["DESTINATION_FOLDER"]).c_str());

	std::string imgFolder = sanitizePath(settings["DESTINATION_FOLDER"]+"/img/");
	std::string trainFolder = sanitizePath(settings["DESTINATION_FOLDER"]+"/train/");
	std::string valFolder = sanitizePath(settings["DESTINATION_FOLDER"]+"/val/");

	dirExist = boost::filesystem::exists(imgFolder);
	// We erase the images in the folder if it exists
	if(dirExist && std::stoi(settings["ERASE_PREV_IMAGES"])) {
		std::system(std::string("rm "+imgFolder+"*.jpg").c_str());
		std::system(std::string("rm "+imgFolder+"*.ppm").c_str());
	} else {
		std::system(std::string("mkdir "+imgFolder).c_str());
	}
	debug("All is good with folders");	
	// Time to work on these images
	Image img;
	debug("Working on these images");
	if(images.empty())
		debug("No images to process... moving on.");
	for(auto imgName:images)
	{		
		while(!allTiles.empty())
			allTiles.pop_back();
		debug("Just popped up a couple tiles...");
		try {
			inputAllTilesForImage(imgName);
		} catch (std::exception &e) {
			std::cerr << "Exception catched : " << e.what() << std::endl;
			errors.push_back("Could not input tiles for image "+imgName);
		}

		std::vector<Tile> tiles = allTiles;
		debug("Processing tiles");
		for(auto ti: tiles)
		{
			totalTiles ++;
			if(validTile(ti))
			{
				debug("Valid tile");
				if(isolateContourForImg(imgName, ti.getID(), img, std::stoul(settings["CROP_MARGIN"]), std::stol(settings["BLACK_CROP"])))
				{
					debug("Valid contour");
					if(label(caffeLabels, labels, std::to_string(imgCounter)+".jpg", ti.getName(), labelsCounter)) {
						std::cout << "------ Processed image " << imgCounter << "\t" << ti.getName() << "\t" << ti.getID() << std::endl;
						saveImg(img, sanitizePath(imgFolder+std::to_string(imgCounter)+".ppm"));
					} else {
						debug("Error with label");
						errors.push_back(imgName+":"+ti.getName()+":"+ti.getID());	
						labelFailTiles ++;
					}
					imgCounter ++;
				} else {
					failedTiles ++;
				}
			} else {
				invalidTiles ++;
			}
		}
	}
	if(!errors.empty())
		generateErrorsFile(errors);

	float failureRate = 0;
	if(totalTiles > 0)
		failureRate  = float(labelFailTiles+invalidTiles)/float(totalTiles-failedTiles);
	std::cout << failureRate << std::endl;
	std::cout << "Total tiles: " << totalTiles << std::endl;
	std::cout << "Successful tiles: " << imgCounter << std::endl;
	std::cout << "Invalid tiles: " << invalidTiles << "\t\t(Tiles that were unnamed or named blank)" << std::endl;
	std::cout << "Tiles that were removed: " << failedTiles << "\t\t()" << std::endl;
	std::cout << "Invalid labels: " << labelFailTiles << "\t\t (Tiles that had unknown labels, update labels.txt to include new ones)" << std::endl;
	std::cout << "Sucess rate: " << std::fixed << std::setprecision(2) << (1-failureRate)*100.00 << "%" << std::endl;
	std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;
	
	// We convert all the pictures to jpeg
	std::system(std::string("mogrify -format jpg "+imgFolder+"*.ppm").c_str());	

	if(DEBUG)
	{
		caffeLabels.clear();
		std::ifstream fullData;
		fullData.open("../full_caffeData.txt");
		std::string line;
		while(std::getline(fullData, line))
			caffeLabels.insert({line.substr(0, line.find("\t")), std::stoul(line.substr(line.find("\t")+1))});

	}

	std::vector<Label> labelsObj;

	debug("Creating labels classes");
	for(auto kv: caffeLabels)
	{
		std::cout << "Looking for label " << kv.second << std::endl;
		auto iter = std::find(labelsObj.begin(), labelsObj.end(), Label("", kv.second, 0));
		if(iter != labelsObj.end())
		{
			debug("Label already exist: "+reversedLabels.at(kv.second)+"|"+std::to_string(kv.second));
			iter->addImage(kv.first);
		} else {
			debug("Creating new label object: "+reversedLabels.at(kv.second)+"|"+std::to_string(kv.second));
			Label newLabel(reversedLabels.at(kv.second), kv.second, 0);
			newLabel.addImage(kv.first);
			labelsObj.push_back(newLabel);
		}

	}	
	
	debug("Sorting labels");
	std::sort(labelsObj.begin(), labelsObj.end());

	const int minimumImgPerLabel = std::stol(settings["MINIMUM_IMG_COUNT_PER_LABEL"]);
	debug("Removing labels that are below the minimum: "+std::to_string(minimumImgPerLabel));
	for(auto labelIter = labelsObj.begin(); labelIter != labelsObj.end(); ++labelIter)
	{
		if(labelIter->getCount() < minimumImgPerLabel)
			labelsObj.erase(labelIter);
	}

	std::cout << "K-Folding and creating training and validation sets" << std::endl;
	const int folds = std::stoi(settings["FOLDS"]);
	std::map<std::string, unsigned int> train, val;
	for(auto label: labelsObj)
	{
		debug(std::to_string(label.getCount())+"\t"+label.getLabel());
		kfoldLabel(label, folds, train, val);	
	}

	debug("Saving to folder");	

	setupCaffeDirectory(settings["DESTINATION_FOLDER"]);

	saveCreateDataScript(settings["DESTINATION_FOLDER"], settings["CAFFE_PATH"], std::stol(settings["IMAGE_RESIZE"]));
	saveMakeDataMeanScript(settings["DESTINATION_FOLDER"], settings["CAFFE_PATH"]);
	saveResumeTrainingScript(settings["DESTINATION_FOLDER"], settings["CAFFE_PATH"]);
	saveTrainNetScript(settings["DESTINATION_FOLDER"], settings["CAFFE_PATH"]);

	saveSynset(labels, "synset_words.txt", settings["DESTINATION_FOLDER"]);
	saveSynset(labels, "det_synset_words.txt", settings["DESTINATION_FOLDER"]);

	saveTrainValSet(train, "train.txt", settings["DESTINATION_FOLDER"]);
	saveTrainValSet(val, "val.txt", settings["DESTINATION_FOLDER"]);

	moveTrainValImages(train, imgFolder, sanitizePath(settings["DESTINATION_FOLDER"]+"/example/train/"));
	moveTrainValImages(val, imgFolder, sanitizePath(settings["DESTINATION_FOLDER"]+"/example/val/"));

	//saveDoItScript(settings["DESTINATION_FOLDER"]);

	std::system(std::string("rm -R "+imgFolder).c_str());
}

int main(int argc, char *argv[])
{

	debug("DEBUG MODE ON");
	// We initialize one tile as it prepares Eriol
	std::string contourName = "101lecam3";
	std::string imgName = "1182L";
	debug("Loading initial tile");
//	Tile &t = loadJustOneTile(contourName, imgName);
	Tile &t = loadJustOneTile("135leesn", "1098L");
	debug(t.getName());

	debug("Loading settings");	
	// We load the settings file
	std::map<std::string, std::string> settings = loadSettings();
	bool multipleSettings = false;
	bool promptSettings = false;
	for(int i = 0; i < argc; ++i)
	{
		if(std::string(argv[i]) == "-p")
			promptSettings = true;
		if(std::string(argv[i]) == "-ms")
			multipleSettings = true;		
	}
			
	if(promptSettings && multipleSettings)
	{
		// while loop, promt settings and such,
		// ask to continue and then run settings
	} else if(promptSettings) {
		prompt_settings(settings);
		run_with_settings(settings);
	} else if(multipleSettings) {
		// Discover all config files in settings/configs/
		// Load the settings
		std::vector<std::map<std::string, std::string> > allSettings;
		boost::filesystem::path dir(sanitizePath("settings/configs"));

		boost::filesystem::recursive_directory_iterator iter(dir), eod;

		BOOST_FOREACH(boost::filesystem::path const& i, std::make_pair(iter, eod)){
			if (is_regular_file(i)){
				allSettings.push_back(loadSettings(i.string()));
			}
		}		
		for(auto sett: allSettings)
			run_with_settings(sett);
	} else {
		run_with_settings(settings);
	}
}	
