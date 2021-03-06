// **********************************************************************
//
// Copyright (c) 2003-2017 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
#include <TestCommon.h>
#include <Callback.h>

using namespace std;
using namespace Test;

namespace
{

class Server : public Ice::Application
{
public:

    virtual int run(int, char*[]);
};

class CallbackI : public Callback
{

public:

    virtual void
    initiateCallback(ICE_IN(CallbackReceiverPrxPtr) proxy, const Ice::Current& current)
    {
        proxy->callback(current.ctx);
    }

    virtual void
    shutdown(const Ice::Current& current)
    {
        current.adapter->getCommunicator()->shutdown();
    }
};

}

int
Server::run(int, char**)
{
    communicator()->getProperties()->setProperty("DeactivatedAdapter.Endpoints", getTestEndpoint(communicator(), 1));
    communicator()->createObjectAdapter("DeactivatedAdapter");

    communicator()->getProperties()->setProperty("CallbackAdapter.Endpoints", getTestEndpoint(communicator(), 0));
    Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("CallbackAdapter");
    adapter->add(ICE_MAKE_SHARED(CallbackI), Ice::stringToIdentity("callback"));
    adapter->activate();
    communicator()->waitForShutdown();

    return EXIT_SUCCESS;
}

int
main(int argc, char* argv[])
{
    Server app;
    Ice::InitializationData initData = getTestInitData(argc, argv);
    return app.main(argc, argv, initData);
}

