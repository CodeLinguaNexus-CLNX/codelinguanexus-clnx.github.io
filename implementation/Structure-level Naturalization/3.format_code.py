import os
import concurrent.futures

def format_file(file_path):
    clang_format_cmd = 'clang-format -style="{BasedOnStyle: llvm, IndentWidth: 4, ColumnLimit: 600}" -i'
    os.system(f"{clang_format_cmd} {file_path}")

def main():
    clnx_train_path = "You path"  # Replace with the actual path

    # Create a list to hold all file paths
    files_to_format = []

    # Iterate through each subdirectory and add file paths to the list
    for subdir in os.listdir(clnx_train_path):
        subdir_path = os.path.join(clnx_train_path, subdir)
        if os.path.isdir(subdir_path):
            files_to_format.append(os.path.join(subdir_path, "func_before.c"))
            files_to_format.append(os.path.join(subdir_path, "func_after.c"))

    # Use a ThreadPoolExecutor to format files in parallel
    with concurrent.futures.ThreadPoolExecutor() as executor:
        executor.map(format_file, files_to_format)

    print("clang-format applied to all files.")

if __name__ == "__main__":
    main()
