#include "eriolHeader.h"
#include "images.h"

Tile &loadJustOneTile(const string &tileID, const string &imgName);
vector<PixelLoc> getPixelsFor(int);
void inputAllTilesForImage(const string &);
string getAssignedNameAt(PixelLoc);

// Pretty Print a list of tiles
void printTiles(std::vector<Tile> &tiles)
{
	std::cout << "ID\t|\tLabel" << std::endl;
	std::cout << "-------------------" << std::endl;
	for(std::vector<Tile>::const_iterator i = tiles.begin(); i != tiles.end(); ++i)
	{
		if(i->getName().size() > 0 && i->getName() != "blank")
		{
			std::cout << i->getID()<< "\t|\t" << i->getName() << std::endl;
		}
	}
}

// Set a pixel to the given color in an image
void setPixelColor(Image &img, const PixelLoc &p, unsigned char red, unsigned char green , unsigned char blue)
{
	img.setPixel(p, red, 0);
	img.setPixel(p, green, 1);
	img.setPixel(p, blue, 2);
}

// Set a pixel purple in an image at a given point
void setPixelPurple(Image &img, const PixelLoc &p)
{
	unsigned char red = 255;
	unsigned char green = 0;
	unsigned char blue = 255;
	setPixelColor(img, p, red, green, blue);
}

// Set a pixel black in an image at a given point
void setPixelBlack(Image &img, const PixelLoc &p)
{
	setPixelColor(img, p, 0, 0, 0);
}

// Return true if a pixel is purple
bool purplePixel(const Image &img, const PixelLoc &p)
{
	return img.getCharPixel(p,0) == 255 && img.getCharPixel(p,1) == 0 && img.getCharPixel(p,2) == 255;
}

// Return true if a pixel is absolute black
bool blackPixel(const Image &img, const PixelLoc &p)
{
	return img.getCharPixel(p,0) == 0 && img.getCharPixel(p,1) == 0 && img.getCharPixel(p,2) == 0;
}

void revertPixelColors(Image &img, Image &colors, PixelLoc &p)
{
	img.setPixel(p, colors.getCharPixel(p,0), 0);
	img.setPixel(p, colors.getCharPixel(p,1), 1);
	img.setPixel(p, colors.getCharPixel(p,2), 2);
}

void saveImg(Image &img, const std::string &filename)
{
        std::ofstream imgFile;
        imgFile.open(filename);
        img.print(imgFile);
        imgFile.close();
}

void saveImg(Image *img, const std::string &filename)
{
        std::ofstream imgFile;
        imgFile.open(filename);
        img->print(imgFile);
        imgFile.close();
}

bool isolateContourForImg(const std::string &imgName, const std::string &contourName, Image &img, const int cropMargin, const bool blackCrop)
{
        std::vector<PixelLoc> contour;
        try
        {
                contour = getContour(contourName, imgName);
        } catch ( const std::exception & e ) {
                return false;
        }
        if(contour.size() == 0)
                return false;

	img = Image(imgName.c_str());

        PixelLoc ic = contour.at(0); 
        PixelLoc minX(ic), maxX(ic), minY(ic), maxY(ic);
        for(std::vector<PixelLoc>::iterator i = contour.begin(); i != contour.end(); ++i)
        {
                if(i->x < minX.x) minX = *i;
                if(i->x > maxX.x) maxX = *i;
                if(i->y < minY.y) minY = *i;
                if(i->y > maxY.y) maxY = *i;
        }

        PixelLoc bottomLeft(std::max(0, minX.x-cropMargin), std::max(0, minY.y-cropMargin));
        PixelLoc topRight(std::min(int(img.getWidth()-1), maxX.x+cropMargin), std::min(int(img.getHeight()-1), maxY.y+cropMargin));

        img =  *img.createCrop(BBox(bottomLeft, topRight));
        Image colors(img);
	
        for(std::vector<PixelLoc>::iterator i = contour.begin(); i != contour.end(); ++i)
        {
       		 
	        i->x = std::min(std::max(0, i->x-minX.x+cropMargin), int(img.getWidth()-1));
                i->y = std::min(std::max(0, i->y-minY.y+cropMargin), int(img.getHeight()-1));
                setPixelPurple(img, *i);
        }

        for(unsigned int i = 0; i < img.getWidth(); ++i)
        {
                for(unsigned int j = 0; j < img.getHeight(); ++j)
                {
                        PixelLoc p(i,j);
                        if(purplePixel(img, p))
                                revertPixelColors(img, colors,  p);
                        else if(blackCrop)
                                setPixelBlack(img, p);
                 
                }
        }

        return true;
}


bool validTile(const Tile &t)
{
        return t.getName().size() != 0 && t.getName() != "blank";
}
