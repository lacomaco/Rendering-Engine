#include "ContentsBrowser.h"
#include <filesystem>
#include <iostream>
#include <stack>
#include <imgui.h>

char ContentsBrowser::xmlName[128] = "";

ContentsBrowser::ContentsBrowser() {

	folderIcon = std::make_shared<IconTexture>("./assets/icons/folder.png");
	xmlIcon = std::make_shared<IconTexture>("./assets/icons/xml.png");
	fbxIcon = std::make_shared<IconTexture>("./assets/icons/fbx.png");

	RootFolder = new FileStructure();
	RootFolder->type = FileType::Folder;
	RootFolder->path = "./assets";
	RootFolder->name = "assets";
	currentPath = "./assets";

	currentFolder = RootFolder;
	visitPath.push_back("assets");

	createDirectory(RootFolder);
}

void ContentsBrowser::CreateXML(std::string fileName) {

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
					if (extension == ".fbx" || extension == ".xml" || extension == ".gltf" || extension == ".obj") {
						child->type = extension == ".xml" ? FileType::XML : FileType::FBX;
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
	ImGui::Begin(currentFolder->path.c_str(),
		NULL, 
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

	if (currentFolder->parent && ImGui::Button("Back")) {
		currentFolder = currentFolder->parent;
	}

	if (ImGui::Button("Create New Level xml")) {
		ImGui::OpenPopup("Create XML");
	}

	if (ImGui::BeginPopupModal("Create XML", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Enter XML Name:");
		ImGui::InputText("##xmlname", xmlName, IM_ARRAYSIZE(xmlName));

		if (ImGui::Button("Create")) {
			std::string newXmlName(xmlName);
			std::cout << "New XML Name: " << newXmlName << std::endl;
			xmlName[0] = '\0';
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	for (const auto& file : currentFolder->child) {

		auto textureId = file->type == FileType::Folder ?
			folderIcon->getTexture() :
			file->type == FileType::FBX ?
			fbxIcon->getTexture() : xmlIcon->getTexture();

		ImGui::Image(
			(void*)(intptr_t)textureId,
			ImVec2(32, 32)
		);

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			if (file->type == FileType::Folder) {
				currentFolder = file;
			}
		}

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