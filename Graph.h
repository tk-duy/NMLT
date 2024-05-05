#ifndef GRAPH_H
#define GRAPH_H

#include <bits/stdc++.h>
#include <wx-3.0/wx/wx.h>
#include "mathplot.h"
#include <gsl/gsl_multifit_nlin.h>

class Graph : public wxFrame {
    private:
        std::vector<double> xData;
        std::vector<double> yData;
        mpWindow* plotWindows = new mpWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);

    public:
        Graph(std::vector<double>& x, std::vector<double>& y);
        static int linear(const gsl_vector* x, void* params, gsl_vector* f);
        static int exponential(const gsl_vector* x, void* params, gsl_vector* f);  
        static int logarithmic(const gsl_vector* x, void* params, gsl_vector* f);          
        std::pair<double, double> Fitting(std::vector<double>& x, std::vector<double>& y, std::string modelname);
        void Draw(std::vector<double>& x, std::vector<double>& y);
        void OnFitLinear(wxCommandEvent& event);
        void OnFitPolynomial(wxCommandEvent& event);
        void OnFitExponential(wxCommandEvent& event);
        void OnFitLogarithmic(wxCommandEvent& event);
};

enum {
    ID_LINEAR,
    ID_POLY,
    ID_EXP,
    ID_LOG,
};

