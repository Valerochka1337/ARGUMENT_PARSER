#include "ArgParser.h"
#include <utility>
#include <cstring>

ArgumentParser::ArgParser::ArgParser(std::string str) {
  name = std::move(str);
}
bool ArgumentParser::ArgParser::Parse(int argc, char** argv) {
  std::vector<std::string> str_vec(argc);
  for (int i = 0; i < argc; ++i) {
    str_vec[i] = argv[i];
  }
  bool t = Parse(str_vec);
  return t;
}
bool ArgumentParser::ArgParser::Parse(std::vector<std::string> str_vec) {
  unsigned int size = str_vec.size();

  for (int i = 1; i < size; i++) {
    if (str_vec[i][0] == '-') {
      if (str_vec[i][1] == '-') {
        int j = 2;
        while (j < str_vec[i].size() && str_vec[i][j] != '=') {
          j++;
        }
        std::string arg_name = str_vec[i].substr(2, j - 2);
        std::string argument;
        if (str_vec[i].size() > j) {
          argument = str_vec[i].substr(j + 1);
        } else {
          argument = "";
        }

        unsigned int size_arguments = string_arguments.size();
        bool t = true;
        for (int k = 0; k < size_arguments; ++k) {
          if (strcmp(string_arguments[k]->long_name.c_str(), arg_name.c_str()) == 0) {
            string_arguments[k]->string_value = argument;
            t = false;
            break;
          }
        }

        size_arguments = integer_arguments.size();
        if (t) {
          for (int k = 0; k < size_arguments; ++k) {
            if (strcmp(integer_arguments[k]->long_name.c_str(), arg_name.c_str()) == 0) {
              integer_arguments[k]->not_initialized = false;
              if (integer_arguments[k]->multi_value) {
                integer_arguments[k]->mult_int_val.push_back(std::stoi(argument));
              } else {
                integer_arguments[k]->int_value = std::stoi(argument);
              }
              t = false;
              break;
            }
          }
        }

        size_arguments = boolean_arguments.size();
        if (t) {
          for (int k = 0; k < size_arguments; ++k) {
            if (strcmp(boolean_arguments[k]->long_name.c_str(), arg_name.c_str()) == 0) {
              boolean_arguments[k]->bool_value = true;
              break;
            }

          }
        }
      } else {
        if (str_vec[i][2] == '=') {
          char flag = str_vec[i][1];
          std::string argument = str_vec[i].substr(3);

          unsigned int flag_vec_size = string_arguments.size();
          for (int j = 0; j < flag_vec_size; ++j) {
            if (string_arguments[j]->short_name == flag) {
              string_arguments[j]->string_value = argument;
            }
          }
        } else {
          int flag_index = 1;
          while (flag_index < str_vec[i].size()) {
            char flag = str_vec[i][flag_index];
            unsigned int flag_vec_size = boolean_arguments.size();
            for (int j = 0; j < flag_vec_size; ++j) {
              if (boolean_arguments[j]->short_name == flag) {
                boolean_arguments[j]->bool_value = true;
                break;
              }
            }
            flag_index++;
          }
        }

      }
    } else {
      unsigned int size_vec = string_arguments.size();
      for (int j = 0; j < size_vec; ++i) {
        if (string_arguments[j]->positional_argument) {
          if (string_arguments[j]->multi_value) {
            string_arguments[j]->mult_string_val.push_back(str_vec[i]);
          } else {
            string_arguments[j]->string_value = str_vec[i];
          }
          break;
        }
      }
      size_vec = integer_arguments.size();
      for (int j = 0; j < size_vec; ++i) {
        if (integer_arguments[j]->positional_argument) {
          integer_arguments[j]->not_initialized = false;
          if (integer_arguments[j]->multi_value) {
            integer_arguments[j]->mult_int_val.push_back(std::stoi(str_vec[i]));
          } else {
            integer_arguments[j]->int_value = std::stoi(str_vec[i]);
          }
          break;
        }
      }

    }
  }
  unsigned int size_vec = boolean_arguments.size();
  for (int i = 0; i < size_vec; ++i) {
    if (boolean_arguments[i]->is_help && boolean_arguments[i]->bool_value) {
      return true;
    }
    if (boolean_arguments[i]->storing) {
      *boolean_arguments[i]->stored_value = boolean_arguments[i]->bool_value;
    }
  }

  size_vec = string_arguments.size();
  for (int i = 0; i < size_vec; ++i) {
    if (string_arguments[i]->string_value.empty()) {
      return false;
    } else if (string_arguments[i]->storing) {
      *string_arguments[i]->stored_value = string_arguments[i]->string_value;
    }
  }
  size_vec = integer_arguments.size();
  for (int i = 0; i < size_vec; ++i) {
    if (integer_arguments[i]->multi_value) {
      if ((int) integer_arguments[i]->mult_int_val.size() < integer_arguments[i]->min_args) {
        return false;
      } else if (integer_arguments[i]->storing) {
        *integer_arguments[i]->mult_stored_value = integer_arguments[i]->mult_int_val;
      }
    } else {
      if (integer_arguments[i]->not_initialized) {
        return false;
      } else if (integer_arguments[i]->storing) {
        *integer_arguments[i]->stored_value = integer_arguments[i]->int_value;
      }
    }
  }

  return true;
}
std::string ArgumentParser::ArgParser::GetStringValue(const std::string& arg_name) {
  unsigned int size = string_arguments.size();
  for (int i = 0; i < size; i++) {
    if (strcmp(arg_name.c_str(), string_arguments[i]->long_name.c_str()) == 0) {
      return string_arguments[i]->string_value;
    }
  }
  return "";
}
std::string ArgumentParser::ArgParser::GetStringValue(const std::string& arg_name, unsigned int index) {
  unsigned int size = string_arguments.size();
  for (int i = 0; i < size; i++) {
    if (strcmp(arg_name.c_str(), string_arguments[i]->long_name.c_str()) == 0) {
      return string_arguments[i]->mult_string_val[index];
    }
  }
  return "";
}

