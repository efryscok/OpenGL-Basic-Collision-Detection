#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>

#include "cShaderManager.h"

bool cShaderManager::useShaderProgram(unsigned int ID) {
	glUseProgram(ID);
	return true;
}

bool cShaderManager::useShaderProgram(std::string friendlyName) {
	std::map< std::string , unsigned int >::iterator
		itShad = this->m_name_to_ID.find(friendlyName);

	if (itShad == this->m_name_to_ID.end()) {
		return false;
	}
	glUseProgram(itShad->second);

	return true;
}

unsigned int cShaderManager::getIDFromFriendlyName(std::string friendlyName) {
	std::map< std::string , unsigned int >::iterator
		itShad = this->m_name_to_ID.find(friendlyName);

	if (itShad == this->m_name_to_ID.end()) {
		return 0;
	}
	return itShad->second;
}

std::string cShaderManager::m_loadSourceFromFile(std::string filename) {
	std::ifstream theFile(filename.c_str());

	std::stringstream ssSource;
	std::string temp;

	while (theFile >> temp) {
		ssSource << temp << " ";
	}

	theFile.close();
	return ssSource.str();
}

bool cShaderManager::m_WasThereACompileError(unsigned int shaderID,
	std::string &errorText) {
	errorText = "";

	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);

		errorText.append(pLogText);
		return true;
	}
	return false;
}

std::string cShaderManager::getLastError(void) {
	std::string lastErrorTemp = this->m_lastError;
	this->m_lastError = "";
	return lastErrorTemp;
}

bool cShaderManager::createProgramFromFile(std::string friendlyName, cShader &vertexShad, cShader &fragShader) {

	vertexShad.ID = glCreateShader(GL_VERTEX_SHADER);
	vertexShad.Source = this->m_loadSourceFromFile(vertexShad.fileName);

	const char* tempVertChar = vertexShad.Source.c_str();

	glShaderSource(vertexShad.ID, 1, &tempVertChar, NULL);
	glCompileShader(vertexShad.ID); //???? 

	delete tempVertChar;

	std::string errorText = "";
	if (this->m_WasThereACompileError(vertexShad.ID, errorText)) {
		std::stringstream ssError;
		ssError << "Vertex shader compile error: ";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	fragShader.ID = glCreateShader(GL_FRAGMENT_SHADER);
	fragShader.Source = this->m_loadSourceFromFile(fragShader.fileName);

	const char* tempFragChar = fragShader.Source.c_str();
	glShaderSource(fragShader.ID, 1, &tempFragChar, NULL);
	glCompileShader(fragShader.ID);
	delete tempFragChar;

	if (this->m_WasThereACompileError(vertexShad.ID, errorText)) {
		std::stringstream ssError;
		ssError << "Fragment shader compile error: ";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	cShaderProgram curProgram;
	curProgram.ID = glCreateProgram();

	glAttachShader(curProgram.ID, vertexShad.ID);
	glAttachShader(curProgram.ID, fragShader.ID);
	glLinkProgram(curProgram.ID);

	curProgram.friendlyName = friendlyName;

	this->m_ID_to_Shader[curProgram.ID] = curProgram;
	this->m_name_to_ID[curProgram.friendlyName] = curProgram.ID;

	return true;
}
