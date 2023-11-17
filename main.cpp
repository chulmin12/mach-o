#include <iostream>
#include <fstream>
#include <vector>
#include <mach-o/loader.h>

using namespace std;

vector<int> header_info;

void _mach_header(const mach_header_64& header) {
    std::cout << "Mach-O Header Information:" << std::endl;
    std::cout << "Magic Number: 0x" << std::hex << header.magic << std::dec << std::endl;
    std::cout << "CPU Type: 0x" << std::hex << header.cputype << std::dec << std::endl;
    std::cout << "CPU Subtype: 0x" << std::hex << header.cpusubtype << std::dec << std::endl;
    std::cout << "File Type: 0x" << std::hex << header.filetype << std::dec << std::endl;
    std::cout << "Number of Load Commands: " << header.ncmds << std::endl;
    std::cout << "Size of Load Commands: " << header.sizeofcmds << " bytes" << std::endl;
    std::cout << "Flags: 0x" << std::hex << header.flags << std::dec << std::endl;
    std::cout << "Reserved: 0x" << std::hex << header.reserved << std::dec << std::endl;
}

void _load_command(const std::vector<load_command>& loadCommands) {
    std::cout << "Load Commands:" << std::endl;
    for (const auto& cmd : loadCommands) {
        std::cout << "Load Command: 0x" << std::hex << cmd.cmd << " (Size: " << cmd.cmdsize << " bytes)" << std::dec << std::endl;
    }
}

void _uuid_command(const std::vector<uuid_command>& uuidCommands) {
    std::cout << "UUID Commands:" << std::endl;
    for (const auto& cmd : uuidCommands) {
        std::cout << "Load Command: 0x" << std::hex << cmd.cmd << " (Size: " << cmd.cmdsize << " bytes)" << std::dec << std::endl;
        std::cout << "UUID: ";
        for (int i = 0; i < 16; ++i) {
            std::cout << std::hex << static_cast<int>(cmd.uuid[i]) << " ";
        }
        std::cout << std::dec << std::endl;
    }
}

void _segment_command(const std::vector<segment_command_64>& segmentCommands) {
    std::cout << "Segment Commands:" << std::endl;
    for (const auto& cmd : segmentCommands) {
        std::cout << "Load Command: 0x" << std::hex << cmd.cmd << " (Size: " << cmd.cmdsize << " bytes)" << std::dec << std::endl;
        std::cout << "Segment Name: " << cmd.segname << std::endl;
        std::cout << "VM Address: 0x" << std::hex << cmd.vmaddr << std::dec << std::endl;
        std::cout << "VM Size: " << cmd.vmsize << " bytes" << std::endl;
        std::cout << "File Offset: " << cmd.fileoff << " bytes" << std::endl;
        std::cout << "File Size: " << cmd.filesize << " bytes" << std::endl;
        std::cout << "Max Protection: 0x" << std::hex << cmd.maxprot << std::dec << std::endl;
        std::cout << "Init Protection: 0x" << std::hex << cmd.initprot << std::dec << std::endl;
        std::cout << "Number of Sections: " << cmd.nsects << std::endl;
        std::cout << "Flags: 0x" << std::hex << cmd.flags << std::dec << std::endl;
    }
}

int main() {
    std::ifstream machoFile("sample.macho", std::ios::binary);
    if (!machoFile.is_open()) {
        std::cerr << "Failed to open Mach-O file." << std::endl;
        return 1;
    }

    mach_header_64 header;
    machoFile.read(reinterpret_cast<char*>(&header), sizeof(header));

    std::vector<load_command> loadCommands;
    loadCommands.reserve(header.ncmds);
    for (uint32_t i = 0; i < header.ncmds; ++i) {
        load_command cmd;
        machoFile.read(reinterpret_cast<char*>(&cmd), sizeof(cmd));
        loadCommands.push_back(cmd);
    }

    std::vector<uuid_command> uuidCommands;
    for (const auto& cmd : loadCommands) {
        if (cmd.cmd == LC_UUID) {
            uuid_command uuidCmd;
            machoFile.read(reinterpret_cast<char*>(&uuidCmd), sizeof(uuidCmd));
            uuidCommands.push_back(uuidCmd);
        }
    }

    std::vector<segment_command_64> segments;
    for (const auto& cmd : loadCommands) {
        if (cmd.cmd == LC_SEGMENT_64) {
            segment_command_64 segment;
            machoFile.read(reinterpret_cast<char*>(&segment), sizeof(segment));
            segments.push_back(segment);
        }
    }

    machoFile.close();

    _mach_header(header);
    _load_command(loadCommands);
    _uuid_command(uuidCommands);
    _segment_command(segments);

    return 0;
}