ArgumentParser::StringArgument& ArgumentParser::ArgParser::AddStringArgument(char c,
                                                                             const std::string& arg_name,
                                                                             const std::string& discription) {
  auto* arg = new StringArgument();
  arg->short_name = c;
  arg->long_name = arg_name;
  arg->description = discription;

  return *arg;
}
ArgumentParser::StringArgument& ArgumentParser::ArgParser::AddStringArgument(char c, const std::string& arg_name) {
  auto* arg = new StringArgument();
  arg->short_name = c;
  arg->long_name = arg_name;
  string_arguments.push_back(arg);
  return *arg;
}
ArgumentParser::StringArgument& ArgumentParser::ArgParser::AddStringArgument(const std::string& arg_name) {
  auto* arg = new StringArgument();
  arg->long_name = arg_name;
  string_arguments.push_back(arg);
  return *arg;
}
ArgumentParser::StringArgument& ArgumentParser::ArgParser::AddStringArgument(char c) {
  auto* arg = new StringArgument();
  arg->short_name = c;
  string_arguments.push_back(arg);
  return *arg;
}
ArgumentParser::IntArgument& ArgumentParser::ArgParser::AddIntArgument(const char& c) {
  auto* arg = new IntArgument();
  arg->short_name = c;
  integer_arguments.push_back(arg);
  return *arg;
}
ArgumentParser::IntArgument& ArgumentParser::ArgParser::AddIntArgument(const std::string& string) {
  auto* arg = new IntArgument();
  arg->long_name = string;
  integer_arguments.push_back(arg);
  return *arg;
}
ArgumentParser::IntArgument& ArgumentParser::ArgParser::AddIntArgument(const char& c, const std::string& string) {
  auto* arg = new IntArgument();
  arg->short_name = c;
  arg->long_name = string;
  integer_arguments.push_back(arg);
  return *arg;
}
ArgumentParser::IntArgument& ArgumentParser::ArgParser::AddIntArgument(const std::string& string,
                                                                       const std::string& description) {
  auto* arg = new IntArgument();
  arg->long_name = string;
  arg->description = description;
  integer_arguments.push_back(arg);
  return *arg;
}
int ArgumentParser::ArgParser::GetIntValue(const char* string) {
  unsigned int size = integer_arguments.size();
  for (int i = 0; i < size; i++) {
    if (strcmp(string, integer_arguments[i]->long_name.c_str()) == 0) {
      return integer_arguments[i]->int_value;
    }
  }
  return 0;
}
int ArgumentParser::ArgParser::GetIntValue(const char* string, int index) {
  unsigned int size = integer_arguments.size();
  for (int i = 0; i < size; i++) {
    if (strcmp(string, integer_arguments[i]->long_name.c_str()) == 0) {
      return integer_arguments[i]->mult_int_val[index];
    }
  }
  return 0;
}
ArgumentParser::BoolArgument& ArgumentParser::ArgParser::AddFlag(char i, const char* string) {
  auto* arg = new BoolArgument();
  arg->short_name = i;
  arg->long_name = string;
  boolean_arguments.push_back(arg);
  return *arg;
}
ArgumentParser::BoolArgument& ArgumentParser::ArgParser::AddFlag(const char* string, const char* description) {
  auto* arg = new BoolArgument();
  arg->long_name = string;
  arg->description = description;
  boolean_arguments.push_back(arg);
  return *arg;
}
ArgumentParser::BoolArgument& ArgumentParser::ArgParser::AddFlag(char i, const char* string, const char* description) {
  auto* arg = new BoolArgument();
  arg->short_name = i;
  arg->long_name = string;
  arg->description = description;
  boolean_arguments.push_back(arg);
  return *arg;
}
bool ArgumentParser::ArgParser::GetFlag(const char* string) {
  unsigned int size = boolean_arguments.size();
  for (int i = 0; i < size; i++) {
    if (strcmp(string, boolean_arguments[i]->long_name.c_str()) == 0) {
      return boolean_arguments[i]->bool_value;
    }
  }
  return false;
}
ArgumentParser::BoolArgument& ArgumentParser::ArgParser::AddHelp(char c,
                                                                 const char* arg_name,
                                                                 const char* description) {
  auto* arg = new BoolArgument();
  arg->short_name = c;
  arg->long_name = arg_name;
  arg->description = description;
  arg->is_help = true;

  boolean_arguments.push_back(arg);

  return *arg;
}
bool ArgumentParser::ArgParser::Help() {
  unsigned int size = boolean_arguments.size();
  for (int i = 0; i < size; ++i) {
    if (boolean_arguments[i]->is_help && boolean_arguments[i]->bool_value) {
      return true;
    }
  }
  return false;
}
std::string ArgumentParser::ArgParser::HelpDescription() {
  std::string description;
  description = name + "\n";
  description += "Some Description about program\n\n";
  unsigned int size = string_arguments.size();
  for (int i = 0; i < size; ++i) {
    std::string str(1, string_arguments[i]->short_name);
    description += "-" + str;
    description += ", --" + string_arguments[i]->long_name;
    description += string_arguments[i]->description;
    if (string_arguments[i]->multi_value) {
      description += "[";
      description += "repeated ";
      description += "min_args=" + std::to_string(string_arguments[i]->min_args);
      description += "]";
    }
    description += "\n";
  }

  size = boolean_arguments.size();
  for (int i = 0; i < size; ++i) {
    if (boolean_arguments[i]->short_name != ' ') {
      std::string str(1, boolean_arguments[i]->short_name);
      description += "-" + str;
      description += ",";
    }
    description += " --" + boolean_arguments[i]->long_name;
    description += ", " + boolean_arguments[i]->description;
    if (boolean_arguments[i]->multi_value) {
      description += "[";
      description += "repeated, ";
      description += "min_args=" + std::to_string(boolean_arguments[i]->min_args);
      description += "]";
    }
    description += "\n";
  }

  size = integer_arguments.size();
  for (int i = 0; i < size; ++i) {
    if (integer_arguments[i]->short_name != ' ') {
      std::string str(1, integer_arguments[i]->short_name);
      description += str;
      description += "-" + str;
      description += ",";
    }
    description += " --" + integer_arguments[i]->long_name;
    description += "=<int>";
    description += ", " + integer_arguments[i]->description;
    if (integer_arguments[i]->multi_value) {
      description += "[";
      description += "repeated, ";
      description += "min_args=" + std::to_string(integer_arguments[i]->min_args);
      description += "]";
    }
    description += "\n";
  }

  return description;
}
