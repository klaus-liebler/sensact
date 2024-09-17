    
    std::vector<sensact::iHost *> hosts{
        new cApplicationHost(halobj, nodemaster, canMBP),
        new cGatewayHost(),
        new cIoHost(),
    };
