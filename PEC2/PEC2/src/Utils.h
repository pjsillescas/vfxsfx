#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

class Utils
{
public:
	//! Splits a string given a character for separation
	static std::vector<std::string> splitString(std::string aString, char separationCharacter);

	static void fillMatrixData(GLuint id, glm::mat4& projectionMatrix, glm::mat4& viewMatrix);

};