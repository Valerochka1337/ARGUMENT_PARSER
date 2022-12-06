#pragma once
#include <string>
#include <utility>
#include <vector>

namespace ArgumentParser {

struct StringArgument {
  char short_name = ' ';
  std::string long_name = " ";
  std::string description = " ";

  bool multi_value = false;
  bool positional_argument = false;

  int min_args = 1;

  std::string string_value;
  bool storing = false;
  std::string* stored_value = nullptr;

  std::vector<std::string> mult_string_val;
  std::vector<std::string>* mult_stored_val;

  StringArgument& Default(std::string& value) {
    string_value = value;
    return *this;
  }
  StringArgument& Default(const char* value) {
    string_value = value;
    return *this;
  }
  StringArgument& Default(std::vector<std::string> value) {
    mult_string_val = std::move(value);
    return *this;
  }
  StringArgument& Multivalue() {
    multi_value = true;
    return *this;
  }
  StringArgument& StoreValue(std::string& str) {
    storing = true;
    stored_value = &str;
    return *this;
  }
  StringArgument& StoreValues(std::vector<std::string>& str_vec) {
    storing = true;
    mult_stored_val = &str_vec;
    return *this;
  }

  StringArgument& MultiValue() {
    multi_value = true;
    return *this;
  }
  StringArgument& MultiValue(int min) {
    multi_value = true;
    this->min_args = min;
    return *this;
  }
  StringArgument& Positional() {
    positional_argument = true;
    return *this;
  }

};

struct IntArgument {
  char short_name = ' ';
  std::string long_name = " ";
  std::string description = " ";

  bool not_initialized = true;

  bool multi_value = false;
  bool positional_argument = false;

  int min_args = 1;

  int int_value;

  std::vector<int> mult_int_val;

  bool storing = false;
  int* stored_value = nullptr;
  std::vector<int>* mult_stored_value = nullptr;

  IntArgument& Default(int value) {
    not_initialized = false;
    int_value = value;
    return *this;
  }

  IntArgument& Default(std::vector<int> value) {
    not_initialized = false;
    mult_int_val = std::move(value);
    return *this;
  }

  IntArgument& StoreValue(int& val) {
    storing = true;
    stored_value = &val;
    return *this;
  }
  IntArgument& StoreValues(std::vector<int>& val_vec) {
    storing = true;
    mult_stored_value = &val_vec;
    return *this;
  }

  IntArgument& MultiValue() {
    multi_value = true;
    return *this;
  }

  IntArgument& MultiValue(int min) {
    this->min_args = min;
    multi_value = true;
    return *this;
  }
  IntArgument& Positional() {
    positional_argument = true;
    return *this;
  }

};

struct BoolArgument {
  char short_name = ' ';
  std::string long_name = " ";
  std::string description = " ";

  bool multi_value = false;
  bool positional_argument = false;
  bool is_help = false;

  int min_args = -1;

  bool bool_value = false;

  std::vector<bool> mult_bool_val;

  bool storing = false;
  bool* stored_value = nullptr;

  std::vector<bool>* mult_stored_value = nullptr;

  BoolArgument& Default(bool value) {
    bool_value = value;
    return *this;
  }

  BoolArgument& Default(std::vector<bool> value) {
    mult_bool_val = std::move(value);
    return *this;
  }

  BoolArgument& StoreValue(bool& val) {
    storing = true;
    stored_value = &val;
    return *this;
  }
  BoolArgument& StoreValues(std::vector<bool>& val_vec) {
    storing = true;
    mult_stored_value = &val_vec;
    return *this;
  }

  BoolArgument& MultiValue() {
    multi_value = true;
    return *this;
  }

  BoolArgument& MultiValue(int min) {
    min_args = min;
    multi_value = true;
    return *this;
  }

  BoolArgument& Positional() {
    positional_argument = true;
    return *this;
  }

};
class ArgParser {
 private:
  std::vector<StringArgument*> string_arguments;
  std::vector<IntArgument*> integer_arguments;
  std::vector<BoolArgument*> boolean_arguments;
  std::string name;

 public:
  explicit ArgParser(std::string str);
  bool Parse(int argc, char** argv);
  bool Parse(std::vector<std::string> str_vec);

  StringArgument& AddStringArgument(const std::string& arg_name);
  StringArgument& AddStringArgument(char c, const std::string& arg_name);
  StringArgument& AddStringArgument(char c);
  static StringArgument& AddStringArgument(char c, const std::string& arg_name, const std::string& discription);
  std::string GetStringValue(const std::string& arg_name);
  std::string GetStringValue(const std::string& arg_name, unsigned int index);

  IntArgument& AddIntArgument(const std::string& string);
  IntArgument& AddIntArgument(const std::string& string, const std::string& description);
  IntArgument& AddIntArgument(const char& c, const std::string& string);
  IntArgument& AddIntArgument(const char& c);
  int GetIntValue(const char* string);
  int GetIntValue(const char* string, int index);

  BoolArgument& AddFlag(char i, const char* string);
  BoolArgument& AddFlag(const char* string, const char* description);
  BoolArgument& AddFlag(char i, const char* string, const char* description);
  bool GetFlag(const char* string);

  BoolArgument& AddHelp(char i, const char* string, const char* string_1);
  bool Help();
  std::string HelpDescription();
};

} // namespace ArgumentParser