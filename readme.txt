readme.txt
December 14, 2015
Ben Le Cam

Introduction
------------

"1776" is a software written in C++. Its goal is to create trainable dataset for the deep learning library Caffe. It uses Eriol, an image 3D modeling software written by Olaf Hall-Holt at St. Olaf College, MN. From a list of images and a list of valid labels, 1776 will go get these images, create images for each contour and create a label association. It will then use a k-fold strategy to split the data in a training set and a validation set. It will create a directory structure and the corresponding scripts to be ran right away in Caffe.

How to compile
--------------

1776 relies on all the headers file in headers/. Make sure they are present.
Run `make` to compile.

How to use
----------

1) Create an settings/images.txt file with a list of images numbers. Each image number should be on its own line.
2) Create a settings/labels.txt file with the list of labels and their correspond ids. Each pair should be on its own line.
3) If needed, create a settings/settings.txt file.
4) Run the 1776 executable

Settings
--------

Here are the settings available to change along with their default value:
	- CROP_MARGIN(10): define the margin around the object for the crop
	-  
