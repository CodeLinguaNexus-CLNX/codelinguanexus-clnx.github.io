/*
This function is responsible for flushing the trace buffer and discarding written trace records, potentially interacting with a trace file if tracing is enabled.

Here's the detail:
1. When invoked, the st_flush_trace_buffer function first checks if the trace file is enabled. If tracing is enabled, it proceeds to flush the contents of the trace file using the flush_trace_file function. This operation may involve writing any buffered trace data to the trace file and ensuring its synchronization with the underlying storage.
2. After flushing the trace file (if applicable), the function proceeds to discard the written trace records held in memory by resetting the trace_idx variable to 0. This effectively clears the trace buffer, indicating that the previously recorded trace data has been successfully processed and can be overwritten in subsequent tracing operations.

Need's to notice:
1. The proper handling of trace data and flushing of trace files is important for maintaining the integrity and reliability of traced information, particularly in debugging and performance analysis scenarios. Care should be taken to ensure that the trace data is accurately and efficiently written to the trace file, and that any buffered data is appropriately synchronized with the storage medium.
2. The behavior of this function may be dependent on the configuration and state of the tracing subsystem, and it is essential to understand the impact of enabling or disabling tracing on the overall system behavior and resource utilization.
3. The discarding of written trace records and resetting of the trace buffer should be performed with caution, as it may affect the ability to analyze historical trace data for debugging or performance profiling purposes. Consideration should be given to the retention and management of trace data based on specific requirements and use cases.
4. This function's interaction with the trace subsystem and file I/O operations should be well-documented and understood, especially when making modifications to the tracing behavior or integrating with different trace output formats or destinations.
5. The impact of trace data management and flushing on system performance and resource usage should be taken into account, particularly in scenarios where frequent flushing and discarding of trace records may impose overhead on system operations and I/O activity. Keep in mind the potential trade-offs between trace data retention and system performance.
*/
void st_flush_trace_buffer(void)

{

    if (trace_file_enabled) {

        flush_trace_file();

    }



    /* Discard written trace records */

    trace_idx = 0;

}
