/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file event.hpp
 * @brief Typed runtime attribute with reflection, serialization, and metadata.
 * @copyright Copyright (c) 2026
 */
#pragma once

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

namespace meta
{

class EventConnection
{
public:
  EventConnection() = default;

  EventConnection(std::function<void()> disconnect)
      : disconnect_(std::move(disconnect))
  {
  }

  ~EventConnection() { disconnect(); }

  EventConnection(const EventConnection &) = delete;
  EventConnection &operator=(const EventConnection &) = delete;

  EventConnection(EventConnection &&other) noexcept
      : disconnect_(std::move(other.disconnect_))
  {
    other.disconnect_ = {};
  }

  EventConnection &operator=(EventConnection &&other) noexcept
  {
    if (this != &other)
    {
      disconnect();

      disconnect_ = std::move(other.disconnect_);
      other.disconnect_ = {};
    }

    return *this;
  }

  void disconnect()
  {
    if (disconnect_)
    {
      disconnect_();
      disconnect_ = {};
    }
  }

private:
  std::function<void()> disconnect_;
};

template <typename... Args> class Event
{
public:
  using callback_t = std::function<void(Args...)>;
  using listener_id_t = std::size_t;

  EventConnection subscribe(callback_t callback)
  {
    const listener_id_t id = next_id_++;

    listeners_.push_back({id, std::move(callback)});

    return EventConnection([this, id]() { unsubscribe(id); });
  }

  void notify(Args... args)
  {
    // copy to allow callbacks to disconnect themselves safely
    auto listeners = listeners_;

    for (auto &listener : listeners)
    {
      listener.callback(args...);
    }
  }

private:
  struct Listener
  {
    listener_id_t id;
    callback_t    callback;
  };

  void unsubscribe(listener_id_t id)
  {
    listeners_.erase(std::remove_if(listeners_.begin(),
                                    listeners_.end(),
                                    [id](const Listener &listener)
                                    { return listener.id == id; }),
                     listeners_.end());
  }

  std::vector<Listener> listeners_;

  listener_id_t next_id_ = 0;
};

} // namespace meta