#include <iostream>
#include <fstream>
#include <vector>
#include <mach-o/loader.h>

void printHeaderInfo(const mach_header_64& header) {
    std::cout << "Mach-O Header Information:" << std::endl;
    std::cout << "Magic Number: 0x" << std::hex << header.magic << std::dec << std::endl;
    std::cout << "CPU Type: 0x" << std::hex << header.cputype << std::dec << std::endl;
    std::cout << "File Type: 0x" << std::hex << header.filetype << std::dec << std::endl;
    std::cout << "Number of Load Commands: " << header.ncmds << std::endl;
    std::cout << "Size of Load Commands: " << header.sizeofcmds << " bytes" << std::endl;
}

void printLoadCommands(const std::vector<load_command>& loadCommands) {
    std::cout << "Load Commands:" << std::endl;
    for (const auto& cmd : loadCommands) {
        std::cout << "Load Command: 0x" << std::hex << cmd.cmd << " (Size: " << cmd.cmdsize << " bytes)" << std::dec << std::endl;
    }
}

void printSegments(const std::vector<segment_command_64>& segments) {
    std::cout << "Segments:" << std::endl;
    for (const auto& segment : segments) {
        std::cout << "Segment Name: " << segment.segname << std::endl;
        std::cout << "Number of Sections: " << segment.nsects << std::endl;
        std::cout << "Segment VM Address: 0x" << std::hex << segment.vmaddr << std::dec << std::endl;
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

    std::vector<segment_command_64> segments;
    for (const auto& cmd : loadCommands) {
        if (cmd.cmd == LC_SEGMENT_64) {
            segment_command_64 segment;
            machoFile.read(reinterpret_cast<char*>(&segment), sizeof(segment));
            segments.push_back(segment);
        }
    }

    machoFile.close();

    printHeaderInfo(header);
    printLoadCommands(loadCommands);
    printSegments(segments);

    return 0;
}
