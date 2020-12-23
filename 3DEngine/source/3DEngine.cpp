// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Headers/My3DEngine.h"


int main()
{
	MyEngine3d demo;
	if (demo.ConstructConsole(256, 240, 4, 4))
		demo.Start();
}
