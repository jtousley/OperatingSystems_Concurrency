Compiling information:
makefile attached, gcc version 4.8.2

Image output file: generatedMandelbrotImage.ppm

Chart of threads vs time: ThreadChart.jpg

Summary of why the chart looks that way:

Threading was useful in this program up to the number of processors the machine had (8). After which, any improvements seen were merely speed ups from utilizing the cache more efficiently. As the graph depicts, improvement essentially stopped after 8 processors.