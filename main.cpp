#include <wx/wx.h>
#include "Sheet.h"


class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        Sheet* frame = new Sheet("MathPlotFit");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
