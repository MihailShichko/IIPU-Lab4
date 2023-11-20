#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <dshow.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <format>
#include <winsock2.h>
//#include "Client.h"

//using namespace cv;
using namespace std;

//derect show 
//media foundation 

#pragma comment(lib, "strmiids.lib")
//#pragma comment(lib, "ws2_32.lib")


string GetVebCamInfo() {
    string info;
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        throw std::exception("Ошибка инициализации COM-библиотеки.");
    }

    // Создание объекта фильтра
    ICreateDevEnum* pDevEnum = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)&pDevEnum);
    if (FAILED(hr))
    {
        CoUninitialize();
        throw std::exception("Ошибка создания объекта фильтра.");
    }

    // Создание перечислителя видеоустройств
    IEnumMoniker* pEnum = NULL;
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr == S_OK)
    {
        // Перебор доступных видеоустройств
        IMoniker* pMoniker = NULL;
        ULONG cFetched;
        while (pEnum->Next(1, &pMoniker, &cFetched) == S_OK)
        {
            IPropertyBag* pPropBag;
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropBag);
            if (SUCCEEDED(hr))
            {
                // Получение имени устройства
                VARIANT varName;
                VariantInit(&varName);
                hr = pPropBag->Read(L"FriendlyName", &varName, 0);
                if (SUCCEEDED(hr))
                {
                    // Вывод информации о веб-камере
                    //std::wcout << L"Имя устройства: " << varName.bstrVal << std::endl;
                    info = std::format("{} {}", "VebCam name:", varName.bstrVal);
                    VariantClear(&varName);
                }

                pPropBag->Release();
            }

            pMoniker->Release();
        }

        pEnum->Release();
    }

    pDevEnum->Release();

    // Освобождение ресурсов COM
    CoUninitialize();
    return info;
}


int main(int, char**)
{
    WSADATA wsaData;
    //WSAStartup(MAKEWORD(2, 2), &wsaData);
    /*int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return 1;
    }*/

    //Client client;
    //client.TryToConnect("127.0.0.1", 8888);
    cout << "Connected to server" << endl;


    setlocale(LC_ALL, "Russian");
    string info = GetVebCamInfo();
    cout << info << endl;
    cv::Mat frame;
    cv::VideoCapture cap;

    int deviceID = 0; 
    cap.open(deviceID, cv::CAP_ANY);

    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    cout << "Start grabbing" << endl
        << "Press any key to terminate" << endl;
    for (;;)
    {
        cap.read(frame);
        if (frame.empty()) {
            std::cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        imshow("Live", frame);
        if (cv::waitKey(5) >= 0)
            break;
    }

    WSACleanup();
    return 0;
}