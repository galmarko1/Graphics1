#include "InputManager.h"
// #include "../DisplayGLFW/display.h"
#include "game.h"
#include "../res/includes/glm/glm.hpp"

int main(int argc,char *argv[])
{
	const int DISPLAY_WIDTH = 512;
	const int DISPLAY_HEIGHT = 512;
	const float CAMERA_ANGLE = 0.0f;
	const float NEAR = 1.0f;
	const float FAR = 100.0f;

	Game *scn = new Game(CAMERA_ANGLE,(float)DISPLAY_WIDTH/DISPLAY_HEIGHT,NEAR,FAR);
	
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");
	
	Init(display);
	
	scn->Init();

	display.SetScene(scn);

	while(!display.CloseWindow())
	{
        scn->SetShapeTex(0, 0);
		scn->Draw(1,0,scn->BACK,true,false, false);
        scn->SetShapeTex(0, 1);
        scn->Draw(1,0,scn->BACK,false,false, false);
        scn->SetShapeTex(0, 2);
        scn->Draw(1,0,scn->BACK,false,false, true);
        scn->SetShapeTex(0, 3);
        scn->Draw(1,0,scn->BACK,false,true, false);
		scn->Motion();
		display.SwapBuffers();
		display.PollEvents();	
			
	}
	delete scn;
	return 0;
}
