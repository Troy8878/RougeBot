/*********************************
 * RubyFile.cpp
 * Leonardo Saikali
 * Created 2014/08/19
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#include "Common.h"
#include <shlobj.h>

// ----------------------------------------------------------------------------

struct MrbDir
{
  explicit MrbDir(const fs::path &path);

  fs::path path;
  std::vector<fs::path> children;
};

struct MrbFile
{
  fs::path path;
  std::fstream file;
};

// ----------------------------------------------------------------------------

mrb_data_type mrb_dir_dt;
mrb_data_type mrb_file_dt;
mrb_data_type mrb_file_linereader_dt;

static void mrb_file_free(mrb_state *, void *_file)
{
  auto file = static_cast<MrbFile *>(_file);
  if (file->file.is_open())
    file->file.close();

  delete file;
}

static void mrb_file_linereader_free(mrb_state *, void *)
{
}

// ----------------------------------------------------------------------------

static mrb_value mrb_dir_new(mrb_state *mrb, mrb_value self);
static mrb_value mrb_dir_inspect(mrb_state *mrb, mrb_value self);
static mrb_value mrb_dir_path(mrb_state *mrb, mrb_value self);
static mrb_value mrb_dir_child_count(mrb_state *mrb, mrb_value self);
static mrb_value mrb_dir_child_at(mrb_state *mrb, mrb_value self);
static mrb_value mrb_dir_appdata(mrb_state *mrb, mrb_value self);

static mrb_value mrb_file_new(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_path(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_open(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_read(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_write(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_close(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_lines(mrb_state *mrb, mrb_value self);

static mrb_value mrb_file_lines_each(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_file_init(mrb_state *mrb)
{
  ruby::data_type_init<MrbDir>(mrb_dir_dt, ruby::data_scalar_delete<MrbDir>);
  ruby::data_type_init<MrbFile>(mrb_file_dt, mrb_file_free);
  ruby::data_type_init<MrbFile>(mrb_file_linereader_dt, mrb_file_linereader_free);

#pragma region Dir class

  auto dir = mrb_define_class(mrb, "Dir", mrb->object_class);

  mrb_define_class_method(mrb, dir, "new", mrb_dir_new, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, dir, "[]", mrb_dir_new, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, dir, "appdata", mrb_dir_appdata, MRB_ARGS_NONE());

  mrb_define_method(mrb, dir, "inspect", mrb_dir_inspect, MRB_ARGS_NONE());
  mrb_define_method(mrb, dir, "to_s", mrb_dir_path, MRB_ARGS_NONE());
  mrb_define_method(mrb, dir, "path", mrb_dir_path, MRB_ARGS_NONE());
  mrb_define_method(mrb, dir, "length", mrb_dir_child_count, MRB_ARGS_NONE());
  mrb_define_method(mrb, dir, "[]", mrb_dir_child_at, MRB_ARGS_REQ(1));

  mrb_include_module(mrb, dir, mrb_module_get(mrb, "Enumerable"));
  mrb_include_module(mrb, dir, mrb_module_get(mrb, "NativeEnumerable"));

#pragma endregion

#pragma region File class

  auto file = mrb_define_class(mrb, "File", mrb->object_class);

  mrb_define_class_method(mrb, file, "new", mrb_file_new, MRB_ARGS_NONE());

  mrb_define_method(mrb, file, "path", mrb_file_path, MRB_ARGS_NONE());
  mrb_define_method(mrb, file, "open", mrb_file_open, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, file, "read", mrb_file_read, MRB_ARGS_NONE());
  mrb_define_method(mrb, file, "write", mrb_file_write, MRB_ARGS_NONE());
  mrb_define_method(mrb, file, "close", mrb_file_close, MRB_ARGS_NONE());

  mrb_define_method(mrb, file, "lines", mrb_file_lines, MRB_ARGS_NONE());

#pragma endregion

#pragma region File::LineReader class

  auto lines = mrb_define_class_under(mrb, file, "LineReader", mrb->object_class);

  mrb_define_method(mrb, lines, "each", mrb_file_lines_each, MRB_ARGS_BLOCK());

  mrb_funcall(mrb, mrb_obj_value(lines), "include",
              1, mrb_obj_value(mrb_module_get(mrb, "Enumerable")));

#pragma endregion
}

// ----------------------------------------------------------------------------

MrbDir::MrbDir(const fs::path &path)
  : path(path)
{
  for (auto child : fs::directory_contents(path))
  {
    auto filename = child.filename();
    if (filename == L"." || filename == L"..")
      continue;

    children.push_back(narrow(child));
  }
}

// ----------------------------------------------------------------------------

static mrb_value mrb_dir_new_internal(mrb_state *mrb, const fs::path &path)
{
  static auto cls = mrb_class_get(mrb, "Dir");
  auto dir = new MrbDir(path);
  auto obj = mrb_data_object_alloc(mrb, cls, dir, &mrb_dir_dt);
  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_dir_new(mrb_state *mrb, mrb_value)
{
  mrb_value path;
  mrb_get_args(mrb, "S", &path);
  return mrb_dir_new_internal(mrb, mrb_str_to_stdstring(path));
}

mrb_value mrb_dir_inspect(mrb_state* mrb, mrb_value self)
{
  auto &dir = *ruby::data_get<MrbDir>(mrb, self);
  std::ostringstream msg;

  msg << "<#Dir:0x" << self.value.p;
  msg << " path=" << dir.path.directory_string();
  msg << ">";

  auto str = msg.str();
  return mrb_str_new(mrb, str.c_str(), str.size());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_dir_path(mrb_state *mrb, mrb_value self)
{
  auto &dir = *static_cast<MrbDir *>(mrb_data_get_ptr(mrb, self, &mrb_dir_dt));
  std::string path = dir.path;
  return mrb_str_new(mrb, path.c_str(), path.size());
}

// ----------------------------------------------------------------------------

static mrb_value mrb_dir_child_count(mrb_state *mrb, mrb_value self)
{
  auto &dir = *static_cast<MrbDir *>(mrb_data_get_ptr(mrb, self, &mrb_dir_dt));
  return mrb_fixnum_value(static_cast<mrb_int>(dir.children.size()));
}

// ----------------------------------------------------------------------------

static mrb_value mrb_dir_child_at(mrb_state *mrb, mrb_value self)
{
  mrb_value index;
  mrb_get_args(mrb, "o", &index);

  auto &dir = *static_cast<MrbDir *>(mrb_data_get_ptr(mrb, self, &mrb_dir_dt));

  if (!mrb_fixnum_p(index) && !mrb_string_p(index))
  {
    mrb_raise(mrb, E_TYPE_ERROR, "Directory subscript takes Fixnum or String");
  }

  fs::path path;
  if (mrb_fixnum_p(index))
    path = dir.children[assert_limits_mrb<size_t>(mrb, mrb_fixnum(index))];
  else
    path = dir.path / mrb_str_to_stdstring(index);

  auto does_exist = exists(path);
  if (does_exist && !is_directory(path))
  {
    auto file = path.file_string();
    return mrb_str_new(mrb, file.c_str(), file.size());
  }

  if (!does_exist)
  {
    create_directory(path);
  }

  return mrb_dir_new_internal(mrb, path);
}

// ----------------------------------------------------------------------------

mrb_value mrb_dir_appdata(mrb_state* mrb, mrb_value )
{
  wchar_t *path;
  if (FAILED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path)))
  {
    mrb_raise(mrb, E_RUNTIME_ERROR, "Failed to get Local App Data D:");
  }

  mrb_value dir = mrb_dir_new_internal(mrb, narrow(path));
  CoTaskMemFree(path);
  return dir;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_new(mrb_state *mrb, mrb_value)
{
  static auto file_c = mrb_class_get(mrb, "File");

  auto file = new MrbFile;
  auto obj = mrb_data_object_alloc(mrb, file_c, file, &mrb_file_dt);

  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_path(mrb_state *mrb, mrb_value self)
{
  auto &file = *static_cast<MrbFile *>(mrb_data_get_ptr(mrb, self, &mrb_file_dt));
  std::string path = file.path;
  return mrb_str_new(mrb, path.c_str(), path.size());
}

// ----------------------------------------------------------------------------

std::ios::openmode parse_modeopt(mrb_state *mrb, mrb_value opt)
{
  auto name = mrb_sym2name(mrb, mrb_symbol(opt));
  if (strcmp(name, "in") == 0)
    return std::ios::in;
  if (strcmp(name, "out") == 0)
    return std::ios::out;
  if (strcmp(name, "atend") == 0)
    return std::ios::ate;
  if (strcmp(name, "append") == 0)
    return std::ios::app;
  if (strcmp(name, "truncate") == 0)
    return std::ios::trunc;

  mrb_raise(mrb, E_TYPE_ERROR, "Invalid file opt");
}

static mrb_value mrb_file_open(mrb_state *mrb, mrb_value self)
{
  auto &file = *static_cast<MrbFile *>(mrb_data_get_ptr(mrb, self, &mrb_file_dt));

  mrb_value filename_v;
  mrb_value *optv;
  mrb_int optc;
  mrb_get_args(mrb, "S*", &filename_v, &optv, &optc);

  std::ios::openmode mode;
  if (optc)
  {
    mode = 0;
    for (auto &opt : array_iterator(optv, optc))
    {
      mode |= parse_modeopt(mrb, opt);
    }
  }
  else
  {
    mode = std::ios::in | std::ios::out;
  }

  file.path = mrb_str_to_stdstring(filename_v);
  file.file.open(file.path.file_string().c_str(), mode);

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_read(mrb_state *mrb, mrb_value self)
{
  auto &file = *static_cast<MrbFile *>(mrb_data_get_ptr(mrb, self, &mrb_file_dt));

  if (!file.file)
    mrb_raise(mrb, E_RUNTIME_ERROR, "Cannot read from an invalid file");

  std::string line;
  mrb_value str = mrb_str_new(mrb, "", 0);

  file.file.seekg(0);

  for (;;)
  {
    std::getline(file.file, line);
    if (!file.file)
      break;

    mrb_value linestr = mrb_str_new(mrb, line.c_str(), line.size());

    str = mrb_str_append(mrb, str, linestr);
  }

  file.file.seekg(0);

  return str;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_write(mrb_state *mrb, mrb_value self)
{
  auto &file = *static_cast<MrbFile *>(mrb_data_get_ptr(mrb, self, &mrb_file_dt));

  mrb_value data;
  mrb_get_args(mrb, "o", &data);

  if (mrb_string_p(data))
  {
    file.file.write(RSTRING_PTR(data), RSTRING_LEN(data));
  }
  else 
  {
    data = mrb_convert_type(mrb, data, MRB_TT_ARRAY, "Array", "to_ary");
    for (auto value : ruby::array_each(mrb, data))
    {
      value = mrb_convert_type(mrb, value, MRB_TT_FIXNUM, "Fixnum", "to_i");

      auto intval = static_cast<byte>(mrb_fixnum(value));
      file.file.put(reinterpret_cast<const char &>(intval));
    }
  }

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_close(mrb_state *mrb, mrb_value self)
{
  auto &file = *static_cast<MrbFile *>(mrb_data_get_ptr(mrb, self, &mrb_file_dt));

  file.file.close();
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_lines(mrb_state *mrb, mrb_value self)
{
  static auto line_c = mrb_class_get_under(mrb, mrb_class_get(mrb, "File"), "LineReader");

  auto file = static_cast<MrbFile *>(mrb_data_get_ptr(mrb, self, &mrb_file_dt));
  auto obj = mrb_data_object_alloc(mrb, line_c, file, &mrb_file_linereader_dt);

  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_lines_each(mrb_state *mrb, mrb_value self)
{
  mrb_value block;
  mrb_get_args(mrb, "|&", &block);

  if (mrb_nil_p(block))
  {
    static const auto to_enum = mrb_intern_lit(mrb, "to_enum");
    static const auto each = mrb_symbol_value(mrb_intern_lit(mrb, "each"));

    return mrb_funcall_argv(mrb, self, to_enum, 1, &each);
  }

  auto &file = *static_cast<MrbFile *>(mrb_data_get_ptr(mrb, self, &mrb_file_linereader_dt));

  file.file.seekg(0);

  std::string line;
  for (;;)
  {
    std::getline(file.file, line);
    if (!file.file)
      break;

    mrb_value str = mrb_str_new(mrb, line.c_str(), line.size());
    mrb_yield(mrb, block, str);
  }

  file.file.seekg(0);

  return self;
}

// ----------------------------------------------------------------------------