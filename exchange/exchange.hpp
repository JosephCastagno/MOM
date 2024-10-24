#include "../althea/wolfie/actor.hpp"
#include "../althea/wolfie/timeout_provider.hpp"

#include <random>

class exchange_t : public actor_t {
 private:
     std::mt19937 m_random_engine;
     std::uniform_int_distribution<int> m_distribution;
     timeout_provider_t m_timeout_pro;
     std::unordered_map<std::string, int> m_participant_to_count;

     void handle_pulse(const message_t &);
     void handle_order_received(const message_t &);
     void handle_timeout_alert();
     void handle_message(const message_t &) override;
     void mw_setup() override;

 public:
     exchange_t(const std::string &mw_ip, 
                const int mw_port);
     void shutdown();
}; // exchange_t  
