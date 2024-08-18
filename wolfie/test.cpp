#include <iostream>
#include <string>

#include "../build/messages.pb.cc"

void UserLogin() {
    Message message;
    message.set_topic("login");

    Login login;
    login.set_username("rosebud");
    login.set_admin(true);

    std::string payload;
    login.SerializeToString(&payload);
    message.set_payload(payload);

    std::string serialized_message;
    message.SerializeToString(&serialized_message);
    
    std::cout << "serialized message: " << serialized_message << std::endl;
}

int main() {
    UserLogin(); 
}
