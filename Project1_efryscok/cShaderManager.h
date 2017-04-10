#pragma once

#include <string>
#include <map>

// Class used for managing shader information
class cShaderManager {
	// Data
public:
	class cShader {
	public:
		cShader() : ID(0) {};
		unsigned int ID;
		std::string Source;
		std::string fileName;
	};

	class cShaderProgram {
	public:
		cShaderProgram() : ID(0) {};
		unsigned int ID;
		std::string friendlyName;
	};

private:
	std::string m_lastError;

	std::map< unsigned int , cShaderProgram > m_ID_to_Shader;
	std::map< std::string , unsigned int > m_name_to_ID;

	// Functions
public:
	bool useShaderProgram(unsigned int ID);
	bool useShaderProgram(std::string friendlyName);
	bool createProgramFromFile(std::string friendlyName, cShader &vertexShad, cShader &fragShader);
	unsigned int getIDFromFriendlyName(std::string friendlyName);
	std::string getLastError();

private:
	std::string m_loadSourceFromFile(std::string filename);
	bool m_WasThereACompileError(unsigned int shaderID, std::string &errorText);
};
