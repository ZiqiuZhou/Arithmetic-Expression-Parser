
#include <exception>  // for terminate(), set_terminate_handler()

#ifdef _WIN32
# include <Windows.h>  // for Get/SetConsoleCP(), Get/SetConsoleOutputCP()
#endif // _WIN32

#include "utf8-console.h"


namespace asc::cpp_practice_ws20::ex08 {


#ifdef _WIN32
    std::terminate_handler prevTerminateHandler = nullptr;

    static auto& cpRestorer()
    {
        static struct CPRestorer
        {
            UINT prevInputCP = CP_UTF8;
            UINT prevOutputCP = CP_UTF8;

            void restore()
            {
                if (prevInputCP != CP_UTF8)
                {
                    SetConsoleCP(prevInputCP);
                    prevInputCP = CP_UTF8;
                }
                if (prevOutputCP != CP_UTF8)
                {
                    SetConsoleOutputCP(prevOutputCP);
                    prevOutputCP = CP_UTF8;
                }
            }
            ~CPRestorer()
            {
                restore();
            }
        } instance;
        return instance;
    }
    static void utf8ConsoleTerminateHandler()
    {
        cpRestorer().restore();
        if (prevTerminateHandler != nullptr) prevTerminateHandler();
    }

    void enableUTF8Console()
    {
        cpRestorer().prevInputCP = GetConsoleCP();
        cpRestorer().prevOutputCP = GetConsoleOutputCP();
        prevTerminateHandler = std::set_terminate(utf8ConsoleTerminateHandler);
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    }
#else // _WIN32
    void enableUTF8Console()
    {
    }
#endif // _WIN32


} // namespace asc::cpp_practice_ws20::ex08
