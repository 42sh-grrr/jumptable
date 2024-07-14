#include <criterion/criterion.h>
#include "saltus/resource_pool.hh"

TestSuite(libsaltus);

Test(libsaltus, simple)
{
    int init_value = 1;
    saltus::ResourcePool<int> ints(
        [init_value](){ return std::make_unique<int>(init_value); }
    );

    saltus::ResourceHandle<int> handle = ints.get();
    cr_assert_eq(*handle, init_value, "%d == %d", *handle, init_value);
}

Test(libsaltus, get_assign_release_get)
{
    const int init_value = 1;
    const int new_value = 42;
    saltus::ResourcePool<int> ints(
        [init_value](){ return std::make_unique<int>(init_value); }
    );

    {
        saltus::ResourceHandle<int> handle = ints.get();
        cr_assert_eq(*handle, init_value, "%d == %d", *handle, init_value);
        *handle = new_value;
        cr_assert_eq(*handle, new_value, "%d == %d", *handle, new_value);
    }

    // Should be the same value previously gotten
    {
        saltus::ResourceHandle<int> handle = ints.get();
        cr_assert_eq(*handle, new_value, "%d == %d", *handle, new_value);
    }
}

Test(libsaltus, multiple_at_the_same_time)
{
    const int init_value = 1;
    const int new_value = 42;
    saltus::ResourcePool<int> ints(
        [init_value](){ return std::make_unique<int>(init_value); }
    );

    saltus::ResourceHandle<int> handle1 = ints.get();
    cr_assert_eq(*handle1, init_value, "%d == %d", *handle1, init_value);
    saltus::ResourceHandle<int> handle2 = ints.get();
    cr_assert_eq(*handle2, init_value, "%d == %d", *handle2, init_value);

    cr_assert_neq(handle1.get(), handle2.get());

    *handle1 = new_value;

    cr_assert_eq(*handle1, new_value, "%d == %d", *handle1, new_value);
    cr_assert_eq(*handle2, init_value, "%d == %d", *handle2, init_value);

    handle1.~Handle();

    // Should be handle1
    saltus::ResourceHandle<int> handle3 = ints.get();
    cr_assert_eq(*handle3, new_value, "%d == %d", *handle3, new_value);
    cr_assert_eq(*handle2, init_value, "%d == %d", *handle2, init_value);
}
