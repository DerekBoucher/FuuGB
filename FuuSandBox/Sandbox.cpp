//
//  Sandbox.cpp
//  FuuSandbox
//
//  Created by Derek Boucher on 2019-04-19.
//

#include "FuuGB.h"

class Sandbox : public FUUGB_APP
{
public:
    Sandbox()
    {
        
    }
    ~Sandbox()
    {
        
    }
};

int main()
{
    Sandbox* sandbox = new Sandbox();
    sandbox->run();
    delete sandbox;
}
