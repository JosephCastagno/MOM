#include "../althea/wolfie/actor.hpp"

class order_placer_t : public actor_t {
 private:
     int m_threshold;
     int m_orders_placed;

     void handle_market_data(const message_t &);
     void handle_heartbeat(const message_t &);
     void handle_shutdown(const message_t &);

     void handle_message(const message_t &) override;
     void mw_setup() override;

 public:
     order_placer_t(const std::string &mw_ip,
                    const int mw_port,
                    const int threshold,
                    const std::string &placer_name);
}; // order_placer_t

 
