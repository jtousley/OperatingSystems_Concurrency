#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "GlobalVariables.h"
#include <vector>

//fills in sections of vector Percentages to reflect which percentage of the total 
//each iteration value in section startIndex <-> endIndex has
void calculateColorBand(std::vector<int> const &Histogram 
						,std::vector<double> &Percentages
						,int startIndex, int EndIndex)
{
	//total number of iterations in section
	auto band_total = 0;
	for(int i = startIndex; i < EndIndex; i++){
		band_total += Histogram[i];
	}

	auto running_total = 0.0;
	//percentage of iterations each iteration value has
	for(int i = startIndex; i < EndIndex; i++){
		running_total += static_cast<double>(Histogram[i]);
		Percentages[i] = running_total / static_cast<double>(band_total); //cumulative
	}
} //calculateColorBand

//fills in vector PictureToPrint with RGB values based on percentage of iterations
void configurePictureToPrint(
	std::vector<std::vector<ColorIndex>> &PictureToPrint,
	std::vector<std::vector<int>> const &PictureIterations, 
	std::vector<int> const &Histogram,
	std::vector<double> const &Percentages, 
	const ColorIndex startColor, const ColorIndex endColor,
	const int startIndex, const int endIndex)
{
	for(int i = 0; i < PIXELS; i++){
		for(int j = 0; j < PIXELS; j++){
			//all pixels with iterations between startIndex and endIndex
			if(PictureIterations[i][j] > startIndex && PictureIterations[i][j] < endIndex)
			{
				//choose an RGB value based on the percentage of iterations that pixel has in its respective band
				PictureToPrint[i][j] = ColorIndex( 
											static_cast<int>((1 - Percentages[Histogram[PictureIterations[i][j]]]) * startColor.red 
											+ Percentages[Histogram[PictureIterations[i][j]]] * endColor.red)
											, static_cast<int>((1 - Percentages[Histogram[PictureIterations[i][j]]]) * startColor.green
											+ Percentages[Histogram[PictureIterations[i][j]]] * endColor.green)
											, static_cast<int>((1 - Percentages[Histogram[PictureIterations[i][j]]]) * startColor.blue
											+ Percentages[Histogram[PictureIterations[i][j]]] * endColor.blue));
			} //if
		} // for
	} // for
} // configurePictureToPrint


#endif