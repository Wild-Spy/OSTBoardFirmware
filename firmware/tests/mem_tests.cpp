//
// Created by mcochrane on 26/05/17.
//

#include <stdio.h>
#include <cmocka.h>
#include <stdint.h>
#include <memory/MemoryWrapper.h>

extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;
extern uint32_t _heap_start;
extern uint32_t _heap_end;

static void null_test_success(void** state) {
    (void) state; // unused
    assert_true(true);
}

static void stack_heap_locations(void** state) {
//    CEXCEPTION_T e;
//    void* c;
//    Try {
//        c = MemoryWrapper_AllocateMemory(100, 1);
//    } Catch(e) {
//        fail_msg("Exception thrown!");
//    }
//
//    assert_ptr_equal(c, )
//

    print_message("[stackstart] : %08lX\r\n", (uint32_t)&_sstack);
    print_message("[ stackend ] : %08lX\r\n", (uint32_t)&_estack);
    print_message("[heap start] : %08lX\r\n", (uint32_t)&_heap_start);
    print_message("[ heap end ] : %08lX\r\n", (uint32_t)&_heap_end);

}

static void heap_overflow(void** state) {
    CEXCEPTION_T e;
    void* c;
    Try {
        c = MemoryWrapper_AllocateMemory(1024 * 100, 1);
        fail_msg("Should have thrown an exception!");
    } Catch(e) {
        assert_int_equal(e, EX_MEM_OUT_OF_MEMORY);
    }
}

const struct CMUnitTest mem_tests[] = {
    cmocka_unit_test(null_test_success),
    cmocka_unit_test(stack_heap_locations),
    cmocka_unit_test(heap_overflow),
};

void run_mem_tests() {
    printf("s111111111");
    print_message("stack start1: %08lX\r\n", _sstack);
    printf("stack end1: %08lX\r\n", _estack);
    printf("stack start1: %08lX\r\n", _heap_start);
    printf("stack end1: %08lX\r\n", _heap_end);
    cmocka_run_group_tests(mem_tests, NULL, NULL);
}