#include <cmath>

#include "utility.h"
#include "expression-evaluate.h"

namespace asc::cpp_practice_ws20::ex08 {

	namespace expr {
		// unaryFucntion
		double evaluate(Positive, double x) { return x; }
		double evaluate(Negative, double x) { return -x; }
		double evaluate(Sin, double x) { return std::sin(x); }
		double evaluate(Cos, double x) { return std::cos(x); }
		double evaluate(Tan, double x) { return std::tan(x); }
		double evaluate(ArcSin, double x) { return std::asin(x); }
		double evaluate(ArcCos, double x) { return std::acos(x); }
		double evaluate(ArcTan, double x) { return std::atan(x); }
		double evaluate(Exp, double x) { return std::exp(x); }
		double evaluate(Log, double x) { return std::log(x); }
		double evaluate(Sqrt, double x) { return std::sqrt(x); }

		// binaryFucntion
		double evaluate(Add, double lhs, double rhs) { return lhs + rhs; }
		double evaluate(Subtract, double lhs, double rhs) { return lhs - rhs; }
		double evaluate(Multiply, double lhs, double rhs) { return lhs * rhs; }
		double evaluate(Divide, double num, double den) { return num / den; }
		double evaluate(Pow, double base, double exp) { return std::pow(base, exp); }
		double evaluate(LogBase, double base, double val) { return std::log(val) / std::log(base); }

		constexpr double evaluate(RationalConstant c)
		{
			return boost::rational_cast<double>(c.value);
		}

		constexpr double evaluate(RealConstant c)
		{
			return c.value;
		}

		constexpr double evaluate(NamedConstant c)
		{
			switch (c)
			{
			case NamedConstant::e: return 2.71828182845904523536;
			case NamedConstant::pi: return 3.14159265358979323846;
			}
			std::terminate();
		}

		double evaluate(
				Expression const& expr,
				std::unordered_map<std::string, double> const& variableSubsitituions)
		{
			return std::visit(
				overload{
					[&variableSubsitituions]
					(Variable const& v)
					{
						auto it = variableSubsitituions.find(v.name);
						if (it == variableSubsitituions.end())
						{
							throw UnknownVariableValue(v.name, "No value given for variable.");
						}
						return it->second;
					},

					[&variableSubsitituions]
					(UnaryFunctionExpression const& unaryExpr)
					{
						return std::visit(
						[x = evaluate(unaryExpr.x, variableSubsitituions)]
						(auto f)
						{
							return evaluate(f, x);
						},
						unaryExpr.f);
					},

					[&variableSubsitituions]
					(BinaryFunctionExpression const& binaryExpr)
					{
						return std::visit(
							[x = evaluate(binaryExpr.x, variableSubsitituions),
							 y = evaluate(binaryExpr.y, variableSubsitituions)]
							(auto f)
							{
								return evaluate(f, x, y);
							},
							binaryExpr.f);
					},

					[]
					(auto const& c)
					{
						return evaluate(c);
					}
				}, expr.value());
		}


		using IntermediaryResult = std::variant<
			double, // scalar value
			std::span<double const>, // variable value
			std::vector<double> >; // temporary value

		template <typename F>
		IntermediaryResult evaluateUnaryFunction(F f, double x)
		{
			return evaluate(f, x);
		}

		template <typename F>
		IntermediaryResult evaluateUnaryFunction(F f, std::span<double const> x)
		{
			// Allocate a result vector.
			auto result = std::vector<double>(x.size());

			// Plain loops such as this one are trivial to vectorize.
			for (std::size_t i = 0, n = std::ssize(x); i != n; ++i)
			{
				result[i] = evaluate(f, x[i]);
			}
			return result;
		}

		template <typename F>
		IntermediaryResult evaluateUnaryFunction(F f, std::vector<double> x)
		{
			// Reuse vector `x` to save an allocation.
			for (std::size_t i = 0, n = std::ssize(x); i != n; ++i)
			{
				x[i] = evaluate(f, x[i]);
			}
			return x;
		}

		template <typename F>
		IntermediaryResult evaluateBinaryFunction(F f, double x, double y)
		{
			return evaluate(f, x, y);
		}

		template <typename F>
		IntermediaryResult evaluateBinaryFunction(F f, std::span<double const> x, double y)
		{
			// Allocate a result vector. Broadcast the scalar value `y`.
			auto result = std::vector<double>(x.size());
			for (std::size_t i = 0, n = std::ssize(x); i != n; ++i)
			{
				result[i] = evaluate(f, x[i], y);
			}
			return result;
		}

		template <typename F>
		IntermediaryResult evaluateBinaryFunction(F f, double x, std::span<double const> y)
		{
			// Allocate a result vector. Broadcast the scalar value `x`.
			auto result = std::vector<double>(y.size());
			for (std::size_t i = 0, n = std::ssize(y); i != n; ++i)
			{
				result[i] = evaluate(f, x, y[i]);
			}
			return result;
		}

		template <typename F>
		IntermediaryResult evaluateBinaryFunction(F f, std::vector<double> x, double y)
		{
			// Reuse vector `x` to save an allocation. Broadcast the scalar value `y`.
			for (std::size_t i = 0, n = std::ssize(x); i != n; ++i)
			{
				x[i] = evaluate(f, x[i], y);
			}
			return x;
		}

