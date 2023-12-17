import os
import subprocess
import threading
import time
import shutil
import traceback
from CFG_analyze_func_before_v1 import analyze_func_before
from cpgqls_client import CPGQLSClient, import_code_query
import logging
import signal


class BasicBlock:
    counter = 1  # Global counter for this class
    basic_block_dict = {}  # Used to store all BasicBlock objects; used for dynamic programming after the graph is generated

    def __init__(self, start_line, end_line=None):
        self.start_line = start_line
        self.end_line = end_line
        self.id = f"BB{BasicBlock.counter}"
        self.type = 'normal'
        BasicBlock.counter += 1
        # Add the new node to basic_blocks_dict
        BasicBlock.basic_block_dict[self.id] = self

    def output(self):
        print("start_line=", self.start_line)
        print("end_line=", self.end_line)
        print("id=", self.id)
        print("type=", self.type)

class Graph:
    def __init__(self):
        self.graph = {}

    def add_node(self, node):
        if node not in self.graph:
            self.graph[node] = []

    def add_edge(self, from_node, to_node, weight):
        self.add_node(from_node)
        self.add_node(to_node)
        self.graph[from_node].append((to_node, weight))

    def get_connections(self, node):
        return self.graph.get(node, []) 

def parse_line_range(line):
    """Parse lines in the format @@ -a,b +c,d @@ and extract the range of lines from the original file."""
    parts = line.split(' ')
    old_file_range = parts[1]
    line_numbers = old_file_range.split(',')[0]  # Extract the line number part
    start_line = int(line_numbers.split('-')[1])
    # Default to 1 line if no line count is specified
    line_count = int(old_file_range.split(',')[1]) if ',' in old_file_range else 1
    return start_line, start_line + line_count - 1

def analyze_patch(patch_path):
    affected_lines = []
    with open(patch_path, 'r') as file:
        for line in file:
            if line.startswith('@@'):
                start_line, end_line = parse_line_range(line)
                affected_lines.append((start_line, end_line))
    return affected_lines

# Original code:
# def interval_overlap(interval1, interval2):
#     """Check if two intervals overlap."""
#     return max(interval1[0], interval2[0]) <= min(interval1[1], interval2[1])
# Here, we added a check for None values in the intervals.

def interval_overlap(interval1, interval2, node, basic_block_dict):
    """Check if two intervals overlap."""
    start1, end1 = interval1
    start2, end2 = interval2
    if None in [start1, end1, start2, end2]:
        logging.warning("Node_interval contains None values")
        print("Node information:")
        node.output()
        return False

    return max(interval1[0], interval2[0]) <= min(interval1[1], interval2[1])

def find_affected_nodes(affected_lines, basic_block_dict):
    affected_nodes = []
    for node_id, node in basic_block_dict.items():
        node_interval = (node.start_line, node.end_line)
        for affected_interval in affected_lines:
            if interval_overlap(node_interval, affected_interval, node, basic_block_dict):
                affected_nodes.append(node_id)
                break 
    return affected_nodes

def read_lines_from_file(file_path, start_line, end_line):
    """Read lines from a file within the specified range."""
    lines = []
    with open(file_path, 'r') as file:
        for current_line, line_content in enumerate(file, 1):
            if current_line >= start_line and current_line <= end_line:
                lines.append(line_content)
            elif current_line > end_line:
                break
    return ''.join(lines)

def read_first_and_last_line(file_path):
    """Read the first and last lines of a file."""
    with open(file_path, 'r') as file:
        first_line = file.readline()  # Read the first line
        file.seek(0, os.SEEK_END)     # Jump to the end of the file
        file.seek(file.tell() - 1, os.SEEK_SET)  # Move back one character position
        while file.read(1) != '\n':   # Move back until a newline character is found
            file.seek(file.tell() - 2, os.SEEK_SET)
        last_line = file.readline()   # Read the last line
    return first_line, last_line

def optimize_code(func_before_path, key_path_nodes, basic_block_dict):
    """Optimize code by generating optimized code."""
    total_lines = int(os.popen(f"wc -l < {func_before_path}").read().strip())  # Get the total number of lines in the file

    optimized_code = ""
    
    # Check and add the first line
    if not key_path_nodes or basic_block_dict[key_path_nodes[0]].start_line > 1:
        first_line = read_lines_from_file(func_before_path, 1, 1)
        optimized_code += first_line

    # Add code for key path nodes
    for node_id in key_path_nodes:
        node = basic_block_dict[node_id]
        node_code = read_lines_from_file(func_before_path, node.start_line, node.end_line)
        optimized_code += node_code

    # Check and add the last line
    if not key_path_nodes or basic_block_dict[key_path_nodes[-1]].end_line < total_lines:
        last_line = read_lines_from_file(func_before_path, total_lines, total_lines)
        optimized_code += last_line
    
    return optimized_code

def update_node_states(node, graph, key_nodes, node_states, predecessors):
    for neighbor, weight in graph[node]:
        path_len, covered_keys = node_states[node]
        new_path_len = path_len + weight
        new_covered_keys = covered_keys + (1 if neighbor in key_nodes else 0)
        
        if new_covered_keys > node_states[neighbor][1] or \
           (new_covered_keys == node_states[neighbor][1] and new_path_len < node_states[neighbor][0]):
            node_states[neighbor] = (new_path_len, new_covered_keys)
            predecessors[neighbor] = node
            update_node_states(neighbor, graph, key_nodes, node_states, predecessors)

