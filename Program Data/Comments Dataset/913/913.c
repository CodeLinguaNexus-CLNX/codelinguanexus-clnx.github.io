/*
This function is responsible for enumerating the available CPUs based on the provided bitmask and returning a string representation of the enumerated CPUs.

Here's the detail:
The function starts by initializing some variables, including a GString object `s` to store the enumerated CPUs.
It then iterates through the bitset of CPUs specified by the `cpus` parameter, using the `find_first_bit` and `find_next_bit` functions to find the next set bit (representing an available CPU).
Inside the loop, it appends the CPU number to the `s` string, preceded by a space if it is not the first CPU encountered.
Finally, it returns the string representation of the enumerated CPUs by freeing the GString object and passing FALSE as the second parameter to g_string_free, indicating that the memory should not be freed.

Need's to notice:
1. The `cpus` parameter is a bitmask representing the available CPUs.
2. The `max_cpus` parameter specifies the maximum number of CPUs in the bitmask.
3. The function uses the `find_first_bit` and `find_next_bit` functions to iterate through the set bits in the `cpus` bitmask and retrieve the CPU numbers.
4. The enumerated CPUs are stored in a GString object `s`.
5. Before returning the string representation, the GString object is freed using the g_string_free function, and the memory is not freed (FALSE is passed as the second parameter).
*/
static char *enumerate_cpus(unsigned long *cpus, int max_cpus)

{

    int cpu;

    bool first = true;

    GString *s = g_string_new(NULL);



    for (cpu = find_first_bit(cpus, max_cpus);

        cpu < max_cpus;

        cpu = find_next_bit(cpus, max_cpus, cpu + 1)) {

        g_string_append_printf(s, "%s%d", first ? "" : " ", cpu);

        first = false;

    }

    return g_string_free(s, FALSE);

}
