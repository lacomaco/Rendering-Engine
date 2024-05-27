#include "ContentsBrowser.h"
#include <filesystem>
#include <iostream>
#include <stack>

ContentsBrowser::ContentsBrowser() {
	RootFolder = new FileStructure();
	RootFolder->type = FileType::Folder;
	RootFolder->path = "./assets";
	RootFolder->name = "assets";

	currentFolder = RootFolder;
	visitPath.push_back("assets");

	createDirectory(RootFolder);
}

void ContentsBrowser::createDirectory(FileStructure* file) {
	try {
		if (std::filesystem::exists(file->path) && std::filesystem::is_directory(file->path)) {
			for (const auto& entry : std::filesystem::directory_iterator(file->path)) {
				FileStructure* child = new FileStructure();
				child->path = entry.path().string();
				child->name = entry.path().filename().string();
				child->parent = file;

				if (std::filesystem::is_regular_file(entry)) {
					std::string extension = entry.path().extension().string();
					if (extension == ".fbx" || extension == ".xml") {
						child->type = extension == ".fbx" ? FileType::FBX : FileType::XML;
						file->child.push_back(child);
					}
				}
				else if (std::filesystem::is_directory(entry)) {
					child->type = FileType::Folder;
					createDirectory(child);
				}
			}
		}
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::cerr << "Filesystem error: " << e.what() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "General error: " << e.what() << std::endl;
	}
}

void ContentsBrowser::UpdateContentsBrowserGUI() {

}

ContentsBrowser::~ContentsBrowser() {
	if (RootFolder == NULL) {
		return;
	}

	std::stack<FileStructure*> nodes;

	nodes.push(RootFolder);

	while (!nodes.empty()) {
		FileStructure* current = nodes.top();
		nodes.pop();

		for (FileStructure* childNode : current->child) {
			nodes.push(childNode);
		}

		delete current;
	}
}