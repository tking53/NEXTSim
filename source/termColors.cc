#include <iomanip>
#include <iostream>

#include <cstdlib>

#include "termColors.hh"

using namespace TermColors;

static const size_t leaderLength = 50;

bool Display::hasColorTerm = true;

void Display::SetColorTerm(){
#ifdef NOCOLORINTERFACE
	hasColorTerm = false;
#else
	std::string termname = getenv("TERM");
	
	if (termname == "xterm")
		hasColorTerm = true;
#endif
}

void Display::LeaderPrint(const std::string &str){
	char ch = std::cout.fill();

	std::cout.fill('.');
	std::cout << std::setw(leaderLength) << std::left << " " << str << std::flush;
	std::cout.fill(ch);
}

std::string Display::CriticalStr(const std::string &str){
	if (hasColorTerm)
		return (std::string(BtRed) + Flashing + str + Reset);
	return (std::string("###") + str + "###");
}

std::string Display::ErrorStr(const std::string &str){
	if (hasColorTerm)
		return (DkRed + str + Reset);
	return str;
}

std::string Display::InfoStr(const std::string &str){
	if (hasColorTerm)
		return (DkBlue + str + Reset);
	return str;
}

std::string Display::OkayStr(const std::string &str){
	if (hasColorTerm)
		return (DkGreen + str + Reset);
	return str;
}

std::string Display::WarningStr(const std::string &str){
	if (hasColorTerm)
		return (DkYellow + str + Reset);
	return str;
}

void Display::CriticalPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << BtRed << Flashing << "[CRITICAL ERROR]";
	if(!name.empty())
		std::cout << " " << name << ":";
	std::cout << " " << str << Reset << std::endl;
}

void Display::ErrorPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << DkRed << "[ERROR]";
	if(!name.empty())
		std::cout << " " << name << ":";
	std::cout << " " << str << Reset << std::endl;
}

void Display::InfoPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << DkBlue << "[INFO]";
	if(!name.empty())
		std::cout << " " << name << ":";
	std::cout << " " << str << Reset << std::endl;
}

void Display::OkayPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << DkGreen << "[OK]";
	if(!name.empty())
		std::cout << " " << name << ":";
	std::cout << " " << str << Reset << std::endl;
}

void Display::WarningPrint(const std::string &str, const std::string &name/*=""*/){
	std::cout << DkYellow << "[WARNING]";
	if(!name.empty())
		std::cout << " " << name << ":";
	std::cout << " " << str << Reset << std::endl;
}

bool Display::StatusPrint(bool errorStatus){
	if (errorStatus) 
		std::cout << Display::ErrorStr() << std::endl;
	else
		std::cout << Display::OkayStr() << std::endl;
	return errorStatus;
}
