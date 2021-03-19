#pragma once

#ifndef INCLUDED_CPP_PRACTICE_EX08_LINES_HPP_
#define INCLUDED_CPP_PRACTICE_EX08_LINES_HPP_

#include <span>
#include <string_view>
#include <string>
#include <iterator>
#include <algorithm>
#include <iostream>

#include "file-io.h"  // for FileReader


namespace asc::cpp_practice_ws20::ex08 {


	// TODO: define `copyLines()`
	template<std::output_iterator<std::string_view> OutIt>
	OutIt copyLines(FileReader fileReader, OutIt outIt)
	{
		constexpr std::size_t bufSize = 4096;
		auto readBuf = std::string(bufSize, '\0');

		std::size_t lineStart = 0;
		std::size_t pos = 0;
		std::size_t end = 0;
		bool endOfFile = false;

		while (!(pos == end && endOfFile))
		{
			auto posIt = readBuf.begin() + pos;
			auto endIt = readBuf.begin() + end;
			auto newLinePos = pos + (std::find(posIt, endIt, '\n') - posIt);

			if (newLinePos == end && !endOfFile)
			{
				if (lineStart != 0)
				{
					auto fragmentLength = end - lineStart;
					auto lineStartIt = readBuf.begin() + lineStart;
					std::copy(lineStartIt, endIt, readBuf.begin());
					lineStart = 0;
					pos = fragmentLength;

					if (readBuf.size() > bufSize && fragmentLength <= bufSize)
					{
						readBuf.resize(bufSize);
					}
				}
				else if (end == std::ssize(readBuf))
				{
					readBuf.resize(readBuf.size() + bufSize);
					pos = end;
				}

				auto readSpan = std::span(readBuf.begin() + pos, readBuf.end());
				auto readResult = fileReader.readTo(std::as_writable_bytes(readSpan));
				end = pos + readResult.numBytesRead;
				endOfFile = readResult.endOfFile || readResult.numBytesRead < std::ssize(readSpan);
			}
			else
			{
				auto lineEnd = newLinePos;
				auto lineView = std::string_view(
					readBuf.begin() + lineStart, readBuf.begin() + lineEnd);

				if (lineEnd != end)
				{
					pos = lineEnd + 1;
					lineStart = pos;
				}

				*outIt++ = lineView;
			}
		}

		return outIt;
	}
	// TODO: define `linesInFile()`

	namespace detail {
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
	} // end namespace detail


	inline detail::LineRange linesInFile(FileReader fileReader)
	{
		return { std::move(fileReader) };
	}



} // namespace asc::cpp_practice_ws20::ex08


#endif // INCLUDED_CPP_PRACTICE_EX08_LINES_HPP_

