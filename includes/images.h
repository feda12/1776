#ifndef IMAGE_HELPER_H
#define IMAGE_HELPER_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "eriolHeader.h"

// Pretty Print a list of tiles
void printTiles(std::vector<Tile> &tiles);

// Set a pixel to the given color in an image
void setPixelColor(Image &img, const PixelLoc &p, unsigned char red, unsigned char green , unsigned char blue);

// Set a pixel purple in an image at a given point
void setPixelPurple(Image &img, const PixelLoc &p);

// Set a pixel black in an image at a given point
void setPixelBlack(Image &img, const PixelLoc &p);

// Return true if a pixel is purple
bool purplePixel(const Image &img, const PixelLoc &p);

// Return true if a pixel is absolute black
bool blackPixel(const Image &img, const PixelLoc &p);

// Set the color of image to color of other image at a given pixel
void revertPixelColors(Image &img, Image &colors, PixelLoc &p);

// Save a reference to an image to the given path
void saveImg(Image &img, const std::string &filename);

// Save a pointer to an image to the given path
void saveImg(Image *img, const std::string &filename);

// Isolate a contour in a given img
bool isolateContourForImg(const std::string &imgName, const std::string &contourName, Image &img, const int cropMargin=0, const bool blackCrop=true);

// Return true if tile is valid(not empty and not named "blank")
bool validTile(const Tile &t);

#endif
