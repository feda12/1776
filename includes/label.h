#ifndef LABEL_H
#define LABEL_H

#include <string>
#include <vector>

class Label
{
	public:
		Label();
		Label(const std::string &label, const unsigned int id, const int count=0);
		std::string getLabel() const;
		 int getCount() const;
		unsigned int getId() const;
		std::vector<std::string> getImages() const;
		void addImage(const std::string &img);
		bool operator<(const Label &l) const;
		friend bool operator==(const Label &l1, const Label & l2);
	private:
		std::string _label;
		int _count;
		unsigned int _id;
		std::vector<std::string> _images;
};

#endif
