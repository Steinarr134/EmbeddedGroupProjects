#ifndef PARSED_DEFINED
#define PARSED_DEFINED

#include <random_defs.h>

struct Parsed
{
  unsigned char what;
  double val;
};

Parsed parse()
{
  // b should now be on the form p=1.34\0\0\0\0
  Parsed ret;
  ret.what = b[0];

  double value = 0;
  int tens = 1;
  bool decimal_done = false;
  for (int i = 1; i < buffer_size; i++)
  {
    if (b[i] == '.' || ('0' <= b[i] && b[i] <= '9'))
    {
      if (b[i] == '.')
      {
        decimal_done = true;
        tens = 10;
        continue;
      }
      if (b[i] == 0)
      {
        ret.val = value;
        return ret;
      }
      if (!decimal_done)
      {
        value *= tens;
        value += b[i] - '0';
        tens *= 10;
      }
      else
      {
        value += (double)(b[i] - '0') / tens;
        tens *= 10;
      }
    }
  }
  ret.val = value;
  return ret;
}



#endif
