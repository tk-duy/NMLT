# NMLT
MathPlotFit - GUI app for data visualization and analysis.

Tutorial:
1. Install wxWidgets and GSL libraries
2. Clone is repository
3. In this folder, direct to sources folder, open terminal and run this command:
g++ -o ../bin/main main.cpp ../lib/mathplot.cpp \`wx-config --cxxflags --libs\` -lgsl -lgslcblas
../bin/main
