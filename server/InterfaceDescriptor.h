
class InterfaceDescriptor {
    public:
        InterfaceDescriptor(std::string configFile);
        ~InterfaceDescriptor();

        bool conflictsWith(Operation *operation, Version* version);

    private:
        // the conflict table, encoded operation attributes
        
}
