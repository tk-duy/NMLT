#include <bits/stdc++.h>

class Sheet
{
private:
    /* data */
    static const std::string xName = NULL;
    static const std::string yName = NULL;
    static const std::vector<doubble> xData = NULL;
    static const std::vector<double> yData = NULL;
public:
    Sheet(const std::string xName, const std::string yName, 
          const std::vector<double> xData, const std::vector<double> yData);
    ~Sheet();
    
};

Sheet::Sheet(const std::string xName, const std::string yName,
             const std::vector<double> xData, const std::vector<double> yData) {
    Sheet->xName = xName;
    Sheet->yName = yName;
    Sheet->xData = xData;
    Sheet->yData = yData;
}

Sheet::~Sheet()
{
}
