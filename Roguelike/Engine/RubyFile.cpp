/*********************************
 * RubyFile.cpp
 * Leonardo Saikali
 * Created 2014/08/19
 *********************************/

#include "Common.h"

// ----------------------------------------------------------------------------

struct MrbFile
{
  std::fstream file;

};

// ----------------------------------------------------------------------------

mrb_data_type mrb_file_dt;
mrb_data_type mrb_file_linereader_dt;

static void mrb_file_free(mrb_state *, void *_file)
{
  auto file = (MrbFile *) _file;
  if (file->file.is_open())
    file->file.close();

  delete file;
}

static void mrb_file_linereader_free(mrb_state *, void *)
{
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_new(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_open(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_read(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_write(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_close(mrb_state *mrb, mrb_value self);
static mrb_value mrb_file_lines(mrb_state *mrb, mrb_value self);

static mrb_value mrb_file_lines_each(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_file_init(mrb_state *mrb)
{
  mrb_file_dt.dfree = mrb_file_free;
  mrb_file_dt.struct_name = typeid(MrbFile).name();

  mrb_file_linereader_dt.dfree = mrb_file_linereader_free;
  mrb_file_linereader_dt.struct_name = typeid(MrbFile).name();
  
  #pragma region File class

  auto file = mrb_define_class(mrb, "File", mrb->object_class);

  mrb_define_class_method(mrb, file, "new", mrb_file_new, MRB_ARGS_NONE());
  mrb_define_method(mrb, file, "open", mrb_file_open, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, file, "read", mrb_file_read, MRB_ARGS_NONE());
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

static mrb_value mrb_file_new(mrb_state *mrb, mrb_value)
{
  static auto file_c = mrb_class_get(mrb, "File");

  auto file = new MrbFile;
  auto obj = mrb_data_object_alloc(mrb, file_c, file, &mrb_file_dt);

  return mrb_obj_value(obj);
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_open(mrb_state *mrb, mrb_value self)
{
  auto& file = *(MrbFile *)mrb_data_get_ptr(mrb, self, &mrb_file_dt);

  mrb_value filename_v;
  mrb_get_args(mrb, "S", &filename_v);

  file.file.open(mrb_str_to_cstr(mrb, filename_v));

  return mrb_nil_value();
}

// ----------------------------------------------------------------------------


static mrb_value mrb_file_read(mrb_state *mrb, mrb_value self)
{
  auto& file = *(MrbFile *)mrb_data_get_ptr(mrb, self, &mrb_file_dt);

  std::string line;
  mrb_value str = mrb_str_new(mrb, "", 0);

  file.file.seekg(0);

  for(;;)
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
  auto& file = *(MrbFile *)mrb_data_get_ptr(mrb, self, &mrb_file_dt);

  (file);
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------


static mrb_value mrb_file_close(mrb_state *mrb, mrb_value self)
{
  auto& file = *(MrbFile *)mrb_data_get_ptr(mrb, self, &mrb_file_dt);

  file.file.close();
  return mrb_nil_value();
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_lines(mrb_state *mrb, mrb_value self)
{
  static auto line_c = mrb_class_get_under(mrb, mrb_class_get(mrb, "File"), "LineReader");

  auto file = (MrbFile *) mrb_data_get_ptr(mrb, self, &mrb_file_dt);
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

  auto& file = *(MrbFile *) mrb_data_get_ptr(mrb, self, &mrb_file_linereader_dt);
  
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

