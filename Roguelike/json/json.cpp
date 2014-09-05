/*********************************
 * json.cpp
 * Connor Hilarides
 * Created 2014/05/30
 *********************************/

#include "json.h"

#include <sstream>
#include <cassert>
#include <iomanip>
#include <codecvt>
#include <locale>

namespace json
{

// ----------------------------------------------------------------------------

  #pragma region Basics

  template <typename T>
  inline void construct(void *mem, const void *copy)
  {
    if (copy) new (mem) T(*reinterpret_cast<const T *>(copy));
    else new (mem) T();
  }

// ----------------------------------------------------------------------------

  template <typename T, typename U>
  inline void construct(void *mem, const void *copy, const U& def)
  {
    if (copy) new (mem) T(*reinterpret_cast<const T *>(copy));
    else new (mem) T(def);
  }

// ----------------------------------------------------------------------------

  void value::shared_value::set_type(json_type new_type, const void *copy)
  {
    // destruct types
    switch (type)
    {
      case json_type::jobject:
        data.as<object_t>()->~object_t();
        break;

      case json_type::jarray:
        data.as<array_t>()->~array_t();
        break;

      case json_type::jstring:
        data.as<string_t>()->~string_t();
        break;
    }

    // construct types
    type = new_type;
    switch (type)
    {
      case json_type::jnull:
        construct<null_t>(&data, copy, nullptr);
        break;

      case json_type::jobject:
        construct<object_t>(&data, copy);
        break;

      case json_type::jarray:
        construct<array_t>(&data, copy);
        break;

      case json_type::jstring:
        construct<string_t>(&data, copy, "");
        break;

      case json_type::jnumber:
        construct<number_t>(&data, copy, 0.0l);
        break;

      case json_type::jbool:
        construct<bool_t>(&data, copy, false);
        break;
    }
  }

// ----------------------------------------------------------------------------

  std::shared_ptr<value::shared_value> value::make_object()
  {
    return std::make_shared<shared_value>();
  }

// ----------------------------------------------------------------------------

  value value::null()
  {
    return value{make_object()};
  }

// ----------------------------------------------------------------------------

  value value::number(long double number)
  {
    value jnum{make_object()};
    jnum.data->set_type(json_type::jnumber, &number);

    return jnum;
  }

// ----------------------------------------------------------------------------

  value value::boolean(bool boolean)
  {
    value jbool{make_object()};
    jbool.data->set_type(json_type::jbool, &boolean);

    return jbool;
  }

// ----------------------------------------------------------------------------

  value value::string(const std::string& str)
  {
    value jstring{make_object()};
    jstring.data->set_type(json_type::jstring, &str);

    return jstring;
  }

// ----------------------------------------------------------------------------

  template <typename MT>
  void value::load_object_from_pairs(value& jobject, const MT& pairs)
  {
    // fill up the map with the pairs
    auto& map = *jobject.data->data.as<object_t>();
    map.reserve(pairs.size());
    for (auto& pair : pairs)
      map[pair.first] = pair.second;
  }

// ----------------------------------------------------------------------------

  value value::object(const object_t& pairs)
  {
    value jobject{make_object()};
    jobject.data->set_type(json_type::jobject, &pairs);
    return jobject;
  }

// ----------------------------------------------------------------------------

  template <typename MT>
  void value::load_array_from_values(value& jarray, const MT& values)
  {
    // fill up the vector with the items
    auto& vector = *jarray.data->data.as<array_t>();
    vector.reserve(values.size());

    for (auto& item : values)
      vector.emplace_back(item);
  }

// ----------------------------------------------------------------------------

  value value::array(const array_t& values)
  {
    value jarray{make_object()};
    jarray.data->set_type(json_type::jarray, &values);

    return jarray;
  }

// ----------------------------------------------------------------------------

  void value::set_null(nullptr_t)
  {
    data->set_type(json_type::jnull, nullptr);
  }

// ----------------------------------------------------------------------------

  void value::set_object(const object_t& object)
  {
    data->set_type(json_type::jobject, &object);
  }

// ----------------------------------------------------------------------------

  void value::set_array(const array_t& array)
  {
    data->set_type(json_type::jarray, &array);
  }

// ----------------------------------------------------------------------------

  void value::set_string(const string_t& string)
  {
    data->set_type(json_type::jstring, &string);
  }

// ----------------------------------------------------------------------------

  void value::set_number(number_t number)
  {
    data->set_type(json_type::jnumber, &number);
  }

// ----------------------------------------------------------------------------

  void value::set_bool(bool_t value)
  {
    data->set_type(json_type::jbool, &value);
  }

// ----------------------------------------------------------------------------

  #pragma endregion

// ----------------------------------------------------------------------------
  
  #pragma region Convert From

  bool value::is(json_type type)
  {
    return data->type == type;
  }

// ----------------------------------------------------------------------------

