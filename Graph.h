#ifndef GRAPH_H
#define GRAPH_H

#include <bits/stdc++.h>
#include <wx-3.0/wx/wx.h>
#include "mathplot.h"
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multifit_nlin.h>

class Graph : public wxFrame {
    private:
        std::vector<double>* xData;
        std::vector<double>* yData;
    public:
        Graph(std::vector<double>* xData, std::vector<double>* yData);
        void OnFitLinear(wxCommandEvent& event);
};

Graph::Graph(std::vector<double>* xData, std::vector<double>* yData) 
    : wxFrame(NULL, wxID_ANY, "title", wxDefaultPosition, wxSize(600,600)) {
    mpWindow* plotWindows = new mpWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

    // Add menu 
    wxMenu* fitting = new wxMenu;
    fitting->Append(wxID_ANY, "Linear model");

    wxMenuBar* bar = new wxMenuBar;
    bar->Append(fitting, "&Fitting");
    SetMenuBar(bar);

    // Connect event handler for menu
    Bind(wxEVT_MENU, &Graph::OnFitLinear, this, wxID_ANY);


    this->xData = xData;
    this->yData = yData;
    auto minX = *std::min_element((*(this->xData)).begin(), (*(this->xData)).end());
    auto maxX = *std::max_element((*(this->xData)).begin(), (*(this->xData)).end());
    auto minY = *std::min_element((*(this->yData)).begin(), (*(this->yData)).end());
    auto maxY = *std::max_element((*(this->yData)).begin(), (*(this->yData)).end());

    // Create a plot
    mpFXYVector* plot  = new mpFXYVector();
    plot->SetData(*(this->xData), *(this->yData));
    plot->SetContinuity(false);

    // Create pen 
    wxPen pen(wxColour(255, 0, 0), 8, wxPENSTYLE_SOLID); 
     // Set the pen for the plot
    plot->SetPen(pen);

    // Adjust data to fit into screen
    plotWindows->Fit(minX, maxX, minY, maxY);
    plotWindows->AddLayer(plot);
    plotWindows->UpdateAll();

    // Set scale
    mpScaleX *sx = new mpScaleX( wxT("x axis"), 1, false);
    sx->SetAlign(mpALIGN_CENTER);
    mpScaleY *sy = new mpScaleY( wxT("y axis"), 1, false);
    sy->SetAlign(mpALIGN_CENTER);
    plotWindows->AddLayer(sx);
    plotWindows->AddLayer(sy);

    plotWindows->EnableDoubleBuffer(true);
    plotWindows->LockAspect();
    // Set plot window as the main window
    Show(true);  
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

std::pair<double, double> Fitting(std::vector<double>& xData, std::vector<double>& yData) {
    // Initialize GSL fitting
    const size_t numParams = 2; // Number of parameters (slope and intercept)
    const size_t numData = xData.size(); // Number of data points
    gsl_multifit_function_fdf fittingFunc;
    fittingFunc.f = linear;
    fittingFunc.df = nullptr;
    fittingFunc.fdf = nullptr;
    fittingFunc.n = numData;
    fittingFunc.p = numParams;
    fittingFunc.params = &yData;

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

void Graph::OnFitLinear(wxCommandEvent& event) {
    Fitting(*(this->xData), *(this->yData));
}



#endif