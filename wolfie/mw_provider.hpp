#include <iostream>
#include <string>
#include <thread>  // ?
#include <unordered_set>
#include <boost/asio.hpp>

#include "actor.hpp"

using boost::asio::ip::tcp;

class mw_provier_t : public actor_t {
 private:
     void receive_messages(); 

 public:
     mw_provider_t(std::string name) : actor_t(std::move(name)) {}

}; // mw_provider_t
