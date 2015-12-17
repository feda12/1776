#include "caffe.h"
#include "files.h"

void saveFile(const std::string &filename, const std::string &content)
{
	std::ofstream f;
	f.open(filename);
	assert(f.is_open());
	f << content;
	f.close();
}

void saveSettingsFile(const std::map<std::string, std::string> &settings, const std::string &destinationFolder)
{
	std::string c;
	for(auto kv: settings)
	{
		c.append(kv.first);
		c.append("\t");
		c.append(kv.second);
		c.append("\n");
	}
	saveFile(sanitizePath(destinationFolder+"/data_settings.txt"), c);	
}

void saveDoItScript(const std::string &destinationFolder)
{
	std::string c;
	c.append("#!/usr/bin/env sh\n");
	c.append("# This script runs all steps necessary to have a working net\n\n");
	c.append("exec example/create_lmdb.sh && exec example/make_mean.sh && exec example/train.sh\n");
	saveFile(sanitizePath(destinationFolder+"/doit.sh"), c);
}

void saveCreateDataScript(const std::string &destinationFolder, const std::string &caffePath, const int resize)
{
	std::string c;
	c.append("#!/usr/bin/env sh\n");
	c.append("# This script creates the lmdb input\n\n");
	c.append("EXAMPLE=example/\n");
	c.append("DATA=data/\n");
	c.append("TOOLS="+sanitizePath(caffePath+"/build/tools/")+"\n\n");
	c.append("TRAIN_DATA_ROOT=example/train/\n");
	c.append("VAL_DATA_ROOT=example/val/\n\n");
	c.append("RESIZE_HEIGHT="+std::to_string(resize)+"\n");
	c.append("RESIZE_WIDTH="+std::to_string(resize)+"\n");
	c.append("if [ ! -d \"$TRAIN_DATA_ROOT\" ]; then\n");
	c.append("echo \"Error: TRAIN_DATA_ROOT not a path to directory\"\n");
	c.append("exit 1\n");
	c.append("fi\n");
	c.append("if [ ! -d \"$VAL_DATA_ROOT\" ]; then\n");
	c.append("echo \"Error: VAL_DATA_ROOT not a path to directory\"\n");
	c.append("exit 1\n");
	c.append("fi\n");
	c.append("echo \"Creating train lmdb...\"\n");
	c.append("GLOG_logtostderr=1 $TOOLS/convert_imageset --resize_height=$RESIZE_HEIGHT --resize_width=$RESIZE_WIDTH --shuffle $TRAIN_DATA_ROOT $DATA/train.txt $EXAMPLE/train_lmdb\n");
	c.append("echo \"Creating val lmdb...\"\n");
	c.append("GLOG_logtostderr=1 $TOOLS/convert_imageset --resize_height=$RESIZE_HEIGHT --resize_width=$RESIZE_WIDTH --shuffle $VAL_DATA_ROOT $DATA/val.txt $EXAMPLE/val_lmdb\n");
	c.append("echo \"Done.\"\n");
	
	saveFile(sanitizePath(destinationFolder+"/example/create_lmdb.sh"), c); 

}

void saveMakeDataMeanScript(const std::string &destinationFolder, const std::string &caffePath)
{
	std::string c;
	c.append("#!/usr/bin/env sh\n");
	c.append("# This script computes the mean image from the training lmdb\n\n");
	c.append("EXAMPLE=example/\n");
	c.append("DATA=data/\n");
	c.append("TOOLS="+sanitizePath(caffePath+"/build/tools/")+"\n\n");
	c.append("$TOOLS/compute_image_mean $EXAMPLE/train_lmdb $DATA/mean.binaryproto\n");
	c.append("echo \"Done.\"\n");
	
	saveFile(sanitizePath(destinationFolder+"/example/make_mean.sh"), c);
}

void saveResumeTrainingScript(const std::string &destinationFolder, const std::string &caffePath)
{
	std::string c;
	c.append("#!/usr/bin/env sh\n");
	c.append("# This script resumes training where left off\n\n");
	c.append(sanitizePath(caffePath+"/build/tools/caffe")+" train --solver=example/solver.prototxt --snapshot=example/caffenet_train_10000.solverstate.h4\n");

	saveFile(sanitizePath(destinationFolder+"/example/resume_training.sh"), c);
}

void saveTrainNetScript(const std::string &destinationFolder, const std::string &caffePath)
{
	std::string c;
	c.append("#!/usr/bin/env sh\n");
	c.append("# This script simply runs the training for the net\n");
	c.append(sanitizePath(caffePath+"/build/tools/caffe")+" train --solver=example/solver.prototxt\n");

	saveFile(sanitizePath(destinationFolder+"/example/train.sh"), c);
}

void setupCaffeDirectory(const std::string &destinationFolder)
{
	// Setup the data and examples
	std::system(std::string("mkdir "+sanitizePath(destinationFolder+"/example/")).c_str());
	std::system(std::string("mkdir "+sanitizePath(destinationFolder+"/data/")).c_str());
	std::system(std::string("mkdir "+sanitizePath(destinationFolder+"/example/train/")).c_str());
	std::system(std::string("mkdir "+sanitizePath(destinationFolder+"/example/val/")).c_str());
}

std::string labelsToStr(const std::map<std::string, unsigned int> &labels)
{
	std::string c;
	for(auto kv: labels)
	{
		c.append(kv.first);
		c.append(" ");
		c.append(std::to_string(kv.second));
		c.append("\n");
	}
	return c;
}
std::string labelsToStrReversed(const std::map<std::string, unsigned int> &labels)
{
	std::string c;
	for(auto kv: labels)
	{
		c.append(std::to_string(kv.second));
		c.append(" ");
		c.append(kv.first);
		c.append("\n");
	}
	return c;
}

void saveSynset(const std::map<std::string, unsigned int> &labels, const std::string &filename, const std::string &destinationFolder)
{
	saveFile(sanitizePath(destinationFolder+"/data/"+filename), labelsToStrReversed(labels));
}

void saveTrainValSet(const std::map<std::string, unsigned int> &set, const std::string &filename, const std::string &destinationFolder)
{
	saveFile(sanitizePath(destinationFolder+"/data/"+filename), labelsToStr(set));
}

void moveTrainValImages(std::map<std::string, unsigned int> &set, const std::string &imgFolder, const std::string &destinationFolder)
{		
	for(auto kv:set)
	{
		std::string cmd = std::string("mv "+sanitizePath(imgFolder+"/"+kv.first)+" "+destinationFolder);
		std::cout << cmd << std::endl;
		std::system(cmd.c_str());
	}
}
