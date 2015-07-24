#ifndef _INTERFACE_DESCRIPTOR_H_
#define _INTERFACE_DESCRIPTOR_H_

class Operation {
    public:
        bool reads;
        bool writes;
        std::string operationName;
        Value* performOperation(Value* current);
};

class InterfaceDescriptor {
    public:
        InterfaceDescriptor(std::string configFile);
        ~InterfaceDescriptor();

        bool conflictsWith(Operation *operation, Version* version);

    private:
        // the conflict table, encoded operation attributes
        
}

#endif
