#pragma once
#include "pch.h"

#include <functional>
#include <stdexcept>

namespace testUtils
{
    void inline testException(const std::function<void()>& f, const std::string& exceptionText)
    {
        auto test = [](const std::function<void()>& f, const std::string& exceptionText)
        {
            try
            {
                f();
            }
            catch (const std::runtime_error& ex)
            {
                ASSERT_STREQ(ex.what(), exceptionText.c_str());
                throw;
            }
        };
        ASSERT_THROW(test(f, exceptionText), std::runtime_error);
    }

    auto inline toUpper(std::vector<std::string> inp)
    {
        for (auto& word : inp) for (auto& ch : word) ch = std::toupper(ch);
        return inp;
    }
}
