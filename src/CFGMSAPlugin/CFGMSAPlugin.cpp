#include "pch.h"
#include "CFGMSAPlugin.h"
#include <wrl.h>
#include <string>
#include <wil\resource.h>
#include "CCGPlugin_h.h"
#include <windows.h>
#include <fstream>
#include <iostream>

using namespace std;
using namespace Microsoft::WRL;

//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//                     )
STDAPI_(BOOL) DllMain(_In_opt_ HINSTANCE hinst, DWORD ul_reason_for_call, _In_opt_ void*)
{
    std::ofstream myfile;
    myfile.open("c:\\dllmain.txt");
    myfile << "Hello, world!\n";

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        myfile << "DLL_PROCESS_ATTACH.\n";
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        myfile << "DLL_PROCESS_DETACH.\n";
        break;
    }

    myfile.close();
    return TRUE;
}

// UUID generated by the developer
//[uuid("b1874be0-e29b-4deb-9209-4340edefa288")]
class CCGPlugin : public RuntimeClass<RuntimeClassFlags<RuntimeClassType::ClassicCom>, ICcgDomainAuthCredentials>
{
public:
    CCGPlugin() {}

    ~CCGPlugin() {}

    IFACEMETHODIMP GetPasswordCredentials(
        _In_ LPCWSTR pluginInput,
        _Outptr_ LPWSTR* domainName,
        _Outptr_ LPWSTR* username,
        _Outptr_ LPWSTR* password)
    {
        wofstream myfile;
        myfile.open("c:\\plugin.txt");
        myfile << "Hello, world!\n";
      
        std::wstring domainParsed, userParsed, passwordParsed;
        try
        {
            if (domainName == NULL || username == NULL || password == NULL)
            {
                return STG_E_INVALIDPARAMETER;
            }
            *domainName = NULL;
            *username = NULL;
            *password = NULL;
            wstring pluginInputString(pluginInput);
            
            myfile << "Plugin input string: " << pluginInputString << "\n";
            if (count(pluginInputString.begin(), pluginInputString.end(), ':') < 2)
            {
                return CO_E_NOT_SUPPORTED;
            }
            // Extract creds of this format Domain:Username:Password
            size_t sep1 = pluginInputString.find(L":");
            size_t sep2 = pluginInputString.find(L":", sep1 + 1);
            domainParsed = pluginInputString.substr(0, sep1);
            userParsed = pluginInputString.substr(sep1 + 1, sep2 - sep1 - 1);
            passwordParsed = pluginInputString.substr(sep2 + 1);
        }
        catch (...)
        {
            return EVENT_E_INTERNALERROR;
        }

        auto userCo = wil::make_cotaskmem_string_nothrow(userParsed.c_str());
        auto passwordCo = wil::make_cotaskmem_string_nothrow(passwordParsed.c_str());
        auto domainCo = wil::make_cotaskmem_string_nothrow(domainParsed.c_str());
        if (userCo == nullptr || passwordCo == nullptr || domainCo == nullptr)
        {
            return STG_E_INSUFFICIENTMEMORY;
        }
        myfile << "Final result:\t" << userParsed << "\t" << passwordParsed << "\t" << domainParsed << "\n";

        *domainName = domainCo.release();
        *username = userCo.release();
        *password = passwordCo.release();

        myfile << "Returning OK!\n";
        myfile.close();
        return S_OK;
    }
};
CoCreatableClass(CCGPlugin);