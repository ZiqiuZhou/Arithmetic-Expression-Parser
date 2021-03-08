#include <iostream>
#include "boost-spirit-helpers.h"


namespace asc::cpp_practice_ws20::ex08 {

    namespace detail {


        std::size_t
            countCodePoints(Iterator start, Iterator pos)
        {
            std::size_t result = 0;
            while (start != pos)
            {
                ++start;
                ++result;
            }
            return result;
        }

        [[noreturn]] void
            doReportError(std::string_view expr, std::string_view errorAnnotation, std::string_view msg)
        {
            throw std::runtime_error(std::string(expr));
            std::cout << std::endl;
            throw std::runtime_error(std::string(errorAnnotation));
            std::cout << std::endl;
            throw std::runtime_error(std::string(msg));
            std::cout << std::endl;
        }

        [[noreturn]] void
            reportError(std::string_view expr, Iterator where, std::string_view msg)
        {
            std::size_t annotationSize = countGraphemeClusters(Iterator(expr.begin()), Iterator(expr.end()));
            std::size_t annotationPos = countGraphemeClusters(Iterator(expr.begin()), where);
            auto errorAnnotation = std::string(annotationSize + 1, ' ');
            errorAnnotation[annotationPos] = '^';
            doReportError(expr, errorAnnotation, msg);
        }


    } // namespace detail

} // namespace asc::cpp_practice_ws20::ex08
