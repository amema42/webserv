#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <sstream>

std::string dirTree(const std::string& dirPath, int depth) //partire da 0
{
	DIR *dir;
	struct dirent *ent;
	std::stringstream html;

	if ((dir = opendir(dirPath.c_str())) != NULL)
	{
		if (depth == 0)
		{
			html << "<!DOCTYPE html>\n<html>\n<head>\n<title>Contenuto directory: " 
				 << dirPath << "</title>\n</head>\n<body>\n";
			html << "<h1>Contenuto directory: " << dirPath << "</h1>\n";
		}
		html << "<ul>\n";
		while ((ent = readdir(dir)) != NULL)
		{
			std::string name = ent->d_name;
			
			if (name != "." && name != "..") {
				std::string fullPath = dirPath + "/" + name;
				struct stat statbuf;
				
				if (stat(fullPath.c_str(), &statbuf) == 0)
				{
					if (S_ISDIR(statbuf.st_mode))
					{
						html << "<li><strong>" << name << "/</strong>\n";
						html << dirTree(fullPath, depth + 1);
						html << "</li>\n";
					} 
					else 
						html << "<li>" << name << "</li>\n";
				}
			}
		}
		html << "</ul>\n";
		closedir(dir);
		if (depth == 0) 
			html << "</body>\n</html>\n";
	} 
	else
		html << "<p>Impossibile aprire la directory</p>";
	return html.str();
}

#include <iostream>

int main() {
    std::string directoryPath = ".";
    std::string htmlPage = dirTree(directoryPath, 0);
    
    std::cout << htmlPage << std::endl;
    
    return 0;
}