  value::object_t& value::as_object()
  {
    assert(is(json_type::jobject));
    return *data->data.as<object_t>();
  }

// ----------------------------------------------------------------------------

  value::array_t& value::as_array()
  {
    assert(is(json_type::jarray));
    return *data->data.as<array_t>();
  }

// ----------------------------------------------------------------------------

  value::string_t& value::as_string()
  {
    assert(is(json_type::jstring));
    return *data->data.as<string_t>();
  }

// ----------------------------------------------------------------------------

  value::number_t value::as_number()
  {
    assert(is(json_type::jnumber));
    return *data->data.as<number_t>();
  }

// ----------------------------------------------------------------------------

  value::bool_t value::as_bool()
  {
    assert(is(json_type::jbool));
    return *data->data.as<bool_t>();
  }

// ----------------------------------------------------------------------------
  
  #pragma region is_object_of

  static bool all_of_object_are(value& value, json_type type)
  {
    if (!value.is(json_type::jobject))
      return false;

    for (auto& pair : value.as_object())
    {
      if (!pair.second.is(type))
        return false;
    }

    return true;
  }

  template <>
  bool value::is_object_of<value::null_t>()
  {
    return all_of_object_are(*this, json_type::jnull);
  }

  template <>
  bool value::is_object_of<value::object_t>()
  {
    return all_of_object_are(*this, json_type::jobject);
  }

  template <>
  bool value::is_object_of<value::array_t>()
  {
    return all_of_object_are(*this, json_type::jarray);
  }

  template <>
  bool value::is_object_of<value::string_t>()
  {
    return all_of_object_are(*this, json_type::jstring);
  }

  template <>
  bool value::is_object_of<value::number_t>()
  {
    return all_of_object_are(*this, json_type::jnumber);
  }

  template <>
  bool value::is_object_of<value::bool_t>()
  {
    return all_of_object_are(*this, json_type::jbool);
  }

  #pragma endregion

// ----------------------------------------------------------------------------

  #pragma region as_object_of

  template <typename T, typename F>
  value::object_of_t<T> all_of_object_as(value& object, F convert)
  {
    value::object_of_t<T> map;

    for (auto& pair : object.as_object())
    {
      map[pair.first] = convert(pair.second);
    }

    return map;
  }

  #define LAMBDA_VALUE_CONVERT(fn) [](value& v){ return v.fn(); }

  template <>
  value::object_of_t<value::object_t> value::as_object_of<value::object_t>()
  {
    return all_of_object_as<object_t>(*this, LAMBDA_VALUE_CONVERT(as_object));
  }

  template <>
  value::object_of_t<value::array_t> value::as_object_of<value::array_t>()
  {
    return all_of_object_as<array_t>(*this, LAMBDA_VALUE_CONVERT(as_array));
  }

  template <>
  value::object_of_t<value::string_t> value::as_object_of<value::string_t>()
  {
    return all_of_object_as<string_t>(*this, LAMBDA_VALUE_CONVERT(as_string));
  }

  template <>
  value::object_of_t<value::number_t> value::as_object_of<value::number_t>()
  {
    return all_of_object_as<number_t>(*this, LAMBDA_VALUE_CONVERT(as_number));
  }

  template <>
  value::object_of_t<value::bool_t> value::as_object_of<value::bool_t>()
  {
    return all_of_object_as<bool_t>(*this, LAMBDA_VALUE_CONVERT(as_bool));
  }

  #pragma endregion

// ----------------------------------------------------------------------------

  #pragma region is_array_of

  static bool all_of_array_are(value& value, json_type type)
  {
    if (!value.is(json_type::jarray))
      return false;

    for (auto& value : value.as_array())
    {
      if (!value.is(type))
        return false;
    }

    return true;
  }

  template <>
  bool value::is_array_of<value::null_t>()
  {
    return all_of_array_are(*this, json_type::jnull);
  }

  template <>
  bool value::is_array_of<value::object_t>()
  {
    return all_of_array_are(*this, json_type::jobject);
  }

  template <>
  bool value::is_array_of<value::array_t>()
  {
    return all_of_array_are(*this, json_type::jarray);
  }

  template <>
  bool value::is_array_of<value::string_t>()
  {
    return all_of_array_are(*this, json_type::jstring);
  }

  template <>
  bool value::is_array_of<value::number_t>()
  {
    return all_of_array_are(*this, json_type::jnumber);
  }

  template <>
  bool value::is_array_of<value::bool_t>()
  {
    return all_of_array_are(*this, json_type::jbool);
  }

  #pragma endregion

// ----------------------------------------------------------------------------

  #pragma region as_array_of

  template <typename T, typename F>
  value::array_of_t<T> all_of_array_as(value& object, F convert)
  {
    value::array_of_t<T> vector;

    for (auto& value : object.as_array())
    {
      vector.push_back(convert(value));
    }

    return vector;
  }

