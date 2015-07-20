
class InterfaceDescriptor {
    public:
        InterfaceDescriptor(ConfigFile config);
        ~InterfaceDescriptor();

        bool conflictsWith(Operation *operation, Version* version);

    private:
        // the conflict table, encoded operation attributes
        
}
