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

#ifndef _ROUTING_SERVICE_H_
#define _ROUTING_SERVICE_H_

#include <vector>
#include <thrift/transport/TSocket>

using namespace apache::thrift;


class RoutingService {
    public:
        RoutingService(FILE* configFile); //configFile contains the list of servers
        ~RoutingService();

        shared_ptr<TSocket> getServer(Key k);

    private:
        std::vector<shared_ptr<TSocket>> servers;
};

#endif
