#include "../include/xmlsets.h"

struct stSettings settings;
tinyxml2::XMLDocument *xmlSettings;

int LoadSettings()
{
	// load
	size_t filePathSize = strlen(ROOT_PATH) + strlen("/config.xml") + 1;
	char *filePath = (char *)malloc(filePathSize);
	sprintf(filePath, "%s/config.xml", ROOT_PATH);

	if (filePath == nullptr)
	{
		fprintf(stderr, "Failed to allocate memory for filePath\n");
		QApplication::quit();
	}
	tinyxml2::XMLError xmlErr = xmlSettings->LoadFile(filePath);
	if (xmlErr != 0)
	{
		WorkerClass worker(Globals::instance().getCentralWidget());
		char *ErrMsg = (char *)malloc(70);
		sprintf(ErrMsg, "Failed to load the config file : %s", xmlSettings->ErrorIDToName(xmlErr));
		if (ErrMsg != nullptr)
		{
			emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8(ErrMsg), QMessageBox::Ok, QMessageBox::Critical, nullptr, ZMessageBox::Exit);
			delete ErrMsg;
		}
	}

	tinyxml2::XMLElement *rakSAMPElement = xmlSettings->FirstChildElement("LiteSAMP");
	if (rakSAMPElement)
	{
		// get RespawnTick
		rakSAMPElement->QueryIntAttribute("respawntick", (int *)&settings.RespawnTick);

		// get data UpdateRate
		rakSAMPElement->QueryIntAttribute("updaterate", (int *)&settings.UpdateRate);

		// get fps simulation
		rakSAMPElement->QueryIntAttribute("updatestats", (int *)&settings.iUpdateStats);

		// get min simulated fps
		rakSAMPElement->QueryIntAttribute("minfps", (int *)&settings.iMinFPS);

		// get max simulated fps
		rakSAMPElement->QueryIntAttribute("maxfps", (int *)&settings.iMaxFPS);

		// get client version
		strcpy(settings.szClientVersion, (char *)rakSAMPElement->Attribute("clientversion"));

		// get followplayer
		/*strcpy(settings.szFollowingPlayerName, (char *)rakSAMPElement->Attribute("followplayer"));
		rakSAMPElement->QueryIntAttribute("followplayerwithvehicleid", &settings.iFollowingWithVehicleID);
		rakSAMPElement->QueryFloatAttribute("followXOffset", &settings.fFollowXOffset);
		rakSAMPElement->QueryFloatAttribute("followYOffset", &settings.fFollowYOffset);
		rakSAMPElement->QueryFloatAttribute("followZOffset", &settings.fFollowZOffset);*/

		// get the first server
		tinyxml2::XMLElement *serverElement = rakSAMPElement->FirstChildElement("server");
		if (serverElement)
		{
			char *pszAddr = (char *)serverElement->GetText();
			if (pszAddr)
			{
				int iPort;
				char *pszAddrBak = pszAddr;

				while (*pszAddrBak)
				{
					if (*pszAddrBak == ':')
					{
						*pszAddrBak = 0;
						pszAddrBak++;
						iPort = atoi(pszAddrBak);
					}
					pszAddrBak++;
				}

				strcpy(settings.server.szAddr, pszAddr);
				settings.server.iPort = iPort;
				strcpy(settings.server.szNickname, (char *)serverElement->Attribute("nickname"));
				strcpy(settings.server.szPassword, (char *)serverElement->Attribute("password"));
			}
		}
	}

	xmlSettings->Clear();
	free(filePath);

	return 1;
}

int UnLoadSettings()
{
	memset(&settings, 0, sizeof(settings));

	return 1;
}

int ReloadSettings()
{
	if (UnLoadSettings() && LoadSettings())
	{
		AppendLogF("Settings reloaded");
		_logs->Log(LogLevel::DEBUG, "Settings reloaded");
		return 1;
	}

	AppendLogF("Failed to reload settings");
	_logs->Log(LogLevel::DEBUG, "Failed to reload settings");

	return 0;
}
