#include "label.h"

Label::Label(): _label(std::string()), _count(0)
{}

Label::Label(const std::string &label, const unsigned int id, const int count): _label(label), _count(count), _id(id)
{
}

std::string Label::getLabel() const
{
	return _label;
}

int Label::getCount() const
{
	return _count;
}

unsigned int Label::getId() const
{
	return _id;
}

std::vector<std::string> Label::getImages() const
{
	return _images;
}

void Label::addImage(const std::string &img)
{
	_images.push_back(img);
	_count = _images.size();
}

bool Label::operator<(const Label &l) const
{
	return _count < l._count && _label < l._label;
}

bool operator==(const Label &l1, const Label &l2)
{
	return l1.getId() == l2.getId();
}