  template <>
  value::array_of_t<value::object_t> value::as_array_of<value::object_t>()
  {
    return all_of_array_as<object_t>(*this, LAMBDA_VALUE_CONVERT(as_object));
  }

  template <>
  value::array_of_t<value::array_t> value::as_array_of<value::array_t>()
  {
    return all_of_array_as<array_t>(*this, LAMBDA_VALUE_CONVERT(as_array));
  }

  template <>
  value::array_of_t<value::string_t> value::as_array_of<value::string_t>()
  {
    return all_of_array_as<string_t>(*this, LAMBDA_VALUE_CONVERT(as_string));
  }

  template <>
  value::array_of_t<value::number_t> value::as_array_of<value::number_t>()
  {
    return all_of_array_as<number_t>(*this, LAMBDA_VALUE_CONVERT(as_number));
  }

  template <>
  value::array_of_t<value::bool_t> value::as_array_of<value::bool_t>()
  {
    return all_of_array_as<bool_t>(*this, LAMBDA_VALUE_CONVERT(as_bool));
  }

  #pragma endregion

// ----------------------------------------------------------------------------

  #pragma endregion

// ----------------------------------------------------------------------------
  
  #pragma region Parse

  #define PARSE_ERROR(msg) throw json_syntax_exception(msg, input.tellg())

  value value::parse(const std::string& str_input)
  {
    std::istringstream stream{str_input};
    return parse(stream);
  }

// ----------------------------------------------------------------------------

  value value::parse(std::istream& input)
  {
    skip_ws_and_comments(input);

    switch (input.peek())
    {
      case '{':
        return parse_object(input);

      case '[':
        return parse_array(input);

      case '"':
        return parse_string(input);

      default:
        return parse_basic(input);
    }
  }

// ----------------------------------------------------------------------------

  value value::parse_object(std::istream& input)
  {
    skip_ws_and_comments(input);
    if (input.get() != '{')
      PARSE_ERROR("Expected object begin ('{')");

    auto object = value::object();
    auto& map = object.as_object();
    
    skip_ws_and_comments(input);
    if (input.peek() == '}')
      return object;

    for (;;)
    {
      auto key = parse_string(input);
      
      skip_ws_and_comments(input);
      if (input.get() != ':')
        PARSE_ERROR("Expected kvp separator (':')");

      auto value = parse(input);

      map[key.as_string()] = value;

      skip_ws_and_comments(input);
      auto next = input.get();
      if (next == '}')
        return object;
      else if (next != ',')
        PARSE_ERROR("Expected object end or separator ('}' or ',')");
    }
  }

// ----------------------------------------------------------------------------

  value value::parse_array(std::istream& input)
  {
    skip_ws_and_comments(input);
    if (input.get() != '[')
      PARSE_ERROR("Expected array begin ('[')");

    auto array = value::array();
    auto& vector = array.as_array();

    for (;;)
    {
      auto value = value::parse(input);
      vector.push_back(value);

      skip_ws_and_comments(input);
      auto next = input.get();
      if (next == ']')
        return array;
      else if (next != ',')
        PARSE_ERROR("Expected array end or separator (']' or ',')");
    }
  }

// ----------------------------------------------------------------------------

  inline std::string narrow(const std::wstring& wide_string)
  {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wide_string);
  }

// ----------------------------------------------------------------------------

  static bool read_char(std::istream& input, wchar_t& c)
  {
    auto val = input.get();

    if (!input)
      PARSE_ERROR("Unexpected end of stream");

    if (val == '\\')
    {
      val = input.get();

      if (!input)
        PARSE_ERROR("Unexpected end of stream");

      switch (val)
      {
        case '"' : c = '"' ; break;
        case '\\': c = '\\'; break;
        case '/' : c = '/' ; break;
        case 'b' : c = '\b'; break;
        case 'f' : c = '\f'; break;
        case 'n' : c = '\n'; break;
        case 'r' : c = '\r'; break;
        case 't' : c = '\t'; break;
        case 'u' :
          char buf[5] = {0};
          input.read(buf, 4);
          sscanf_s(buf, "%hx", &c);
          break;
      }

      return true;
    }
    if (val == '"')
    {
      return false;
    }
    else
    {
      c = (wchar_t) val;
      return true;
    }
  }

// ----------------------------------------------------------------------------

  value value::parse_string(std::istream& input)
  {
    skip_ws_and_comments(input);
    if (input.get() != '\"')
      PARSE_ERROR("Expected string begin ('\"')");

    std::wstring buffer;
    wchar_t c;

    while (read_char(input, c))
      buffer += c;

    return value::string(narrow(buffer));
  }

