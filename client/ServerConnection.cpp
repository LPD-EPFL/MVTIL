#include "ServerConnection.h"

ServerConnection::ServerConnection(std::string host, int port) {
    socket = new TSocket(host,port);
    transport = new TBufferedTransport(socket);
    protocol = new TBinaryProtocol(transport);
    client = new DataServerClient(protocol);
    transport->open(); //TODO is it a good idae to keep the transprot open all the time?

}

ServerConnection::~ServerConnection() {
    if (tranport->isOpen()) {
        transport->close();
    }
}

