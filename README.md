# Message Oriented Middleware
This project is a Message Oriented Middleware (MOM) implemened with actors. An actor is an abstraction of a thread; actor model systems tend to scale better than other multithreaded systems that lock shared resources. Each actor has its own message queue from which it continuously processes messages. Each message is handled according to logic defined within the actor. Actors can publish and subscribe to messages corresponding to a particular topic to communicate with other actors. When an actor publishes a message, it is sent to a TCP-based middleware server which is responsible for forwarding the message to all actors that are subscribed to that topic. Actors can also create other actors, shut down other actors, and shut down themselves.  

## althea
`althea` contains implementations of the middleware server, the actor, and also defines the message protocol.

### rosebud
`rosebud` contains the middleware server and the tcp connection abstraction. To start the middleware server, run:

`./althea/build/rosebud <PORT>`

Where `<PORT>` is the port on which the server will accept TCP connections.
- `mw_server_t` class: Responsible for managing incoming client connections using `boost.asio`. It accepts connections and routes messages through the middleware system.
- `tcp_conn_t` class: Manages individual TCP connections between the server and clients. It handles sending messages from the server to the client.
- `run.cpp` is the entrypoint for the middleware server.

### wolife
`wolfie` contains the base actor, the message queue, and two additional providers: namely `mw_provider` and `timeout_provider`.
- `actor_t` class: Serves as the base class for different types of actors in the system. It handles the reception of messages and interaction with the `message_queue_t`.
- `message_queue_t` class: Responsible for queueing messages between actors. It ensures thread-safe access for enqueuing and dequeuing messages.
- `mw_provider_t` class: Handles the connection setup and message forwarding logic using `boost.asio`. It listens for incoming messages from the network and places them into the associated message queue. The `mw_provider_t` requires the IP address and Port that the middleware server is running on.
- `timeout_provider_t` class: allows actors to send themselves alert messages at a certain time interval.

### msg
`msg` defines the messaging protocol used by the actors. Messages are xml-based using `pugixml`.
- `message_t` class: Defines the structure and attributes of messages exchanged between actors and the middleware. It serves as the core message data structure for communication.
- `message_types.cpp`: Contains definitions for different message types used in the system, facilitating easy handling and categorization of messages and their data.
- `message_utils.hpp`: Utility functions for processing and manipulating messages and timestamps.

# Usage Example
The `exchange` and `order_placer` directories serve as an example of how the system works. The `exchange` publishes order data, and the `order_placer` subscribes to this market data, and determines whether or not to place an order depending on the market data received. In the example, two order placers are created: one that shuts itself down after placing 10 orders, and another that gets shut down by the exchange after placing 5 orders.

## exchange
The `exchange` directory contains an actor class `exchange_t` that mocks an exchange. The exchange generates random prices every second using the `timeout_provider_t`, and publishes them under the `market_data` topic to simulate an exchange broadcasting the bbo. To start the exchange, run:

`./exchange/exchange`

Prereq: the middleware server is running. The exchange will run continuously until `q` is read from `stdin`.

Subscribes to:
- `pulse`
- `order`

Publishes on:
- `market_data`
- `shutdown`

## order_placer
The `order_placer` directory contains an actor class `order_placer_t` that mocks an order placing engine. The order placer is initialized with some maximum price threshold for which it will place an order. If the price is below the threshold, it issues an `order` message to notify the exchange of its intent to buy the stock. The order_placer also periodically tests its latency with the exchange. It publishes a `pulse` message with a timestamp, and when the exchange receives the `pulse` message, it publishes a `heartbeat` message with another timestamp denoting when the `pulse` message was received. When the order_placer receives the `heartbeat` message, it calculates the time taken for the To run two `order_placer_t` actors, run:

`./order_placer/order_placer`

Prereq: the middleware server is running. The process will exit when both `order_placer_t` actors are shut down.

Subscribes to:
- `heartbeat`
- `market_data`
- `shutdown`

Publishes on:
- `order`
- `pulse`

## TODO
- Graceful shutdown of middleware server
- Storage of messages processed by middleware server
- Comprehensive testing suite (testing middleware server, exchange, and order_placer all on separate hosts!)
- Improve compile time efficientcy
