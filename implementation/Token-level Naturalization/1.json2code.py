import json
import os

def process_jsonl(jsonl_file, output_dir):
    # Ensure the output directory exists
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)
    
    with open(jsonl_file, 'r', encoding='utf-8') as file:
        for line in file:
            # Parse each line of JSON
            data = json.loads(line)
            idx = data['idx']
            func = data['func']

            # Create a folder named after idx
            folder_path = os.path.join(output_dir, str(idx))
            if not os.path.exists(folder_path):
                os.makedirs(folder_path)

            # Create and write to a .c file
            c_file_path = os.path.join(folder_path, f"{idx}.c")
            with open(c_file_path, 'w', encoding='utf-8') as c_file:
                c_file.write(func)

# Call the function
jsonl_file = 'path/to/your/jsonl_file.jsonl'  # Please replace with your actual jsonl file path
output_dir = 'path/to/your/output_directory'
process_jsonl(jsonl_file, output_dir)
