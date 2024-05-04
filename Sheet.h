#ifndef SHEET_H
#define SHEET_H


#include <wx-3.0/wx/wx.h>
#include "mathplot.h"
#include <bits/stdc++.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multifit_nlin.h>

class Sheet
{
private:
    /* data */
    std::string xName;
    std::string yName;
    std::vector<double>* xData = nullptr;
    std::vector<double>* yData = nullptr;
public:
    Sheet(std::string xname, std::string yname, 
          std::vector<double> xdata, std::vector<double> ydata);
    ~Sheet();
    void Plotting();
    std::pair<double, double> Fitting(const std::vector<double>& xData, const std::vector<double>& yData);
    std::vector<double> getxData();
    std::vector<double> getyData();
    void OnFitLinear(wxCommandEvent& event);
};

Sheet::Sheet(std::string xname, std::string yname,
             std::vector<double> xdata, std::vector<double> ydata) {
    this->xName = xname;
    this->yName = yname;
    this->xData = &xdata;
    this->yData = &ydata;
}

Sheet::~Sheet()
{
}

std::vector<double> Sheet::getxData() {
    return *(this->xData);
}

std::vector<double> Sheet::getyData() {
    return *(this->yData);
}


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



std::pair<double, double> Sheet::Fitting(const std::vector<double>& xData, const std::vector<double>& yData) {
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

void Sheet::OnFitLinear(wxCommandEvent& event) {
    // Sheet::Fitting(Sheet::getxData(), Sheet::getyData());
}

enum {
    ID_LINEAR,
    ID_POLY,
    ID_EXP,
    ID_LOG
};

void Sheet::Plotting () {
    auto x = Sheet::getxData();
    auto y = Sheet::getyData();
    auto minX = *std::min_element(x.begin(), x.end());
    auto maxX = *std::max_element(x.begin(), x.end());
    auto minY = *std::min_element(y.begin(), y.end());
    auto maxY = *std::max_element(y.begin(), y.end());

    wxFrame* plotFrame = new wxFrame(NULL, wxID_ANY, "Graph Plot",wxDefaultPosition, wxSize(800,600));
    mpWindow* plotWindows = new mpWindow(plotFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
    wxMenuBar* mbar = new wxMenuBar;
    wxMenu* fitMenu = new wxMenu;

    fitMenu->Append(ID_LINEAR, "Linear model");
    fitMenu->Append(ID_POLY, "Polynomial Model");
    fitMenu->Append(ID_EXP, "Exponetial model");
    fitMenu->Append(ID_LOG, "Logarithmic model");

    mbar->Append(fitMenu, "&Fitting");
    plotFrame->SetMenuBar(mbar);

    // Connect event handlers for fitting options
    plotFrame->Bind(wxEVT_MENU, &Sheet::OnFitLinear, wxID_ANY, ID_LINEAR);
    // frame->Bind(wxEVT_MENU, &OnFitExponential, frame, wxID_ANY);
    // frame->Bind(wxEVT_MENU, &OnFitLogarithmic, frame, wxID_ANY);

    // Create a new plot and add it to the plot window
    mpFXYVector* plot = new mpFXYVector();
    plot->SetData(x, y);
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




#endif