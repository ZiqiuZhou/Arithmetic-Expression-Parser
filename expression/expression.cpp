#include "expression.h"

namespace asc::cpp_practice_ws20::ex08 {

	namespace expr {
	
        bool operator ==(UnaryFunctionExpression const& lhs, UnaryFunctionExpression const& rhs)
        {
            return lhs.f.index() == rhs.f.index()
                && lhs.x == rhs.x;
        }
        bool operator ==(BinaryFunctionExpression const& lhs, BinaryFunctionExpression const& rhs)
        {
            return lhs.f.index() == rhs.f.index()
                && lhs.x == rhs.x
                && lhs.y == rhs.y;
        }
        bool operator ==(Expression const& lhs, Expression const& rhs)
        {
            return lhs.value() == rhs.value();
        }
	}

}