/*
 * Copyright (c) 2013 Pavel Shramov <shramov@mexmat.net>
 *
 * json2pb is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#ifndef __JSON2PB_H__
#define __JSON2PB_H__

#include <string>
#include <stdexcept>

namespace google {
    namespace protobuf {
        class Message;
    }
}

namespace json2pb {
    enum throw_error_code {
      unknown = 0,

      // json error
      parse_error,
      format_error,
      miss_field,
      type_mismatch,
      value_mismatch,

      // proto error
      no_descriptor,
      no_reflection,
    };

    class j2pb_error : public std::runtime_error {
        throw_error_code _code = unknown;
        std::string _error;
    public:
        j2pb_error(const throw_error_code &error_code_, const std::string &info_)
                : std::runtime_error(info_),
                  _code(error_code_),
                  _error(info_) {}

        virtual ~j2pb_error() throw() {};

        throw_error_code code() { return _code; }

        virtual const char *what() const throw() { return _error.c_str(); };
    };


    void json2pb(google::protobuf::Message &msg, const std::string &in);

    void pb2json(std::string &out, const google::protobuf::Message &msg);
}
#endif//__JSON2PB_H__
