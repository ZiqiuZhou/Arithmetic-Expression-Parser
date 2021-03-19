#pragma once

#ifndef INCLUDED_CPP_PRACTICE_EX08_FILE_IO_HPP_
#define INCLUDED_CPP_PRACTICE_EX08_FILE_IO_HPP_


#include <span>
#include <cstdio>      // for FILE etc.
#include <memory>      // for unique_ptr<>
#include <cstddef>     // for byte
#include <utility>     // for move()
#include <filesystem>  // for path



namespace asc::cpp_practice_ws20::ex08 {


    // General helper functions and types
    enum class FileMode { binary, text };


    struct FileReadResult
    {
        std::ptrdiff_t numBytesRead;
        bool endOfFile;
    };


    namespace detail {


        enum class FileAccessMode { read, write };

        bool
            readFile(FileReadResult& result, std::FILE* file, std::span<std::byte> buffer);

        bool
            writeFile(std::FILE* file, std::span<std::byte const> buffer);


        struct FileDeleter
        {
            void
                operator ()(std::FILE* file) const noexcept
            {
                // This function is called by the destructor of `std::unique_ptr<>`, so we ignore any errors returned by
                // `std::fclose()`.
                std::fclose(file);
            }
        };

        // Define a "file handle" as a `unique_ptr<>` with a custom deleter: lifetime management is done as usual, but when
        // the resource is to be freed, `unique_ptr<>` will pass the file pointer to an instaance of our deleter rather than
        // using a `delete` expression.
        using FileHandle = std::unique_ptr<std::FILE, FileDeleter>;


        // `openFile()` and `closeFile()` implement the logic shared by `FileReader` and `FileWriter`.

        FileHandle
            openFile(std::filesystem::path const& path, FileAccessMode accessMode, FileMode contentMode);

        void
            closeFile(FileHandle file);


    } // namespace detail


    class FileReader
    {
    private:
        detail::FileHandle file_;

        FileReader(std::filesystem::path const& path, FileMode mode)
            : file_(detail::openFile(path, detail::FileAccessMode::read, mode))
        {
        }

    public:
        static FileReader
            open(std::filesystem::path const& path, FileMode mode)
        {
            return { path, mode };
        }

        [[nodiscard]] FileReadResult
            readTo(std::span<std::byte> buffer)
        {
            FileReadResult result;
            if (!detail::readFile(result, file_.get(), buffer))
            {
                // Move the handle to a local variable which will be destroyed upon leaving the scope (i.e. close the handle).
                [[maybe_unused]] auto _ = std::move(file_);
                throw std::runtime_error("failed to read from file");
            }
            return result;
        }

        void
            close()
        {
            detail::closeFile(std::move(file_));
        }
    };

    class FileWriter
    {
    private:
        detail::FileHandle file_;

        FileWriter(std::filesystem::path const& path, FileMode mode)
            : file_(detail::openFile(path, detail::FileAccessMode::write, mode))
        {
        }

    public:
        static FileWriter
            open(std::filesystem::path const& path, FileMode mode)
        {
            return { path, mode };
        }

        void
            write(std::span<std::byte const> buffer)
        {
            if (!detail::writeFile(file_.get(), buffer))
            {
                // Move the handle to a local variable which will be destroyed upon leaving the scope (i.e. close the handle).
                [[maybe_unused]] auto _ = std::move(file_);
                throw std::runtime_error("failed to read from file");
            }
        }

        void
            close()
        {
            detail::closeFile(std::move(file_));
        }
    };


} // namespace asc::cpp_practice_ws20::ex08


#endif // INCLUDED_CPP_PRACTICE_EX08_FILE_IO_HPP_
