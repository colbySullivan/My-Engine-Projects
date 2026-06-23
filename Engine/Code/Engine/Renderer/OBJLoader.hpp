#pragma once
#include <string>

class Model;

class OBJLoader
{
public:
	static void Load( std::string const& filepath, Model& model );
};