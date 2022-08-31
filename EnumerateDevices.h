#pragma once
#include <vector>
#include <memory>
#include <string>
class EnumerateDevicesImpl;
class EnumerateDevices
{
public:
    EnumerateDevices(bool playback = true);
    ~EnumerateDevices();
    struct DeviceDescription {
        DeviceDescription(const std::wstring &id, const std::wstring &user,
                int rate, int channels) : 
            m_id(id), m_user(user), m_sharedSampleRate(rate), m_sharedSampleChannels(channels)
        {}
        std::wstring m_id;
        std::wstring m_user;
        int m_sharedSampleRate;
        int m_sharedSampleChannels;
    };
    std::vector<DeviceDescription> GetDescriptions();
    DeviceDescription GetDefault();
protected:
    std::shared_ptr<EnumerateDevicesImpl> m_impl;
};
