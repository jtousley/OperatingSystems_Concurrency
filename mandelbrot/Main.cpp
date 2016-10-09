/*
Jonathan Tousley
A01908436
CS3100 - Assignment 3
Dr. Kenneth Sundberg
*/

#include "Mandel.h"
#include "Tools.h"
#include <chrono>

int main(){
	const int THREADSIZE = 32;
	double point1_1 = .3515, point1_2= .353, 
		point2_1 = .353, point2_2 = .3545;
	std::complex<double> c1 (point1_1, point1_2), c2 (point2_1, point2_2);
	std::vector<int> Histogram (MAX_ITER); //stores number of times each iteration occurs
	std::vector<double> Percentages(MAX_ITER);	//stores percentage of times each iteration occurs (as integral)
	std::vector<std::vector<ColorIndex>> PictureToPrint(PIXELS, std::vector<ColorIndex>(PIXELS));		//RGB values for every pixel
	std::vector<std::vector<int>> PictureIterations(PIXELS, std::vector<int>(PIXELS));	//number of iterations at each pixel
	std::vector<std::thread> Threadpool;
	std::vector<int> ThreadDivider(THREADSIZE + 1);

	ThreadDivider[0] = 0;
	for(int i = 1; i < THREADSIZE; i++){
		ThreadDivider[i] = (PIXELS / THREADSIZE) + ThreadDivider[i-1];
	}
	ThreadDivider[THREADSIZE] = PIXELS;

	auto timeTaken = timeFunction([&](){
		try{
			for(int i = 0; i < THREADSIZE; i++){
				//if(i == (THREADSIZE - 1)){ continue; }
				Threadpool.push_back(std::thread([&, i](){
					drawMandelbrot(ThreadDivider[i], ThreadDivider[i + 1], c1, c2, Histogram, PictureIterations); 
				}));
			}

			std::for_each(Threadpool.begin(), Threadpool.end(), [](std::thread &t){ t.join(); });
		}
		catch(std::string errorMessage){
			std::cout << errorMessage << std::endl;
		}
	});

	//calculate vector values for each section of indices
	//ENDBAND1, ENDBAND2, ENDBAND3, ENDBAND4 -- defined in GlobalVariables.h

	auto startIndex = 0;
	auto endIndex = ENDBAND1;
	calculateColorBand(Histogram, Percentages, startIndex, endIndex);
	configurePictureToPrint(PictureToPrint, PictureIterations, Histogram, Percentages, StartColor1, StartColor2, startIndex,endIndex);

	startIndex = endIndex;
	endIndex = ENDBAND2;
	calculateColorBand(Histogram, Percentages, startIndex, endIndex);
	configurePictureToPrint(PictureToPrint, PictureIterations, Histogram, Percentages, StartColor2, StartColor3, startIndex,endIndex);
	
	startIndex = endIndex;
	endIndex = ENDBAND3;
	calculateColorBand(Histogram, Percentages, startIndex, endIndex);
	configurePictureToPrint(PictureToPrint, PictureIterations, Histogram, Percentages, StartColor3, StartColor4, startIndex,endIndex);

	startIndex = endIndex;
	endIndex = ENDBAND4;
	calculateColorBand(Histogram, Percentages, startIndex, endIndex);
	configurePictureToPrint(PictureToPrint, PictureIterations, Histogram, Percentages, StartColor5, EndColor, startIndex,endIndex);

	//create a ppm file using the values stored in the vectors
	std::ofstream fout;
	fout.open("generatedMandelbrotImage.ppm");
	if(!fout){
		throw "Error opening file\n";
	}

	fout << "P3\n" << PIXELS << " " << PIXELS << std::endl << "255" << std::endl;

	for(int i = 0; i < PIXELS; i++){
		for(int j = 0; j < PIXELS; j++){
				fout << PictureToPrint[i][j].red << " "
				<< PictureToPrint[i][j].green << " "
				<< PictureToPrint[i][j].blue << " ";
				
		}
		fout << std::endl;
	}

	std::cout << timeTaken << std::endl;

	return 0;
}