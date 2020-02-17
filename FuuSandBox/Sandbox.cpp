//
//  Sandbox.cpp
//  FuuSandbox
//
//  Created by Derek Boucher on 2019-04-19.
//

#include "Sandbox.h"

int main(int argc, char** argv)
{
	Sandbox* sandbox = new Sandbox();
	sandbox->run();
	delete sandbox;
}
