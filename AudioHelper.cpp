// This is the main DLL file.

#include "stdafx.h"

#include "EnumerateDevices.h"
#include "AudioHelper.h"

namespace AudioHelper {

    DeviceChooser::DeviceChooser()
    {}

    cli::array<DeviceDescription^> ^DeviceChooser::GetDescriptions(bool pb)
    {
        EnumerateDevices ed(pb);
        std::vector<EnumerateDevices::DeviceDescription> devices;
        try
        {
            devices = ed.GetDescriptions();
        }
        catch (const std::exception &e)
        {
            throw gcnew System::Exception(gcnew System::String(e.what()));
        }
        cli::array<DeviceDescription^> ^ret = gcnew cli::array<DeviceDescription^>(devices.size());
        for (unsigned i = 0; i < devices.size(); i++)
        {
            DeviceDescription ^d = gcnew DeviceDescription();
            d->guid = gcnew System::String(devices[i].m_id.c_str());
            d->id = gcnew System::String(devices[i].m_user.c_str());
            d->channels = devices[i].m_sharedSampleChannels;
            d->rate = devices[i].m_sharedSampleRate;
            ret[i] = d;
        }
        return ret;
    }
    cli::array<DeviceDescription^> ^DeviceChooser::GetRecorderDescriptions()
    {
        return GetDescriptions(false);
    }
    cli::array<DeviceDescription^> ^DeviceChooser::GetDescriptions()
    {
        return GetDescriptions(true);
    }

    DeviceDescription ^DeviceChooser::GetDefaultDevice(bool pb)
    {
        EnumerateDevices ed(pb);
        DeviceDescription ^d = gcnew DeviceDescription();
        auto dd = ed.GetDefault();
        d->guid = gcnew System::String(dd.m_id.c_str());
        d->id = gcnew System::String(dd.m_user.c_str());
        d->channels = dd.m_sharedSampleChannels;
        d->rate = dd.m_sharedSampleRate;
        return d;
    }

}