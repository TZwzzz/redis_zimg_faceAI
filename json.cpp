#include "json.h"

using json = nlohmann::json;


std::string ret_rect_to_json(const std::vector<ret_rect>& rects) 
{
    json j;
    j["size"] = rects.size();
    for (const auto& ret_rect : rects) {
        json r;
        r["topLx"] = ret_rect.topLx;
        r["topLy"] = ret_rect.topLy;
        r["width"] = ret_rect.width;
        r["height"] = ret_rect.height;
        r["threshold"] = ret_rect.threshold;
        j["ret_rects"].push_back(r);
    }
    auto json_str = j.dump();
    get_jsonLogs()->info("ret_rect_to_json() return: {}", json_str);
    return json_str;
}

