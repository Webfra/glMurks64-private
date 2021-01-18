//======================================================================
#include "mainwindow.h"
#include "utils.h"
//======================================================================
#include <SDL2/SDL.h>
//======================================================================
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cerrno>

//========================================================================
#if defined(__linux__)
    #include <unistd.h> // For "readlink()" on GNU/linux.
#endif
#if defined(_WIN32)
    #include <libloaderapi.h> // For GetModuleFileName() on Windows
#endif

//======================================================================
namespace utils {
    //======================================================================
    Resource RM; // Singleton resource manager for the whole program
    //======================================================================
    Resource::Resource()
    {
        path exe = Resource::get_exe_path();
        resource_folder = find_resource_path( exe );
        if( get_path() == "" )
        {
            std::cerr << "***ERROR: Can't find resource folder!\n";
            exit(-1);
        }
    }
    //======================================================================
    Buffer Resource::load( const std::string & filename )
    {
        path full_path = resource_folder / filename;
        return Buffer( full_path );
    }
    //======================================================================
    path Resource::get_exe_path()
    {
        char exe_path[65536];
        memset(exe_path, 0, 65536);
    #if defined(__linux__)
        readlink("/proc/self/exe", exe_path, 65535);
    #elif defined(_WIN32)
        (void)::GetModuleFileName(nullptr, exe_path, 65535);
    #else
    #error Operating system is not supported! Must be either Linux or Windows!
    #endif
        std::filesystem::path exe {exe_path};
        return exe.parent_path();
    }
    //======================================================================
    path Resource::find_resource_path(const path &check_dir)
    {
        if( !std::filesystem::exists( check_dir) ) return "";
        if( check_dir == "/" ) return "";
        if( !std::filesystem::is_directory(check_dir) ) return "";

        path test_dir = check_dir / "resource";
        if( std::filesystem::is_directory(test_dir) )
        {
            return test_dir;
        }

        return find_resource_path( check_dir.parent_path() );
    }
}

//======================================================================
int main(int, char**)
{
    // auto chargen { utils::RM.load("roms/chargen") };

    if(SDL_Init(SDL_INIT_VIDEO) >= 0)
    {
        atexit( SDL_Quit );
        auto win { MainWindow() };
        win.loop();
    }
    return 0;
}

//======================================================================
