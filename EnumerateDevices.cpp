#include "EnumerateDevices.h"

#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <MMSystem.h>
#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <atlbase.h>
#include <stdexcept>
#include <functional>

class EnumerateDevicesImpl {
public:
    EnumerateDevicesImpl(bool pb)
        : m_Playback(pb)
    {}

    std::vector<EnumerateDevices::DeviceDescription> GetDescriptions()
    {
        CComPtr<IMMDeviceEnumerator> pEnum;
        pEnum.CoCreateInstance(__uuidof(MMDeviceEnumerator));
        CComPtr<IMMDeviceCollection> pEndpointCollection;
        std::vector<EnumerateDevices::DeviceDescription> ret;
        HRESULT hr = pEnum->EnumAudioEndpoints(m_Playback ? eRender : eCapture, DEVICE_STATE_ACTIVE, &pEndpointCollection);
        if (FAILED(hr))
            throw std::runtime_error(m_Playback ? "Failed to enumerate render endpoints" : "Failed to enumerate capture endpoints");

        UINT numRender=0;
        if (!SUCCEEDED(hr) || 
            (hr = pEndpointCollection->GetCount(&numRender), 
            (!SUCCEEDED(hr) || (numRender == 0))))
        {
            throw std::runtime_error(m_Playback ? "There are no active playback endpoints on the system" : "There are no active capture endpoints on the system");
        }

        for (UINT i = 0; i < numRender; i++ )
        {
            CComPtr<IMMDevice> pDevice;
            HRESULT hr = pEndpointCollection->Item(i, &pDevice);
            if (SUCCEEDED(hr))
            {
                processDevice(pDevice, [&ret](LPWSTR devId, LPWSTR name, DWORD nSamplesPerSec, WORD nChannels)
                {
                    ret.push_back(
                        EnumerateDevices::DeviceDescription(devId, name,
                            nSamplesPerSec, nChannels));
                });
            }
        }

        return ret;
    }

    typedef std::function<void(LPWSTR,LPWSTR,DWORD, WORD )> DeviceFcn_t;

    bool processDevice(IMMDevice *pDevice, const DeviceFcn_t &fcn)
    {
        CComPtr<IPropertyStore> pStore;
        HRESULT hr = pDevice->OpenPropertyStore(STGM_READ , &pStore);
        if (SUCCEEDED(hr))
        {
            LPWSTR devId;
            if (!SUCCEEDED(pDevice->GetId(&devId)))
                return false;

            // Initialize container for property value.
            PROPVARIANT varName;
            PropVariantInit(&varName);

            // Get the endpoint's friendly-name property.
            hr = pStore->GetValue(PKEY_Device_FriendlyName, &varName);

            if (SUCCEEDED(hr))
            {
                CComPtr<IAudioClient> pAudioClient;
                HRESULT hr = pDevice->Activate(
                    __uuidof(pAudioClient), CLSCTX_ALL,
                    NULL, (void**)&pAudioClient);
                WAVEFORMATEX *pWf;
                if (SUCCEEDED(pAudioClient->GetMixFormat(&pWf)))
                {
                    fcn(devId, varName.pwszVal,
                            pWf->nSamplesPerSec, pWf->nChannels);
                    ::CoTaskMemFree(pWf);
                }
            }
            PropVariantClear(&varName);
            ::CoTaskMemFree(devId);
            return true;
        }
        return false;
    }
    
    EnumerateDevices::DeviceDescription GetDefault()
    {
        CComPtr<IMMDeviceEnumerator> pEnum;
        pEnum.CoCreateInstance(__uuidof(MMDeviceEnumerator));
        CComPtr<IMMDevice> pEndpoint;
        std::shared_ptr<EnumerateDevices::DeviceDescription> ret;
        HRESULT hr = pEnum->GetDefaultAudioEndpoint(m_Playback ? eRender : eCapture, eCommunications, &pEndpoint);
        if (FAILED(hr))
            throw std::runtime_error(m_Playback ? "Failed to acquire default render" : "Failed to acquire default capture");

        processDevice(pEndpoint, [&ret](LPWSTR devId, LPWSTR name, DWORD nSamplesPerSec, WORD nChannels)
        {
            ret.reset(new EnumerateDevices::DeviceDescription(devId, name,
                    nSamplesPerSec, nChannels));
        });
        return *ret;
    }
    
    const bool m_Playback;

};


EnumerateDevices::EnumerateDevices(bool pb) : m_impl(new EnumerateDevicesImpl(pb))
{
}

EnumerateDevices::~EnumerateDevices()
{
}

std::vector<EnumerateDevices::DeviceDescription> EnumerateDevices::GetDescriptions()
{
    return m_impl->GetDescriptions();
}

EnumerateDevices::DeviceDescription EnumerateDevices::GetDefault()
{
    return m_impl->GetDefault();
}
