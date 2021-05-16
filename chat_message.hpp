//
// Created by corey on 3/17/21.
//

#ifndef CLIENTCHATAPP_CHAT_MESSAGE_HPP
#define CLIENTCHATAPP_CHAT_MESSAGE_HPP

#include <iostream>
#include <cstring>
#include <bson.h>
#include <zstd.h>
#include <chrono>


class message {
public:
    const char* Deliverer;
    const char* Receiver;
    const char* Content_Type;
    // const uint8_t** File_Data;
    const char* Text_Message;
};

class chat_message: public message {
public:

    chat_message() : message() {}

    ~chat_message() {
        delete[] file_buffer;
        delete[] cc_buff;
        delete[] data_;
        bson_free(bson_str);
    }

    uint8_t* data() const
    {
        return data_;
    }

    const uint8_t* data()
    {
        return data_;
    }

    std::size_t length() const
    {
        return HEADER_LENGTH + body_length_;
    }

    uint8_t* body()
    {
        return data_ + HEADER_LENGTH;
    }

    uint8_t* body() const
    {
        return data_ + HEADER_LENGTH;
    }

    std::size_t body_length() const
    {
        return body_length_;
    }

    void set_size(std::size_t size) {
        body_length_ = size;
        data_ = new uint8_t [length() + 1];
    }

    void read_file(const char* file_name) {

        FILE* in_file = fopen(file_name, "rb");

        if (in_file == nullptr) {
            return;
        }

        fseek(in_file, 0L, SEEK_END);

        std::size_t const file_size = ftell(in_file);

        rewind(in_file);

        file_buffer = new unsigned char[file_size];
        size_t read_size = fread(file_buffer, 1, file_size, in_file);

        if (!read_size) {
            return;
        }

        fclose(in_file);

        size_t const c_buff_size = ZSTD_compressBound(file_size);

        cc_buff = new unsigned char [c_buff_size];
        c_size = ZSTD_compress(cc_buff, c_buff_size, file_buffer, file_size, 5);

    }

    unsigned char * decompress(unsigned char* data) {

        unsigned long long const rSize = ZSTD_getFrameContentSize(data, c_size);
        auto* decompressed = new unsigned char[rSize];

        dSize = ZSTD_decompress(decompressed, rSize, data, c_size);

        if (dSize == c_size) {
            std::cout << "Success" << std::endl;
        }
        else {
            std::cout << "decompressed size " << dSize << " Compressed size " << c_size << std::endl;
        }
        return decompressed;
    }

    const uint8_t* create_bson(char* receiver, char* deliverer, char* type, char* text = nullptr) {

        bson_t document;
        bson_init(&document);
        bson_append_utf8(&document, "Receiver", -1, receiver, -1);
        bson_append_utf8(&document, "Deliverer", -1, deliverer, -1);
        bson_append_utf8(&document, "Type", -1, type, -1);
        if(cc_buff != nullptr) {
            bson_append_binary(&document, "Data", -1, BSON_SUBTYPE_BINARY, cc_buff, c_size);
            bson_append_int32(&document, "Size", -1, static_cast<int>(c_size));
        }

        else if(text != nullptr)
            bson_append_utf8(&document, "Data", -1, text, -1);

        body_length_ = document.len;

        bson = bson_get_data(&document);

        bson_destroy(&document);

        return bson;

    }

    bool decode_header() {
        body_length_ = std::atoi((char*)header);
        std::cout << "size " << body_length_ << std::endl;
        set_size(body_length_);
        std::memcpy(data_, header, HEADER_LENGTH);
        if(body_length_ > MAX_MESSAGE_SIZE) {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void parse_bson(const uint8_t *bson_data, std::size_t size) {

        const bson_t *received;
        bson_reader_t *reader;
        bson_iter_t iter;
        // size_t size1 = 0;
        // uint32_t* binary_len = nullptr;
        char text_type[] = "Text";
        const char* str;

        for(int i = 0; i < body_length_; i++)
            std::cout << &bson_data[i];

        reader = bson_reader_new_from_data(bson_data, size);

        //str = bson_as_canonical_extended_json(received, nullptr);
        //std::cout << str << std::endl;
        received = bson_reader_read(reader, nullptr);

        if (bson_iter_init_find(&iter, received, "Receiver") && BSON_ITER_HOLDS_UTF8(&iter)) {
            Receiver = bson_iter_utf8(&iter, nullptr);
        }

        if (bson_iter_init_find(&iter, received, "Deliverer") && BSON_ITER_HOLDS_UTF8(&iter)) {
            Deliverer = bson_iter_utf8(&iter, nullptr);
        }

        if (bson_iter_init_find(&iter, received, "Type") && BSON_ITER_HOLDS_UTF8(&iter)) {
            Content_Type = bson_iter_utf8(&iter, nullptr);
        }

        if (bson_iter_init_find(&iter, received, "Data") && BSON_ITER_HOLDS_UTF8(&iter)) {
            if (Content_Type && std::strcmp(Content_Type, text_type) == 0)
                Text_Message = bson_iter_utf8(&iter, nullptr);
            //else {
            //     bson_iter_binary(&iter, nullptr, binary_len, File_Data);
           // }
        }

        bson_reader_destroy(reader);

    }

    bool encode_header() const {
        std::cout << "Body Length " << body_length_ << std::endl;
        if (body_length_ <= MAX_MESSAGE_SIZE && body_length_) {

            uint8_t header_size[HEADER_LENGTH + 1] = "";
            std::sprintf((char*)header_size, "%4d", static_cast<int>(body_length_));
            std::memcpy((char*)data_, header_size, HEADER_LENGTH);
            return true;
        }
        return false;
    }

    const uint8_t *bson{};
    uint8_t* data_{};
    char* bson_str{};
    std::size_t body_length_{};
    uint8_t* cc_buff = nullptr;
    unsigned char* file_buffer{};
    enum { MAX_MESSAGE_SIZE = 9999 };
    enum { HEADER_LENGTH = 4 };
    uint8_t header[HEADER_LENGTH + 1]{};
    std::size_t dSize{};
    std::size_t c_size{};
};

#endif //CLIENTCHATAPP_CHAT_MESSAGE_HPP
