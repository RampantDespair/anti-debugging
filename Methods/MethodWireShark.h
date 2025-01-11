#pragma once

#include <windows.h>
#include <iostream>
#include <winsvc.h>
#include <string>

class DriverDetector {
private:

    SC_HANDLE scManager;

public:

    DriverDetector() {
        scManager = OpenSCManager(
            nullptr,
            nullptr,
            SC_MANAGER_ENUMERATE_SERVICE
        );
    }

    ~DriverDetector() {
        if (scManager) {
            CloseServiceHandle(scManager);
        }
    }

    bool isDriverRunning(const std::string& driverName) const {
        SC_HANDLE serviceHandle = OpenServiceA(
            scManager,
            driverName.c_str(),
            SERVICE_QUERY_STATUS
        );

        if (serviceHandle == nullptr) {
            return false;
        }

        SERVICE_STATUS_PROCESS statusBuffer;
        DWORD bytesNeeded;
        bool isRunning = false;

        if (QueryServiceStatusEx(
            serviceHandle,
            SC_STATUS_PROCESS_INFO,
            reinterpret_cast<LPBYTE>(&statusBuffer),
            sizeof(SERVICE_STATUS_PROCESS),
            &bytesNeeded)) {

            isRunning = (statusBuffer.dwCurrentState == SERVICE_RUNNING);
        }

        CloseServiceHandle(serviceHandle);
        return isRunning;
    }

    bool stopDriver(const std::string& driverName) const {
        SC_HANDLE serviceHandle = OpenServiceA(
            scManager,
            driverName.c_str(),
            SERVICE_STOP
        );

        if (serviceHandle == nullptr) {
            return false;
        }

        SERVICE_STATUS status;
        bool success = ControlService(
            serviceHandle,
            SERVICE_CONTROL_STOP,
            &status
        );

        CloseServiceHandle(serviceHandle);
        return success;
    }
};

// Usage example
bool MethodWireShark() {
    DriverDetector detector;

    // Check for specific driver (In our case WireShark)
    std::string targetDriver = "npcap";

    if (detector.isDriverRunning(targetDriver)) {
        std::cout << "The target driver was found!" << std::endl;

        // Stop the driver * REQUIRES ADMINISTRATOR PRIVILEGES *
        /*
        if (detector.stopDriver(targetDriver)) {
            std::cout << "Stopped driver: " << targetDriver << std::endl;
        }
        else
        {
            std::cerr << "Failed to stop driver: " << targetDriver << std::endl;
        }
        */

        return true;
    }
    else
    {
        std::cout << "The target driver was not found." << std::endl;
        return false;
    }
}
