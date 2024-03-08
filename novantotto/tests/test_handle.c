/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include "../windefs.h"
#include "../handle.h"
#include "miniunit.h"

int       tests_run = 0;
RESOURCE *res;
RESOURCE *res2;
RESOURCE *res3;

static char *test_allocate_handler()
{
    res = AllocateHandle(HANDLE_WINDOW, NULL);
    mu_assert("error, res is NULL", res != NULL);
    mu_assert("error, res->handle is NULL", res->handle != NULL);
    return NULL;
}

static char *test_allocate_handler2()
{
    res2 = AllocateHandle(HANDLE_WINDOW, NULL);
    mu_assert("error, res2 is NULL", res2 != NULL);
    mu_assert("error, res2->handle is NULL", res2->handle != NULL);
    return NULL;
}

static char *test_allocate_handler3()
{
    res3 = AllocateHandle(HANDLE_CONTROL, res2->handle);
    mu_assert("error, res3 is NULL", res3 != NULL);
    mu_assert("error, res3->handle is NULL", res3->handle != NULL);
    return NULL;
}

static char *test_not_found()
{
    HANDLE t = GetProperty(res->props, "test");
    mu_assert("error, GetProperty is not NULL", t == NULL);
    return NULL;
}

static char *test_test_len1()
{
    DelProperty(res->props, "test");
    SetProperty(res->props, "test", (HANDLE)1);
    mu_assert("error, len is not 1", res->props->len == 1);
    return NULL;
}

static char *test_get_property()
{
    HANDLE *t = GetProperty(res->props, "test");
    mu_assert("error, GetProperty test is not 1", t == (HANDLE)1);
    return NULL;
}

static char *test_get_property2()
{
    SetProperty(res->props, "test2", (HANDLE)2);
    mu_assert("error, len is not 2", res->props->len == 2);
    HANDLE *t = GetProperty(res->props, "test");
    mu_assert("error, GetProperty test is not 1", t == (HANDLE)1);
    t = GetProperty(res->props, "test2");
    mu_assert("error, GetProperty test2 is not 2", t == (HANDLE)2);
    return NULL;
}

static char *test_del_properties()
{
    DelProperty(res->props, "not-existing");
    DelProperty(res->props, "test");
    mu_assert("error, len is not 1", res->props->len == 1);
    HANDLE *t = GetProperty(res->props, "test");
    mu_assert("error, GetProperty is not NULL", t == NULL);
    DelProperty(res->props, "test2");
    mu_assert("error, len is not 0", res->props->len == 0);
    DelProperty(res->props, "test2");
    mu_assert("error, len is not 0", res->props->len == 0);
    return NULL;
}

static char *test_set_many()
{
    int  i;
    int  c = res2->props->capacity * 2;
    char buf[20];
    for (i = 0; i < c; i++)
    {
        snprintf(buf, sizeof(buf), "p%d", i);
        SetProperty(res2->props, buf, (HANDLE)i);
    }
    mu_assert("error, len is not c", res2->props->len == c);
    mu_assert("error, len is not <= capacity", res2->props->len <= res2->props->capacity);
    for (i = 0; i < c; i++)
    {
        snprintf(buf, sizeof(buf), "p%d", i);
        HANDLE *t = GetProperty(res2->props, buf);
        mu_assert("error, wrong property value", (int)t == i);
    }
    return NULL;
}

static char *test_release_handle()
{
    ReleaseHandle(res);
    SetProperty(res2->props, "aaaa", (HANDLE)1111);
    SetProperty(res2->props, "bbbb", (HANDLE)2222);
    SetProperty(res2->props, "cccc", (HANDLE)3333);
    ReleaseHandle(res2);
    ReleaseHandle(res3);
    return NULL;
}

static char *test_release_empty()
{
    RESOURCE *resx = AllocateHandle(HANDLE_WINDOW, NULL);
    ReleaseHandle(resx);
    return NULL;
}

static char *test_get_dlg_item()
{
    RESOURCE *resx = AllocateHandle(HANDLE_WINDOW, NULL);
    RESOURCE *i1 = AllocateHandle(HANDLE_CONTROL, resx->handle);
    i1->control.nIDDlgItem = 1001;
    RESOURCE *i2 = AllocateHandle(HANDLE_CONTROL, resx->handle);
    i2->control.nIDDlgItem = 1002;
    RESOURCE *i3 = AllocateHandle(HANDLE_CONTROL, res2->handle);
    i3->control.nIDDlgItem = 1001;
    mu_assert("error, GetDlgItem 1001 != i1", GetDlgItem(resx->handle, 1001) == i1->handle);
    mu_assert("error, GetDlgItem 1001 parent != handle", GetHandle(GetDlgItem(resx->handle, 1001), HANDLE_ANY)->hwndParent == resx->handle);
    mu_assert("error, GetDlgItem 1002 != i2", GetDlgItem(resx->handle, 1002) == i2->handle);
    mu_assert("error, GetDlgItem 1001 != i1", GetDlgItem(res2->handle, 1001) == i3->handle);
    mu_assert("error, GetDlgItem 1111 != NULL", GetDlgItem(resx->handle, 1111) == NULL);
    ReleaseHandle(resx);
    mu_assert("error, GetDlgItem 1001 != NULL", GetDlgItem(resx->handle, 1001) == NULL);
    return NULL;
}

static char *all_tests()
{
    mu_run_test(test_allocate_handler);
    mu_run_test(test_allocate_handler2);
    mu_run_test(test_allocate_handler3);
    mu_run_test(test_not_found);
    mu_run_test(test_test_len1);
    mu_run_test(test_get_property);
    mu_run_test(test_get_property2);
    mu_run_test(test_del_properties);
    mu_run_test(test_set_many);
    mu_run_test(test_release_handle);
    mu_run_test(test_release_empty);
    mu_run_test(test_get_dlg_item);
    return 0;
}

int main(int argc, char **argv)
{
    char *result = all_tests();
    if (result != 0)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
