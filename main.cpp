// g++ -o main main.cpp mathplot.cpp `wx-config --cxxflags --libs`
#include <wx/wx.h>
#include "MyFrame.h"


class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame("MathPlotFit");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
