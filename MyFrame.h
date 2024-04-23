#include <wx/wx.h>
#include <wx-3.0/wx/grid.h>
#include <bits/stdc++.h>
#include <fstream>
#include <wx-3.0/wx/tokenzr.h>
// #include <wx-3.0/wx/string.h>
#include <string.h>
#include "mathplot.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multifit_nlin.h>
#include <functional>

class MyFrame : public wxFrame {
    public:
        MyFrame(const wxString& title);

    private:
        wxGrid* grid = NULL;

        void OnOpen(wxCommandEvent& event);
        void Plotting(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void LoadFileToGrid(const wxString& filePath);
};

enum
{
    ID_PLOT,
};

// Implementing functions
//Re-define constructor
MyFrame::MyFrame(const wxString& title) 
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    //Add menu Bar
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_PLOT, "&Plotting...\tCtrl-P");    
    menuFile->Append(wxID_OPEN);
    // menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

 
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
 
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");
 
    SetMenuBar( menuBar );
 
    // Add event to menu
    Bind(wxEVT_MENU, &MyFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MyFrame::Plotting, this, ID_PLOT);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

    //Add a grid
    MyFrame::grid = new wxGrid(panel, wxID_ANY);
    grid->CreateGrid(300, 300); // Create a 5x5 grid
    grid->SetDefaultCellBackgroundColour(*wxWHITE);
    grid->SetDefaultCellTextColour(*wxBLACK);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(grid, 1, wxEXPAND | wxALL, 10);

    panel->SetSizerAndFit(sizer);
}
// File openning method
void MyFrame::OnOpen(wxCommandEvent& event) {
    // Create a file dialog
    wxFileDialog wxFileDialog(this, "Open File", "", "", 
                            "Text Files (*.txt)|*.txt|CSV Files (*.csv)|*.csv", 
                            wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    // Show the file
    if (wxFileDialog.ShowModal() == wxID_CANCEL)
        return; 

    wxString filePath = wxFileDialog.GetPath();
    LoadFileToGrid(filePath);
}

// Exitting method
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}
// About mathplotfit
void MyFrame::OnAbout(wxCommandEvent& event) {
    wxMessageBox("MathPlotFit is a tool for data processing.",
                    "About MathPlotFit", wxOK | wxICON_INFORMATION);
}
// Method of load file to grid 
void MyFrame::LoadFileToGrid(const wxString& filePath) {
    std::ifstream file(filePath.ToStdString());
    if (!file.is_open()) {
        wxMessageBox("Failed to open file!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::string line;
    int row = 0;
    while (std::getline(file, line) && row < MyFrame::grid->GetNumberRows()) {
        wxStringTokenizer tokenizer(line, ",");
        int col = 0;
        while (tokenizer.HasMoreTokens() && col < MyFrame::grid->GetNumberCols()) {
            wxString token = tokenizer.GetNextToken();
            grid->SetCellValue(row, col, token);
            col++;
        }
        row++;
    }
    file.close();  
}


/*

++++++++++++ FITTING FEATURE ++++++++++++++

*/ 

typedef std::function<int(const gsl_vector*, void*, gsl_vector*)> ModelFunction;

// Linear model: y = ax + b
int linear(const gsl_vector* x, void* params, gsl_vector* f) {
    double m = gsl_vector_get(x, 0); // Slope
    double c = gsl_vector_get(x, 1); // Intercept
    size_t n = ((std::vector<double>*)params)->size(); // Number of data points
    std::vector<double>& data = *((std::vector<double>*)params);

    for (size_t i = 0; i < n; ++i) {
        double t = i; // Independent variable (e.g., time)
        double y_model = m * t + c; // Model prediction
        gsl_vector_set(f, i, y_model - data[i]); // Residual
    }

    return GSL_SUCCESS;
}

// Polynomial model: y = ax^2 + bx + c
int polynomial(const gsl_vector* x, void* params, const gsl_vector* f) {
    return GSL_SUCCESS;
}

// Exponential model: y = a*exp(bx)
int exponential(const gsl_vector* x, void* params, const gsl_vector* f) {
    return GSL_SUCCESS;
}

// Logarithmic model: y = a + bln(x)
int logarithmic(const gsl_vector* x, void* params, const gsl_vector* f) {
    return GSL_SUCCESS;
}

// fitting method
std::pair<double, double> fitData(const std::vector<double>& xData, 
                                  const std::vector<double>& yData) {
    // Initialize GSL fitting
    const size_t numParams = 2; // Number of parameters (slope and intercept)
    const size_t numData = xData.size(); // Number of data points
    gsl_multifit_function_fdf fittingFunc;
    fittingFunc.f = linear;
    fittingFunc.df = nullptr;
    fittingFunc.fdf = nullptr;
    fittingFunc.n = numData;
    fittingFunc.p = numParams;
    fittingFunc.params = const_cast<std::vector<double>*> (&yData);

    // Initial guess for parameters
    gsl_vector* params = gsl_vector_alloc(numParams);
    gsl_vector_set(params, 0, 1.0); // Initial guess for slope
    gsl_vector_set(params, 1, 0.0); // Initial guess for intercept

    // Fitting using Levenberg-Marquardt algorithm
    gsl_multifit_fdfsolver* solver = gsl_multifit_fdfsolver_alloc(gsl_multifit_fdfsolver_lmsder, numData, numParams);
    gsl_multifit_fdfsolver_set(solver, &fittingFunc, params);

    // Perform fitting
    int status;
    size_t iter = 0;
    do {
        iter++;
        status = gsl_multifit_fdfsolver_iterate(solver);
        if (status)
            break;
        status = gsl_multifit_test_delta(solver->dx, solver->x, 1e-4, 1e-4);
    } while (status == GSL_CONTINUE && iter < 100);

    // Get fitted parameters
    double slope = gsl_vector_get(solver->x, 0);
    double intercept = gsl_vector_get(solver->x, 1);

    // Clean up
    gsl_vector_free(params);
    gsl_multifit_fdfsolver_free(solver);

    return std::make_pair(slope, intercept);
}

int OnFitLinear(wxCommandEvent& event, const std::vector<double>& xData) {
    wxMessageBox("MathPlotFit is a tool for data processing.",
                    "About MathPlotFit", wxOK | wxICON_INFORMATION);
    return 0;
}

// Plotting method
void MyFrame::Plotting(wxCommandEvent& event) {
    std::vector<double> xData = {-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
    std::vector<double> yData = {-5, -4, -2, -1, -4, 4, 1, 1, 7, 2, 5};
    auto minX = *std::min_element(xData.begin(), xData.end());
    auto maxX = *std::max_element(xData.begin(), xData.end());
    auto minY = *std::min_element(yData.begin(), yData.end());
    auto maxY = *std::max_element(yData.begin(), yData.end());

    wxFrame* plotFrame = new wxFrame(NULL, wxID_ANY, "Graph Plot",wxDefaultPosition, wxSize(800,600));
    mpWindow* plotWindows = new mpWindow(plotFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    wxMenuBar* mbar = new wxMenuBar;
    wxMenu* fitMenu = new wxMenu;

    fitMenu->Append(wxID_ANY, "Linear model");
    fitMenu->Append(wxID_ANY, "Polynomial Model");
    fitMenu->Append(wxID_ANY, "Exponetial model");
    fitMenu->Append(wxID_ANY, "Logarithmic model");

    mbar->Append(fitMenu, "&Fit");
    plotFrame->SetMenuBar(mbar);

    // Connect event handlers for fitting options
    plotFrame->Bind(wxEVT_MENU, &OnFitLinear, wxID_ANY, wxID_ANY, plotFrame);
    // frame->Bind(wxEVT_MENU, &OnFitExponential, frame, wxID_ANY);
    // frame->Bind(wxEVT_MENU, &OnFitLogarithmic, frame, wxID_ANY);

    // Create a new plot and add it to the plot window
    mpFXYVector* plot = new mpFXYVector();
    plot->SetData(xData, yData);
    plot->SetContinuity(false);

    // Set line thickness for the plot
    // Create a pen with blue color and thickness 2
    wxPen pen(wxColour(255, 0, 0), 8, wxPENSTYLE_SOLID); 
     // Set the pen for the plot
    plot->SetPen(pen);

    plotWindows->Fit(minX, maxX, minY, maxY);
    plotWindows->AddLayer(plot);
    plotWindows->UpdateAll();

    // Add scale
    mpScaleX *sx = new mpScaleX( wxT("x axis"), 1, false);
    sx->SetAlign(mpALIGN_CENTER);
    mpScaleY *sy = new mpScaleY( wxT("y axis"), 1, false);
    sy->SetAlign(mpALIGN_CENTER);
    plotWindows->AddLayer(sx);
    plotWindows->AddLayer(sy);

    plotWindows->EnableDoubleBuffer(true);
    plotWindows->LockAspect();
    // Set plot window as the main window
    plotFrame->Show(true);    
}
