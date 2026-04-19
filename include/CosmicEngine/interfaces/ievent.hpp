#ifndef COSMIC_IEVENT_HPP
#define COSMIC_IEVENT_HPP

/**
 * @file ievent.hpp
 * @brief Declares the non-templated base interface used by the event system.
 */

/**
 * @brief Polymorphic base type for all runtime event containers.
 *
 * IEvent is the non-templated base that allows the EventManager to store
 * heterogeneous Event\<Args...\> instances in a single map.  You do not
 * use this class directly; it exists for internal type erasure.
 */
class IEvent {
public:
    virtual ~IEvent() = default;
};


#endif // COSMIC_IEVENT_HPP
