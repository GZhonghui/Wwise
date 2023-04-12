#pragma once

#ifndef WWISE_MANAGER_H
#define WWISE_MANAGER_H

#include <algorithm>
#include <cstdint>
#include <string>

#include <unordered_map>
#include <unordered_set>

#include <AK/SoundEngine/Common/AkMemoryMgr.h>
#include <AK/SoundEngine/Common/IAkStreamMgr.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>

#include "Wwise/IO/Win32/AkFilePackageLowLevelIOBlocking.h"

namespace WwiseIntegration
{
	class WwiseManager
	{
	public:
		static WwiseManager* GetInstance()
		{
			static WwiseManager instance;
			return &instance;
		}

	private:
		WwiseManager() :m_inited(false) {}
		WwiseManager(const WwiseManager&) = delete;
		WwiseManager& operator=(const WwiseManager&) = delete;

		virtual ~WwiseManager() = default;

	private:
		bool m_inited;
		void Init()
		{
			if (m_inited) return;
			m_inited = true;

			m_bankReference.clear();
			m_playingRecord.clear();
		}

	private:
		std::unordered_map<std::string, uint32_t> m_bankReference;
		std::unordered_map<AkGameObjectID, std::unordered_map<AkPlayingID, std::string>> m_playingRecord;

	public:
		void InitEngine();
		void TermEngine();
		void TickEngine(float deltaTime);

	public:
		void Post(AkGameObjectID gameObjectId, std::string eventName);

		void Stop(AkGameObjectID gameObjectId, std::string eventName);
		void Stop(AkGameObjectID gameObjectId, AkPlayingID playingId);
		void StopAll(AkGameObjectID gameObjectId);

		void LoadBank(std::string bankName);
		void ReleaseBank(std::string bankName);
	};
}

#endif // WWISE_MANAGER_H