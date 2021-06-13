#ifndef INCLUDED_SYNC_EVENT_EMITTER_HPP
#define INCLUDED_SYNC_EVENT_EMITTER_HPP

#include<list>
#include<functional>
#include<memory>
#include<iterator>
#include<vector>

namespace Network {
    namespace Internal {
        template<typename _Event>
        using Callback_function = void(_Event&);

        template<typename _Event>
        using Callback_function_object = std::function<Callback_function<_Event>>;

        template<typename _Event>
        using Callback_list = std::list<Callback_function_object<_Event>>;

        template<typename _Event>
        using Callback_handle = typename Callback_list<_Event>::const_iterator;

        template<typename>
        struct Event_of_callback_function_object;

        template<typename _Event>
        struct Event_of_callback_function_object<Callback_function_object<_Event>> {
            typedef _Event Type;
        };

        template<typename _Handle>
        using Event_of_callback_handle = 
            typename Event_of_callback_function_object<
                typename std::iterator_traits<_Handle>::value_type
            >::Type;
        

        template<typename _Event>
        class Sync_event_emitter_base {
        protected:
            explicit Sync_event_emitter_base() = default;

            Callback_handle<_Event> add_callback(Callback_function_object<_Event>&& _callback) {
                return callbacks.emplace(std::cend(callbacks), std::move(_callback));
            }

            void remove_callback(Callback_handle<_Event> _handle) {
                callbacks_for_removal.emplace_back(_handle);
            }

            void post_event(_Event& _event) {
                for(auto const& callback_for_removal : callbacks_for_removal) callbacks.erase(callback_for_removal);
                callbacks_for_removal.clear();

                if(callbacks.size() > 0) {
                    auto begin = std::cbegin(callbacks);
                    auto last = std::prev(std::cend(callbacks));
                    auto current = begin;

                    while(true) {
                        (*current)(_event);
                        if(current == last) break;
                        ++current;
                    }
                }
            }

            ~Sync_event_emitter_base() = default;
            Sync_event_emitter_base(Sync_event_emitter_base const&) = delete;
            Sync_event_emitter_base& operator=(Sync_event_emitter_base const&) = delete;
            Sync_event_emitter_base(Sync_event_emitter_base&&) = delete;
            Sync_event_emitter_base& operator=(Sync_event_emitter_base&&) = delete;

        private:
            Callback_list<_Event> callbacks;
            std::vector<Callback_handle<_Event>> callbacks_for_removal;

        };
    }


    template<typename... _Events>
    class Sync_event_emitter: private Internal::Sync_event_emitter_base<_Events>... {
    public:
        template<typename _Event>
        using Callback_handle = Internal::Callback_handle<_Event>;

        template<typename _Callback>
        auto add_event_callback(_Callback&& _callback) {
            return add_event_callback_impl(std::function(std::forward<_Callback>(_callback)));
        }

        template<typename _Handle>
        void remove_event_callback(_Handle _handle) {
            Internal::Sync_event_emitter_base<Internal::Event_of_callback_handle<_Handle>>::remove_callback(_handle);
        }
        

    protected:
        explicit Sync_event_emitter() = default;

        template<typename _Event>
        auto add_event_callback_impl(Internal::Callback_function_object<_Event>&& _callback) {
            return Internal::Sync_event_emitter_base<_Event>::add_callback(std::move(_callback));
        }
        
        /* add_event_callback() and remove_event_callback() may be called from other callbacks 
        * invoked by post_event(). Those changes do not become visible until post_event() returns. */
        template<typename _Event>
        void post_event(_Event&& _event) {
            /* Pass as l-value. _Event must not be const, if it is, this won't compile. */
            Internal::Sync_event_emitter_base<typename std::decay<_Event>::type>::post_event(_event);
        }

        ~Sync_event_emitter() = default;
        Sync_event_emitter(Sync_event_emitter const&) = delete;
        Sync_event_emitter& operator=(Sync_event_emitter const&) = delete;
        Sync_event_emitter(Sync_event_emitter&&) = delete;
        Sync_event_emitter& operator=(Sync_event_emitter&&) = delete;

    };
}


#endif