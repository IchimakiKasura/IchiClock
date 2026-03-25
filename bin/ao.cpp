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
        for (auto& p : fs::recursive_directory_iterator(current)) {
            if (p.path().filename() == "MeguClock.ino") {
                result = p.path();
                return true;
            }
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

    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    _pclose(pipe);
    return result;
}

fpath getExeDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return fs::path(buffer).parent_path();
}

void help() {
    //cout << "usage s_ao [ option... ]" << endl; // for static libraries
    cout << "usage ao [ option... ]" << endl;
    cout << "-v - verify     -u - upload" << endl;
}

int main(int argc, char* argv[]) {
    string sketch, cmd;
    fpath arduinoCli = getExeDirectory() / "arduino-cli.exe", ino;

    if (argc > 1) {
        sketch = argv[1];
    
        if (!fs::exists(arduinoCli)) {
            cout << "arduino-cli.exe not found in " << arduinoCli.parent_path().string() << "\n"
            << "Please install the following: arduini-cli.exe" << endl;
            return 1;
        }
        
        if (!findSketch(fs::current_path(), ino)) {
            cout << "Sketch not found\n";
            return 1;
        }

        fpath ard_relative = fs::relative(arduinoCli, fs::current_path()),
              ino_relative = fs::relative(ino, fs::current_path());

        if(sketch == "-v") {
            cmd = ard_relative.string() + " compile --fqbn arduino:avr:nano " + ino_relative.string() + " -vvv";
        } else if (sketch == "-u") {

            string rar = ard_relative.string() + " board list";
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
            string verifyCmd = ard_relative.string() + " compile --fqbn arduino:avr:nano " + ino_relative.string() + " -vvv";
            cmd = verifyCmd + " && " + ard_relative.string() + " upload -p " + comport + " --fqbn arduino:avr:nano " + ino_relative.string() + " -vvv";
        } else {
            help();
            return 0;
        }

        system(cmd.c_str());

        return 0;
    }

    help();

    return 0;
}