// Put any additional header includes here

#include "diddy.h"

void drawImage(int width, int height, const unsigned short *image_data);
void draw_an_image() {

	// Make a call to draw the image here
	drawImage(DIDDY_WIDTH, DIDDY_HEIGHT, diddy_data);

}

