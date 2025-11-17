#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>

namespace Orchard {

class Event {
public:
    virtual ~Event() = default;
    bool Handled = false;
};

class EventSystem {
public:
    template<typename T>
    using EventCallback = std::function<void(T&)>;
    
    template<typename T>
    void Subscribe(EventCallback<T> callback);
    
    template<typename T>
    void Dispatch(T& event);
    
    void Clear();
    
private:
    struct IEventCallbackWrapper {
        virtual ~IEventCallbackWrapper() = default;
    };
    
    template<typename T>
    struct EventCallbackWrapper : public IEventCallbackWrapper {
        std::vector<EventCallback<T>> callbacks;
    };
    
    std::unordered_map<std::type_index, std::unique_ptr<IEventCallbackWrapper>> m_Callbacks;
};

template<typename T>
void EventSystem::Subscribe(EventCallback<T> callback) {
    auto typeIndex = std::type_index(typeid(T));
    
    auto it = m_Callbacks.find(typeIndex);
    if (it == m_Callbacks.end()) {
        auto wrapper = std::make_unique<EventCallbackWrapper<T>>();
        wrapper->callbacks.push_back(std::move(callback));
        m_Callbacks[typeIndex] = std::move(wrapper);
    } else {
        auto wrapper = static_cast<EventCallbackWrapper<T>*>(it->second.get());
        wrapper->callbacks.push_back(std::move(callback));
    }
}

template<typename T>
void EventSystem::Dispatch(T& event) {
    auto typeIndex = std::type_index(typeid(T));
    
    auto it = m_Callbacks.find(typeIndex);
    if (it != m_Callbacks.end()) {
        auto wrapper = static_cast<EventCallbackWrapper<T>*>(it->second.get());
        for (auto& callback : wrapper->callbacks) {
            if (event.Handled) break;
            callback(event);
        }
    }
}

}
