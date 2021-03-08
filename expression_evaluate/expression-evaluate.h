#pragma once
#include <span>
#include <string>
#include <variant>
#include <stdexcept>      // for runtime_error
#include <string_view>
#include <unordered_map>

#include "expression.h"

namespace asc::cpp_practice_ws20::ex08 {
	
	namespace expr {
		
        class UnknownVariableValue : public std::runtime_error
        {
        private:
            std::string variable_;

        public:
            UnknownVariableValue(std::string _variable, std::string const& _msg)
                : std::runtime_error(_msg),
                variable_(std::move(_variable))
            {
            }

            std::string_view
                variable() const noexcept
            {
                return variable_;
            }
        };

        class BroadcastError : public std::runtime_error
        {
        public:
            BroadcastError(std::string const& _msg)
                : std::runtime_error(_msg)
            {
            }
        };

        // Evaluates an expression using the provided variable substitutions.
        // Throws an exception of type `UnknownVariableValue` if no substitution was provided for a variable.
        double evaluate(Expression const& expr, 
            std::unordered_map<std::string, double> const& variableSubsitituions);

        using EvaluationResult = std::variant<double, std::vector<double> > ;
        using VariableSubstitution = std::variant<double, std::span<double const> >;

        // Evaluates an expression using the provided variable substitutions.
        // Supports implicit broadcasting of scalar values.
        // Throws an exception of type `UnknownVariableValue` if no substitution was provided for a variable.
        // Throws an exception of type `BroadcastError` if two operands cannot be broadcast due to mismatching array sizes.
        EvaluationResult evaluate(Expression const& expr, 
            std::unordered_map<std::string, VariableSubstitution> const& variableSubsitituions);


	}
}