#include <wx/wx.h>
#include "../mathplot.h" // Include the wxMathPlot header
#include <wx-3.0/wx/grid.h>

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
        wxPanel* panel = new wxPanel(this, wxID_ANY);

        // Create a grid
        grid = new wxGrid(panel, wxID_ANY);
        grid->CreateGrid(5, 2); // Assume you have a grid with 5 rows and 2 columns

        // Add the grid to the sizer
        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(grid, 1, wxEXPAND | wxALL, 10);

        // Create a button to plot the graph
        wxButton* plotButton = new wxButton(panel, wxID_ANY, "Plot Graph");
        plotButton->Bind(wxEVT_BUTTON, &MyFrame::OnPlotButtonClick, this);
        sizer->Add(plotButton, 0, wxALIGN_CENTER | wxALL, 10);

        panel->SetSizerAndFit(sizer);
    }

    void OnPlotButtonClick(wxCommandEvent& event) {
        // Extract data from the grid and plot the graph
        PlotGraph();
    }

    void PlotGraph() {
        // Assuming you have extracted x and y data from the grid
        // Replace this with your data extraction logic
        std::vector<double> xData = {1.0, 2.0, 3.0, 4.0, 5.0};
        std::vector<double> yData = {2.0, 3.0, 4.0, 5.0, 6.0};

        // Create a wxMathPlot window
        wxFrame* plotFrame = new wxFrame(NULL, wxID_ANY, "Graph Plot", wxDefaultPosition, wxSize(600, 400));
        mpWindow* plotWindow = new mpWindow(plotFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

        // Create a new plot and add it to the plot window
        // mpFXYVector* plot = new mpFXYVector("Data Plot");
        // plot->SetData(xData, yData);
        // plotWindow->AddLayer(plot,true);
        
        // Add scale
        mpScaleX *sx = new mpScaleX( wxT("x axis"), 1, false);
        mpScaleY *sy = new mpScaleY( wxT("y axis"), 1, false);
        plotWindow->AddLayer(sx);
        plotWindow->AddLayer(sy);

        plotWindow->UpdateAll();
        // Set plot properties (optional)
        // plotWindow->SetMPScrollbars(true);
        plotWindow->EnableDoubleBuffer(true);

        // Set plot window as the main window
        plotFrame->Show(true);
    }

private:
    wxGrid* grid;
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame("Graph Plot Example");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);