		template <typename F>
		IntermediaryResult evaluateBinaryFunction(F f, double x, std::vector<double> y)
		{
			// Reuse vector `y` to save an allocation. Broadcast the scalar value `x`.
			for (std::size_t i = 0, n = std::ssize(y); i != n; ++i)
			{
				y[i] = evaluate(f, x, y[i]);
			}
			return y;
		}

		template <typename F, typename X, typename Y>
		IntermediaryResult evaluateBinaryFunctionWithBroadcast(F f, X x, Y y)
		{
			if (x.size() != y.size())
			{
				// If this didn't work, at least try to generate a helpful error message.
				throw BroadcastError("operands with different shapes could not be broadcast together");
			}

			// Try to broadcast the result.
			if (x.size() == 1)
			{
				return evaluateBinaryFunction(f, x[0], std::move(y));
			}
			if (y.size() == 1)
			{
				return evaluateBinaryFunction(f, std::move(x), y[0]);
			}
		}

		template <typename F>
		IntermediaryResult evaluateBinaryFunction(F f, std::span<double const> x, std::span<double const> y)
		{
			if (x.size() != y.size())
			{
				// If this didn't work, at least try to generate a helpful error message.
				throw BroadcastError("operands with different shapes could not be broadcast together");
			}

			// Allocate a result vector.
			auto result = std::vector<double>(x.size());
			for (std::size_t i = 0, n = std::ssize(x); i != n; ++i)
			{
				result[i] = evaluate(f, x[i], y[i]);
			}
			return result;
		}

		template <typename F, typename SpanOrVectorT>
		IntermediaryResult evaluateBinaryFunction(F f, std::vector<double> x, SpanOrVectorT y)
		{
			if (x.size() != y.size())
			{
				// If this didn't work, at least try to generate a helpful error message.
				throw BroadcastError("operands with different shapes could not be broadcast together");
			}

			// Reuse vector `x` to save an allocation.
			for (std::size_t i = 0, n = std::ssize(x); i != n; ++i)
			{
				x[i] = evaluate(f, x[i], y[i]);
			}
			return x;
		}

		template <typename F>
		IntermediaryResult evaluateBinaryFunction(F f, std::span<double const> x, std::vector<double> y)
		{
			if (x.size() != y.size())
			{
				// If this didn't work, at least try to generate a helpful error message.
				throw BroadcastError("operands with different shapes could not be broadcast together");
			}

			// Reuse vector `y` to save an allocation.
			for (std::size_t i = 0, n = std::ssize(x); i != n; ++i)
			{
				y[i] = evaluate(f, x[i], y[i]);
			}
			return y;
		}

		// Casts a variant `U = std::variant<Xs...>` to another variant `V = std::variant<Ys...>`, where each `X` in `Xs...` is
		// implicitly convertible to exactly one `Y` in `Ys...`.
		template <typename V, typename U>
		constexpr V
			variant_cast(U&& u)
		{
			return std::visit<V>(
				[]
			<typename T>
				(T && val)
			{
				return std::forward<T>(val);
			},
				std::forward<U>(u));
		}

		IntermediaryResult evaluateImpl(
				Expression const& expression,
				std::unordered_map<std::string, VariableSubstitution> const& variableSubstitutions)
		{
			return std::visit<IntermediaryResult>(
				overload{
					[&variableSubstitutions]
					(Variable const& v)
					{
						auto it = variableSubstitutions.find(v.name);
						if (it == variableSubstitutions.end())
						{
							throw UnknownVariableValue(v.name, "No value given for variable");
						}
						return variant_cast<IntermediaryResult>(it->second);
					},

					[&variableSubstitutions]
					(UnaryFunctionExpression const& unaryExpr)
					{
						auto xV = evaluateImpl(unaryExpr.x, variableSubstitutions);
						return std::visit(
							[](auto f, auto x)
							{
								return evaluateUnaryFunction(f, std::move(x));
							},
							unaryExpr.f, std::move(xV));
					},

					[&variableSubstitutions]
					(BinaryFunctionExpression const& binaryExpr)
					{
						auto xV = evaluateImpl(binaryExpr.x, variableSubstitutions);
						auto yV = evaluateImpl(binaryExpr.y, variableSubstitutions);
						return std::visit(
							[](auto f, auto x, auto y)
							{
								return evaluateBinaryFunction(f, std::move(x), std::move(y));
							},
							binaryExpr.f, std::move(xV), std::move(yV));
					},

					[]
					(auto const& c)
					{
						return evaluate(c);
					},
				},
				expression.value());
		}

		EvaluationResult evaluate(
				Expression const& expression,
				std::unordered_map<std::string, VariableSubstitution> const& variableSubstitutions)
		{
			auto result = evaluateImpl(expression, variableSubstitutions);
			return std::visit<EvaluationResult>(  // we need to be explicit about the return type here
				overload{
					[](double scalar)
					{
						return scalar;
					},
					[](std::span<double const> span)
					{
						return std::vector<double>(span.begin(), span.end());
					},
					[](std::vector<double> vector)
					{
						return vector;
					}
				},
				std::move(result));
		}

	}
}