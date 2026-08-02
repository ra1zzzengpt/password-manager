#pragma once
#include <string>
#include <expected>
#include <cstdint>
#include <domain/error/error.hpp>
#include <stdexcept>


template<typename T> std::expected<T, err::Error> transform(const std::string& value)
{

    try
    {
        const uint64_t temp = std::stoull(value);
        if (temp > std::numeric_limits<T>::max())
        {
            return std::unexpected(err::Error{err::TransformError::TransformFailed, "numeric_limits error max type >>>"});
        }
        return static_cast<T>(temp);
    }
    catch (const std::out_of_range& e)
    {
        return std::unexpected(err::Error{err::TransformError::TransformFailed, e.what()});
    }
    catch (const std::invalid_argument& e)
    {
        return std::unexpected(err::Error{err::TransformError::TransformFailed, e.what()});
    }
}