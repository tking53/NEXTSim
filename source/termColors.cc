#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <sstream>

#include "termColors.hh"

using namespace TermColors;

const size_t leaderLength = 50;

std::string Display::fatalStr = BtRed + Flashing + "[FATAL ERROR] ";
std::string Display::errorStr = DkRed + "[ERROR] ";
std::string Display::warnStr = DkYellow + "[WARNING] ";
std::string Display::infoStr = BtBlue + "[INFO] ";
std::string Display::okayStr = DkGreen + "[OK] ";

void Display::LeaderPrint(const std::string &str){
	char ch = std::cout.fill();

	std::cout.fill('.');
	std::cout << std::setw(leaderLength) << std::left << " " << str << std::flush;
	std::cout.fill(ch);
}

std::string Display::CriticalStr(const std::string &name/*=""*/){
	if(name.empty())
		return fatalStr;
	return (fatalStr + name + ": ");
}

std::string Display::ErrorStr(const std::string &name/*=""*/){
	if(name.empty())
		return errorStr;
	return (errorStr + name + ": ");
}

std::string Display::InfoStr(const std::string &name/*=""*/){
	if(name.empty())
		return infoStr;
	return (infoStr + name + ": ");
}

std::string Display::OkayStr(const std::string &name/*=""*/){
	if(name.empty())
		return okayStr;
	return (okayStr + name + ": ");
}

std::string Display::WarningStr(const std::string &name/*=""*/){
	if(name.empty())
		return warnStr;
	return (warnStr + name + ": ");
}

std::string Display::ResetStr(){
	return Reset;
}

void Display::CriticalPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << CriticalStr(name) << str << Reset << std::endl;
}

void Display::ErrorPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << ErrorStr(name) << str << Reset << std::endl;
}

void Display::InfoPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << InfoStr(name) << str << Reset << std::endl;
}

void Display::OkayPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << OkayStr(name) << str << Reset << std::endl;
}

void Display::WarningPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << WarningStr(name) << str << Reset << std::endl;
}

bool Display::StatusPrint(bool errorStatus){
	if (errorStatus) 
		std::cout << ErrorStr() << std::endl;
	else
		std::cout << OkayStr() << std::endl;
	return errorStatus;
}
