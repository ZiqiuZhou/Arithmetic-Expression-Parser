#ifndef INCLUDED_CPP_PRACTICE_EX08_EXPRESSION_H_
#define INCLUDED_CPP_PRACTICE_EX08_EXPRESSION_H_

#include <iosfwd>
#include <string>
#include <memory>       // for shared_ptr<>
#include <utility>      // for move()
#include <variant>
#include <string_view>
#include <string>
#include <string_view>

#include <boost/rational.hpp>


namespace asc::cpp_practice_ws20::ex08 {

	namespace expr {
	
		struct Positive { };
		struct Negative { };
		struct Sqrt { };
		struct Exp { };
		struct Log { };
		struct Sin { };
		struct Cos { };
		struct Tan { };
		struct ArcSin { };
		struct ArcCos { };
		struct ArcTan { };
		using UnaryFunction = std::variant<
			Positive,
			Negative,
			Sqrt,
			Exp,
			Log,
			Sin,
			Cos,
			Tan,
			ArcSin,
			ArcCos,
			ArcTan>;
		
		struct Add { };
		struct Subtract { };
		struct Multiply { };
		struct Divide { };
		struct Pow { };
		struct LogBase { };
		using BinaryFunction = std::variant<
			Add,
			Subtract,
			Multiply,
			Divide,
			Pow,
			LogBase>;
		
		struct RationalConstant
		{
			boost::rational<int> value;

			friend bool operator ==(RationalConstant const& lhs, RationalConstant const& rhs)
			{
				return lhs.value == rhs.value;
			}
			friend bool operator !=(RationalConstant const& lhs, RationalConstant const& rhs)
			{
				return !(lhs == rhs);
			}
		};

		struct RealConstant
		{
			double value;

			friend bool operator ==(RealConstant const& lhs, RealConstant const& rhs) 
			{
				return lhs.value == rhs.value; 
			}
			friend bool operator !=(RealConstant const& lhs, RealConstant const& rhs)
			{
				return !(lhs == rhs); 
			}
		};

		enum class NamedConstant
		{
			e,
			pi
		};

		struct Variable
		{
			std::string name;

			friend bool operator ==(Variable const& lhs, Variable const& rhs)
			{
				return lhs.name == rhs.name; 
			}
			friend bool operator !=(Variable const& lhs, Variable const& rhs) 
			{
				return !(lhs == rhs); 
			}
		};

		struct UnaryFunctionExpression;
		struct BinaryFunctionExpression;

		class Expression
		{
		public:
			using RawExpression = std::variant<
				RealConstant,
				RationalConstant,
				NamedConstant,
				Variable,
				UnaryFunctionExpression,
				BinaryFunctionExpression
			>;

		private:
			std::shared_ptr<RawExpression> rawExpression_;

		public:
			Expression(RawExpression _rawExpression);

			RawExpression const& value() const;

			// Parse an expression from a given string.
			// Throws `std::runtime_error` if the argument cannot be parsed.
			// Implemented in "expression-parse.cpp".
			static Expression parse(std::string_view expr);

			friend bool operator ==(Expression const& lhs, Expression const& rhs);
			friend bool operator !=(Expression const& lhs, Expression const& rhs) { return !(lhs == rhs); }

			// Write a string representation of the expression to the stream.
		// Implemented in "expression-print.cpp".
			friend std::ostream& operator <<(std::ostream& stream, Expression const& expr);
		};

		// Return a string representation of the expression.
		// Implemented in "expression-print.cpp".
		std::string to_string(Expression const& expr);

		struct UnaryFunctionExpression
		{
			UnaryFunction f;
			Expression x;

			friend bool operator ==(UnaryFunctionExpression const& lhs, UnaryFunctionExpression const& rhs);
			friend bool operator !=(UnaryFunctionExpression const& lhs, UnaryFunctionExpression const& rhs) { return !(lhs == rhs); }

		};

		struct BinaryFunctionExpression
		{
			BinaryFunction f;
			Expression x;
			Expression y;

			friend bool operator ==(BinaryFunctionExpression const& lhs, BinaryFunctionExpression const& rhs);
			friend bool operator !=(BinaryFunctionExpression const& lhs, BinaryFunctionExpression const& rhs) { return !(lhs == rhs); }
		};

		inline Expression::Expression(RawExpression _rawExpression)
			: rawExpression_(std::make_shared<RawExpression>(std::move(_rawExpression)))
		{
		}

		inline Expression::RawExpression const& Expression::value() const
		{
			return *rawExpression_;
		}
	}
}
#endif