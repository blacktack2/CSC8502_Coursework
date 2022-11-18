#include "../NCLGL/window.h"
#include "Renderer.h"

int main()	{
	Window w("CSC8502 Coursework S.L.", 1280, 720, true);

	if(!w.HasInitialised())
		return -1;
	
	Renderer renderer(w);
	if(!renderer.HasInitialised())
		return -1;

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while(w.UpdateWindow()  && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)){
		renderer.UpdateScene(w.GetTimer()->GetTimeDeltaSeconds());
		renderer.RenderScene();
		renderer.SwapBuffers();
	}
	return 0;
}