// ----------------------------------------------------------------------------

  value value::parse_basic(std::istream& input)
  {
    char buf[8] = {0};
    number_t num;

    auto first = input.peek();
    switch (first)
    {
      case 't':
      case 'T':
        if (!input.read(buf, 4) || _strcmpi(buf, "true"))
          PARSE_ERROR("Expected 'true'");
        return value::boolean(true);

      case 'f':
      case 'F':
        if (!input.read(buf, 5) || _strcmpi(buf, "false"))
          PARSE_ERROR("Expected 'false'");
        return value::boolean(false);

      case 'n':
      case 'N':
        if (!input.read(buf, 4) || _strcmpi(buf, "null"))
          PARSE_ERROR("Expected 'null'");
        return value::null();

      default:
        if (!(input >> num))
          PARSE_ERROR("Expected number");
        return value::number(num);
    }
  }

// ----------------------------------------------------------------------------

  void value::skip_ws_and_comments(std::istream& input)
  {
    input >> std::ws;
    
    if (input.peek() == '/')
    {
      // It's not valid JSON if it's not a comment at this point, 
      // so just make sure the comment is well-formed
      input.get();
      if (input.peek() == '/')
      {
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      }
      else if (input.peek() == '*')
      {
        while (input.peek() != '/')
          input.ignore(std::numeric_limits<std::streamsize>::max(), '*');

        input.get();
      }
      else
      {
        PARSE_ERROR("Expected a comment");
      }

      // Skip any extra whitespace after the comment
      input >> std::ws;
    }
  }

  #pragma endregion

// ----------------------------------------------------------------------------
  
  #pragma region Serialize

  std::string value::serialize()
  {
    std::ostringstream out;
    serialize(out);
    return out.str();
  }

// ----------------------------------------------------------------------------

  struct serialize_method_manager
  {
    typedef void(value::*serialize_fn)(std::ostream& out);
    serialize_fn serialize_fns[(uint64_t) json_type::num_types];

    serialize_fn operator[](json_type type)
    {
      assert((uint64_t)type < (uint64_t)json_type::num_types);
      return serialize_fns[(uint64_t) type];
    }

    serialize_method_manager()
    {
      serialize_fns[(uint64_t) json_type::jnull] = &value::serialize_null;
      serialize_fns[(uint64_t) json_type::jobject] = &value::serialize_object;
      serialize_fns[(uint64_t) json_type::jarray] = &value::serialize_array;
      serialize_fns[(uint64_t) json_type::jstring] = &value::serialize_string;
      serialize_fns[(uint64_t) json_type::jnumber] = &value::serialize_number;
      serialize_fns[(uint64_t) json_type::jbool] = &value::serialize_bool;
    }
  };

  void value::serialize(std::ostream& out)
  {
    static serialize_method_manager serialize_fns;
    (this ->* serialize_fns[data->type])(out);
  }

// ----------------------------------------------------------------------------

  void value::serialize_null(std::ostream& out)
  {
    out << "null";
  }

// ----------------------------------------------------------------------------

  void value::serialize_object(std::ostream& out)
  {
    out << '{';

    bool first = true;
    for (auto& pair : as_object())
    {
      if (first)
        first = false;
      else
        out << ',';

      value::string(pair.first).serialize_string(out);
      out << ':';
      pair.second.serialize(out);
    }

    out << '}';
  }

// ----------------------------------------------------------------------------

  void value::serialize_array(std::ostream& out)
  {
    out << '[';

    bool first = true;
    for (auto& value : as_array())
    {
      if (first)
        first = false;
      else
        out << ',';

      value.serialize(out);
    }

    out << ']';
  }

// ----------------------------------------------------------------------------

  void write_escaped_char(std::ostream& out, wchar_t c)
  {
    if (c >= ' ' && c <= 127 && c != '"' && c != '\\')
    {
      out << (char) c;
      return;
    }

    switch (c)
    {
      case '"' : out << "\\\""; break;
      case '\\': out << "\\\\"; break;
      case '\r': out << "\\r" ; break;
      case '\n': out << "\\n" ; break;
      case '\t': out << "\\t" ; break;

      default:
        out << "\\u"
            << std::hex << std::setfill('0')
            << std::setw(4) << c
            << std::dec << std::setfill(' ');
        break;
    }
  }

  inline std::wstring widen(const std::string& narrow_string)
  {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(narrow_string.c_str());
  }

  void value::serialize_string(std::ostream& out)
  {
    out << '"';

    for (auto c : widen(as_string()))
      write_escaped_char(out, c);

    out << '"';
  }

// ----------------------------------------------------------------------------

  void value::serialize_number(std::ostream& out)
  {
    out << as_number();
  }

// ----------------------------------------------------------------------------

  void value::serialize_bool(std::ostream& out)
  {
    out << (as_bool() ? "true" : "false");
  }

// ----------------------------------------------------------------------------

  #pragma endregion

// ----------------------------------------------------------------------------

}
