#ifndef DEBUG_H
#define DEBUG_h

bool DEBUG=false;

void debug(const std::string &str)
{
	if(DEBUG)
		std::cerr << "[DEBUG] " << str << std::endl;
}

#endif
