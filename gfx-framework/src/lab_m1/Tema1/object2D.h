#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object2D
{

    Mesh* CreateBlock(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateGun(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 baseColor, glm::vec3 barrelColor, glm::vec3 pivotColor, bool fill);
    Mesh* CreateBumper(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 baseColor, glm::vec3 arcColor, bool fill);
	Mesh* CreateEngine(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateButton(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
    Mesh* CreateArrow(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill);
	Mesh* CreateBall(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill);
}
