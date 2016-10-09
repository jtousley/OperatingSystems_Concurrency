#ifndef MANDEL_H
#define MANDEL_H

#include "Color.h"
#include "Histogram.h"
#include "GlobalVariables.h"
#include <complex>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

//create a mandelbrot image between points point_one and point_two
//saves to "generatedMandelbrotImage.ppm"
//constants (XPIXEL, YPIXEL, etc...) are defined in GlobalVariables.h
void drawMandelbrot(int startPicture, int endPicture, std::complex<double> point_one, std::complex<double> point_two,
					std::vector<int> &Histogram,
					std::vector<std::vector<int>> &PictureIterations)
{
	std::complex<double> c;
	int iterations = 0;

	for(int i = startPicture; i < endPicture; i++){
		for(int j = 0; j < PIXELS; j++){
			//scale pixels to graph
			c = std::complex<double>
				(std::real(point_one) + (j*(std::real(point_two) - std::real(point_one))) / PIXELS
				,std::imag(point_one) + (i*(std::imag(point_two) - std::imag(point_one))) / PIXELS);

			//calculates the number of iterations
			//not a function because I want to use the z value
			std::complex<double> z (0.0, 0.0);
			iterations = 0;
			while(std::real(z)*std::real(z) + std::imag(z)*std::imag(z) < 4 && iterations < MAX_ITER){
				z = z*z + c;
				iterations++;
			} //while

			//generate histogram for coloring
			Histogram[iterations] = Histogram[iterations] + 1;
			PictureIterations[i][j] = iterations;
		}// for
	} // for

} // drawMandelbrot

#endif
