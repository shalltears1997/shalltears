#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include "gv_extern.hpp"

int ReadConfigXML(char cwd[])
{
	try
	{
		char full_path[256];
		sprintf(full_path, "%s/config.xml", cwd);

		boost::property_tree::ptree pt;
		read_xml(full_path, pt);

		if (boost::optional<int> tmp = pt.get_optional<int>("connection.poR")){
			poR = tmp.get();} else throw -1;
		if (boost::optional<int> tmp = pt.get_optional<int>("connection.poO")){
			poO = tmp.get();} else throw -1;
		if (boost::optional<int> tmp = pt.get_optional<int>("connection.poS")){
			poS = tmp.get();} else throw -1;
		if (boost::optional<int> tmp = pt.get_optional<int>("connection.poF")){
			poF = tmp.get();} else throw -1;
		if (boost::optional<int> tmp = pt.get_optional<int>("connection.HARK_connect")){
			HARK_connect = tmp.get();} else throw -1;
		if (boost::optional<int> tmp = pt.get_optional<int>("connection.ROBOT")){
			ROBOT = tmp.get();} else throw -1;

		if (boost::optional<int> tmp = pt.get_optional<int>("setting.change_attachment")){
			change_attachment = tmp.get();} else throw -1;
		if (boost::optional<int> tmp = pt.get_optional<int>("setting.random_model")){
			random_model = tmp.get();} else throw -1;
		if (boost::optional<bool> tmp = pt.get_optional<bool>("setting.exp_condition")){
			exp_condition = tmp.get();} else throw -1;
		if (boost::optional<int> tmp = pt.get_optional<int>("setting.simRefreshRatio_clock")){
			simRefreshRatio_clock = tmp.get();} else throw -1;
	}
	catch(std::exception&)
	{
		std::cout << "xml読み込みエラー: " << std::endl;
		return -1;
	}

	std::cout << "-- read config file --" << std::endl;
	std::cout << "poR: " << poR << std::endl;
	std::cout << "poO: " << poO << std::endl;
	std::cout << "poS: " << poS << std::endl;
	std::cout << "poF: " << poF << std::endl;
	std::cout << "HARK_connect: " << HARK_connect << std::endl;
	std::cout << "ROBOT: " << ROBOT << std::endl;
	std::cout << "change_attachment: " << change_attachment << std::endl;
	std::cout << "random_model: " << random_model << std::endl;
	std::cout << "exp_condition: " << exp_condition << std::endl;
	std::cout << "simRefreshRatio_clock: " << simRefreshRatio_clock << std::endl;
	std::cout << "-- read config file --\n" << std::endl;

	PRINT("exp_condition: %d", exp_condition);
	

	return 0;
}
