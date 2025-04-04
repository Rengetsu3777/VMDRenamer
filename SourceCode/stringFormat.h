#pragma once

#include <string>
#include <cstdio>
#include <stdexcept>
#include <memory>

/////////////////////////////
// stringFormat.h: Loggerクラスでログを生成する時に、書式指定子を使用してログを出力するために用意。以下のサイトのコードをそのまま使用。
// リファレンス： an-embedded-engineer様, C++でログ出力, https://an-embedded-engineer.hateblo.jp/entry/2020/08/29/230546, 2025年4月アクセス
/////////////////////////////

namespace detail
{
#if 1
    /* C++ 17版 */
    /* std::string型をconst char*に変換し、それ以外はそのまま出力 */
    template<typename T>
    auto Convert(T&& value)
    {
        /* std::string型をconst char*に変換 */
        if constexpr (std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>::value)
        {
            return std::forward<T>(value).c_str();
        }
        /* std::string型以外は、そのまま出力 */
        else
        {
            return std::forward<T>(value);
        }
    }
#else
    /* C++ 11/14版 */
    /* std::string型をconst char*に変換 */
    template<typename T, typename std::enable_if<std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>::value>::type* = nullptr>
    auto Convert(T&& value)
    {
        return std::forward<T>(value).c_str();
    }
    /* std::string型以外は、そのまま出力 */
    template<typename T, typename std::enable_if<!std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>::value>::type* = nullptr>
    auto Convert(T&& value)
    {
        return std::forward<T>(value);
    }
#endif
    /* 文字列のフォーマッティング(内部処理) */
    template<typename ... Args>
    std::string StringFormatInternal(const std::string& format, Args&& ... args)
    {
        /* フォーマット後の文字数を算出 */
        int str_len = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args) ...);
        /* フォーマット失敗 */
        if (str_len < 0)
        {
            std::runtime_error("String Formatting Error");
        }
        else
        {
            /* Nothing to do */
        }
        /* バッファサイズを算出(文字列長 + null文字サイズ) */
        size_t buffer_size = str_len + sizeof(char);
        /* バッファサイズ分メモリ確保 */
        std::unique_ptr<char[]> buffer(new char[buffer_size]);
        /* 文字列のフォーマット */
        std::snprintf(buffer.get(), buffer_size, format.c_str(), args ...);
        /* 文字列をstd::string型に変換して出力 */
        return std::string(buffer.get(), buffer.get() + str_len);
    }
}
/* 文字列のフォーマッティング */
template<typename ... Args>
std::string StringFormat(const std::string& format, Args&& ... args)
{
    /* 各パラメータの型を変換して、文字列のフォーマッティング */
    return detail::StringFormatInternal(format, detail::Convert(std::forward<Args>(args)) ...);
}