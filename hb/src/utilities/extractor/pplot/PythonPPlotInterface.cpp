#include "PythonPPlotInterface.h"

#include "PPlot.h"

#ifndef PYTHON
PythonPPlotInterface::PythonPPlotInterface () 
{
}
PythonPPlotInterface::~PythonPPlotInterface () 
{
}


bool PythonPPlotInterface::ApplyScript (PPlot &ioPPlot, const string &inScript) {
  return false;
}

#else

#include <Python.h>

extern "C" {
void init_pplot(void);
}

PythonPPlotInterface::PythonPPlotInterface () 
{
  Py_Initialize ();
  //SWIG_Init ();
  
  //  Python_init_loadpath ();

  init_pplot ();
}

PythonPPlotInterface::~PythonPPlotInterface () {
  Py_Finalize ();
}


bool PythonPPlotInterface::ApplyScript (PPlot &ioPPlot, const string &inScript) 
{
  SetCurrentPPlot (&ioPPlot);
 
  int theState = PyRun_SimpleString ((char *)inScript.c_str ());

  //  SetCurrentPPlot (0);

  if (theState) {
    /*
    string errorString = STR2CSTR (rb_obj_as_string (rb_inspect(Python_errinfo)));
    rb_p (Python_errinfo);
    return false;*/
  }

  return true;
  
}


#endif
