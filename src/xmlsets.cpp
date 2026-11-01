#include "../include/xmlsets.h"

struct stSettings settings;
struct stLogging LogSettings;

void LoadSettings()
{
	char filePath[512];
	snprintf(filePath, sizeof(filePath), "%s/config.xml", ROOT_PATH ? ROOT_PATH : ".");

	tinyxml2::XMLDocument xmlSettings;
	tinyxml2::XMLError xmlErr = xmlSettings.LoadFile(filePath);
	if (xmlErr != tinyxml2::XML_SUCCESS)
	{
		_logs->Log(LogLevel::FATAL, "Failed to load config file (%s): %s", filePath, xmlSettings.ErrorIDToName(xmlErr));
		WorkerClass worker(Globals::instance().getCentralWidget());
		QString errMsg = QString("Failed to load config file (%1): %2")
			.arg(QString::fromUtf8(filePath))
			.arg(xmlSettings.ErrorIDToName(xmlErr));
		emit worker.MessageBox(QString::fromUtf8("Error"), errMsg, QMessageBox::Ok, QMessageBox::Critical);
		QApplication::exit(1);
		return;
	}

	tinyxml2::XMLElement *rakSAMPElement = xmlSettings.FirstChildElement("LiteSAMP");
	if (rakSAMPElement)
	{
		// get RespawnTick
		rakSAMPElement->QueryIntAttribute("respawntick", (int *)&settings.RespawnTick);
		if (settings.RespawnTick <= 0)
			settings.RespawnTick = 1000;

		// get data UpdateRate
		rakSAMPElement->QueryIntAttribute("updaterate", (int *)&settings.UpdateRate);
		if (settings.UpdateRate <= 0)
			settings.UpdateRate = 40;

		// get fps simulation
		rakSAMPElement->QueryIntAttribute("updatestats", (int *)&settings.iUpdateStats);

		// get min simulated fps
		rakSAMPElement->QueryIntAttribute("minfps", (int *)&settings.iMinFPS);
		if (settings.iMinFPS <= 0)
			settings.iMinFPS = 20;

		// get max simulated fps
		rakSAMPElement->QueryIntAttribute("maxfps", (int *)&settings.iMaxFPS);
		if (settings.iMaxFPS <= 0)
			settings.iMaxFPS = 90;

		// get client version
		const char *ver = rakSAMPElement->Attribute("clientversion");
		if (ver)
		{
			strncpy(settings.szClientVersion, ver, sizeof(settings.szClientVersion) - 1);
			settings.szClientVersion[sizeof(settings.szClientVersion) - 1] = '\0';
		}

		// get the first server
		tinyxml2::XMLElement *serverElement = rakSAMPElement->FirstChildElement("server");
		if (serverElement)
		{
			const char *pszAddr = serverElement->GetText();
			if (pszAddr)
			{
				char addrBuf[256];
				strncpy(addrBuf, pszAddr, sizeof(addrBuf) - 1);
				addrBuf[sizeof(addrBuf) - 1] = '\0';

				int iPort = 7777;
				char *colon = strchr(addrBuf, ':');
				if (colon)
				{
					*colon = '\0';
					iPort = atoi(colon + 1);
				}

				strncpy(settings.server.szAddr, addrBuf, sizeof(settings.server.szAddr) - 1);
				settings.server.szAddr[sizeof(settings.server.szAddr) - 1] = '\0';
				settings.server.iPort = iPort;

				const char *nick = serverElement->Attribute("nickname");
				if (nick)
				{
					strncpy(settings.server.szNickname, nick, sizeof(settings.server.szNickname) - 1);
					settings.server.szNickname[sizeof(settings.server.szNickname) - 1] = '\0';
				}

				const char *pass = serverElement->Attribute("password");
				if (pass)
				{
					strncpy(settings.server.szPassword, pass, sizeof(settings.server.szPassword) - 1);
					settings.server.szPassword[sizeof(settings.server.szPassword) - 1] = '\0';
				}
			}
		}

		tinyxml2::XMLElement *LoggingElement = rakSAMPElement->FirstChildElement("log");
		if (LoggingElement)
		{
			LogSettings.RPC = static_cast<bool>(LoggingElement->IntAttribute("rpc", 0));
			LogSettings.Packet = static_cast<bool>(LoggingElement->IntAttribute("pkt", 0));
		}

		_logs->Log(LogLevel::INFO, "[CONFIG] Settings loaded: Nickname='%s', Server=%s:%d, ClientVersion='%s', Logging(RPC=%d, Packet=%d)",
			settings.server.szNickname, settings.server.szAddr, settings.server.iPort, settings.szClientVersion, LogSettings.RPC, LogSettings.Packet);
	}
}

void UnLoadSettings()
{
	memset(&settings, 0, sizeof(settings));
	memset(&LogSettings, 0, sizeof(LogSettings));
}

void ReloadSettings()
{
	UnLoadSettings();
	AppendLogF("Settings unloaded");
	_logs->Log(LogLevel::DEBUG, "Settings unloaded");
	LoadSettings();
	AppendLogF("Settings reloaded");
	_logs->Log(LogLevel::DEBUG, "Settings reloaded");
}
