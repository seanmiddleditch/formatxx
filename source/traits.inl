// formatxx - C++ string formatting library.
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non - commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>
//
// Authors:
//   Sean Middleditch <sean@middleditch.us>

namespace formatxx {
namespace {

template <typename CharT> struct FormatTraits;

template <> struct FormatTraits<char>
{
	static constexpr char cFormatBegin = '{';
	static constexpr char cFormatEnd = '}';
	static constexpr char cFormatSep = ':';

	static constexpr char cPrintfSpec = '%';

	// string_view over string literals is safe on all platforms for which I'm aware
	static constexpr string_view sErrBadFormat{"#BADF", 5};
	static constexpr string_view sErrIncomplete{"#INCL", 5};
	static constexpr string_view sErrOutOfRange{"#RNGE", 5};

	static constexpr string_view sTrue{"true", 4};
	static constexpr string_view sFalse{"false", 5};

	static constexpr string_view sPrintfSpecifiers{"bcsdioxXufFeEaAgGp", 18};
};

constexpr string_view FormatTraits<char>::sErrBadFormat;
constexpr string_view FormatTraits<char>::sErrIncomplete;
constexpr string_view FormatTraits<char>::sErrOutOfRange;
constexpr string_view FormatTraits<char>::sTrue;
constexpr string_view FormatTraits<char>::sFalse;
constexpr string_view FormatTraits<char>::sPrintfSpecifiers;

template <> struct FormatTraits<wchar_t>
{
	static constexpr wchar_t cFormatBegin = L'{';
	static constexpr wchar_t cFormatEnd = L'}';
	static constexpr wchar_t cFormatSep = L':';

	static constexpr wchar_t cPrintfSpec = L'%';

	// string_view over string literals is safe on all platforms for which I'm aware
	static constexpr wstring_view sErrBadFormat{L"#BADF", 5};
	static constexpr wstring_view sErrIncomplete{L"#INCL", 5};
	static constexpr wstring_view sErrOutOfRange{L"#RNGE", 5};

	static constexpr wstring_view sTrue{L"true", 4};
	static constexpr wstring_view sFalse{L"false", 5};

	static constexpr wstring_view sPrintfSpecifiers{L"bcsdioxXufFeEaAgGp", 18};
};

constexpr wstring_view FormatTraits<wchar_t>::sErrBadFormat;
constexpr wstring_view FormatTraits<wchar_t>::sErrIncomplete;
constexpr wstring_view FormatTraits<wchar_t>::sErrOutOfRange;
constexpr wstring_view FormatTraits<wchar_t>::sTrue;
constexpr wstring_view FormatTraits<wchar_t>::sFalse;
constexpr wstring_view FormatTraits<wchar_t>::sPrintfSpecifiers;

} // anonymous namespace
} // namespace formatxx