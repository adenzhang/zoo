#ifndef ZOO_DERIVEDVTABLEPOLICY_H
#define ZOO_DERIVEDVTABLEPOLICY_H

#include "VTablePolicy.h"

namespace zoo {

template<typename BasePolicy, typename... Extensions>
struct DerivedVTablePolicy {
    using ComposedFrom = BasePolicy;

    using MemoryLayout = typename BasePolicy::MemoryLayout;

    struct VTable: BasePolicy::VTable, Extensions::VTableEntry... {};

    using TypeSwitch = VTable;

    struct ExtraAffordances: Extensions::template Raw<VTable, MemoryLayout>... {};

    struct DefaultImplementation: MemoryLayout {
        constexpr static inline VTable Default = {
            BasePolicy::DefaultImplementation::Default,
            Extensions::template Default<DefaultImplementation>...
        };

        DefaultImplementation(): MemoryLayout(&Default) {}

        /// \todo is this needed?
        using MemoryLayout::MemoryLayout;
    };

    template<typename V>
    struct Builder: BasePolicy::template Builder<V> {
        using Base = typename BasePolicy::template Builder<V>;

        constexpr static inline VTable Operations = {
            Base::Operations,
            Extensions::template Operation<Builder>...
        };

        template<typename... Args>
        Builder(Args &&...args):
            Base(
                static_cast<const typename BasePolicy::VTable *>(&Operations),
                std::forward<Args>(args)...
            )
        {}
    };

    template<typename AnyC>
    struct Affordances: Extensions::template UserAffordance<AnyC>... {};
};

}

#endif //ZOO_DERIVEDVTABLEPOLICY_H