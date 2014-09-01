/*********************************
 * json.h
 * Connor Hilarides
 * Created 2014/05/30
 *********************************/

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace json
{
  enum class json_type
  {
    jnull = 0,
    jobject,
    jarray,
    jstring,
    jnumber,
    jbool,

    num_types
  };

  class value
  {
      #pragma region Internal data

    public:
      template <typename JT>
      using object_of_t = std::unordered_map<std::string, JT>;

      template <typename JT>
      using array_of_t = std::vector<JT>;

      typedef object_of_t<value> object_t;
      typedef array_of_t<value> array_t;
      typedef std::string string_t;
      typedef long double number_t;
      typedef bool bool_t;
      typedef void *null_t;

    private:
      class shared_value
      {
        typedef unsigned __int8 byte_t;

        union data_t
        {
          byte_t object_data[sizeof(object_t)];
          byte_t array_data[sizeof(object_t)];
          byte_t string_data[sizeof(string_t)];
          byte_t number_data[sizeof(number_t)];
          byte_t bool_data[sizeof(bool_t)];
          byte_t null_data[sizeof(null_t)];

          template <typename T>
          inline T *as() { return reinterpret_cast<T *>(this); }
        };
      
        json_type type = json_type::jnull;
        data_t data;

        void set_type(json_type new_type, const void *copy = nullptr);

        friend class value;
      };

      template <typename MT>
      static void load_object_from_pairs(value& jobject, const MT& pairs);

      template <typename MT>
      static void load_array_from_values(value& jarray, const MT& values);

      std::shared_ptr<shared_value> data;

      explicit value(std::shared_ptr<shared_value> constructed)
        : data(constructed)
      {
      }

      static std::shared_ptr<shared_value> make_object();

      void set_null(nullptr_t);
      void set_object(const object_t& object);
      void set_array(const array_t& array);
      void set_string(const string_t& string);
      void set_number(number_t number);
      void set_bool(bool_t value);

      friend struct serialize_method_manager;

      #pragma endregion

      #pragma region Creators

    public:
      value() : value(null()) {}

      static value null();
      static value number(number_t value = 0);
      static value boolean(bool_t value = false);
      static value string(const string_t& str = "");
      static value object(const object_t& pairs = object_t{});
      static value array(const array_t& values = array_t{});

      #pragma endregion

      #pragma region Convert From

    public:
      bool is(json_type type);

      object_t& as_object();
      array_t& as_array();
      string_t& as_string();
      number_t as_number();
      bool_t as_bool();
    
      template <typename JsonType>
      bool is_object_of();
      template <typename JsonType>
      object_of_t<JsonType> as_object_of();
    
      template <typename JsonType>
      bool is_array_of();
      template <typename JsonType>
      array_of_t<JsonType> as_array_of();

      #pragma endregion

      #pragma region Parse

    public:
      static value parse(const std::string& str_input);
      static value parse(std::istream& input);

    private:
      static value parse_object(std::istream& input);
      static value parse_array(std::istream& input);
      static value parse_string(std::istream& input);
      static value parse_basic(std::istream& input);

      #pragma endregion
  
      #pragma region Serialize

    public:
      std::string serialize();
      void serialize(std::ostream& out);

    private:
      void serialize_null(std::ostream& out);
      void serialize_object(std::ostream& out);
      void serialize_array(std::ostream& out);
      void serialize_string(std::ostream& out);
      void serialize_number(std::ostream& out);
      void serialize_bool(std::ostream& out);

      #pragma endregion
  };

  #pragma region Exceptions

  class json_syntax_exception : public std::exception
  {
  public:
    const std::streampos position;

    json_syntax_exception(const char *message, const std::streampos& pos)
      : std::exception(message), position(pos)
    {
    }

    json_syntax_exception& operator=(const json_syntax_exception&) = delete;
  };

  #pragma endregion

  #pragma region Template Specializations

  #pragma region is_object_of

  template <>
  bool value::is_object_of<value::null_t>();

  template <>
  bool value::is_object_of<value::object_t>();

  template <>
  bool value::is_object_of<value::array_t>();

  template <>
  bool value::is_object_of<value::string_t>();

  template <>
  bool value::is_object_of<value::number_t>();

  template <>
  bool value::is_object_of<value::bool_t>();

  #pragma endregion

  #pragma region as_object_of

  template <>
  value::object_of_t<value::object_t> value::as_object_of<value::object_t>();

  template <>
  value::object_of_t<value::array_t> value::as_object_of<value::array_t>();

  template <>
  value::object_of_t<value::string_t> value::as_object_of<value::string_t>();

  template <>
  value::object_of_t<value::number_t> value::as_object_of<value::number_t>();

  template <>
  value::object_of_t<value::bool_t> value::as_object_of<value::bool_t>();

  #pragma endregion

  #pragma region is_array_of

  template <>
  bool value::is_array_of<value::null_t>();

  template <>
  bool value::is_array_of<value::object_t>();

  template <>
  bool value::is_array_of<value::array_t>();

  template <>
  bool value::is_array_of<value::string_t>();

  template <>
  bool value::is_array_of<value::number_t>();

  template <>
  bool value::is_array_of<value::bool_t>();

  #pragma endregion

  #pragma region as_array_of

  template <>
  value::array_of_t<value::object_t> value::as_array_of<value::object_t>();

  template <>
  value::array_of_t<value::array_t> value::as_array_of<value::array_t>();

  template <>
  value::array_of_t<value::string_t> value::as_array_of<value::string_t>();

  template <>
  value::array_of_t<value::number_t> value::as_array_of<value::number_t>();

  template <>
  value::array_of_t<value::bool_t> value::as_array_of<value::bool_t>();

  #pragma endregion

  #pragma endregion

  #pragma region Template Implementations
    
  template <typename JsonType>
  bool value::is_object_of()
  {
    static_assert(
      false, 
      "An object cannot be converted to a mapping of this type");
  }

  template <typename JsonType>
  value::object_of_t<JsonType> value::as_object_of()
  {
    static_assert(
      false, 
      "An object cannot be converted to a mapping of this type");
  }
    
  template <typename JsonType>
  bool value::is_array_of()
  {
    static_assert(
      false, 
      "An array cannot be converted to a vector of this type");
  }

  template <typename JsonType>
  value::array_of_t<JsonType> value::as_array_of()
  {
    static_assert(
      false, 
      "An array cannot be converted to a vector of this type");
  }

  #pragma endregion

}