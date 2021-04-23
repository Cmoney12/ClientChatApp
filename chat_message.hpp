//
// Created by corey on 3/17/21.
//

#ifndef CLIENTCHATAPP_CHAT_MESSAGE_HPP
#define CLIENTCHATAPP_CHAT_MESSAGE_HPP

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

class chat_message {
public:
    enum {HEADER_SIZE = 5 };

    chat_message(): body_length_(0)
    {
    }

    ~chat_message() {
        if(data_) {
            delete[] data_;
        }
    }

    const char* data() const
    {
        return data_;
    }

    char* data()
    {
        return data_;
    }

    std::size_t length() const
    {
        return HEADER_SIZE + body_length_;
    }

    const char* body() const
    {
        return data_ + HEADER_SIZE;
    }

    char* body()
    {
        return data_ + HEADER_SIZE;
    }

    std::size_t body_length() const
    {
        return body_length_;
    }

    void display_json() const {
        pt::write_json(std::cout, root);
    }

    void body_length(std::size_t new_length)
    {
        body_length_ = new_length;
        data_ = new char[HEADER_SIZE + body_length_];
        if (body_length_ > MAXIMUM_MESSAGE_SIZE)
            body_length_ = MAXIMUM_MESSAGE_SIZE;
    }

    std::string write_json() {
        std::ostringstream oss;
        std::string message;
        pt::write_json(oss, root);
        body_length_ = oss.str().size();
        if (body_length_ <= MAXIMUM_MESSAGE_SIZE) {
            std::string str_size = std::to_string(body_length_);

            if (str_size.length() < HEADER_SIZE) {
                std::string zeros = (str_size.size() - HEADER_SIZE, "0");
                str_size = zeros + str_size;
                message = str_size + oss.str();
            }
        }
        return message;
    }

    void encode_header()
    {
        char header[HEADER_SIZE + 1] = "";
        std::sprintf(header, "%5d", static_cast<int>(body_length_));
        //set data size
        std::memcpy(data_, header, HEADER_SIZE);
    }

    std::vector<std::string> read_json(std::string& message) {
        std::istringstream is(message);
        pt::read_json(is, root);
        std::string to = root.get<std::string>("Header.To");
        std::string from = root.get<std::string>("Header.From");
        std::string type = root.get<std::string>("Contents.Type");
        std::string content = root.get<std::string>("Contents.Body");

        std::vector<std::string> parsed_json;
        parsed_json.push_back(to);
        parsed_json.push_back(from);
        parsed_json.push_back(type);
        parsed_json.push_back(content);
        return parsed_json;
    }


    bool decode_header()
    {
        char header[HEADER_SIZE + 1] = "";
        std::strncat(header, data_, HEADER_SIZE);
        body_length_ = std::atoi(header);
        if (body_length_ > MAXIMUM_MESSAGE_SIZE)
        {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    static std::string json_write(const std::string& recipient, const std::string& deliverer,
                                  const std::string& body, const std::string& type = "") {

        boost::property_tree::ptree pt;
        pt.put("Header.To", recipient);
        pt.put("Header.From", deliverer);
        pt.put("Contents.Type", type);
        pt.put("Contents.Body", body);

        std::stringstream ss;
        boost::property_tree::write_json(ss, pt);
        std::string json = ss.str();
        ss.clear();

        return json;

    }

private:
    std::size_t body_length_;
    enum { MAXIMUM_MESSAGE_SIZE = 99999 };
    char* data_;
    pt::ptree root;
};
#endif //CLIENTCHATAPP_CHAT_MESSAGE_HPP
