// AudioHelper.h

#pragma once
using namespace System;

namespace AudioHelper {

    public ref class DeviceDescription
    {
    public:
        property System::String ^id;
        property System::String ^guid;
        property int rate;
        property int channels;
    };

	public ref class DeviceChooser
	{
    public:
        DeviceChooser();
        cli::array<DeviceDescription^> ^GetDescriptions();
        cli::array<DeviceDescription^> ^GetRecorderDescriptions();
        DeviceDescription ^GetDefaultRecorder() { return GetDefaultDevice(false);}
        DeviceDescription ^GetDefaultPlayback() {return GetDefaultDevice(true);}
    private:
        cli::array<DeviceDescription^> ^GetDescriptions(bool pb);
        DeviceDescription ^GetDefaultDevice(bool pb);
	};
}
