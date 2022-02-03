#include<fstream>
#include"yaml-cpp/yaml.h"

namespace test
{
	void YamlWrite()
	{
		YAML::Node node;
		node["key"] = "value";
		node["seq"].push_back("first element");
		node["seq"].push_back("second element");

		std::ofstream fout("config.yaml");
		fout << node;
	}
}