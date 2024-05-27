#include "ContentsBrowser.h"
#include <filesystem>
#include <iostream>
#include <stack>
#include <imgui.h>

ContentsBrowser::ContentsBrowser() {

	folderIcon = std::make_shared<IconTexture>("./icons/folder.png");
	xmlIcon = std::make_shared<IconTexture>("./icons/xml.png");
	fbxIcon = std::make_shared<IconTexture>("./icons/fbx.png");

	RootFolder = new FileStructure();
	RootFolder->type = FileType::Folder;
	RootFolder->path = "./assets";
	RootFolder->name = "assets";
	currentPath = "./assets";

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
					file->child.push_back(child);
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
	ImGui::SetNextWindowPos(ImVec2(0, 720));
	ImGui::SetNextWindowSize(ImVec2(1280, 240));
	ImGui::Begin(currentPath.c_str(),
		NULL, 
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	for (const auto& file : currentFolder->child) {

		auto textureId = file->type == FileType::Folder ?
			folderIcon->getTexture() :
			file->type == FileType::FBX ?
			fbxIcon->getTexture() : xmlIcon->getTexture();

		ImGui::Image(
			(void*)(intptr_t)textureId,
			ImVec2(32, 32)
		);
		ImGui::SameLine();
		ImGui::Text(file->name.c_str());
	}


	// 컨텍스트 메뉴 시작
	if (ImGui::BeginPopupContextItem("item context menu"))
	{
		if (ImGui::MenuItem("New Level"))
		{
			// "New Level" 항목이 선택되었을 때 실행할 코드
			// 예: 새로운 레벨을 생성하는 함수 호출
			std::cout << "New Level Created!" << std::endl;
		}
		ImGui::EndPopup();
	}

	ImGui::End();
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