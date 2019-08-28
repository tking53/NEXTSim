#ifndef TERM_COLORS_HH
#define TERM_COLORS_HH

#include <string>

namespace TermColors {
	static std::string DkRed     = "\033[0;31m";
	static std::string DkGreen   = "\033[0;32m";
	static std::string DkYellow  = "\033[0;33m";
	static std::string DkBlue    = "\033[0;34m";
	static std::string DkMagenta = "\033[0;35m";
	static std::string DkCyan    = "\033[0;36m";
	static std::string DkWhite   = "\033[0;37m";

	static std::string BtRed     = "\033[1;31m";
	static std::string BtGreen   = "\033[1;32m";
	static std::string BtYellow  = "\033[1;33m";
	static std::string BtBlue    = "\033[1;34m";
	static std::string BtMagenta = "\033[1;35m";
	static std::string BtCyan    = "\033[1;36m";
	static std::string BtWhite   = "\033[1;37m";

	static std::string Flashing  = "\033[5m";
	static std::string Underline = "\033[4m"; 
	static std::string Reset     = "\033[0m";
}

namespace Display {
	std::string CriticalStr(const std::string &name="");

	std::string ErrorStr(const std::string &name="");

	std::string InfoStr(const std::string &name="");

	std::string OkayStr(const std::string &name="");

	std::string WarningStr(const std::string &name="");

	std::string ResetStr();

	void CriticalPrint(const std::string &str, const std::string &name="");

	void ErrorPrint(const std::string &str, const std::string &name="");

	void InfoPrint(const std::string &str, const std::string &name="");

	void OkayPrint(const std::string &str, const std::string &name="");

	void WarningPrint(const std::string &str, const std::string &name="");

	void LeaderPrint(const std::string &str);

	bool StatusPrint(bool errorStatus);

	extern std::string fatalStr;
	extern std::string errorStr;
	extern std::string warnStr;
	extern std::string infoStr;
	extern std::string okayStr;
}

#endif
