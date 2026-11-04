#include <assert.h>
#include "../client_functions.c"

int main(void)
{
    assert(isValidDate("01/06-05/06"));
    assert(isValidDate("29/02-01/03"));
    assert(!isValidDate(NULL));
    assert(!isValidDate(""));
    assert(!isValidDate("31/04-01/05"));
    assert(!isValidDate("01/13-02/13"));
    assert(!isValidDate("01x06-05x06"));
    assert(!isValidDate("01/06x05/06"));
    assert(!isValidDate("0x/06-05/06"));
    char *converted = convertDate("01/06-05/06");
    assert(strcmp(converted, "0601-0605") == 0);
    free(converted);
    return 0;
}
