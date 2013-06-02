#ifndef __PYTHONPPLOTINTERFACE_H__
#define __PYTHONPPLOTINTERFACE_H__

// ruby manipulation of PPlot

class PPlot;
#include<string>
using std::string;

class PythonPPlotInterface {
 public:
  PythonPPlotInterface ();
  ~PythonPPlotInterface ();

  bool ApplyScript (PPlot &ioPPlot, const string &inString);

};

#endif
