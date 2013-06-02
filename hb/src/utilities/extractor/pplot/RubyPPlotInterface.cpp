#include "RubyPPlotInterface.h"

#include "PPlot.h"
#include <stdio.h>

#ifndef RUBY

RubyPPlotInterface::RubyPPlotInterface () 
{
}


bool RubyPPlotInterface::ApplyScript (PPlot &ioPPlot, const string &inScript, string& outErrorString) {
  fprintf (stderr, "ruby not implemented\n");
  return false;
}

#else

#include "ruby.h"

extern "C" {
void Init_pplot(void);
}

RubyPPlotInterface::RubyPPlotInterface () 
{
  ruby_init ();
  
  ruby_init_loadpath ();

  Init_pplot ();
}


bool RubyPPlotInterface::ApplyScript (PPlot &ioPPlot, const string &inScript, string& outErrorString) 
{
  SetCurrentPPlot (&ioPPlot);
 
  int theState = 0;
  rb_eval_string_protect (inScript.c_str (), &theState);

  if (theState) {
    string errorString = STR2CSTR (rb_obj_as_string (rb_inspect(ruby_errinfo)));
    rb_p (ruby_errinfo);
    fprintf (stderr, "ruby fail %s\n", errorString.c_str ());
    outErrorString = errorString;
    return false;
  }

  return true;
  
}

#endif
