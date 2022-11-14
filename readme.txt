Assignement 1 - 2D
Dolev Dor & Gal Markovich

Changes in the engine:

-> Scene.cpp: We've changed the Draw function by adding a new boolean parameter, "flag". We're using it to distinguish between the different outputs of the filtered 		      images. Also added calls to glViewPort in order to split the screen to 4.

-> Texture.cpp: Added 3 new functions, each for every alogrithm we were asked to implement.
		Changed the constructor by adding a new int paramater "type" to determine which function we need to call, and if needed to configure the height & width 		(halftone algorithm).
		
-> main.cpp: In the main loop we added more calls to setShapeTex and Draw in order to draw 4 different pictures.

-> Mesh.cpp: Configured the colors to display colors properly & flipped the image horizontally.

-> Game.cpp: Added 3 more Textures in the Init function.

-> Added lena256.jpg to the textures directory
