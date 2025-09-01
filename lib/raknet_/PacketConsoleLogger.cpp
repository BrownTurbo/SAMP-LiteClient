#include "PacketConsoleLogger.h"
#include "LogCommandParser.h"

PacketConsoleLogger::PacketConsoleLogger()
{
	logCommandParser = 0;
}

void PacketConsoleLogger::SetLogCommandParser(LogCommandParser *lcp)
{
	logCommandParser = lcp;
	if (logCommandParser)
		logCommandParser->AddChannel("PacketConsoleLogger");
}
void PacketConsoleLogger::WriteAppendLogF(const char *str)
{
	if (logCommandParser)
		logCommandParser->WriteAppendLogF("PacketConsoleLogger", str);
}
