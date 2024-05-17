#ifndef GRAPH_H
#define GRAPH_H

#include <bits/stdc++.h>
#include <wx-3.0/wx/wx.h>
#include "mathplot.h"
#include <gsl/gsl_multifit_nlin.h>
#include <numeric>

class Graph : public wxFrame {
    private:
        std::vector<double> xData;
        std::vector<double> yData;
        mpWindow* plotWindows = new mpWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
        mpText* textLayer = nullptr;

    public:
        Graph(std::vector<double>& x, std::vector<double>& y);
        static int linear(const gsl_vector* x, void* params, gsl_vector* f);
        static int exponential(const gsl_vector* x, void* params, gsl_vector* f);  
        static int logarithmic(const gsl_vector* x, void* params, gsl_vector* f);          
        std::pair<double, double> Fitting(std::vector<double>& x, std::vector<double>& y, std::string modelname);
        void Draw(std::vector<double>& x, std::vector<double>& y, std::string label);
        double calculateR2(const std::vector<double>& observed, const std::vector<double>& predicted);
        void OnFitLinear(wxCommandEvent& event);
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
    : wxFrame(NULL, wxID_ANY, "2D Graph", wxDefaultPosition, wxSize(800,600))
     {

    // Add menu 
    wxMenu* fitting = new wxMenu;
    fitting->Append(ID_LINEAR, "Linear");
    fitting->Append(ID_EXP, "Exponential");
    fitting->Append(ID_LOG, "Logarithmic");

    wxMenuBar* bar = new wxMenuBar;
    bar->Append(fitting, "&Fitting");
    SetMenuBar(bar);

    // Connect event handler for menu
    Bind(wxEVT_MENU, &Graph::OnFitLinear, this, ID_LINEAR);
    Bind(wxEVT_MENU, &Graph::OnFitExponential, this, ID_EXP);
    Bind(wxEVT_MENU, &Graph::OnFitLogarithmic, this, ID_LOG);
    
    // Assign parameters to the attributes
    this->xData = x;
    this->yData = y;

    // Get min and max values of vectors
    auto minX = *std::min_element(x.begin(), x.end());
    auto maxX = *std::max_element(x.begin(), x.end());
    auto minY = *std::min_element(y.begin(), y.end());
    auto maxY = *std::max_element(y.begin(), y.end());

    // Create a plot
    mpFXYVector* plot  = new mpFXYVector();
    plot->SetData(x, y);
    plot->SetContinuity(false);

    // Create pen 
    wxPen pen(wxColour(255, 0, 0), 8, wxPENSTYLE_SOLID); 
    plot->SetPen(pen);

    // Adjust data to fit into screen
    plotWindows->Fit(minX, maxX, minY, maxY);
    plotWindows->AddLayer(plot);

    // Set scale
    mpScaleX *sx = new mpScaleX( wxT("x axis"), 1, true);
    sx->SetAlign(mpALIGN_CENTER);
    mpScaleY *sy = new mpScaleY( wxT("y axis"), 1, true);
    sy->SetAlign(mpALIGN_CENTER);
    
    plotWindows->AddLayer(sx);
    plotWindows->AddLayer(sy);

    plotWindows->EnableDoubleBuffer(true);
    plotWindows->LockAspect();
    plotWindows->UpdateAll();

    // Set plot window as the main window
    Show(true);

}


// Define models
// Linear model: y = ax + b
int Graph::linear(const gsl_vector* x, void* params, gsl_vector* f) {

    double m = gsl_vector_get(x, 0); // Slope
    double c = gsl_vector_get(x, 1); // Intercept
    auto* data = static_cast<std::pair<const std::vector<double>*, const std::vector<double>*>*>(params);
    const std::vector<double>& xdata = *(data->first);
    const std::vector<double>& ydata = *(data->second);

    size_t n = xdata.size(); // Number of data points

    for (size_t i = 0; i < n; ++i) {
        double t = xdata[i]; // Independent variable
        double y_model = m * t + c; // Model prediction
        gsl_vector_set(f, i, y_model - ydata[i]); // Residual
    }

    return GSL_SUCCESS;
}

// Exponential model: y = a*exp(bx)
int Graph::exponential(const gsl_vector* x, void* params, gsl_vector* f) {
    double a = gsl_vector_get(x, 0); // Coef. a
    double b = gsl_vector_get(x, 1); // Coef. b
    auto* data = static_cast<std::pair<const std::vector<double>*, const std::vector<double>*>*>(params);
    const std::vector<double>& xdata = *(data->first);
    const std::vector<double>& ydata = *(data->second);

    size_t n = xdata.size(); // Number of data points

    for (size_t i = 0; i < n; ++i) {
        double t = xdata[i]; // Independent variable
        double y_model = a * exp(b*t); // Exponetial model
        gsl_vector_set(f, i, y_model - ydata[i]); // Residual
    }

    return GSL_SUCCESS;
}

// Logarithmic model: y = a*ln(x) + b
int Graph::logarithmic(const gsl_vector* x, void* params, gsl_vector* f) {
    double a = gsl_vector_get(x, 0); // Coef. a
    double b = gsl_vector_get(x, 1); // Coef. b
    auto* data = static_cast<std::pair<const std::vector<double>*, const std::vector<double>*>*>(params);
    const std::vector<double>& xdata = *(data->first);
    const std::vector<double>& ydata = *(data->second);

    size_t n = xdata.size(); // Number of data points

    for (size_t i = 0; i < n; ++i) {
        double t = xdata[i]; // Independent variable
        if (t <= 0) {
            // Handle the case where t is zero or negative to avoid log(0) or log(negative number)
            gsl_vector_set(f, i, std::numeric_limits<double>::infinity());
        } else {
            double y_model = a * log(t) + b; // Logarithmic model
            gsl_vector_set(f, i, y_model - ydata[i]); // Residual
        }
    }

    return GSL_SUCCESS;
}


// Method of fitting data to a function
std::pair<double, double> Graph::Fitting(std::vector<double>& x, std::vector<double>& y, std::string modelname) {
    // Initialize GSL fitting
    const size_t numParams = 2; // Number of parameters (a and b)
    const size_t numData = x.size(); // Number of data points
    gsl_multifit_function_fdf fittingFunc;
    try
    {
        /* If model name passed is invalid: */
        if(modelname == "linear") fittingFunc.f = &Graph::linear;
        else if(modelname == "exponential") fittingFunc.f = &Graph::exponential;
        else if(modelname == "logarithmic") fittingFunc.f = &Graph::logarithmic;
        else throw std::runtime_error("Invalid model");
    }
    catch(const std::exception& e)
    {
        wxMessageBox("The passed model is invalid, please double-check the model name.",
                "Runtime error", wxOK | wxICON_INFORMATION);
        return std::make_pair(0, 0);
    }
    fittingFunc.df = nullptr;
    fittingFunc.fdf = nullptr;
    fittingFunc.n = numData;
    fittingFunc.p = numParams;
    std::pair<const std::vector<double>*, const std::vector<double>*> paramsPair(&x, &y);
    fittingFunc.params = &paramsPair;

    // Initial guess for parameters
    gsl_vector* params = gsl_vector_alloc(numParams);
    gsl_vector_set(params, 0, 1.0); // Initial guess for a
    gsl_vector_set(params, 1, 0.0); // Initial guess for b

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
        status = gsl_multifit_test_delta(solver->dx, solver->x, 1e-6, 1e-6);
    } while (status == GSL_CONTINUE && iter < 1000);

