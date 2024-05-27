#pragma once
#include <string>
#include <vector>

enum FileType {
	Folder,
	FBX,
	XML // level 파일
};

struct FileStructure {
	FileType type;
	std::string path;
	std::string name;

	FileStructure* parent;

	// Folder일경우 존재.
	std::vector<FileStructure*> child;
};

class ContentsBrowser
{
private:
	void createDirectory(FileStructure* file);

	FileStructure* RootFolder; // ./assets
	FileStructure* currentFolder;
	std::vector<std::string> visitPath;
public:
	ContentsBrowser();
	~ContentsBrowser();
	void UpdateContentsBrowserGUI();

	std::string currentPath;
};

