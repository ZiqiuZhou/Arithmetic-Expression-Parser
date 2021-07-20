#pragma once
#ifndef ARITHMETIC_EXPRESSION_PARSER_LINES_HPP
#define ARITHMETIC_EXPRESSION_PARSER_LINES_HPP

#include <span>
#include <string_view>
#include <string>
#include <iterator>
#include <algorithm>
#include <iostream>

#include "file-io.h"  // for FileReader

using arithmetic_expression_::file_reader::FileReader;

namespace arithmetic_expression_::read_lines {
	
	class LineRange
	{
	private:
		FileReader fileReader_;

		std::string readBuf_;

		std::size_t lineStart_ = 0;
		std::size_t pos_ = 0;
		std::size_t end_ = 0;
		bool endOfFile_ = false;

		std::string_view nextLine_;

		bool tryReadNextLine();

	public:
		LineRange(FileReader&& _fileReader);

		class iterator
		{
			friend LineRange;

		private:
			LineRange* lineRange_;

			void tryReadNextLine()
			{
				if (!lineRange_->tryReadNextLine())
				{
					lineRange_ = nullptr;
				}
			}

			explicit iterator(LineRange& lineRange)
				: lineRange_(&lineRange)
			{
				tryReadNextLine();
			}

			iterator() noexcept
				: lineRange_(nullptr)
			{
			}

		public:
			using value_type = std::string_view;
			using difference_type = std::ptrdiff_t;
			using reference = std::string_view;
			using pointer = std::string_view*;
			using iterator_category = std::input_iterator_tag;
			using iterator_concept = std::input_iterator_tag;

			friend bool operator ==(iterator lhs, iterator rhs)
			{
				return lhs.lineRange_ == rhs.lineRange_;
			}

			friend bool operator !=(iterator lhs, iterator rhs)
			{
				return !(lhs == rhs);
			}

			reference operator *() const
			{
				if (lineRange_ != nullptr)
				{
					return lineRange_->nextLine_;
				}
			}

			pointer operator ->() const
			{
				return &lineRange_->nextLine_;
			}
			iterator& operator ++()
			{
				tryReadNextLine();
				return *this;
			}
			iterator operator ++(int)
			{
				// For strict input iterators there is no difference between prefix and postfix increment.
				return ++ * this;
			}
		};

		iterator begin()
		{
			return iterator{ *this };
		}

		iterator end()
		{
			return iterator{ };
		}
	};

	inline LineRange linesInFile(FileReader&& fileReader)
	{
		return { std::move(fileReader) };
	}
}

#endif // !ARITHMETIC_EXPRESSION_PARSER_LINES_HPP
