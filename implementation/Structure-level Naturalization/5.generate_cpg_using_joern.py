import os
import concurrent.futures
from multiprocessing import Value, Lock

def parse_file(subdir_path, total_count, processed_count, lock):
    func_before_path = os.path.join(subdir_path, "func_before.c")
    cpg_bin_path = os.path.join(subdir_path, "cpg.bin")

    # Execute the joern-parse command
    os.chdir(subdir_path)
    os.system(f"joern-parse {func_before_path} > {cpg_bin_path}")

    # Update and print the processed file count
    with lock:
        processed_count.value += 1
        percentage = (processed_count.value / total_count) * 100
        print(f"Processed {processed_count.value}/{total_count} files ({percentage:.2f}%)")

def main():
    clnx_train_path = "You Path"  # Replace with the actual path

    # Create a list of all subdirectory paths
    subdir_paths = [os.path.join(clnx_train_path, subdir) for subdir in os.listdir(clnx_train_path) 
                    if os.path.isdir(os.path.join(clnx_train_path, subdir))]

    total_count = len(subdir_paths)
    processed_count = Value('i', 0)
    lock = Lock()

    # Use a ThreadPoolExecutor to parse files in parallel
    with concurrent.futures.ThreadPoolExecutor() as executor:
        # Map the parse_file function to each subdirectory path
        futures = [executor.submit(parse_file, subdir_path, total_count, processed_count, lock) 
                   for subdir_path in subdir_paths]

        # Wait for all futures to complete
        concurrent.futures.wait(futures)

    print("Parsing complete for all func_before.c files.")

if __name__ == "__main__":
    main()

