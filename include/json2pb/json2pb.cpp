#define PICOJSON_USE_INT64

#include <google/protobuf/message.h>

#include <json2pb/json2pb.h>
#include <picojson/picojson.h>

#include <cstdint>
#include <chrono>

#include <json2pb/base.h>

namespace json2pb {
    using base = encode::base;
    using base64 = encode::alphabet::base64;
    using date = std::chrono::system_clock::time_point;

    using google::protobuf::Message;
    using google::protobuf::MessageFactory;
    using google::protobuf::Descriptor;
    using google::protobuf::FieldDescriptor;
    using google::protobuf::EnumDescriptor;
    using google::protobuf::EnumValueDescriptor;
    using google::protobuf::Reflection;

    static picojson::value _pb2json(const Message &msg);

    static picojson::value _field2json(const Message &msg, const FieldDescriptor *field, size_t index) {
      const Reflection *ref = msg.GetReflection();
      const bool repeated = field->is_repeated();
      switch (field->cpp_type()) {
#define _CONVERT(type, ctype, sfunc, afunc)  \
  case FieldDescriptor::type: {   \
   const ctype value = (repeated)?  \
    ref->afunc(msg, field, index): \
    ref->sfunc(msg, field);  \
   return picojson::value(value);  \
  }

        _CONVERT(CPPTYPE_DOUBLE, double, GetDouble, GetRepeatedDouble);
        _CONVERT(CPPTYPE_FLOAT, float, GetFloat, GetRepeatedFloat);
        _CONVERT(CPPTYPE_INT64, int64_t, GetInt64, GetRepeatedInt64);
        _CONVERT(CPPTYPE_BOOL, bool, GetBool, GetRepeatedBool);

#undef _CONVERT
        case FieldDescriptor::CPPTYPE_UINT64: {
          const uint64_t value = (repeated) ? ref->GetRepeatedUInt64(msg, field, index) : ref->GetUInt64(msg, field);
          return picojson::value((int64_t) value);
        }

        case FieldDescriptor::CPPTYPE_INT32: {
          const uint64_t value = (repeated) ? ref->GetRepeatedInt32(msg, field, index) : ref->GetInt32(msg, field);
          return picojson::value((int64_t) value);
        }

        case FieldDescriptor::CPPTYPE_UINT32: {
          const uint64_t value = (repeated) ? ref->GetRepeatedUInt32(msg, field, index) : ref->GetUInt32(msg, field);
          return picojson::value((int64_t) value);
        }

        case FieldDescriptor::CPPTYPE_STRING: {
          std::string scratch;
          const std::string &value = (repeated) ?
                                     ref->GetRepeatedStringReference(msg, field, index, &scratch) :
                                     ref->GetStringReference(msg, field, &scratch);
          if (field->type() == FieldDescriptor::TYPE_BYTES)
            return picojson::value(base::encode<base64>(value));
          return picojson::value(value);
        }
        case FieldDescriptor::CPPTYPE_MESSAGE: {
          const Message &mf = (repeated) ?
                              ref->GetRepeatedMessage(msg, field, index) :
                              ref->GetMessage(msg, field);
          return _pb2json(mf);
        }
        case FieldDescriptor::CPPTYPE_ENUM: {
          const EnumValueDescriptor *ef = (repeated) ?
                                          ref->GetRepeatedEnum(msg, field, index) :
                                          ref->GetEnum(msg, field);
          return picojson::value((int64_t) ef->number());
        }
        default:
          break;
      }
      return picojson::value();
    }

    static picojson::value _pb2json(const Message &msg) {
      const Descriptor *d = msg.GetDescriptor();
      const Reflection *ref = msg.GetReflection();
      if (!d || !ref) return picojson::value();

      picojson::value::object root;

      std::vector<const FieldDescriptor *> fields;
      ref->ListFields(msg, &fields);

      for (size_t i = 0; i != fields.size(); i++) {
        const FieldDescriptor *field = fields[i];
        const std::string &name = (field->is_extension()) ? field->full_name() : field->name();
        if (field->is_repeated()) {
          size_t count = ref->FieldSize(msg, field);
          if (!count) continue;
          picojson::value::array arr;
          for (size_t j = 0; j < count; j++)
            arr.push_back(_field2json(msg, field, j));
          root.insert(std::make_pair(name, picojson::value(arr)));
        } else if (ref->HasField(msg, field)) {
          root.insert(std::make_pair(name, _field2json(msg, field, 0)));
        } else
          continue;
      }

      return picojson::value(root);
    }

    static void _json2pb(Message &msg, const picojson::value &root);

