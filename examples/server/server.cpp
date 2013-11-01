#include <iostream>
#include <iterator>
#include <fstream>
#include <server/server.hpp>

int main()
{
  using namespace pwned::server;
  Server server("8080");

  server.Get("/", [](mg_event*, Params const &params) {
    return Server::response(
      "<a href='/add/42/42'>Add 42 + 42</a><br />"
      "<a href='/form'>Submit a form</a>", "text/html");
  });

  server.Get("/add/:first/:second", [](mg_event*, Params const &params){
    int first= std::stoi(params.at("first"));
    int second= std::stoi(params.at("second"));
    std::ostringstream out;
    out<< first<< " + "<< second<< " = "<< (first+ second)<< '\n';
    return Server::response(out.str());
  });

  server.Get("/form", [](mg_event*, Params const &) {
    return Server::response(
      "<form action='/submit' method='POST'>"
        "<input type='text' name='username' /><br />"
        "<input type='password' name='password' /><br />"
        "<input type='submit' />"
      "</form>", "text/html");
  });

  server.Post("/submit", [](mg_event*, Params const &params) {
    return Server::response(params.at("username")+ " "+ params.at("password"));
  });

  server.Get("/index.html", [](mg_event*, Params const &) {
    std::ifstream file("index.html", std::ios::binary| std::ios::ate);
    auto size= file.tellg();
    file.seekg(0, std::ios::beg);
    std::string buffer(size, 0);
    file.read(&buffer[0], size);
    return Server::response(buffer, "text/html");
  });

  std::cin.get();
}

