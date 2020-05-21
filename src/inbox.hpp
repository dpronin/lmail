#pragma once

#include <iostream>
#include <tuple>
#include <deque>

#include <boost/range/algorithm/find_if.hpp>

#include "inbox_message.hpp"

namespace lmail
{

class Inbox
{
public:
    using messages_t = std::deque<InboxMessage>;

public:
    void sync(std::vector<std::tuple<msg_id_t, topic_t>> records)
    {
        size_t new_messages = 0;
        for (auto &record : records)
            new_messages += sync(std::tuple_cat(std::move(record), std::tuple<body_t>{})).second;

        if (messages_.empty())
            std::cout << "There are no messages\n";
        else if (messages_.size() == 1)
            std::cout << "There is 1 message (" << new_messages << " new)" << ":\n";
        else
            std::cout << "There are " << messages_.size()
                      << " messages (" << new_messages << " new)" << ":\n";

        msg_idx_t msg_idx = 0;
        for (auto const &msg : messages_)
            std::cout << '\t' << (++msg_idx) << ". " << msg.topic << '\n';
    }

    void sync(msg_idx_t msg_idx, std::tuple<msg_id_t, topic_t, body_t> message)
    {
        if (msg_idx <= 0 || messages_.size() < msg_idx)
            return;
        auto &msg = messages_[msg_idx - 1];
        msg = {std::get<0>(message), std::move(std::get<1>(message)), std::move(std::get<2>(message))};
        std::cout << "\tIndex: " << msg_idx << "\n\n";
        std::cout << "\tTopic: " << msg.topic << "\n\n";
        std::cout << "\tMessage: " << '\n';
        std::cout << '\t' << msg.body << '\n';
    }

    std::optional<msg_id_t> find(msg_idx_t msg_idx)
    {
        std::optional<msg_id_t> msg_id;
        if (1 <= msg_idx && msg_idx <= messages_.size())
            msg_id = messages_[msg_idx - 1].id;
        return msg_id;
    }

    std::optional<msg_id_t> erase(msg_idx_t msg_idx)
    {
        if (msg_idx <= 0 || messages_.size() < msg_idx)
            return {};
        auto msg_it = std::next(messages_.begin(), msg_idx - 1);
        auto const msg_id = msg_it->id;
        messages_.erase(msg_it);
        return msg_id;
    }

private:
    std::pair<messages_t::iterator, bool> sync(std::tuple<msg_id_t, topic_t, body_t> message)
    {
        auto [msg_id, topic, body] = std::move(message);
        auto msg_it = boost::find_if(messages_, [msg_id](auto const &msg){ return msg.id == msg_id; });
        if (msg_it != messages_.end())
        {
            if (msg_it->topic != topic)
            {
                msg_it->topic = std::move(topic);
                msg_it->body  = std::move(body);
            }
            return {msg_it, false};
        }
        else
        {
            messages_.push_front({msg_id, std::move(topic), std::move(body)});
            return {messages_.begin(), true};
        }
    }

private:
    messages_t messages_;
};

} // namespace lmail
