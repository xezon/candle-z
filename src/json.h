
#pragma once

#pragma warning(push)
#pragma warning(disable : 4189 4100 4459)
#define JSONCONS_HAS_STRING_VIEW
#include <jsoncons/json.hpp>
#include <jsoncons_ext/csv/csv_reader.hpp>
#include <jsoncons_ext/csv/csv_serializer.hpp>
#pragma warning(pop)
#include "types.h"

namespace bot {
namespace json {
using json = jsoncons::basic_json<char, jsoncons::preserve_order_policy, allocator<char>>;
using json_exception = jsoncons::json_exception;
using csv_parameters = jsoncons::csv::basic_csv_parameters<char, allocator>;
} // namespace json
} // namespace bot
