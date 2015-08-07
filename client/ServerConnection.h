/* 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _SERVER_CONNECTION_H_
#define _SERVER_CONNECTION_H_

#include <mutex>
#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "DataServer.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace ::TxProtocol;

class ServerConnection{
    public:
        ServerConnection(std::string host, int port);
        ~ServerConnection();
        DataServerClient* client;
        inline void lock() {
            mutex.lock();
        }
        inline void unlock() {
            mutex.unlock();
        }

        inline void ensureOpen() {
            if (!transport->isOpen()) {
                transport->open();
            }
        }

        std::string host;
        int port;

    private:
        std::mutex mutex;
        boost::shared_ptr<TSocket> socket;
        boost::shared_ptr<TTransport> transport;
        boost::shared_ptr<TProtocol> protocol;

};

struct ServerConnectionHasher
{
    friend class ServerConnection;
    std::size_t operator()(const ServerConnection& sc) const
    {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<string>()(sc.host+std::to_string(sc.port)));
    }
};
#endif
