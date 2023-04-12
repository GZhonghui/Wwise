#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <string>
#include <cstdint>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>

#include "Wwise/IO/Win32/AkFilePackageLowLevelIOBlocking.h"

#include "Assets/Banks/GeneratedSoundBanks/Wwise_IDs.h"

struct PlayingInfo
{
	std::string m_eventName;
};

static std::unordered_map<AkGameObjectID, std::unordered_map<AkPlayingID, PlayingInfo>> g_playRecord;

inline void Record(AkGameObjectID gameObjectID, AkPlayingID playingID, const PlayingInfo& playingInfo)
{
	if (!g_playRecord.count(gameObjectID))
	{
		g_playRecord[gameObjectID] = std::unordered_map<AkPlayingID, PlayingInfo>();
	}

	g_playRecord[gameObjectID][playingID] = playingInfo;
}

inline bool Query(AkGameObjectID gameObjectID, AkPlayingID playingID, PlayingInfo& playingInfo)
{
	if (!g_playRecord.count(gameObjectID)) return false;
	if (!g_playRecord[gameObjectID].count(playingID)) return false;

	playingInfo = g_playRecord[gameObjectID][playingID];

	return true;
}

inline bool InitWwiseMemoryManager()
{
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		puts("Init Wwise Memory Manager Failed");
		return false;
	}

	puts("Wwise Memory Manager Inited");
	return true;
}

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

inline bool InitWwiseStreamingManager()
{
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	if (!AK::StreamMgr::Create(stmSettings))
	{
		puts("Create Streaming Failed");
		return false;
	}

	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		puts("Init Device Failed");
		return false;
	}

	puts("Wwise Steaming Manager Inited");

	return true;
}

inline bool InitWwiseSoundEngine()
{
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;

	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK_Success != AK::SoundEngine::Init(&initSettings, &platformInitSettings))
	{
		puts("Init Wwise Sound Engine Failed");
		return false;
	}

	puts("Wwise Sound Engine Inited");

	return true;
}

inline bool InitWwiseMusicEngine()
{
	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		puts("Init Music Engine Failed");
		return false;
	}

	puts("Wwise Music Engine Inited");

	return true;
}

inline bool InitWwiseSpatialAudio()
{
	AkSpatialAudioInitSettings initSettings; // Keep Default Value
	
	if (AK::SpatialAudio::Init(initSettings) != AK_Success)
	{
		puts("Init Spatial Audio Failed");
		return false;
	}

	puts("Wwise Spatial Audio Inited");

	return true;
}

// AK_OPTIMIZED
// WAAPI
inline bool InitWwiseCommunications()
{

}

inline bool InitWwise()
{
	InitWwiseMemoryManager();
	InitWwiseStreamingManager();
	InitWwiseSoundEngine();
	InitWwiseMusicEngine();
	InitWwiseSpatialAudio();

	puts("Init Wwise Successfully!");
	return true;
}

inline bool TermWwise()
{
	// No Need to Term Spatial Audio

	AK::MusicEngine::Term();
	AK::SoundEngine::Term();

	g_lowLevelIO.Term();
	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	AK::MemoryMgr::Term();

	return true;
}

inline void LoadEssentialBanks()
{
	AKRESULT operResult = AK_Fail;
	operResult = g_lowLevelIO.SetBasePath(AKTEXT("./Assets/Banks/GeneratedSoundBanks/Windows/"));
	printf("Set Base Path: %d\n", (int)operResult);

	AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

	AkBankID bankID = 0;
	// operResult = AK::SoundEngine::LoadBank(AK::BANKS::INIT);
	operResult = AK::SoundEngine::LoadBank(L"Init", bankID);
	if (operResult != AK_Success)
	{
		printf("Load Init Bank Failed: %d\n", (int)operResult);
	}
	else
	{
		printf("Init Bank Loaded, ID = %u\n", (uint32_t)bankID);
	}

	// operResult = AK::SoundEngine::LoadBank(AK::BANKS::DEFAULTBANK);
	operResult = AK::SoundEngine::LoadBank(L"DefaultBank", bankID);
	printf("Default Bank Load: %d\n", (int)operResult);
}

inline void UpdateAudio(float deltaTime)
{
	AK::SoundEngine::RenderAudio();
}

inline bool CatchInput()
{
	return false;
}

inline void Post(AkGameObjectID gameObjectID, std::string eventName, AkPlayingID* playingID)
{

}

inline void Stop(AkGameObjectID gameObjectID, std::string eventName)
{

}

int main()
{
	InitWwise();

	LoadEssentialBanks();

	// Trigger Test Event

	AkGameObjectID defaultListener = 2;
	AK::SoundEngine::RegisterGameObj(defaultListener);
	AK::SoundEngine::SetDefaultListeners(&defaultListener, 1);

	AkGameObjectID gameObjectID = 1;
	AK::SoundEngine::RegisterGameObj(gameObjectID);

	auto playingID = AK::SoundEngine::PostEvent(AK::EVENTS::PLAY_01, gameObjectID);
	printf("Trigger Event, Playing ID = %d\n", (int)playingID);

	AK::SoundEngine::StopPlayingID(playingID);

	// Stop All Events
	AK::SoundEngine::ExecuteActionOnEvent(AK::EVENTS::PLAY_01, AK::SoundEngine::AkActionOnEventType_Stop, gameObjectID);

	while (true)
	{
		if (CatchInput())
		{
			
		}

		UpdateAudio(0.0333f);
	}

	AK::SoundEngine::UnregisterAllGameObj();

	TermWwise();

	return 0;
}

/*

AkBankID bankID = 0;

			AK::SoundEngine::LoadBank(bankName.c_str(), [](
				AkUInt32 in_bankID,
				const void* in_pInMemoryBankPtr,
				AKRESULT in_eLoadResult,
				void* in_pCookie
				)->void {
					return;
				}, nullptr, bankID);

*/