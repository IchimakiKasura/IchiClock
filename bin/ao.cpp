/**************************************************************************
 
    Simple Arduino's Verify and Upload.

    Specifically coded for Win32 only.

    build:
        (if outside)
        - g++ bin/ao.cpp -o bin/ao && g++ bin/ao.cpp -o bin/s_ao -static -DSTATIC
        (if in current folder)
        - g++ ao.cpp -o ao && g++ ao.cpp -o -s_ao -static -DSTATIC

    Author: Ichimaki Kasura

**************************************************************************/
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <windows.h>

using namespace std;
namespace fs = std::filesystem;
#define fpath fs::path

bool findSketch(fpath start, fpath& result) {
    fpath current = start;

    for (int up = 0; up < 3; up++) {
        for (auto& p : fs::recursive_directory_iterator(current))
            if (p.path().filename() == "MeguClock.ino") {
                result = p.path();
                return true;
            }
        if (current.has_parent_path())
            current = current.parent_path();
    }
    return false;
}

string runAndRead(const char* cmd) {
    string result;
    char buffer[256];
    FILE* pipe = _popen(cmd, "r");
    if (!pipe) return "";

    while (fgets(buffer, sizeof(buffer), pipe)) 
        result += buffer;
    _pclose(pipe);
    return result;
}

fpath getExeDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return fs::path(buffer).parent_path();
}

void help() {
    cout << "Customized Arduino Verify/Upload for MeguClock.\n" << endl;
#ifdef STATIC
    cout << "usage s_ao [ option... ]" << endl;
#else
    cout << "usage ao [ option... ]" << endl;
#endif
    cout << "-v - verify     -u - upload" << endl;
}

int main(int argc, char* argv[]) {
    string options, cmd, verifyCmd;
    fpath arduinoCli = getExeDirectory() / "arduino-cli.exe", ino;

    if (argc > 1) {
        options = argv[1];
    
        if (!fs::exists(arduinoCli)) {
            cout << "arduino-cli.exe not found in " << arduinoCli.parent_path().string() << "\n"
            << "Please install the following: arduini-cli.exe" << endl;
            return 1;
        }
        
        if (!findSketch(fs::current_path(), ino)) {
            cout << "Sketch not found\n";
            return 1;
        }

        string ard_relative = fs::relative(arduinoCli, fs::current_path()).string(),
               ino_relative = fs::relative(ino, fs::current_path()).string(),
               ino_lib = ino_relative,
               library_relative = ino_lib.erase(ino_lib.find("MeguClock.ino"), 13),
               buildProperties = "--build-property compiler.c.extra_flags=\"-Os\" "
                                 "--build-property compiler.cpp.extra_flags=\"-Os\" "
                                 "--build-property compiler.c.elf.extra_flags=\"-Wl,--gc-sections\" "
                                 "--build-property compiler.cpp.elf.extra_flags=\"-Wl,--gc-sections\" "
                                 "--build-property compiler.cpp.extra_flags=\"-ffunction-sections -fdata-sections\" "
                                 "--build-property compiler.c.extra_flags=\"-ffunction-sections -fdata-sections\" ";
                                 
        verifyCmd = ard_relative + " compile --fqbn arduino:avr:nano " + ino_relative + " -v --build-property compiler.cpp.extra_flags=\"-std=gnu++17\" " + buildProperties + " --libraries " + library_relative + "libraries";
               
        if(options == "-v")
            cmd = verifyCmd;
        else if (options == "-u") {

            string rar = ard_relative + " board list";
            string boards = runAndRead(rar.c_str());

            if (boards.find("COM") == string::npos) {
                cout << "No board detected. Cancelled.\n";
                return 1;
            }

            string comport;
            size_t pos = boards.find("COM");
            if (pos != string::npos) {
                size_t end = boards.find_first_of(" \n\r", pos);
                comport = boards.substr(pos, end - pos);
            }
            cmd = verifyCmd + " && " + ard_relative + " upload -p " + comport + " --fqbn arduino:avr:nano " + ino_relative + " -v";
        } else {
            help();
            return 0;
        }

        cout << "EXECUTING: " + cmd << endl;
        system(cmd.c_str());

        return 0;
    }

    help();

    return 0;
}