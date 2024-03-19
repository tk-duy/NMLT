// g++ main.cpp -o main `wx-config --cxxflags --libs`
#include <wx/wx.h>
#include "MyFrame.h"


class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame("My App");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