    // Get fitted parameters
    double a = gsl_vector_get(solver->x, 0);
    double b = gsl_vector_get(solver->x, 1);

    // Clean up
    gsl_vector_free(params);
    gsl_multifit_fdfsolver_free(solver);

    return std::make_pair(a, b);
}

double Graph::calculateR2(const std::vector<double>& observed, const std::vector<double>& predicted) {
    // check if exists not consistent vector exception
    try
    {
        if (observed.size() != predicted.size())
            throw std::runtime_error("Not consistent vector.");
    }
    catch(const std::exception& e)
    {
        wxMessageBox("Observed and predicted vector is not consistent, please check again!",
                "About MathPlotFit", wxOK | wxICON_INFORMATION);
        return 0;
    }
    int sz = observed.size();

    // Calculate the mean of the observed values
    double mean = 0;
    for(int k = 0; k < sz; k++) {
        mean += observed[k];
    }
    mean = mean/(sz);

    // Calculate Total Sum of Squares (TSS)
    double tss = 0;
    for(int i = 0; i < sz; i++) {
        double n = std::pow(observed[i] - mean, 2);
        tss += n;
    }

    // Calculate Residual Sum of Squares (RSS)
    double rss = 0;
    for(int j = 0; j < sz; j++) {
        double m = std::pow(observed[j] - predicted[j], 2);
        rss += m;
    }

    // Calculate R-squared
    double r2 = 1.0 - (rss / tss);

    return r2;
}

