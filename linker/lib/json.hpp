#pragma once

using cstring = const char*;

struct string_view{
    cstring data_ptr;
    unsigned int len;
};

struct json{
//..
};

struct json_object{
//..
};

struct json_array{
    json_object* objects;
    unsigned int len;
};

[[noexcept]][[nodiscard]] json parse(cstring path);

[[noexcept]][[nodiscard]] json_object get_object(const json& root,cstring key);

[[noexcept]][[nodiscard]] json_array get_array(const json& root,cstring key);

[[noexcept]][[nodiscard]] string_view get_string(const json& root,cstring key);

[[noexcept]][[nodiscard]] double get_number(const json& root,cstring key);