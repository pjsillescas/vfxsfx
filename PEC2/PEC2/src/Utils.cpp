//Include our classes
#include "Utils.h"
#include <sstream>


std::vector<std::string> Utils::splitString(std::string aString, char separationCharacter)
{
	std::stringstream test(aString);
	std::string segment;
	std::vector<std::string> seglist;

	while(std::getline(test, segment, separationCharacter))
	{
	   seglist.push_back(segment);
	}
	return seglist;
}

void Utils::fillMatrixData(GLuint shaderId, glm::mat4& projectionMatrix, glm::mat4& viewMatrix)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(glGetUniformLocation(shaderId, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
}
