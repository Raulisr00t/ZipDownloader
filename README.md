# ZipDownloader
## Overview
ZipDownloader is a C++ console application that demonstrates how to download a ZIP file from a server, extract its contents, execute a specific executable from the extracted files, and clean up the downloaded and extracted files afterward.

## Features
File Download: Uses WinHTTP to download a ZIP file from a specified URL.
ZIP Extraction: Extracts the contents of the ZIP file using PowerShell's Expand-Archive command.
File Execution: Executes a specified executable (Ransomware.exe) from the extracted files.
Clean-Up: Deletes the downloaded ZIP file and all extracted files after execution.

## Usage
Download and Extract: The application will automatically download the ZIP file from the provided URL and extract it to a specified folder.
Execute: The application waits for user input before executing the specified executable from the extracted files.
Clean-Up: After execution, the application deletes the downloaded ZIP file and the extracted files to clean up the environment.

### Requirements
Windows Operating System
PowerShell (for ZIP extraction)
Internet access to download the ZIP file from the specified URL

### Compilation
Use a C++ compiler that supports Windows API, such as MSVC (Microsoft Visual C++), to compile the program.

### Running the Application
Run the compiled executable.
Ensure that the URL provided in the source code points to a valid ZIP file.

## Security Considerations
This application includes a demonstration of executing a file (Ransomware.exe) after extraction. This is only for educational purposes and should be handled with caution. Running arbitrary executables can be dangerous, especially if they are untrusted.

## License
This project is provided "as-is" for educational purposes. Use at your own risk.
