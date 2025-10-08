#pragma once
#ifdef __APPLE__
    #include <mach-o/dyld.h>
    #include <libgen.h>
    #include <string>
    #include <iostream>

    static std::string GetExecutableDir() {
        char path[1024];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) != 0) {
            std::cerr << "Path buffer too small!" << std::endl;
            return "";
        }
        return std::string(dirname(path));  // 실행 파일이 위치한 폴더
    }
#endif