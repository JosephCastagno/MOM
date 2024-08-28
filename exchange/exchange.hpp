#include "../wolfie/actor.hpp"
#include "../wolfie/timeout_provider.hpp"

class exchange_t : public actor_t {
 private:
     timeout_provider_t m_timeout_pro;
     void generate_market_data(const market_data_t &);
     void handle_shutdown();

 public:
     exchange_t(const std::string &mw_ip, 
                const int mw_port);
} // exchange_t  