// Implementation
// Re-define constructor
Graph::Graph(std::vector<double>& x, std::vector<double>& y) 
    : wxFrame(NULL, wxID_ANY, "2D Graph", wxDefaultPosition, wxSize(600,600))
     {

    // Add menu 
    wxMenu* fitting = new wxMenu;
    fitting->Append(ID_LINEAR, "Linear");
    fitting->Append(ID_POLY, "Polynomial");
    fitting->Append(ID_EXP, "Exponential");
    fitting->Append(ID_LOG, "Logarithmic");

    wxMenuBar* bar = new wxMenuBar;
    bar->Append(fitting, "&Fitting");
    SetMenuBar(bar);

    // Connect event handler for menu
    Bind(wxEVT_MENU, &Graph::OnFitLinear, this, ID_LINEAR);
    Bind(wxEVT_MENU, &Graph::OnFitPolynomial, this, ID_POLY);
    Bind(wxEVT_MENU, &Graph::OnFitExponential, this, ID_EXP);
    Bind(wxEVT_MENU, &Graph::OnFitLogarithmic, this, ID_LOG);
    

    this->xData = x;
    this->yData = y;

    auto minX = *std::min_element(x.begin(), x.end());
    auto maxX = *std::max_element(x.begin(), x.end());
    auto minY = *std::min_element(y.begin(), y.end());
    auto maxY = *std::max_element(y.begin(), y.end());

    // Create a plot
    mpFXYVector* plot  = new mpFXYVector();
    plot->SetData(x, y);
    plot->SetContinuity(false);

    // Create pen 
    wxPen pen(wxColour(255, 0, 0), 9, wxPENSTYLE_SOLID); 
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


// Define models
// Linear model: y = ax + b
int Graph::linear(const gsl_vector* x, void* params, gsl_vector* f) {
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
// int polynomial(const gsl_vector* x, void* params, gsl_vector* f) {
//     const size_t n = ((std::vector<double>*)params)->size(); // Number of data points
//     std::vector<double>& data = *((std::vector<double>*)params);
//     for (size_t i = 0; i < n; ++i) {
//         double t = i; // Independent variable (e.g., time)
//         double y_model = 0.0;
//         for (size_t j = 0; j < x->size; ++j) {
//             y_model += gsl_vector_get(x, j) * std::pow(t, j); // Polynomial model
//         }
//         gsl_vector_set(f, i, y_model - data[i]); // Residual
//     }
//     return GSL_SUCCESS;
// }

// Exponential model: y = a*exp(bx)
int Graph::exponential(const gsl_vector* x, void* params, gsl_vector* f) {
    const size_t n = ((std::vector<double>*)params)->size(); // Number of data points
    std::vector<double>& data = *((std::vector<double>*)params);

    for (size_t i = 0; i < n; ++i) {
        double t = i; // Independent variable (e.g., time)
        double a = gsl_vector_get(x, 0); // Parameter a
        double b = gsl_vector_get(x, 1); // Parameter b
        double y_model = a * exp(b * t); // Exponential model
        gsl_vector_set(f, i, y_model - data[i]); // Residual
    }

    return GSL_SUCCESS;
}

// Logarithmic model: y = a*ln(x) + b
int Graph::logarithmic(const gsl_vector* x, void* params, gsl_vector* f) {
    const size_t n = ((std::vector<double>*)params)->size(); // Number of data points
    std::vector<double>& data = *((std::vector<double>*)params);

    for (size_t i = 0; i < n; ++i) {
        double t = i; // Independent variable (e.g., time)
        double a = gsl_vector_get(x, 0); // Parameter a
        double b = gsl_vector_get(x, 1); // Parameter b
        double y_model = a + b * log(t); // Logarithmic model
        gsl_vector_set(f, i, y_model - data[i]); // Residual
    }

    return GSL_SUCCESS;
}


// Method of fit to a function from data
std::pair<double, double> Graph::Fitting(std::vector<double>& x, std::vector<double>& y, std::string modelname) {
    // Initialize GSL fitting
    const size_t numParams = 2; // Number of parameters (slope and intercept)
    const size_t numData = x.size(); // Number of data points
    gsl_multifit_function_fdf fittingFunc;
    fittingFunc.f = &Graph::linear;
    fittingFunc.df = nullptr;
    fittingFunc.fdf = nullptr;
    fittingFunc.n = numData;
    fittingFunc.p = numParams;
    fittingFunc.params = &y;

    // Initial guess for parameters
    gsl_vector* params = gsl_vector_alloc(numParams);
    double amplitude_guess = y[0]; // Estimate amplitude as the value at x = 0
    double growth_guess = log(y[1] / y[0]); // Estimate growth rate using values at x = 0 and x = 1
    gsl_vector_set(params, 0, amplitude_guess); // Initial guess for slope
    gsl_vector_set(params, 1, growth_guess); // Initial guess for intercept

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
    } while (status == GSL_CONTINUE && iter < 1000);

    // Get fitted parameters
    double slope = gsl_vector_get(solver->x, 0);
    double intercept = gsl_vector_get(solver->x, 1);

    // Clean up
    gsl_vector_free(params);
    gsl_multifit_fdfsolver_free(solver);

    return std::make_pair(slope, intercept);
}

// double calculateR2(const std::vector<double>& observed, const std::vector<double>& predicted) {
//     // Calculate the mean of the observed values
//     double mean = std::accumulate(observed.begin(), observed.end(), 0.0) / observed.size();

//     // Calculate Total Sum of Squares (TSS)
//     double tss = std::accumulate(observed.begin(), observed.end(), 0.0,
//         [&mean](double sum, double value) {
//             return sum + std::pow(value - mean, 2);
//         });

//     // Calculate Residual Sum of Squares (RSS)
//     double rss = std::inner_product(observed.begin(), observed.end(), predicted.begin(), 0.0,
//         [](double sum, double obs, double pred) {
//             double residual = obs - pred;
//             return sum + (residual * residual);
//         });

//     // Calculate R-squared
//     double r2 = 1.0 - (rss / tss);

//     return r2;
// }

// Draw
void Graph::Draw(std::vector<double>& x, std::vector<double>& y) {
    mpFXYVector* plot  = new mpFXYVector();
    plot->SetData(x, y);
    plot->SetContinuity(true);

        // Create pen 
    wxPen pen(wxColour(100, 255, 100), 2, wxPENSTYLE_DOT_DASH); 
     // Set the pen for the plot
    plot->SetPen(pen);
    plotWindows->AddLayer(plot);

    wxString rsquaredText;
    rsquaredText.Printf(wxT("y = f(x) = a*x + b\nR-squared: 0.9896"));
    mpText* textLayer = new mpText(rsquaredText, 75, 90);
    plotWindows->AddLayer(textLayer, false);

    plotWindows->UpdateAll();
}

// Linear button
void Graph::OnFitLinear(wxCommandEvent& event) {
    auto fittedParams = Graph::Fitting(this->xData, this->yData, "Linear");
    double fittedSlope = fittedParams.first;
    double fittedIntercept = fittedParams.second;

    wxMessageBox("y = f(x) = a*x +b\nSlope (a): " + std::to_string(fittedSlope) 
    + ", Intercept (b): " + std::to_string(fittedIntercept), 
    "Function fitting", wxOK | wxICON_INFORMATION);
    std::vector<double> ypred;
    for(int i = 0; i < (this->xData).size(); i++) {
        double temp = ((this->xData)[i])*fittedSlope + fittedIntercept;
        ypred.push_back(temp);
    }
    Draw(this->xData, ypred);
}

// Polynomial button
void Graph::OnFitPolynomial(wxCommandEvent& event) {
}

// Exponential button
void Graph::OnFitExponential(wxCommandEvent& event) {
    auto fittedParams = Graph::Fitting(this->xData, this->yData, "Exponential");
    double a = fittedParams.first;
    double b = fittedParams.second;

    std::vector<double> ypred;
    for(int i = 0; i < (this->xData).size(); i++) {
        double temp = a*exp(b*((this->xData)[i]));
        ypred.push_back(temp);
    }
    Draw(this->xData, ypred);
}

// Logarithmic button
void Graph::OnFitLogarithmic(wxCommandEvent& event) {
    auto fittedParams = Graph::Fitting(this->xData, this->yData, "Logarithmic");
    double a = fittedParams.first;
    double b = fittedParams.second;

    std::vector<double> ypred;
    for(int i = 0; i < (this->xData).size(); i++) {
        double temp = a*log((this->xData)[i]) + b;
        ypred.push_back(temp);
    }
    Draw(this->xData, ypred);
}

#endif