#pragma once
#include <sstream>
#include <iostream>

#include "utility.h"     // for overload<>
#include "expression.h"

namespace asc::cpp_practice_ws20::ex08 {

    namespace expr {

        std::ostream& operator <<(std::ostream& stream, RationalConstant c)
        {
            if (c.value.denominator() == 1)
            {
                return stream << c.value.numerator();
            }
            return stream << c.value;
        }

        std::ostream& operator <<(std::ostream& stream, RealConstant c)
        {
            return stream << c.value;
        }

        std::ostream& operator <<(std::ostream& stream, NamedConstant c)
        {
            switch (c)
            {
            case NamedConstant::e:
                return stream << "e";
            case NamedConstant::pi:
                return stream << "pi";
            }
            std::terminate();
        }

        std::ostream& operator <<(std::ostream& stream, Variable const& c)
        {
            return stream << c.name;
        }

        constexpr auto
            unaryFunctionName = overload{
                [](Positive) { return "+";      },
                [](Negative) { return "-";      },
                [](Sqrt) { return "sqrt";   },
                [](Exp) { return "exp";    },
                [](Log) { return "log";    },
                [](Sin) { return "sin";    },
                [](Cos) { return "cos";    },
                [](Tan) { return "tan";    },
                [](ArcSin) { return "arcsin"; },
                [](ArcCos) { return "arccos"; },
                [](ArcTan) { return "arctan"; }
        };

        constexpr auto
            binaryFunctionName = overload{
                [](Add) { return " + "; },
                [](Subtract) { return " - "; },
                [](Multiply) { return "*";   },
                [](Divide) { return "/";   },
                [](Pow) { return "^";   },
                [](LogBase) { return "log"; }
        };

        // 运算符优先级
        enum class OperatorPrecedence : int
        {
            additive = 0,
            additiveUnary = 1,
            multiplicative = 2,
            power = 3,
            none = -1
        };

        // C++17 and earlier: need to define all relational operators
        //constexpr bool operator < (OperatorPrecedence lhs, OperatorPrecedence rhs) { return int(lhs) <  int(rhs); }
        //constexpr bool operator > (OperatorPrecedence lhs, OperatorPrecedence rhs) { return int(lhs) >  int(rhs); }
        //constexpr bool operator <=(OperatorPrecedence lhs, OperatorPrecedence rhs) { return int(lhs) <= int(rhs); }
        //constexpr bool operator >=(OperatorPrecedence lhs, OperatorPrecedence rhs) { return int(lhs) >= int(rhs); }
        // C++20 and later: only need to define "spaceship operator"
        constexpr std::strong_ordering operator <=>(OperatorPrecedence lhs, OperatorPrecedence rhs)
        {
            return int(lhs) <=> int(rhs);
        }

        // 一元运算符优先级
        constexpr auto unaryOperatorPrecedence = overload{
                [](Positive) { return OperatorPrecedence::additiveUnary; },
                [](Negative) { return OperatorPrecedence::additiveUnary; },
                [](auto) { return OperatorPrecedence::none;          }
        };

        // 二元运算符优先级
        constexpr auto binaryOperatorPrecedence = overload{
                [](Add) { return OperatorPrecedence::additive;       },
                [](Subtract) { return OperatorPrecedence::additive;       },
                [](Multiply) { return OperatorPrecedence::multiplicative; },
                [](Divide) { return OperatorPrecedence::multiplicative; },
                [](Pow) { return OperatorPrecedence::power;          },
                [](auto) { return OperatorPrecedence::none;           }
        };

        using BinaryFunctionFlag = unsigned;
        constexpr BinaryFunctionFlag bffNone = 0;
        constexpr BinaryFunctionFlag bffLeftAssociative = 1; // 左关联
        constexpr BinaryFunctionFlag bffRightAssociative = 2; // 右关联
        constexpr BinaryFunctionFlag bffAssociative = 4;

        constexpr auto binaryFunctionFlags = overload{
                [](Add) { return bffLeftAssociative | bffAssociative; },
                [](Subtract) { return bffLeftAssociative;                  },
                [](Multiply) { return bffLeftAssociative | bffAssociative; },
                [](Divide) { return bffLeftAssociative;                  },
                [](Pow) { return bffRightAssociative;                 },
                [](auto) { return bffNone;                             }
        };