    static void _json2field(Message &msg, const FieldDescriptor *field, const picojson::value &jf) {
      const Reflection *ref = msg.GetReflection();
      const bool repeated = field->is_repeated();

      switch (field->cpp_type()) {
#define _SET_OR_ADD(sfunc, afunc, value)   \
  do {      \
   if (repeated)    \
    ref->afunc(&msg, field, value); \
   else     \
    ref->sfunc(&msg, field, value); \
  } while (0)


        case FieldDescriptor::CPPTYPE_BOOL: {
          if (!jf.is<bool>())
            throw j2pb_error(type_mismatch, "type mismatch! filed:\"" + field->name() + "\", not an boolean");
          bool value = jf.get<bool>();
          _SET_OR_ADD(SetBool, AddBool, value);
          break;
        }

        case FieldDescriptor::CPPTYPE_DOUBLE: {
          if (!jf.is<double>())
            throw j2pb_error(type_mismatch, "type mismatch! filed:\"" + field->name() + "\", not an number");
          double value = jf.get<double>();
          _SET_OR_ADD(SetDouble, AddDouble, value);
          break;
        }

        case FieldDescriptor::CPPTYPE_FLOAT: {
          if (!jf.is<double>())
            throw j2pb_error(type_mismatch, "type mismatch! filed:\"" + field->name() + "\", not an number");
          float value = static_cast<float>(jf.get<double>());
          _SET_OR_ADD(SetFloat, AddFloat, value);
          break;
        }

        case FieldDescriptor::CPPTYPE_INT64: {
          if (!jf.is<int64_t>())
            throw j2pb_error(type_mismatch, "type mismatch! filed:\"" + field->name() + "\", not an number");
          int64_t value = jf.get<int64_t>();
          _SET_OR_ADD(SetInt64, AddInt64, value);
          break;
        }

        case FieldDescriptor::CPPTYPE_UINT64: {
          if (!jf.is<int64_t>())
            throw j2pb_error(type_mismatch, "type mismatch! filed:\"" + field->name() + "\", not an number");
          uint64_t value = static_cast<uint64_t>(jf.get<int64_t>());
          _SET_OR_ADD(SetUInt64, AddUInt64, value);
          break;
        }

        case FieldDescriptor::CPPTYPE_INT32: {
          if (!jf.is<int64_t>())
            throw j2pb_error(type_mismatch, "type mismatch! filed:\"" + field->name() + "\", not an number");
          int32_t value = static_cast<int32_t>(jf.get<int64_t>());
          _SET_OR_ADD(SetInt32, AddInt32, value);
          break;
        }

        case FieldDescriptor::CPPTYPE_UINT32: {
          if (!jf.is<int64_t>())
            throw j2pb_error(type_mismatch, "type mismatch! filed:\"" + field->name() + "\", not an number");
          uint32_t value = static_cast<uint32_t>(jf.get<int64_t>());
          _SET_OR_ADD(SetUInt32, AddUInt32, value);
          break;
        }

        case FieldDescriptor::CPPTYPE_STRING: {
          if (!jf.is<std::string>())
            throw j2pb_error(type_mismatch, "type mismatch! filed:\"" + field->name() + "\", not an string");
          std::string value = jf.get<std::string>();
          if (field->type() == FieldDescriptor::TYPE_BYTES)
            _SET_OR_ADD(SetString, AddString, base::decode<base64>(value));
          else
            _SET_OR_ADD(SetString, AddString, value);
          break;
        }

        case FieldDescriptor::CPPTYPE_MESSAGE: {
          if (!jf.is<picojson::value::object>())
            throw j2pb_error(type_mismatch, "type mismatch! filed:\"" + field->name() + "\", not an object");
          Message *mf = (repeated) ?
                        ref->AddMessage(&msg, field) :
                        ref->MutableMessage(&msg, field);
          _json2pb(*mf, jf);
          break;
        }

        case FieldDescriptor::CPPTYPE_ENUM: {
          const EnumDescriptor *ed = field->enum_type();
          const EnumValueDescriptor *ev = 0;
          if (jf.is<int64_t>()) {
            ev = ed->FindValueByNumber(static_cast<int>(jf.get<int64_t>()));
            if (!ev)
              throw j2pb_error(value_mismatch, "value_mismatch! filed:\"" + field->name() + "\", enum value:\"" +
                                               std::to_string(jf.get<int64_t>()) + "\" not found");
          } else if (jf.is<std::string>()) {
            ev = ed->FindValueByName(jf.get<std::string>());
            if (!ev)
              throw j2pb_error(value_mismatch, "value_mismatch! filed:\"" + field->name() + "\", enum value:\"" +
                                               jf.get<std::string>() + "\" not found");
          } else
            throw j2pb_error(type_mismatch, "type_mismatch! filed:\"" + field->name() + "\", not an integer or string");

          _SET_OR_ADD(SetEnum, AddEnum, ev);
          break;
        }
        default:
          break;
      }
    }

    static void _json2pb(Message &msg, const picojson::value &root) {
      const Descriptor *d = msg.GetDescriptor();
      const Reflection *ref = msg.GetReflection();

      if (!d)
        throw j2pb_error(no_descriptor, "proto miss descriptor.");

      if (!ref)
        throw j2pb_error(no_reflection, "proto miss reflection.");

      if (!root.is<picojson::value::object>())
        throw j2pb_error(format_error, "not an object.");

      for (int i = 0; i < d->field_count(); ++i) {
        const FieldDescriptor *field = d->field(i);
        const std::string name = field->name();

        picojson::value val = root.get(name);

        if (val.is_null())
          throw j2pb_error(miss_field, "miss filed: \"" + name + "\".");

        if (field->is_repeated()) {
          if (!val.is<picojson::value::array>())
            throw j2pb_error(type_mismatch, "type_mismatch! filed:\"" + field->name() + "\", not an array");

          for (const auto &item : val.get<picojson::array>()) {
            _json2field(msg, field, item);
          }
        } else
          _json2field(msg, field, val);
      }
    }

    void json2pb(Message &msg, const std::string &in) {
      picojson::value root;

      std::string err = picojson::parse(root, in);

      if (!err.empty())
        throw j2pb_error(parse_error, err);

      _json2pb(msg, root);
    }

    void pb2json(std::string &out, const google::protobuf::Message &msg) {
      picojson::value root = _pb2json(msg);
      out = root.serialize();
    }
}