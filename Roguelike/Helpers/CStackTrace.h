/*********************************
 * CStackTrace.h
 * Connor Hilarides
 * Created 2014/08/29
 * Copyright © 2014 DigiPen Institute of Technology, All Rights Reserved
 *********************************/

#ifdef __cplusplus
extern "C" 
{
#endif

struct st_trace
{
  char *name;
  char *file;
  int line;
};

struct st_trace *st_get_trace(void *addr);
void st_free_trace(struct st_trace *trace);

#ifdef __cplusplus
}
#endif
