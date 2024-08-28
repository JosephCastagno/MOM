#include "../wolfie/actor.hpp"

class order_placer_t : public actor_t {
 private:
     int m_threshold;
     uint8_t m_orders_placed;

     void handle_market_data(const market_data_t &);

 public:
     order_placer_t(const std::string &mw_ip,
                    const int mw_port,
                    const int threshold);
}; // order_placer_t

 