        OperatorPrecedence expressionPrecedence(Expression const& e)
        {
            return std::visit(
                overload{
                    [](UnaryFunctionExpression const& unaryExpr) { return std::visit(unaryOperatorPrecedence, unaryExpr.f); },
                    [](BinaryFunctionExpression const& binaryExpr) { return std::visit(binaryOperatorPrecedence, binaryExpr.f); },
                    [](auto const&) { return OperatorPrecedence::none; }
                },
                e.value());
        }

        BinaryFunctionFlag expressionBinaryFunctionFlags(Expression const& e)
        {
            return std::visit(
                overload{
                    [](BinaryFunctionExpression const& binaryExpr) { return std::visit(binaryFunctionFlags, binaryExpr.f); },
                    [](auto const&) { return bffNone; }
                },
                e.value());
        }

        bool needParentheses(
                OperatorPrecedence outerPrecedence, OperatorPrecedence innerPrecedence, bool mustEnforceAssociativity = true)
        {
            return outerPrecedence == OperatorPrecedence::none
                || (innerPrecedence != OperatorPrecedence::none
                    && (outerPrecedence > innerPrecedence
                        || (outerPrecedence == innerPrecedence && mustEnforceAssociativity)));
        }

        bool needUnaryArgParentheses(
                OperatorPrecedence outerPrecedence, Expression const& arg)
        {
            auto innerPrecedence = expressionPrecedence(arg);
            return needParentheses(outerPrecedence, innerPrecedence);
        }

        bool needBinaryArgParentheses(
                OperatorPrecedence outerPrecedence, BinaryFunctionFlag outerFlags,
                Expression const& arg, BinaryFunctionFlag associativityMask)
        {
            auto innerPrecedence = expressionPrecedence(arg);
            auto innerFlags = expressionBinaryFunctionFlags(arg);

            // Note: We print expressions such that they always round-trip correctly, i.e. "a + (b + c)" is serialized with
            // parentheses because "+" is left-associative, and hence "a + b + c", although mathematically equivalent, would be
            // parsed as "(a + b) + c", which is a different expression.
            // If we wanted to omit all mathematically unnecessary parentheses, we could use the following alternative instead:
            //
            //     bool mustEnforceAssociativity = (outerFlags & (associativityMask | bffAssociative)) == 0;
            //
            bool mustEnforceAssociativity = (outerFlags & innerFlags & associativityMask) == 0;

            return needParentheses(outerPrecedence, innerPrecedence, mustEnforceAssociativity);
        }

        std::ostream& operator <<(std::ostream& stream, Expression const& expr)
        {
            auto argToStream = [&stream]
            (Expression const& arg, bool needParens)
            {
                if (needParens)
                {
                    stream << '(';
                }
                stream << arg;
                if (needParens)
                {
                    stream << ')';
                }
            };

            std::visit(overload{
                    [&stream, argToStream]
                    (UnaryFunctionExpression const& unaryExpr)
                    {
                        auto outerPrecedence = std::visit(unaryOperatorPrecedence, unaryExpr.f);
                        stream << std::visit(unaryFunctionName, unaryExpr.f);
                        argToStream(unaryExpr.x, needUnaryArgParentheses(outerPrecedence, unaryExpr.x));
                    },
                    [&stream, argToStream]
                    (BinaryFunctionExpression const& binaryExpr)
                    {
                        auto outerPrecedence = std::visit(binaryOperatorPrecedence, binaryExpr.f);
                        auto outerFlags = std::visit(binaryFunctionFlags, binaryExpr.f);
                        auto functionName = std::visit(binaryFunctionName, binaryExpr.f);

                        bool isInfixOperator = outerPrecedence != OperatorPrecedence::none;
                        if (isInfixOperator)
                        {
                            argToStream(binaryExpr.x, needBinaryArgParentheses(outerPrecedence, outerFlags, binaryExpr.x, bffLeftAssociative));
                            stream << functionName;
                            argToStream(binaryExpr.y, needBinaryArgParentheses(outerPrecedence, outerFlags, binaryExpr.y, bffRightAssociative));
                        }
                        else
                        {
                            stream << functionName
                                   << '('
                                   << binaryExpr.x
                                   << ", "
                                   << binaryExpr.y
                                   << ')';
                        }
                    },
                    [&stream]
                    (auto cv)
                    {
                        stream << cv;
                    },
                },
                expr.value());

            return stream;
        }

        std::string
            to_string(Expression const& expr)
        {
            std::stringstream sstr;

            // Set up the stream to throw exceptions in case of failure because this is what our caller expects.
            sstr.exceptions(std::ios_base::badbit | std::ios_base::failbit);

            sstr << expr;
            return sstr.str();
        }
    }
}