def improve_dynamic_programming(graph, entry, exit, key_nodes):
    node_states = {node: (float('inf'), 0) for node in graph}  # (path_length, covered_keys)
    predecessors = {node: None for node in graph}
    node_states[entry] = (0, 1 if entry in key_nodes else 0)

    update_node_states(entry, graph, key_nodes, node_states, predecessors)

    # Reconstruct the path
    path = []
    current = exit
    while current:
        path.append(current)
        current = predecessors[current]
    path.reverse()

    return path

def handle_timeout(joern_process):
    print("-----coffee--------Please wait, Joern service needs to be restarted-------coffee------")
    print("Handling timeouts and restarting the Joern service")
    joern_process.terminate()
    joern_process.wait()
    try:
        # Use the lsof command to find Java processes listening on port 8080
        result = subprocess.check_output(["lsof", "-i", ":8080"])
        lines = result.decode('utf-8').splitlines()
        for line in lines:
            if "qzq" in line:
                pid = int(line.split()[1])
                os.kill(pid, signal.SIGKILL)
    except Exception as e:
        print("Error occurred while trying to terminate Java processes:", e)

    print("Joern server has been closed, and the port has been released. Now waiting for 15s to restart")
    time.sleep(15)  # Wait to ensure the port is released
    return subprocess.Popen(['joern', '--server'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)  # Hide subprocess output

def main():
    clnx_train_path = "You path"
    count = 0 
    count_error = 0
    server_endpoint = "localhost:8080"  # Joern server address
    try:
        joern_process = subprocess.Popen(['joern', '--server'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        time.sleep(10)
    except Exception as e:
        traceback.print_exc()
        raise ValueError("Failed to start joern --server at the beginning")

    client = CPGQLSClient(server_endpoint)  # Joern server

    # List all subitems using os.listdir()
    subitems = os.listdir(clnx_train_path)
    # Ensure that the folder order is consistent each time the script runs
    direct_subfolders = sorted([subdir for subdir in subitems if os.path.isdir(os.path.join(clnx_train_path, subdir))])

    # Create a list of subfolders without func_before_optimize.c
    direct_subfolders_without_fbo = []
    for subdir in direct_subfolders:
        folder_path = os.path.join(clnx_train_path, subdir)
        if not os.path.exists(os.path.join(folder_path, "func_before_optimize.c")):
            direct_subfolders_without_fbo.append(subdir)    
    direct_subfolder_count_left = len(direct_subfolders_without_fbo)

    restart_count = 0  # To be removed

    for subdir in direct_subfolders_without_fbo:
        count += 1

        restart_count += 1  # To be removed
        print("BEGIN----------------------------------")
        folder_path = os.path.join(clnx_train_path, subdir)
        func_before_path = os.path.join(folder_path, "func_before.c")
        patch_path = os.path.join(folder_path, "patch.diff")
        print("folder_path", folder_path)
        print("Current count:", count)
        print("Total remaining files:", direct_subfolder_count_left)
        print("Count error", count_error)
        current_percentage = (count / direct_subfolder_count_left) * 100
        error_percentage = (count_error / direct_subfolder_count_left) * 100
        print("Current progress: {:.2f}%".format(current_percentage))
        print("Error percentage: {:.2f}%".format(error_percentage))

        try:
            mygraph, basic_block_dict, entry, exit =  analyze_func_before(folder_path, client)
            affected_lines = analyze_patch(patch_path)
            affected_nodes = find_affected_nodes(affected_lines, basic_block_dict)
            key_path_nodes = improve_dynamic_programming(mygraph.graph, entry.id, exit.id, affected_nodes)
            func_before_optimize = optimize_code(func_before_path, key_path_nodes, basic_block_dict)

            # Write the optimized code to a new file
            with open(os.path.join(folder_path, "func_before_optimize.c"), 'w') as file:
                file.write(func_before_optimize)
            print("SUCCESS")
        except Exception as e:
            # Copy the content of the original func_before.c to func_before_optimize.c when an error occurs
            print("ERROR---------------------------")
            print("Current Analyze analyze_func_before ERROR---------Specific error details below")
            count_error += 1
            traceback.print_exc()
            shutil.copy(func_before_path, os.path.join(folder_path, "func_before_optimize.c"))

        if restart_count > 200:
            try:
                # Restart the Joern service forcibly when a certain number of times or a timeout occurs
                joern_process = handle_timeout(joern_process)
                # Wait for at least 10 seconds after restart
                print("* After restart, wait for another 10 seconds before continuing")
                time.sleep(10)
                client = CPGQLSClient(server_endpoint)  # Reconnect to the Joern server
                print("Joern service restart successful! Resuming program execution")
                restart_count = 0  # Reset the restart count
            except Exception as e:
                print("ERROR Failed to restart Joern service after timeout")
                traceback.print_exc()
                raise ValueError("ERROR1: Error occurred while extracting function names")

    # Close the joern --server and release the port when the entire loop is finished
    joern_process.terminate()
    joern_process.wait()
    try:
        # Use the lsof command to find Java processes listening on port 8080
        result = subprocess.check_output(["lsof", "-i", ":8080"])
        lines = result.decode('utf-8').splitlines()
        for line in lines:
            if "qzq" in line:
                pid = int(line.split()[1])
                os.kill(pid, signal.SIGKILL)
    except Exception as e:
        print("Error occurred while trying to terminate Java processes:", e)


if __name__ == "__main__":
    main()
