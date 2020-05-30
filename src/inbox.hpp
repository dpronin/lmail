#pragma once

#include <deque>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <utility>

#include <boost/range/algorithm/find_if.hpp>

#include "application.hpp"
#include "inbox_message.hpp"
#include "profile.hpp"
#include "utility.hpp"
#include "color.hpp"

namespace lmail
{

class Inbox
{
public:
    using messages_t = std::deque<InboxMessage>;

public:
    explicit Inbox(std::shared_ptr<Profile> profile) : profile_(std::move(profile))
    {
        if (!profile_)
            throw std::invalid_argument("profile provided cannot be empty");
    }

public:
    std::pair<size_t, size_t> sync(std::vector<std::tuple<msg_id_t, topic_t, bool, username_t>> records)
    {
        size_t old_messages = messages_.size();
        size_t new_messages = 0;
        for (auto &record : records)
            new_messages += sync(std::tuple_cat(std::move(record), std::tuple<body_t>{})).second;
        return {old_messages, new_messages};
    }

    void sync(msg_idx_t msg_idx, std::tuple<msg_id_t, topic_t, bool, username_t, body_t> message)
    {
        if (0 < msg_idx && msg_idx <= messages_.size())
            sync(messages_.begin() + msg_idx - 1, std::move(message));
    }

    void show_topics(std::ostream &out) const
    {
        msg_idx_t msg_idx = 0;
        for (auto const &msg : messages_)
            show_topic(++msg_idx, msg, out);
    }

    std::optional<msg_id_t> erase(msg_idx_t msg_idx)
    {
        std::optional<msg_id_t> msg_id;
        if (0 == msg_idx || msg_idx > messages_.size())
            return msg_id;
        auto msg_it = messages_.begin() + msg_idx - 1;
        msg_id      = msg_it->id;
        messages_.erase(msg_it);
        return msg_id;
    }

    void show(msg_idx_t msg_idx, std::ostream &out) const
    {
        if (1 <= msg_idx && msg_idx <= messages_.size())
            show(msg_idx, messages_[msg_idx - 1], out);
    }

    std::optional<msg_id_t> find(msg_idx_t msg_idx) const
    {
        std::optional<msg_id_t> msg_id;
        if (1 <= msg_idx && msg_idx <= messages_.size())
            msg_id = messages_[msg_idx - 1].id;
        return msg_id;
    }

private:
    void show_topic(msg_idx_t idx, InboxMessage const &msg, std::ostream &out) const
    {
        auto const params = cypher_params(msg);
        if (!params.second)
            return;
        out << '\t' << idx << ". (from " << clblue(msg.user_from) << ") Topic: "
            << (params.first ? decrypt(msg.topic, *params.first) : msg.topic)
            << '\n';
    }

    void show(msg_idx_t idx, InboxMessage const &msg, std::ostream &out) const
    {
        auto const params = cypher_params(msg);
        if (!params.second)
            return;
        out << "\tIndex: " << idx
            << '\n';
        out << "\tFrom: " << clblue(msg.user_from)
            << '\n';
        out << "\tTopic: " << (params.first ? decrypt(msg.topic, *params.first) : msg.topic)
            << "\n\n";
        out << "\tMessage: " << '\n';
        out << '\t' << (params.first ? decrypt(msg.body, *params.first) : msg.body)
            << '\n';
    }

    void sync(messages_t::iterator msg_it, std::tuple<msg_id_t, topic_t, bool, username_t, body_t> message)
    {
        std::tie(msg_it->id, msg_it->topic, msg_it->cyphered, msg_it->user_from, msg_it->body) = std::move(message);
    }

    std::pair<messages_t::iterator, bool> sync(std::tuple<msg_id_t, topic_t, bool, username_t, body_t> message)
    {
        auto msg_it  = boost::find_if(messages_, [msg_id = std::get<0>(message)](auto const &msg) { return msg.id == msg_id; });
        bool new_msg = messages_.end() == msg_it;
        if (new_msg)
        {
            messages_.emplace_front();
            msg_it = messages_.begin();
        }
        sync(msg_it, std::move(message));
        return {msg_it, new_msg};
    }

    std::pair<std::optional<CryptoPP::RSA::PrivateKey>, bool> cypher_params(InboxMessage const &msg) const
    {
        std::optional<CryptoPP::RSA::PrivateKey> priv_key;
        if (msg.cyphered)
            priv_key = fetch_priv_key(msg.user_from);

        if (msg.cyphered && !priv_key)
        {
            std::cerr << "The message is cyphered but the key associated with '"
                      << msg.user_from << "' does not exist. Check your keys and associations";
            return { priv_key, false };
        }

        return { priv_key, true };
    }

    std::optional<CryptoPP::RSA::PrivateKey> fetch_priv_key(username_t const &user_from) const
    {
        if (auto const keys_pair_dir = profile_->find_assoc_key(username_to_keyname(user_from)); !keys_pair_dir.empty())
            try { return load_key<CryptoPP::RSA::PrivateKey>(keys_pair_dir / Application::kPrivKeyName); } catch (...) {}
        return {};
    }

    std::string decrypt(std::string_view cyphered_msg, CryptoPP::RSA::PrivateKey const &key) const
    {
        std::string msg{cyphered_msg.cbegin(), cyphered_msg.end()};
        try
        {
            ::lmail::decrypt(msg, key);
        }
        catch (std::exception const &ex)
        {
            std::cerr << "error occurred while decrypting message, reason: " << ex.what() << '\n';
            msg.clear();
        }
        return msg;
    }

private:
    std::shared_ptr<Profile> profile_;
    messages_t               messages_;
};

} // namespace lmail
