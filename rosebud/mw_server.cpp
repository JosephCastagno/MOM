#include <boost/asio>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <string>

using boost::asio::ip::tcp;

class mw_server_t {
 private:
     tcp::acceptor m_acceptor;
     std::unordered_map<std::string, std::unordered_set<tcp::socket*>>
         m_topic_to_subscriptions;
