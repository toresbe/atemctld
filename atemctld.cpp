#include "pistache/endpoint.h"
#include <exception>
#include <iostream>
#include "pistache/http.h"
#include <pistache/router.h>

#include <json.hpp>
using json = nlohmann::json;

#include "atem.hpp"

using namespace Pistache;

class ATEMHandler {
public:
    Rest::Router router;
    ATEM atem;

    void hello(const Rest::Request& request, Http::ResponseWriter response) {
        response.send(Http::Code::Ok, "Hello, world!");
    };

    void get_program(const Rest::Request& request, Http::ResponseWriter response) {
        auto me_index = request.param(":me_index").as<int>();
        if(me_index) throw std::out_of_range("Only 1 M/E is currently supported!");

        auto input_index = atem.MixEffects.get_program(me_index);

        json current_program;
        current_program["me_index"] = me_index;
        current_program["input_index"] = input_index;

        auto mime = Http::Mime::MediaType::fromString("application/json");
        response.send(Http::Code::Ok, current_program.dump(), mime);
    };

    void set_program(const Rest::Request& request, Http::ResponseWriter response) {
        auto me_index = request.param(":me_index").as<int>();
        if(me_index) throw std::out_of_range("Only 1 M/E is currently supported!");
        std::cout << "Request body:\n" << request.body() << "-----\n";

        json incoming_request = json::parse(request.body());
        std::cout << incoming_request.dump(4) << "\n";
        auto input_index = incoming_request["input_index"].get<uint16_t>();

        atem.MixEffects.set_program(input_index);

        json current_program;
        current_program["me_index"] = me_index;
        current_program["input_index"] = input_index;

        auto mime = Http::Mime::MediaType::fromString("application/json");
        response.send(Http::Code::Ok, current_program.dump(), mime);
    }

    void setupRoutes() {
        using namespace Rest;
        Routes::Get(router, "/hello", Routes::bind(&ATEMHandler::hello, this));
        Routes::Get(router, "/me/:id", Routes::bind(&ATEMHandler::hello, this));
        Routes::Get(router, "/me/:me_index/transition", Routes::bind(&ATEMHandler::hello, this));
        Routes::Get(router, "/me/:me_index/preview", Routes::bind(&ATEMHandler::hello, this));
        Routes::Post(router, "/me/:me_index/preview", Routes::bind(&ATEMHandler::hello, this));
        Routes::Get(router, "/me/:me_index/program", Routes::bind(&ATEMHandler::get_program, this));
        Routes::Post(router, "/me/:me_index/program", Routes::bind(&ATEMHandler::set_program, this));
    }

    void init() {
        atem.connect("10.3.2.1");
        Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));
        Http::Endpoint server(addr);
        auto opts = Pistache::Http::Endpoint::options().threads(1);
        server.init(opts);
        setupRoutes();
        server.setHandler(router.handler());
        server.serve();
    };
};

int main() {
    ATEMHandler handler;
    handler.init();
}
