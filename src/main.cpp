#include "sandbox/Application.hpp"

#include <curl/curl.h>

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);

    sandbox::Application app;
    app.run();

    curl_global_cleanup();

    return 0;
}
