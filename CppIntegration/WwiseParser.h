#pragma once

#ifndef WWISE_PARSER_H
#define WWISE_PARSER_H

#define WWISE_PARSER_CHECK_AVAILABLE if(!m_available) return

#include <algorithm>
#include <string>

#include "WwiseDefine.h"

namespace WwiseIntegration
{
    class WwiseParser
    {
    public:
        void WwiseParser(std::string soundBankPath = WWISE_BASE_PATH)
        {
            m_available = false;
        }
        virtual ~WwiseParser() = default;

    private:
        bool m_available;

    public:
        void GetPlatforms()
        {
            WWISE_PARSER_CHECK_AVAILABLE;


        }
        void GetLanguages();

        void GetBankList();
        void GetEventList();
        void GetSwitchList();
        void GetStatesList();
        void GetRtpcList();
    }
}

#endif