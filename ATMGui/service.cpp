#include "service.h"

Service::Service()
{

}

Service::~Service()
{

}

void Service::slotButtonInstance(int b)
{
    if(b == 1)
    {
        emit signalATMState(true);
    }
    else if(b == 2)
    {
       emit signalUpdateATMScreen("\n\n\nEnter Amount : ");
    }
    else
    {
        emit signalATMState(false);
    }

}
