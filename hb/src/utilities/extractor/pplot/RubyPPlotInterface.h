#ifndef __RUBYPPLOTINTERFACE_H__
#define __RUBYPPLOTINTERFACE_H__

// ruby manipulation of PPlot

class PPlot;
#include<string>
using std::string;

class RubyPPlotInterface {
 public:
  RubyPPlotInterface ();

  bool ApplyScript (PPlot &ioPPlot, const string &inString, string& outErrorString);

};

#endif
