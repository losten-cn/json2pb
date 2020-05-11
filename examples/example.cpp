//
// Created by Administrator on 2020/5/9.
//

#include "json2pb/json2pb.h"
#include "proto/test.pb.h"
#include <iostream>

using google::protobuf::Message;

int main() {
  std::string json(
          "{"
          " \"_str\": \"b\","
          " \"_bin\": \"0a0a0a0a\","
          " \"_bool\": true,"
          " \"_float\": 1,"
          " \"sub\": {"
          "  \"field\": \"subfield\","
          "  \"echo\": [{"
          "   \"text\": \"first\""
          "  }, {"
          "   \"text\": \"second\""
          "  }]"
          " },"
          " \"_int\": [10, 20, 30, 40],"
          " \"_enum\": [\"VALUE1\", 20],"
          " \"str_list\": [\"v0\", \"v1\"],"
          " \"test.e_bool\": false"
          "}");

  try {
    test::ComplexMessage msg;
    json2pb::json2pb(msg, json);

    json.clear();
    json2pb::pb2json(json, msg);

    std::cout << "Message: " << msg.DebugString() << std::endl;
    std::cout << "Json:" << json << std::endl;
  } catch (json2pb::j2pb_error e) {
    std::cerr << "error: " << e.what() << std::endl;
  }
  return 0;
}