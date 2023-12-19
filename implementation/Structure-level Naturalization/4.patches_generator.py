import os
import concurrent.futures

def create_diff(subdir_path):
    # Construct the full paths for func_before.c, func_after.c, and patch.diff
    func_before_path = os.path.join(subdir_path, "func_before.c")
    func_after_path = os.path.join(subdir_path, "func_after.c")
    patch_path = os.path.join(subdir_path, "patch.diff")

    # Execute the diff command and redirect output to patch.diff
    os.system(f"diff -u3 {func_before_path} {func_after_path} > {patch_path}")

def main():
    clnx_train_path = "You path"  # Replace with the actual path

    # Use a ThreadPoolExecutor to create diffs in parallel
    with concurrent.futures.ThreadPoolExecutor() as executor:
        # Create a list of all subdirectory paths
        subdir_paths = [os.path.join(clnx_train_path, subdir) for subdir in os.listdir(clnx_train_path) 
                        if os.path.isdir(os.path.join(clnx_train_path, subdir))]
        
        # Map the create_diff function to each subdirectory path
        executor.map(create_diff, subdir_paths)

    print("Diff files created for all func_before.c and func_after.c files in subdirectories.")

if __name__ == "__main__":
    main()
