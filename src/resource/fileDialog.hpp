#pragma once

#include <filesystem>
#include <vector>

class FileDialog
{
    public:
        static std::vector<std::filesystem::path> ShowFileDialog(bool bIsSave);

};