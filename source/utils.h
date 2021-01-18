#ifndef UTILS_H
#define UTILS_H

//========================================================================

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace utils {

//======================================================================
class Buffer
{
public:
    //========================================================================
    Buffer() = default;
    Buffer( const std::string & filename ) { load(filename); }
    Buffer( size_t size, char * bytes = nullptr) { init(size, bytes); }
    virtual ~Buffer() { destroy(); }
    //========================================================================
    // Remove copy semantics.
    Buffer(const Buffer& other) = delete;
    Buffer & operator=(const Buffer& other) = delete;
    //========================================================================
    // Move semantics.
    Buffer(Buffer&& other) noexcept
    {
        move_helper(std::move(other));
    }
    Buffer & operator=(Buffer&& other) noexcept
    {
        destroy();
        move_helper(std::move(other));
        return(*this);
    }
    //========================================================================
    operator char*() const { return buffer; }
    char* data() const { return buffer; }
    size_t size() const { return _size; }
    char & operator[] (size_t index) const { return buffer[index]; }
    //========================================================================
    void init(size_t size, char * bytes = nullptr)
    {
        destroy();
        _size = size;
        if( bytes == nullptr )
            buffer = new char[_size];
        else
            buffer = bytes;
    }
    //========================================================================
    void load( const std::string &filename )
    {
        std::ifstream in(filename, std::ios::in | std::ios::binary);
        if (in)
        {
          in.seekg(0, std::ios::end);
          init(in.tellg());
          in.seekg(0, std::ios::beg);
          in.read(buffer, _size);
          in.close();
          return;
        }
        throw(errno);
    }
    //========================================================================

private:
    //========================================================================
    char * buffer { nullptr};
    size_t _size {0};
    //========================================================================
    void move_helper(Buffer && other) noexcept
    {
        _size = other._size;
        buffer = other.buffer;
        other.buffer = nullptr;
    }
    //========================================================================
    void destroy()
    {
        if(buffer) delete[] buffer;
        _size = 0;
    }
    //========================================================================
};

//======================================================================

using path = std::filesystem::path;

//======================================================================
class Resource
{
    std::filesystem::path resource_folder;
public:
    //======================================================================
    Resource();
    //======================================================================
    Buffer load( const std::string & filename );
    //======================================================================
    const std::filesystem::path &get_path() { return resource_folder; }
    //======================================================================
private:
    //======================================================================
    static std::filesystem::path get_exe_path();
    static std::filesystem::path find_resource_path(
            const std::filesystem::path &check_dir);
};

//======================================================================
// Singleton resource manager for the whole program
extern Resource RM;

//======================================================================
} // End of namespace utils.

#endif // UTILS_H
