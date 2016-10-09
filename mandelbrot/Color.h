#ifndef COLOR_H
#define COLOR_H

//For passing around RGB values
struct ColorIndex{
	int red, green, blue;

	ColorIndex() : red(0), green(0), blue(0){}
	ColorIndex(int r, int g, int b) : red(r), green(g), blue(b){}
	ColorIndex(ColorIndex const& rhs) = default;
	ColorIndex& operator=(const ColorIndex& rhs){
		red = rhs.red;
		green = rhs.green;
		blue = rhs.blue;

		return *this;
	}
};

ColorIndex const StartColor1(255,255,255), 
				StartColor2(125,120,120), 
				StartColor3(0, 17, 216), 
				StartColor4(0,210,255), 
				StartColor5(255,255,255),
				EndColor(50,50,255);

#endif