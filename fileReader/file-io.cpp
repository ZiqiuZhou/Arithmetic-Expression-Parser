#pragma warning(disable : 4996)
#include <cassert>
#include <stdexcept>  // for runtime_error

#include "file-io.h"


namespace asc::cpp_practice_ws20::ex08 {

    namespace detail {


        // Checks a POSIX error code.
        static void
            posixCheck(int errorCode)
        {
            if (errorCode != 0)
            {
                throw std::system_error(std::error_code(errorCode, std::generic_category()));
            }
        }


        // Checks the status code of functions that return an error code in `errno`.
        static void
            posixAssert(bool success)
        {
            if (!success)
            {
                posixCheck(errno);
            }
        }


        bool
            readFile(FileReadResult& result, std::FILE* file, std::span<std::byte> buffer)
        {
            std::size_t numElementsRead = std::fread(buffer.data(), sizeof(std::byte), buffer.size(), file);
            result = {
                .numBytesRead = static_cast<std::ptrdiff_t>(numElementsRead),
                .endOfFile = std::feof(file) != 0
            };
            return std::ferror(file) == 0;
        }

        bool
            writeFile(std::FILE* file, std::span<std::byte const> buffer)
        {
            std::size_t numElementsWritten = std::fwrite(buffer.data(), sizeof(std::byte), buffer.size(), file);
            return std::ferror(file) == 0 && numElementsWritten == buffer.size();
        }


        static std::string
            getFileAccessModeString(FileAccessMode accessMode)
        {
            switch (accessMode)
            {
            case FileAccessMode::read: return "r";
            case FileAccessMode::write: return "w";
            }
            std::terminate();
        }
        static std::string
            getFileContentModeString(FileMode contentMode)
        {
            switch (contentMode)
            {
            case FileMode::binary: return "b";
            case FileMode::text: return "";
            }
            std::terminate();
        }
        static std::string
            getModeString(FileAccessMode accessMode, FileMode contentMode)
        {
            return getFileAccessModeString(accessMode) + getFileContentModeString(contentMode);
        }


        FileHandle
            openFile(std::filesystem::path const& path, FileAccessMode accessMode, FileMode contentMode)
        {
            auto modeString = getModeString(accessMode, contentMode);
            std::FILE* file = std::fopen(path.string().c_str(), modeString.c_str());
            posixAssert(file != nullptr);
            return FileHandle(file);
        }

        void
            closeFile(FileHandle file)
        {
            // Make the file handle abandon ownership of the underlying pointer.
            std::FILE* rawFile = file.release();

            // Close the file, throw an exception on error.
            int result = std::fclose(rawFile);
            detail::posixAssert(result == 0);
        }


    } // namespace detail

} // namespace asc::cpp_practice_ws20::ex08