// Drawing
void Graph::Draw(std::vector<double>& x, std::vector<double>& y, std::string label) {
    mpFXYVector* plot  = new mpFXYVector();
    plot->SetData(x, y);
    plot->SetContinuity(true);

        // Create pen 
    wxPen pen(wxColour(100, 255, 100), 2, wxPENSTYLE_DOT); 
     // Set the pen for the plot
    plot->SetPen(pen);
    plotWindows->AddLayer(plot);

    plotWindows->DelLayer(textLayer, false, false);
    textLayer = new mpText(label, 70, 85);
    plotWindows->AddLayer(textLayer, false);


    plotWindows->UpdateAll();
}

// Linear button
void Graph::OnFitLinear(wxCommandEvent& event) {
    auto fittedParams = Graph::Fitting(this->xData, this->yData, "linear");
    double a = fittedParams.first;
    double b = fittedParams.second;

    std::vector<double> ypred;
    for(int i = 0; i < (this->xData).size(); i++) {
        double temp = a*((this->xData)[i]) + b;
        ypred.push_back(temp);
    }
    std::string label = "y = F(x) = a*x +b\nSlope (a) = " + std::to_string(a) 
                                        + "\nBias (b) = " + std::to_string(b)
                                        + "\nR-squared = " + std::to_string(calculateR2(this->yData, ypred));
    Draw(this->xData, ypred, label);
}


// Exponential button
void Graph::OnFitExponential(wxCommandEvent& event) {
    auto fittedParams = Graph::Fitting(this->xData, this->yData, "exponential");
    double a = fittedParams.first;
    double b = fittedParams.second;

    std::vector<double> ypred;
    for(int i = 0; i < (this->xData).size(); i++) {
        double temp = a*exp(b*((this->xData)[i]));
        ypred.push_back(temp);
    }
    std::string label = "y = F(x) = a*exp(b*x)\nCoef. a = " + std::to_string(a) 
                                            + "\nCoef. b = " + std::to_string(b)
                                            + "\nR-squared = " + std::to_string(calculateR2(this->yData, ypred));
    Draw(this->xData, ypred, label);
}

// Logarithmic button
void Graph::OnFitLogarithmic(wxCommandEvent& event) {
    auto fittedParams = Graph::Fitting(this->xData, this->yData, "logarithmic");
    double a = fittedParams.first;
    double b = fittedParams.second;

    std::vector<double> ypred;
    for(int i = 0; i < (this->xData).size(); i++) {
        double temp = a*log((this->xData)[i]) + b;
        ypred.push_back(temp);
    }
    std::string label = "y = F(x) = a*ln(x) + b\nCoef. a = " + std::to_string(a) 
                                            + "\nCoef. b = " + std::to_string(b)
                                            + "\nR-squared = " + std::to_string(calculateR2(this->yData, ypred));
    Draw(this->xData, ypred, label);
}

#endif