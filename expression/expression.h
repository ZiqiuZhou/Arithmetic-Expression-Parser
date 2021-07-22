#pragma once

#ifndef ARITHMETIC_EXPRESSION_PARSER_EXPRESSION_HPP
#define ARITHMETIC_EXPRESSION_PARSER_EXPRESSION_HPP

#include <iostream>
#include <string>
#include <memory>       // for shared_ptr<>
#include <utility>      // for move()
#include <variant>
#include <string_view>
#include <string>

#include <boost/rational.hpp>

namespace arithmetic_expression_parser::expression {

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

	struct RationalConstant {
		boost::rational<int> value;

		friend bool operator== (const RationalConstant& lhs, const RationalConstant& rhs) {
			return lhs.value == rhs.value;
		}

		friend bool operator!= (const RationalConstant& lhs, const RationalConstant& rhs) {
			return !(lhs.value == rhs.value);
		}
	};

	struct RealConstant {
		double value;

		friend bool operator ==(const RealConstant& lhs, const RealConstant& rhs)
		{
			return lhs.value == rhs.value;
		}
		friend bool operator !=(const RealConstant& lhs, const RealConstant& rhs)
		{
			return !(lhs == rhs);
		}
	};

	enum class NamedConstant { e, pi };

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

	// recursive basic expressions must be forward-declared here.
	struct UnaryFunctionExpression;
	struct BinaryFunctionExpression;

	// Defining `Expression` with `std::variant<>` is challenging because it is a recursive type. Like most problems in
	// computer science, this one can be solved by adding a layer of indirection, in this case a smart pointer.
	class Expression {
	public:
		using RawExpression = std::variant<RealConstant,
										   RationalConstant,
										   NamedConstant,
										   Variable,
										   UnaryFunctionExpression,
										   BinaryFunctionExpression>;
	private:
		std::unique_ptr<RawExpression> rawExpression;

	public:
		Expression(RawExpression _rawExpression)
			: rawExpression(std::make_unique<RawExpression>(std::move(_rawExpression))) {};

		const RawExpression& value() const {
			return *rawExpression;
		}

		// Parse an expression from a given string.
		// Throws `std::runtime_error` if the argument cannot be parsed.
		// Implemented in "expression-parse.cpp".
		static Expression parse(std::string_view expr);

		friend bool operator ==(Expression const& lhs, Expression const& rhs);
		friend bool operator !=(Expression const& lhs, Expression const& rhs) { return !(lhs == rhs); }

		// Write a string representation of the expression to the stream.
		// Implemented in "expression-print.cpp".
		friend std::ostream& operator<< (std::ostream& stream, const Expression& expr);
	};

	// Return a string representation of the expression.
	// Implemented in "expression-print.cpp".
	std::string to_string(const Expression& expr);

	struct UnaryFunctionExpression {
		UnaryFunction f;
		Expression x;

		friend bool operator ==(UnaryFunctionExpression const& lhs, UnaryFunctionExpression const& rhs);
		friend bool operator !=(UnaryFunctionExpression const& lhs, UnaryFunctionExpression const& rhs) { return !(lhs == rhs); }
	};

	struct BinaryFunctionExpression {
		BinaryFunction f;
		Expression x;
		Expression y;
		
		friend bool operator ==(BinaryFunctionExpression const& lhs, BinaryFunctionExpression const& rhs);
		friend bool operator !=(BinaryFunctionExpression const& lhs, BinaryFunctionExpression const& rhs) { return !(lhs == rhs); }
	};
}

#endif // !ARITHMETIC_EXPRESSION_PARSER_EXPRESSION_HPP
