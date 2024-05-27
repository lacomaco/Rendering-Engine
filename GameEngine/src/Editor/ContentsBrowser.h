#pragma once
#include <string>
#include <vector>
#include "./IconTexture.h"
#include <memory>


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

	std::shared_ptr<IconTexture> folderIcon;
	std::shared_ptr<IconTexture> fbxIcon;
	std::shared_ptr<IconTexture> xmlIcon;

public:
	ContentsBrowser();
	~ContentsBrowser();
	void UpdateContentsBrowserGUI();

	std::string currentPath;
};

