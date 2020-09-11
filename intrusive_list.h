#pragma once

namespace intrusive
{
    struct default_tag;

    template <typename Tag = default_tag>
    struct list_element
    {};

    template <typename T, typename Tag = default_tag>
    struct list
    {};
}
