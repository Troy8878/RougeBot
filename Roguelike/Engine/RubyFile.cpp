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

static void mrb_file_free(mrb_state *, void *_file)
{
  auto file = (MrbFile *) _file;
  if (file->file.is_open())
    file->file.close();

  delete file;
}

// ----------------------------------------------------------------------------

static mrb_value mrb_file_new(mrb_state *mrb, mrb_value self);

static mrb_value mrb_file_open(mrb_state *mrb, mrb_value self);

static mrb_value mrb_file_read(mrb_state *mrb, mrb_value self);

static mrb_value mrb_file_write(mrb_state *mrb, mrb_value self);

static mrb_value mrb_file_close(mrb_state *mrb, mrb_value self);

// ----------------------------------------------------------------------------

extern "C" void mrb_mruby_file_init(mrb_state *mrb)
{
  mrb_file_dt.dfree = mrb_file_free;
  mrb_file_dt.struct_name = typeid(MrbFile).name();

  auto file = mrb_define_class(mrb, "File", mrb->object_class);

  mrb_define_class_method(mrb, file, "new", mrb_file_new, MRB_ARGS_NONE());
  mrb_define_method(mrb, file, "open", mrb_file_open, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, file, "read", mrb_file_read, MRB_ARGS_NONE());
  mrb_define_method(mrb, file, "close", mrb_file_close, MRB_ARGS_NONE());
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

  for(;;)
  {
    std::getline(file.file, line);
    if (!file.file)
      return str;

    mrb_value linestr = mrb_str_new(mrb, line.c_str(), line.size());

    str = mrb_str_append(mrb, str, linestr);
  }
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

