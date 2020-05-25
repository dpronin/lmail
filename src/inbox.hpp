#pragma once

#include <deque>
#include <iostream>
#include <tuple>

#include <boost/range/algorithm/find_if.hpp>

#include "inbox_message.hpp"

namespace lmail
{

class Inbox
{
public:
    using messages_t = std::deque<InboxMessage>;

public:
    void sync(std::vector<std::tuple<msg_id_t, topic_t>> records, std::ostream &out)
    {
        size_t new_messages = 0;
        for (auto &record : records)
            new_messages += sync(std::tuple_cat(std::move(record), std::tuple<body_t>{})).second;

        if (messages_.empty())
            out << "There are no messages\n";
        else if (messages_.size() == 1)
            out << "There is 1 message (" << new_messages << " new)"
                << ":\n";
        else
            out << "There are " << messages_.size()
                << " messages (" << new_messages << " new)"
                << ":\n";

        msg_idx_t msg_idx = 0;
        for (auto const &msg : messages_)
            out << '\t' << (++msg_idx) << ". " << msg.topic << '\n';
    }

    void sync(msg_idx_t msg_idx, std::tuple<msg_id_t, topic_t, body_t> message, std::ostream &out)
    {
        if (0 == msg_idx || msg_idx > messages_.size())
            return;
        auto &msg = messages_[msg_idx - 1];
        msg       = {std::get<0>(message), std::move(std::get<1>(message)), std::move(std::get<2>(message))};
        out << "\tIndex: " << msg_idx << "\n\n";
        out << "\tTopic: " << msg.topic << "\n\n";
        out << "\tMessage: " << '\n';
        out << '\t' << msg.body << '\n';
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
        std::optional<msg_id_t> msg_id;
        if (0 == msg_idx || msg_idx > messages_.size())
            return msg_id;
        auto msg_it = std::next(messages_.begin(), msg_idx - 1);
        msg_id = msg_it->id;
        messages_.erase(msg_it);
        return msg_id;
    }

private:
    std::pair<messages_t::iterator, bool> sync(std::tuple<msg_id_t, topic_t, body_t> message)
    {
        auto [msg_id, topic, body] = std::move(message);
        auto msg_it                = boost::find_if(messages_, [msg_id](auto const &msg) { return msg.id == msg_id; });
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
