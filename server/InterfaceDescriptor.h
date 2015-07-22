#ifndef _INTERFACE_DESCRIPTOR_H_
#define _INTERFACE_DESCRIPTOR_H_

class InterfaceDescriptor {
    public:
        InterfaceDescriptor(std::string configFile);
        ~InterfaceDescriptor();

        bool conflictsWith(Operation *operation, Version* version);

    private:
        // the conflict table, encoded operation attributes
        
}

#endif
