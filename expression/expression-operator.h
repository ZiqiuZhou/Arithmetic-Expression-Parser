#pragma once

#ifndef INCLUDED_CPP_PRACTICE_EX08_EXPRESSION_OPERATORS_H_
#define INCLUDED_CPP_PRACTICE_EX08_EXPRESSION_OPERATORS_H_


#include <utility>  // for move()

#include "expression.h"


namespace asc::cpp_practice_ws20::ex08 {

    namespace expr {


        static const struct Pi
        {
            operator Expression() const
            {
                return { NamedConstant::pi };
            }
        } pi;
        static const struct E
        {
            operator Expression() const
            {
                return { NamedConstant::e };
            }
        } e;

        [[nodiscard]] inline Expression
            operator +(Expression x)
        {
            return { UnaryFunctionExpression{ Positive{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            operator -(Expression x)
        {
            return { UnaryFunctionExpression{ Negative{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            sqrt(Expression x)
        {
            return { UnaryFunctionExpression{ Sqrt{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            exp(Expression x)
        {
            return { UnaryFunctionExpression{ Exp{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            log(Expression x)
        {
            return { UnaryFunctionExpression{ Log{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            sin(Expression x)
        {
            return { UnaryFunctionExpression{ Sin{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            cos(Expression x)
        {
            return { UnaryFunctionExpression{ Cos{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            tan(Expression x)
        {
            return { UnaryFunctionExpression{ Tan{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            arcsin(Expression x)
        {
            return { UnaryFunctionExpression{ ArcSin{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            arccos(Expression x)
        {
            return { UnaryFunctionExpression{ ArcCos{ }, std::move(x) } };
        }
        [[nodiscard]] inline Expression
            arctan(Expression x)
        {
            return { UnaryFunctionExpression{ ArcTan{ }, std::move(x) } };
        }

        [[nodiscard]] inline Expression
            operator +(Expression lhs, Expression rhs)
        {
            return { BinaryFunctionExpression{ Add{ }, std::move(lhs), std::move(rhs) } };
        }
        [[nodiscard]] inline Expression
            operator -(Expression lhs, Expression rhs)
        {
            return { BinaryFunctionExpression{ Subtract{ }, std::move(lhs), std::move(rhs) } };
        }
        [[nodiscard]] inline Expression
            operator *(Expression lhs, Expression rhs)
        {
            return { BinaryFunctionExpression{ Multiply{ }, std::move(lhs), std::move(rhs) } };
        }
        [[nodiscard]] inline Expression
            operator /(Expression lhs, Expression rhs)
        {
            return { BinaryFunctionExpression{ Divide{ }, std::move(lhs), std::move(rhs) } };
        }
        [[nodiscard]] inline Expression
            pow(Expression base, Expression exp)
        {
            return { BinaryFunctionExpression{ Pow{ }, std::move(base), std::move(exp) } };
        }
        [[nodiscard]] inline Expression
            log(Expression base, Expression exp)
        {
            return { BinaryFunctionExpression{ LogBase{ }, std::move(base), std::move(exp) } };
        }


        inline namespace literals {


            [[nodiscard]] inline Expression
                operator ""_expr(unsigned long long c)
            {
                return { RationalConstant{ static_cast<int>(c) } };
            }
            [[nodiscard]] inline Expression
                operator ""_expr(long double c)
            {
                return { RealConstant{ static_cast<double>(c) } };
            }
            [[nodiscard]] inline Expression
                operator ""_expr(char const* v, std::size_t n)
            {
                return { Variable{ std::string(v, v + n) } };
            }


        } // inline namespace literals

    } // namespace expr

} // namespace asc::cpp_practice_ws20::ex08


#endif // INCLUDED_CPP_PRACTICE_EX08_EXPRESSION_OPERATORS_H_
