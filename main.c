#include "re_compiler.h"
#include "machine.h"
#include <stdio.h>

int
main(void)
{
        struct re_compiler *rp = NULL;
        struct machine *mp = NULL;

        rp = re_compiler_new("abc", 6);

        mp = re_compiler_comp(rp);

        re_compiler_free(&rp);
        machine_free(&mp);
        (void)rp;
}
