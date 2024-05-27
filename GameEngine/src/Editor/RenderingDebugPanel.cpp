#include "RenderingDebugPanel.h"
#include "imgui.h"
#include "EditorSharedValue.h"

namespace RenderingDebugPanel {
	void UpdateRenderingDebugPanel() {
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		ImGui::Text(
			"Camera Position (x:%.3f y:%.3f z:%.3f)",
			EditorSharedValue::cameraPosition.x,
			EditorSharedValue::cameraPosition.y,
			EditorSharedValue::cameraPosition.z
		);

		ImGui::Text(
			"Camera Front (x:%.3f y:%.3f z:%.3f)",
			EditorSharedValue::cameraFront.x,
			EditorSharedValue::cameraFront.y,
			EditorSharedValue::cameraFront.z
		);

		ImGui::SliderFloat("cameraSpeed", &EditorSharedValue::cameraSpeed, 0.0f, 10.0f);

		ImGui::Text("Exposure");
		ImGui::SliderFloat("exposure", &EditorSharedValue::exposure, 0.0f, 10.0f);

		ImGui::Checkbox("showNormal", &EditorSharedValue::showNormal);

		if (ImGui::TreeNode("DirectionalLight")) {
			ImGui::Text("Position");
			ImGui::SliderFloat("x", &EditorSharedValue::directionalLightPosition.x, -20.0f, 20.0f);
			ImGui::BeginDisabled();
			ImGui::SliderFloat("y", &EditorSharedValue::directionalLightPosition.y, -20.0f, 20.0f);
			ImGui::EndDisabled();
			ImGui::SliderFloat("z", &EditorSharedValue::directionalLightPosition.z, -20.0f, 20.0f);

			ImGui::Text("Direction");
			ImGui::SliderFloat("x", &EditorSharedValue::directionalLightDirection.x, -1.0f, 1.0f);
			ImGui::BeginDisabled();
			ImGui::SliderFloat("y", &EditorSharedValue::directionalLightDirection.y, -1.0f, 1.0f);
			ImGui::EndDisabled();
			ImGui::SliderFloat("z", &EditorSharedValue::directionalLightDirection.z, -1.0f, 1.0f);

			ImGui::Text("shadow depthMap");
			ImGui::Image(
				(void*)(intptr_t)EditorSharedValue::directionalLightDepthMap,
				ImVec2(200, 200),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("CubeMap Test")) {
			if (ImGui::RadioButton("skybox", EditorSharedValue::select == 0)) {
				EditorSharedValue::select = 0;
			}

			if (ImGui::RadioButton("radiance", EditorSharedValue::select == 1)) {
				EditorSharedValue::select = 1;
			}

			if (ImGui::RadioButton("irradiance", EditorSharedValue::select == 2)) {
				EditorSharedValue::select = 2;
			}

			ImGui::SliderFloat("lodLevel", &EditorSharedValue::lodLevel, 0.0f, 6.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("PBR Test")) {
			ImGui::Text("Metallic");
			ImGui::SliderFloat("metallic", &EditorSharedValue::metallic, 0.0f, 1.0f);

			ImGui::Text("Roughness");
			ImGui::SliderFloat("roughness", &EditorSharedValue::roughness, 0.0f, 1.0f);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Bloom")) {
			ImGui::Text("bloomThreshold");
			ImGui::SliderFloat("bloomCount", &EditorSharedValue::bloomThreshold, 0, 1.0);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("light")) {
			ImGui::Checkbox("useSun", &EditorSharedValue::useSun);
			ImGui::Checkbox("usePointLight", &EditorSharedValue::usePointLight);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("for god ray debugging")) {
			ImGui::Image(
				(void*)(intptr_t)EditorSharedValue::godLightTexture,
				ImVec2(200, 200),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);
			ImGui::SliderFloat("decay", &EditorSharedValue::decay, 0.0f, 1.0f);
			ImGui::SliderFloat("density", &EditorSharedValue::density, 0.0f, 1.0f);
			ImGui::SliderFloat("weight", &EditorSharedValue::weight, 0.0f, 1.0f);
			ImGui::SliderFloat("exposure", &EditorSharedValue::godRayExposure, 0.0f, 1.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("show GBuffer")) {
			ImGui::Text("Position Metallic Texture");
			ImGui::Image(
				(void*)(intptr_t)EditorSharedValue::positionMetallicTexture,
				ImVec2(200, 200),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);

			ImGui::Text("Albedo Roughness Texture");
			ImGui::Image(
				(void*)(intptr_t)EditorSharedValue::albedoRoughnessTexture,
				ImVec2(200, 200),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);

			ImGui::Text("Normal Texture");
			ImGui::Image(
				(void*)(intptr_t)EditorSharedValue::normalTexture,
				ImVec2(200, 200),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);

			ImGui::Text("God Ray Texture");
			ImGui::Image(
				(void*)(intptr_t)EditorSharedValue::godRayTexture,
				ImVec2(200, 200),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("show SSAO Texture")) {
			ImGui::Text("SSAO Texture");
			ImGui::Image(
				(void*)(intptr_t)EditorSharedValue::ssaoTexture,
				ImVec2(200, 200),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);
			ImGui::Text("SSAO Blur Texture");
			ImGui::Image(
				(void*)(intptr_t)EditorSharedValue::ssaoBlurTexture,
				ImVec2(200, 200),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);
			ImGui::SliderFloat("radius", &EditorSharedValue::radius, 0.0f, 5.0f);
			ImGui::SliderFloat("bias", &EditorSharedValue::bias, -1.0f, 1.0f);
			ImGui::Checkbox("useSSAO", &EditorSharedValue::useSSAO);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Lens Flare")) {
			ImGui::SliderInt("ghosts", &EditorSharedValue::uGhosts, 0, 10);
			ImGui::SliderFloat("ghostDispersal", &EditorSharedValue::uGhostDispersal, 0.0f, 1.0f);
			ImGui::Image(
				(void*)(intptr_t)EditorSharedValue::lensFlareTexture,
				ImVec2(200, 200),
				ImVec2(0, 1),
				ImVec2(1, 0)
			);
			ImGui::TreePop();
		}
	}
}