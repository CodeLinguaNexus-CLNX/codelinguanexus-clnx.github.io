import json
import os

base_folder = "You folder"
os.makedirs(base_folder, exist_ok=True)

# open jsonl file
jsonl_file = "data_add_after.jsonl"
with open(jsonl_file, 'r') as f:
    # 遍历每一行
    for line in f:
        data = json.loads(line)
        idx = data["idx"] 
        target = data["target"]
        project = data["project"]
        commit_id = data["commit_id"]
        func = data["func"]
        file_path = data["file_path"]
        func_after = data["func_after"]

        folder_path = os.path.join(base_folder, str(idx))
        os.makedirs(folder_path)


        # save 'func' value to 'before_file_path'
        func = func.replace("\n\n", "\n")
        before_file_path = os.path.join(folder_path, "func_before.c")

        with open(before_file_path, "w") as before_file:
            before_file.write(func)

        # save 'func_after' value to 'after_file_path'
        after_file_path = os.path.join(folder_path, "func_after.c")
        
        with open(after_file_path, "w") as after_file:
            after_file.write(func_after)
 