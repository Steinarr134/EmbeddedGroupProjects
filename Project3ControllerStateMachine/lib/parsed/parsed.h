#ifndef PARSED_DEFINED
#define PARSED_DEFINED

struct Parsed
{
  unsigned char what;
  double val;
};


Parsed parse(unsigned char b[], int buffer_size);

#endif
