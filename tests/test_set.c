#include <criterion/criterion.h>
#include "../src/set.h"
#include <stdio.h>

Test(set, contains_should_return_false_with_no_values)
{
    Set set;
    set_initialize(&set);
    cr_assert_not(set_contains(&set, 1));
}

Test(set, contains_should_return_false_when_value_is_not_present)
{
    Set set;
    set_initialize(&set);
    set_add(&set, 1);
    cr_assert_not(set_contains(&set, 2));
}

Test(set, contains_should_return_true_when_value_is_present)
{
    Set set;
    set_initialize(&set);
    set_add(&set, 1);
    set_add(&set, 2);
    cr_assert(set_contains(&set, 1));
    cr_assert(set_contains(&set, 2));
    cr_assert_not(set_contains(&set, 3));
}

Test(set, contains_should_return_false_after_value_has_been_removed)
{
    Set set;
    set_initialize(&set);
    set_add(&set, 1);
    set_add(&set, 2);
    set_remove(&set, 1);

    cr_assert(set.values[0] == -1);
    cr_assert(set.values[1] == 2);

    cr_assert_not(set_contains(&set, 1));
    cr_assert(set_contains(&set, 2));
}

Test(set, add_should_increase_count)
{
    Set set;
    set_initialize(&set);
    cr_assert(set.count == 0);
    set_add(&set, 1);
    cr_assert(set.count == 1);
    set_add(&set, 2);
    cr_assert(set.count == 2);

}

Test(set, remove_should_decrease_count)
{
    Set set;
    set_initialize(&set);
    set_add(&set, 1);
    set_add(&set, 2);
    cr_assert(set.count == 2);
    set_remove(&set, 1);
    cr_assert(set.count == 1);
    set_remove(&set, 2);
    cr_assert(set.count == 0);
}

Test(set, should_grow)
{
    Set set;
    set_initialize(&set);
    for (int i = 0; i < 5; i++)
        set_add(&set, i);
    
    cr_assert(set.size == INITIAL_SET_SIZE * 2);

    for (int i = 0; i < 5; i++)
        cr_assert(set_contains(&set, i));
}
